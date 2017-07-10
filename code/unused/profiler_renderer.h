	void renderGraph(
		ProfilerSection* pSection,
		i32 aX, i32 aY,
		i32 aWidth, i32 aHeight,
		const Color& rColor = Color::kRed);

		
void Profiler::renderGraph(
	ProfilerSection* pSection,
	i32 aX, i32 aY,
	i32 aWidth, i32 aHeight,
	const Color& rColor)
{
	if (!pSection)
		return;

	debugRender()->disableTexturing();
	debugRender()->beginPrimitives(eRenderPrimitive_TriangleList);
	debugRender()->setColor(0.0f, 0.0f, 0.0f, 0.7f);
	debugRender()->add2dQuad(aX, aY, aWidth, aHeight);
	debugRender()->endPrimitives();
	debugRender()->beginPrimitives(eRenderPrimitive_LineStrip);
	debugRender()->setColor(1.0f, 1.0f, 0.0f, 1.0f);
	debugRender()->add2dQuad(aX, aY, aWidth, aHeight);
	debugRender()->endPrimitives();
	debugRender()->beginPrimitives(eRenderPrimitive_LineList);
	double step = (double)aHeight / 4;

	for (size_t i = 0; i < 4; ++i)
	{
		debugRender()->setColor(0.0f, 1.0f, 0.0f, 0.5f);
		debugRender()->addVertex(aX, aY + i * step, 0.0f);
		debugRender()->addVertex(aX + aWidth, aY + i * step, 0.0f);
	}

	debugRender()->endPrimitives();

	double maxVal = -DBL_MAX;
	double minVal = DBL_MAX;

	if (!pSection->graphTimings.isEmpty())
	{
		debugRender()->beginPrimitives(eRenderPrimitive_LineStrip);
		double x = aX;
		step = (double) aWidth / kMaxGraphTimingsBufferSize;

		for (size_t i = 0, iCount = pSection->graphTimings.size(); i < iCount; ++i)
		{
			if (pSection->graphTimings[i] > maxVal)
			{
				maxVal = pSection->graphTimings[i];
			}

			if (pSection->graphTimings[i] < minVal)
			{
				minVal = pSection->graphTimings[i];
			}
		}

		debugRender()->setColor(rColor);
		f32 xx;
		f32 yy;
	
		for (size_t i = 0, iCount = pSection->graphTimings.size(); i < iCount; ++i)
		{
			xx = x += step;
			yy = aY + aHeight - aHeight * (pSection->graphTimings[i] - minVal)	/ (maxVal - minVal);

			if (yy < aY)
			{
				yy = aY;
			}

			if (yy > aY + aHeight)
			{
				yy = aY + aHeight;
			}

			debugRender()->addVertex(xx, yy);
		}

		debugRender()->endPrimitives();
	}
	else
	{
		minVal = maxVal = 0.0f;
	}

	debugRender()->enableTextureFiltering(false);
	debugRender()->textSettings().textColor = rColor;
	debugRender()->drawTextAt(aX + 5.0f, aY - 5.0f, pSection->name.c_str());

	debugRender()->textSettings().textColor = Color::kWhite;
	debugRender()->drawText(
		" - (msec): Current: %.4g / Peak: %.4g / Average: %.4g",
		pSection->deltaTime,
		pSection->peakTime,
		pSection->averageTime);

	debugRender()->textSettings().textColor = Color::kCyan;
	debugRender()->drawTextAt(aX + 5.0f, aY + 5.0f, "%f", maxVal);
	debugRender()->drawTextAt(aX + 5.0f, aY + aHeight - 5.0f, "%f", minVal);
}
		