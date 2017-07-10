#include "gui/edit.h>
#include "gui/canvas.h>
#include "gui/theme.h>
#include "gui/event.h>
#include "gui/font.h>
#include "gui/manager.h>
#include "gui/assert.h>
#include <algorithm>

namespace horus
{
struct EditKeyShiftInfo
{
	EditKeyShiftInfo(char chr = 0, char shiftedChar = 0)
	{
		charCode = chr;
		shiftedCharCode = shiftedChar;
	}

	char charCode, shiftedCharCode;
};

static std::map<KeyCode, EditKeyShiftInfo> editCharShiftInfos;

#define UI_EDITBOX_MAPKEY(key, char1, char2) editCharShiftInfos[key] = EditKeyShiftInfo(char1, char2)

Edit::Edit()
{
	if (!editCharShiftInfos.size())
	{
		UI_EDITBOX_MAPKEY(KeyCode::Grave, '`', '~');
		UI_EDITBOX_MAPKEY(KeyCode::Minus, '-', '_');
		UI_EDITBOX_MAPKEY(KeyCode::Equals, '=', '+');
		UI_EDITBOX_MAPKEY(KeyCode::Add, '+', '+');
		UI_EDITBOX_MAPKEY(KeyCode::Multiply, '*', '*');
		UI_EDITBOX_MAPKEY(KeyCode::LBracket, '[', '{');
		UI_EDITBOX_MAPKEY(KeyCode::RBracket, ']', '}');
		UI_EDITBOX_MAPKEY(KeyCode::Comma, ',', '<');
		UI_EDITBOX_MAPKEY(KeyCode::Semicolon, ';', ':');
		UI_EDITBOX_MAPKEY(KeyCode::Space, ' ', ' ');
		UI_EDITBOX_MAPKEY(KeyCode::Period, '.', '>');
		UI_EDITBOX_MAPKEY(KeyCode::Slash, '/', '?');
		UI_EDITBOX_MAPKEY(KeyCode::Backslash, '\\', '|');
		UI_EDITBOX_MAPKEY(KeyCode::Apostrophe, '\'', '\"');
		UI_EDITBOX_MAPKEY(KeyCode::Divide, '/', '/');
		UI_EDITBOX_MAPKEY(KeyCode::Subtract, '-', '-');
		UI_EDITBOX_MAPKEY(KeyCode::Decimal, '.', '.');
	}

	padding.left = padding.top = padding.bottom = padding.right = 3.0f;
	flags.focusable = 1;
}

void Edit::update(f32 deltaTime)
{
	caretBlinkTimer += deltaTime;
	
	if (caretBlinkTimer > 1.0f)
		caretBlinkTimer = 0.0f;

	processCurrentKey(deltaTime);
}

void Edit::draw(Canvas& canvas)
{
	ThemeWidgetSkin* skin = theme->skins["Button"];
	ThemeWidgetState* state = nullptr;

	if (flags.enabled)
	{
		if (flags.hovered)
		{
			if (flags.pressed)
			{
				state = &skin->states["hovered_pressed"];
			}
			else
			{
				state = &skin->states["hovered"];
			}
		}
		else
		{
			if (flags.pressed)
			{
				state = &skin->states["pressed"];
			}
			else
			{
				state = &skin->states["normal"];
			}
		}
	}
	else
	{
		state = &skin->states["disabled"];
	}

	if (!state)
	{
		return;
	}

	if (theme->engine == ThemeEngine::Metro)
	{
		std::string textToCursor = text.substr(0, caretPosition);
		FontTextSize textToCursorSize = state->elements["body"].font->computeTextSize(textToCursor);

		f32 cursorWidth = 1.0f;
		Rect cursorRect(
			absRect.x + padding.left + textToCursorSize.width - scrollOffset,
			absRect.y + padding.top,
			cursorWidth,
			absRect.height - padding.top - padding.bottom);

		canvas.setBlendMode(BlendMode::Opaque);
		canvas.beginBatch(theme->atlas);
		canvas.setColor(Color::white);
		canvas.draw(state->elements["body"].item, absRect);
		canvas.setFont(state->elements["body"].font);

		canvas.setClipRect(absRect);

		if (selectionActive)
		{
			int startSel = selectionBegin, endSel = selectionEnd, tmpSel;

			if (startSel > endSel)
			{
				tmpSel = startSel;
				startSel = endSel;
				endSel = tmpSel;
			}

			std::string selectedText = text.substr(startSel, endSel - startSel);
			std::string textToSelectionStart = text.substr(0, startSel);
			FontTextSize selectedTextSize = state->elements["body"].font->computeTextSize(selectedText);
			FontTextSize textToSelectionStartSize = state->elements["body"].font->computeTextSize(textToSelectionStart);

			Rect selRect(
				absRect.x + padding.left + textToSelectionStartSize.width - scrollOffset,
				absRect.y + padding.top,
				selectedTextSize.width,
				absRect.height - padding.top - padding.bottom);

			canvas.setColor(Color::skyBlue);
			canvas.draw(state->elements["body"].item, selRect);
		}

		if (caretBlinkTimer < 0.5f && flags.focused)
		{
			canvas.setColor(Color::black);
			canvas.draw(state->elements["body"].item, cursorRect);
		}

		canvas.endBatch();

		std::string* textToDraw = &text;
		
		if (text.empty())
		{
			canvas.setColor(Color::gray);
			textToDraw = &defaultText;
		}
		else
			canvas.setColor(Color::black);

		canvas.beginBatch(&state->elements["body"].font->getAtlas());
		canvas.draw(
			*textToDraw,
			Point(
			absRect.x + padding.left - scrollOffset,
			absRect.bottom() - padding.bottom),
			HorizontalTextAlignment::Right,
			VerticalTextAlignment::Top, true);
		canvas.endBatch();
		canvas.restoreClipRect();
	}
	else if (theme->engine == ThemeEngine::Custom)
	{
		theme->customEngine->drawWidget(WidgetType::Edit, this, canvas);
	}
	else if (theme->engine == ThemeEngine::Skinned)
	{
		//TODO
	}
}

void Edit::handleEvent(WidgetEvent& e)
{
	switch (e.type)
	{
	case WidgetEventType::KeyUp:
		processKeyEvent(e);
		break;
	case WidgetEventType::KeyDown:
		processKeyEvent(e);
		break;
	case WidgetEventType::MouseButtonDown:
		if (e.button == MouseButton::Left)
		{
			flags.pressed = 1;
			flags.focused = 1;
			flags.captured = 1;
			deselect();
			caretPosition = getTextCharIndexAtX(e.point.x);
			selectingWithMouse = true;
			selectionBegin = caretPosition;
			computeScrollAmount();
		}
		break;
	case WidgetEventType::MouseButtonUp:
		if (e.button == MouseButton::Left)
		{
			if (absRect.contains(e.windowPoint))
			{
				if (actionCallback)
				{
					actionCallback(this, e, WidgetActionType::Clicked);
				}
			}

			flags.pressed = 0;
			flags.captured = 0;
			selectingWithMouse = false;
			computeScrollAmount();
		}
		break;
	case WidgetEventType::MouseMove:
	{
		if (selectingWithMouse)
		{
			selectionEnd = caretPosition = getTextCharIndexAtX(e.point.x);
			selectionActive = true;
			computeScrollAmount();
		}

		break;
	}
	case WidgetEventType::MouseButtonDblClick:
		if (e.button == MouseButton::Left)
		{
			deselect();
			caretPosition = getTextCharIndexAtX(e.point.x);
			selectionActive = true;

			if (text.empty())
			{
				break;
			}

			// select a word
			while (caretPosition >= 0)
			{
				if (text[caretPosition] == ' ')
				{
					selectionBegin = caretPosition;
					break;
				}

				caretPosition--;
			}

			if (caretPosition < 0)
			{
				caretPosition = 0;
			}

			if (text[caretPosition] == ' ')
			{
				caretPosition++;
				selectionBegin++;
			}

			while (caretPosition < text.length())
			{
				if (text[caretPosition] == ' ')
				{
					selectionEnd = caretPosition;
					break;
				}

				caretPosition++;
			}

			if (caretPosition == text.length())
			{
				selectionEnd = caretPosition;
			}
		}
		break;

	case WidgetEventType::MouseIn:
	{
		manager().setMousePointer(MousePointer(MousePointerType::TextCaret));
		break;
	}

	case WidgetEventType::MouseOut:
	{
		manager().setMousePointer(MousePointer(MousePointerType::Default));
		break;
	}

	case WidgetEventType::LostFocus:
	{
		formatNumeric();
		break;
	}

	default:
		break;
	}
}

void Edit::processCurrentKey(f32 deltaTime)
{
	if (lastKeyDown == KeyCode::None)
		return;

	if (autoRepeatTimer >= autoRepeatDelay && autoRepeatStartTimer >= autoRepeatStartDelay || firstCharPress)
	{
		firstCharPress = false;
		autoRepeatTimer = 0.0f;

		if (lastKeyDown == KeyCode::ArrowLeft)
		{
			caretPosition--;

			if (caretPosition < 0)
				caretPosition = 0;

			// do we select ?
			if (lastKeyModifiers.shift)
			{
				if (!selectionActive)
				{
					selectionActive = true;
					selectionBegin = caretPosition + 1;
					selectionEnd = caretPosition;
				}
				else
				{
					selectionEnd = caretPosition;
				}
			}
			else
			{
				selectionActive = false;
			}
		}
		else if (lastKeyDown == KeyCode::ArrowRight)
		{
			caretPosition++;

			if (caretPosition > text.size())
				caretPosition = text.size();

			// do we select ?
			if (lastKeyModifiers.shift)
			{
				if (!selectionActive)
				{
					selectionActive = true;
					selectionBegin = caretPosition - 1;
					selectionEnd = caretPosition;
				}
				else
				{
					selectionEnd = caretPosition;
				}
			}
			else
			{
				selectionActive = false;
			}
		}
		else if (lastKeyDown == KeyCode::Delete)
		{
			if (selectionActive)
			{
				deleteSelection();
			}
			else
			{
				if (!text.empty())
				{
					text.erase(text.begin() + caretPosition);
				}
			}
		}
		else if (lastKeyDown == KeyCode::Backspace)
		{
			if (selectionActive)
			{
				deleteSelection();
			}
			else
			{
				if (!text.empty())
				{
					if (caretPosition - 1 >= 0)
					{
						text.erase(text.begin() + caretPosition - 1);
					}

					--caretPosition;
				}

				if (caretPosition < 0)
				{
					caretPosition = 0;
				}
			}
		}
		else if (lastKeyDown == KeyCode::End || lastKeyDown == KeyCode::ArrowDown)
		{
			// do we select ?
			if (lastKeyModifiers.shift)
			{
				if (!selectionActive)
				{
					selectionActive = true;
					selectionBegin = caretPosition;
					selectionEnd = text.size();
				}
				else
				{
					selectionEnd = text.size();
				}
			}
			else
			{
				selectionActive = false;
			}

			caretPosition = text.size();
		}
		else if (lastKeyDown == KeyCode::Home || lastKeyDown == KeyCode::ArrowUp)
		{
			// do we select ?
			if (lastKeyModifiers.shift)
			{
				if (!selectionActive)
				{
					selectionActive = true;
					selectionBegin = caretPosition;
					selectionEnd = 0;
				}
				else
				{
					selectionEnd = 0;
				}
			}
			else
			{
				selectionActive = false;
			}

			caretPosition = 0;
			scrollOffset = 0;
		}
		else if (lastKeyDown == KeyCode::A && lastKeyModifiers.control)
		{
			selectionActive = true;
			selectionBegin = 0;
			selectionEnd = text.size();
		}
		else if (lastKeyDown == KeyCode::C && lastKeyModifiers.control)
		{
			std::string str = getSelection();

			if (!str.empty())
			{
				copyToClipboard(str);
			}
		}
		else if (lastKeyDown == KeyCode::V && lastKeyModifiers.control)
		{
			if (selectionActive)
			{
				deleteSelection();
			}

			std::string str;
			str = pasteFromClipboard();
			text.insert(caretPosition, str);
			caretPosition += str.size();
		}
		else if (lastKeyDown == KeyCode::X && lastKeyModifiers.control)
		{
			std::string str = getSelection();

			if (selectionActive && !str.empty())
			{
				deleteSelection();
				copyToClipboard(str);
			}
		}
		else
		{
			static std::string chars = "1234567890abcdefghijklmnopqrstuvwxyz";
			static std::string charsShift = "!@#$%^&*()ABCDEFGHIJKLMNOPQRSTUVWXYZ";

			if (numericOnly)
			{
				if (lastKeyDown >= KeyCode::Num1 && lastKeyDown <= KeyCode::Num0)
				{
					if (selectionActive)
						deleteSelection();

					text.insert(text.begin() + caretPosition, chars[(u32)lastKeyDown - (u32)KeyCode::Num1]);

					++caretPosition;
					selectionActive = false;
				}
				else if (lastKeyDown >= KeyCode::NumPad1 && lastKeyDown <= KeyCode::NumPad0)
				{
					if (selectionActive)
						deleteSelection();

					text.insert(text.begin() + caretPosition, chars[(u32)lastKeyDown - (u32)KeyCode::NumPad1]);

					++caretPosition;
					selectionActive = false;
				}
				else if (lastKeyDown == KeyCode::Minus
						|| lastKeyDown == KeyCode::Period)
				{
					auto iter = editCharShiftInfos.find(lastKeyDown);

					if (iter != editCharShiftInfos.end())
					{
						if (selectionActive)
							deleteSelection();
						text.insert(text.begin() + caretPosition, iter->second.charCode);
						++caretPosition;
						selectionActive = false;
					}
				}
			}
			else if (lastKeyDown >= KeyCode::Num1 && lastKeyDown <= KeyCode::Z)
			{
				if (selectionActive)
					deleteSelection();

				if (lastKeyModifiers.shift || lastKeyModifiers.capsLock)
				{
					text.insert(text.begin() + caretPosition, charsShift[(u32)lastKeyDown - (u32)KeyCode::Num1]);
				}
				else
				{
					text.insert(text.begin() + caretPosition, chars[(u32)lastKeyDown - (u32)KeyCode::Num1]);
				}

				++caretPosition;
				selectionActive = false;
			}
			else if (lastKeyDown >= KeyCode::NumPad1 && lastKeyDown <= KeyCode::NumPad0)
			{
				if (selectionActive)
					deleteSelection();

				if (lastKeyModifiers.shift || lastKeyModifiers.capsLock)
				{
					text.insert(text.begin() + caretPosition, charsShift[(u32)lastKeyDown - (u32)KeyCode::NumPad1]);
				}
				else
				{
					text.insert(text.begin() + caretPosition, chars[(u32)lastKeyDown - (u32)KeyCode::NumPad1]);
				}

				++caretPosition;
				selectionActive = false;
			}
			else
			{
				auto iter = editCharShiftInfos.find(lastKeyDown);

				if (iter != editCharShiftInfos.end())
				{
					if (selectionActive)
						deleteSelection();

					if (lastKeyModifiers.shift)
						text.insert(text.begin() + caretPosition, iter->second.shiftedCharCode);
					else
						text.insert(text.begin() + caretPosition, iter->second.charCode);

					++caretPosition;
					selectionActive = false;
				}
			}
		}
	}

	if (maxTextLength != 0)
	{
		if (text.length() > maxTextLength)
		{
			text = text.substr(0, maxTextLength);
			caretPosition = maxTextLength;
		}
	}

	computeScrollAmount();
	autoRepeatTimer += deltaTime;
	autoRepeatStartTimer += deltaTime;
	caretBlinkTimer = 0;
}

void Edit::processKeyEvent(WidgetEvent& e)
{
	if (!flags.focused)
	{
		return;
	}

	// check the forbidden keys
	auto iter = std::find(forbiddenKeys.begin(), forbiddenKeys.end(), e.key);

	if (iter != forbiddenKeys.end())
	{
		return;
	}

	if (e.type == WidgetEventType::KeyDown)
	{
		if (lastKeyDown != e.key)
		{
			autoRepeatTimer = 0.0f;
			autoRepeatStartTimer = 0.0f;
			firstCharPress = true;
		}

		lastKeyModifiers = e.keyModifiers;
		lastKeyDown = e.key;

		if (e.key == KeyCode::Enter)
		{
			flags.focused = 0;
			formatNumeric();

			if (actionCallback)
			{
				actionCallback(this, e, WidgetActionType::Changed);
			}
		}
	}
	else if (e.type == WidgetEventType::KeyUp)
	{
		autoRepeatTimer = 0;
		autoRepeatStartTimer = 0;
		lastKeyDown = KeyCode::None;
	}
}

void Edit::deleteSelection()
{
	std::string str1, str2;
	int startSel = selectionBegin, endSel = selectionEnd, tmpSel;

	if (startSel > endSel)
	{
		tmpSel = startSel;
		startSel = endSel;
		endSel = tmpSel;
	}

	if (!text.empty())
	{
		text.erase(text.begin() + startSel, text.begin() + endSel);
	}

	startSel = startSel < text.size() ? startSel : text.size();
	caretPosition = startSel;
	selectionBegin = selectionEnd = caretPosition;
	selectionActive = false;
}

std::string Edit::getSelection()
{
	if (selectionActive)
	{
		i32 startSel = selectionBegin, endSel = selectionEnd, tmpSel;

		if (startSel > endSel)
		{
			tmpSel = startSel;
			startSel = endSel;
			endSel = tmpSel;
		}

		if (!text.empty() && abs(endSel - startSel))
		{
			std::string str = text.substr(startSel, endSel - startSel);
			return str;
		}
	}

	return "";
}

void Edit::clearText()
{
	text = "";
	selectionBegin = selectionEnd = caretPosition = 0;
	selectionActive = false;
	scrollOffset = 0;
}

i32 Edit::getTextCharIndexAtX(f32 xPos)
{
	FontTextSize rc, rc2;
	std::string subStr, lastChar;
	i32 pos = 0;
	ThemeWidgetSkin* skin = theme->skins["Button"];
	HORUS_ASSERT(skin);

	xPos += scrollOffset;

	if (!skin)
	{
		return 0;
	}

	Font* font = skin->states["normal"].elements["body"].font;

	HORUS_ASSERT(font);

	if (!font)
	{
		return 0;
	}

	//TODO: optimize, slow on long text
	for (size_t i = 0; i < text.size(); ++i)
	{
		subStr = text.substr(0, i);

		if (subStr.size() >= 1)
		{
			lastChar = subStr.substr(subStr.size() - 1, 1);
			
			rc2 = font->computeTextSize(lastChar);
		}
		else
		{
			rc2.width = 0;
		}

		rc = font->computeTextSize(subStr);
		bool passed = false;

		if (((padding.left + rc.width) - rc2.width / 2) > xPos)
		{
			pos = i - 1;
			passed = true;
		}
		else if (padding.left + rc.width >= xPos)
		{
			pos = i;
			passed = true;
		}

		if (passed)
		{
			if (pos < 0)
				pos = 0;

			if (pos > text.size())
				pos = text.size();

			return pos;
		}
	}

	return text.size();
}

void Edit::computeScrollAmount()
{
	ThemeWidgetSkin* skin = theme->skins["Button"];
	HORUS_ASSERT(skin);
	FontTextSize textSizeToCaret = skin->states["normal"].elements["body"].font->computeTextSize(text.substr(0, caretPosition));
	f32 absCursorPosX = absRect.x + textSizeToCaret.width - scrollOffset;
	f32 stepAmount = 30;

	if (absCursorPosX < absRect.x + padding.left)
	{
		scrollOffset -= (absRect.x + padding.left - absCursorPosX) + stepAmount;
	}
	else if (absCursorPosX > absRect.right() - padding.right)
	{
		scrollOffset += absCursorPosX - (absRect.right() - padding.right) + stepAmount;
	}

	if (scrollOffset < 0)
	{
		scrollOffset = 0;
	}
}

void Edit::formatNumeric()
{
	if (numericOnly)
	{
		char valstr[100] = { 0 };
		std::string fmt;
		f32 val = 0;

		sprintf(valstr, "%%.%df", decimalPlaces);
		val = atof(text.c_str());
		fmt = valstr;
		sprintf(valstr, fmt.c_str(), val);
		text = valstr;

		if (caretPosition > text.length())
		{
			caretPosition = text.length();
		}
	}
}

void Edit::setText(const std::string& value)
{
	text = value;
	caretPosition = value.length();
	formatNumeric();
	selectionBegin = selectionEnd = caretPosition;
	selectionActive = false;
}

}