
void Canvas::setClipRect(const Rect& rect)
{
}

void Canvas::restoreClipRect()
{
}

void Canvas::setViewport(const Rect& rect)
{
}

Rect Canvas::getViewport() const
{
}

Rect Canvas::getWindowRect() const
{
}

Rect Canvas::getWindowClientRect() const
}

void Canvas::setColor(const Color& color)
{
	this->color = color;
}

void Canvas::setSpriteSizePolicy(SpriteSizingPolicy policy)
{
	this->spriteSizePolicy = policy;
}

void Canvas::setBlendMode(BlendMode mode)
{
	this->blendMode = mode;
}

void Canvas::setFont(Font* font)
{
	this->font = font;
}

void Canvas::beginBatch(Atlas* atlas)
{
	this->atlas = atlas;
	currentVertexIndex = 0;
}

void setSamplerValueInGpuProgram(
	GLuint program,
	GLuint tex,
	const std::string& constName,
	u32 stage)
{
	GLint loc = glGetUniformLocation(program, constName.c_str());

	if (loc == -1)
	{
		return;
	}

	glUniform1i(loc, stage);
	glActiveTexture(GL_TEXTURE0 + stage);
	glBindTexture(GL_TEXTURE_2D, tex);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	GLfloat fLargest = 0.0f;

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
}

void setIntValueInGpuProgram(
	GLuint program,
	GLuint value,
	const std::string& constName)
{
	GLint loc = glGetUniformLocation(program, constName.c_str());

	if (loc == -1)
	{
		return;
	}

	glUniform1i(loc, value);
}

void Canvas::commitRenderStates()
{
	if (blendMode != BlendMode::Opaque)
	{
		glEnable(GL_BLEND);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
}

void Canvas::endBatch()
{
#define OGL_VBUFFER_OFFSET(i) ((void*)(i))

	glUseProgram((GLuint)program);

	if (atlas)
	{
		setSamplerValueInGpuProgram(
			(GLuint)program,
			(GLuint)atlas->getTexture().getHandle(),
			"diffuseSampler",
			0);
		setIntValueInGpuProgram((GLuint)program, textureEnabled ? 1 : 0, "textured");

		if (smoothFiltering)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}

	GLint loc = glGetUniformLocation((GLuint)program, "mvp");

	if (loc != -1)
	{
		GLint vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);

		f32 left = vp[0];
		f32 right = vp[0] + vp[2];
		f32 top = vp[1];
		f32 bottom = vp[1] + vp[3];

		f32 m[4][4];

		m[0][0] = 2.0f / (right - left);
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;
		m[0][3] = 0.0f;

		m[1][0] = 0.0f;
		m[1][1] = 2.0f / (top - bottom);
		m[1][2] = 0.0f;
		m[1][3] = 0.0f;

		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = 1.0f;
		m[2][3] = 0.0f;

		m[3][0] = (left + right) / (left - right);
		m[3][1] = (top + bottom) / (bottom - top);
		m[3][2] = 0.0f;
		m[3][3] = 1.0f;

		glUniformMatrix4fv(loc, 1, false, (GLfloat*)m);
	}
	else
	{
		return;
	}

	commitRenderStates();
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbo);

	u8* data = (u8*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(UiVertex) * currentVertexIndex, GL_MAP_WRITE_BIT);

	if (!data)
	{
		currentVertexIndex = 0;
		return;
	}

	memcpy(data, vertices, currentVertexIndex * sizeof(UiVertex));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	u32 stride = sizeof(UiVertex);
	u32 attrLoc = 0;
	u32 offsetSum = 0;

	attrLoc = glGetAttribLocation((GLuint)program, "inPOSITION");
		
	if (attrLoc == ~0)
	{
		currentVertexIndex = 0;
		return;
	}

	glEnableVertexAttribArray(attrLoc);
	glVertexAttribPointer(attrLoc, 2, GL_FLOAT, GL_FALSE, stride, OGL_VBUFFER_OFFSET(offsetSum));
	if (glVertexAttribDivisor) glVertexAttribDivisor(attrLoc, 0);
	offsetSum += sizeof(f32) * 2;

	attrLoc = glGetAttribLocation((GLuint)program, "inTEXCOORD0");
	glEnableVertexAttribArray(attrLoc);
	glVertexAttribPointer(attrLoc, 2, GL_FLOAT, GL_FALSE, stride, OGL_VBUFFER_OFFSET(offsetSum));
	if (glVertexAttribDivisor) glVertexAttribDivisor(attrLoc, 0);
	offsetSum += sizeof(f32) * 2;

	attrLoc = glGetAttribLocation((GLuint)program, "inCOLOR");
	glEnableVertexAttribArray(attrLoc);
	glVertexAttribPointer(attrLoc, 4, GL_FLOAT, GL_FALSE, stride, OGL_VBUFFER_OFFSET(offsetSum));
	if (glVertexAttribDivisor) glVertexAttribDivisor(attrLoc, 0);
	offsetSum += sizeof(f32) * 4;

	glDrawArrays(GL_TRIANGLES, 0, currentVertexIndex);
	currentVertexIndex = 0;
}

void Canvas::drawQuad(const Rect& uvRect, const Rect& rect)
{
	textureEnabled = true;

	u32 i = currentVertexIndex;

	vertices[i].x = rect.x;
	vertices[i].y = rect.y;
	vertices[i].u = uvRect.x;
	vertices[i].v = uvRect.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = uvRect.right();
	vertices[i].v = uvRect.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = uvRect.x;
	vertices[i].v = uvRect.bottom();
	vertices[i].color = color;
	i++;

	// 2nd tri

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = uvRect.right();
	vertices[i].v = uvRect.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.bottom();
	vertices[i].u = uvRect.right();
	vertices[i].v = uvRect.bottom();
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = uvRect.x;
	vertices[i].v = uvRect.bottom();
	vertices[i].color = color;
	i++;

	currentVertexIndex = i;

	//TODO: resize if small buffer
}

void Canvas::drawQuadUvRot(const Rect& uvRect, const Rect& rect)
{
	textureEnabled = true;

	u32 i = currentVertexIndex;

	Point t0(uvRect.topLeft());
	Point t1(uvRect.topRight());
	Point t2(uvRect.right(), uvRect.bottom());
	Point t3(uvRect.bottomLeft());

	vertices[i].x = rect.x;
	vertices[i].y = rect.y;
	vertices[i].u = t3.x;
	vertices[i].v = t3.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = t0.x;
	vertices[i].v = t0.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = t2.x;
	vertices[i].v = t2.y;
	vertices[i].color = color;
	i++;

	// 2nd tri

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = t0.x;
	vertices[i].v = t0.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.bottom();
	vertices[i].u = t1.x;
	vertices[i].v = t1.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = t2.x;
	vertices[i].v = t2.y;
	vertices[i].color = color;
	i++;

	currentVertexIndex = i;

	//TODO: resize if small buffer
}

void Canvas::drawQuad(const Rect& rect)
{
	textureEnabled = false;

	u32 i = currentVertexIndex;

	vertices[i].x = rect.x;
	vertices[i].y = rect.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].color = color;
	i++;

	// 2nd tri

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.bottom();
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].color = color;
	i++;

	currentVertexIndex = i;

	//TODO: resize if small buffer
}

void Canvas::draw(Sprite* sprite, const Rect& screenRect)
{
	if (sprite->rotated)
		drawQuadUvRot(sprite->texCoords, screenRect);
	else
		drawQuad(sprite->texCoords, screenRect);
}

void Canvas::draw(Sprite* sprite, const Point& screenPos)
{
	if (sprite->rotated)
		drawQuadUvRot(
			sprite->texCoords,
			Rect(
			screenPos.x,
			screenPos.y,
			sprite->width,
			sprite->height));
	else
		drawQuad(
			sprite->texCoords,
			Rect(
				screenPos.x,
				screenPos.y,
				sprite->width,
				sprite->height));
}

void Canvas::draw(const Rect& atlasUvRect, const Rect& screenRect)
{
	drawQuad(atlasUvRect, screenRect);
}

void Canvas::draw(const Rect& screenRect)
{
	drawQuad(Rect(0.0f, 0.0f, 1.0f, 1.0f), screenRect);
}

void Canvas::drawSolidColor(const Rect& screenRect)
{
	drawQuad(screenRect);
}

void Canvas::draw(const NineCellBox& box, const Rect& screenRect)
{
	// top row
	draw(
		box.topLeft,
		Rect(
			screenRect.x, screenRect.y,
			box.topLeft->width, box.topLeft->height));
	draw(
		box.topMiddle,
		Rect(
			screenRect.x + box.topLeft->width,
			screenRect.y,
			screenRect.width - box.topLeft->width - box.topRight->width,
			box.topMiddle->height));
	draw(
		box.topRight,
		Rect(
			screenRect.right() - box.topRight->width,
			screenRect.y,
			box.topRight->width,
			box.topRight->height));

	// middle row
	draw(
		box.middleLeft,
		Rect(
			screenRect.x,
			screenRect.y + box.topLeft->height,
			box.middleLeft->width,
			screenRect.height - box.topLeft->height - box.bottomLeft->height	));

	draw(
		box.middleCenter,
		Rect(
			screenRect.x + box.topLeft->width,
			screenRect.y + box.topLeft->height,
			screenRect.width - box.middleLeft->width - box.middleRight->width,
			screenRect.height - box.topMiddle->height - box.bottomMiddle->height));

	draw(
		box.middleRight,
		Rect(
		screenRect.right() - box.middleRight->width,
		screenRect.y + box.topRight->height,
		box.middleRight->width,
		screenRect.height - box.topRight->height - box.bottomRight->height));

	// bottom row
	draw(
		box.bottomLeft,
		Rect(
		screenRect.x,
		screenRect.bottom() - box.bottomLeft->height,
		box.bottomLeft->width, box.bottomLeft->height));
	draw(
		box.bottomMiddle,
		Rect(
		screenRect.x + box.bottomLeft->width,
		screenRect.bottom() - box.bottomMiddle->height,
		screenRect.width - box.bottomLeft->width - box.bottomRight->width,
		box.bottomMiddle->height));
	draw(
		box.bottomRight,
		Rect(
		screenRect.right() - box.bottomRight->width,
		screenRect.bottom() - box.bottomRight->height,
		box.bottomRight->width,
		box.bottomRight->height));
}

void Canvas::drawQuad(const Point& pt1, const Point& pt2, const Point& pt3, const Point& pt4)
{
	textureEnabled = false;

	u32 i = currentVertexIndex;

	vertices[i].x = pt1.x;
	vertices[i].y = pt1.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = pt2.x;
	vertices[i].y = pt2.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = pt4.x;
	vertices[i].y = pt4.y;
	vertices[i].color = color;
	i++;

	// 2nd tri

	vertices[i].x = pt2.x;
	vertices[i].y = pt2.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = pt3.x;
	vertices[i].y = pt3.y;
	vertices[i].color = color;
	i++;

	vertices[i].x = pt4.x;
	vertices[i].y = pt4.y;
	vertices[i].color = color;
	i++;

	currentVertexIndex = i;

	//TODO: resize if small buffer
}

void Canvas::draw(u8* utf8Text, const Point& position)
{
	//TODO
}

void Canvas::draw(
	u8* utf8Text,
	const Rect& rect,
	HorizontalTextAlignment horizontal,
	VerticalTextAlignment vertical)
{
	//TODO
}

void Canvas::draw(
	const std::string& text,
	const Point& position,
	HorizontalTextAlignment horizontal,
	VerticalTextAlignment vertical,
	bool useBaseline)
{
	if (!font)
	{
		return;
	}

	FontTextSize textExtent = font->computeTextSize(text);
	Point pos = position;

	switch (vertical)
	{
	case horus::VerticalTextAlignment::Top:
		pos.y = position.y - textExtent.height - font->getTextSizeForAllGlyphs().maxDescent;
		break;
	case horus::VerticalTextAlignment::Center:
		pos.y = position.y - textExtent.height / 2.0f;
		break;
	case horus::VerticalTextAlignment::Bottom:
		pos.y = position.y;
		break;
	default:
		break;
	}

	switch (horizontal)
	{
	case horus::HorizontalTextAlignment::Left:
		pos.x = position.x - textExtent.width;
		break;
	case horus::HorizontalTextAlignment::Center:
		pos.x = position.x - textExtent.width / 2.0f;
		break;
	case horus::HorizontalTextAlignment::Right:
		pos.x = position.x;
		break;
	default:
		break;
	}

	drawTextInternal(text, textExtent, pos, useBaseline);
}

void Canvas::draw(
	std::string& text,
	const Rect& rect,
	HorizontalTextAlignment horizontal,
	VerticalTextAlignment vertical,
	bool useBaseline)
{
	if (!font)
	{
		return;
	}

	FontTextSize textExtent = font->computeTextSize(text);
	Point pos;

	switch (vertical)
	{
	case horus::VerticalTextAlignment::Top:
		pos.y = rect.y;
		break;
	case horus::VerticalTextAlignment::Center:
		pos.y = rect.y + rect.height / 2.0f - textExtent.height / 2.0f;
		break;
	case horus::VerticalTextAlignment::Bottom:
		pos.y = rect.bottom() - textExtent.height;
		break;
	default:
		break;
	}

	switch (horizontal)
	{
	case horus::HorizontalTextAlignment::Left:
		pos.x = rect.x;
		break;
	case horus::HorizontalTextAlignment::Center:
		pos.x = rect.x + (rect.width - textExtent.width) / 2.0f;
		break;
	case horus::HorizontalTextAlignment::Right:
		pos.x = rect.right() - textExtent.width;
		break;
	default:
		break;
	}

	drawTextInternal(text, textExtent, pos, useBaseline);
}

void Canvas::drawTextInternal(
	const std::string& text,
	const FontTextSize& textExtent,
	const Point& atPos,
	bool useBaseline)
{
	Point pos = atPos;
	auto oldBlend = blendMode;
	auto oldFilter = smoothFiltering;
	smoothFiltering = false;
	blendMode = BlendMode::Alpha;
	beginBatch(&font->getAtlas());
	u32 lastChr = 0;

	for (auto chr : text)
	{
		auto glyph = font->getGlyph((u8)chr);
		auto sprite = font->getGlyphSprite((u8)chr);

		if (chr == '\n')
		{
			pos.x = atPos.x;
			pos.y += textExtent.height;
			continue;
		}

		if (!glyph || !sprite)
		{
			continue;
		}

		Point chrPos(pos.x + glyph->bearingX, pos.y - glyph->advanceY + textExtent.height - ((useBaseline) ? 0 : textExtent.maxDescent));
		draw(sprite, chrPos);
		auto kern = font->getKerning(lastChr, chr);
		pos.x += glyph->advanceX + kern;
		lastChr = chr;
	}

	endBatch();
	blendMode = oldBlend;
	smoothFiltering = oldFilter;
}

void Canvas::drawRenderTarget(const Point& screenPos)
{
	beginBatch(nullptr);
	glBindTexture(GL_TEXTURE_2D, (GLuint)renderTarget->textureHandle);
	//TODO
	endBatch();
}