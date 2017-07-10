#pragma once

#include <core/defines.h>
#include <core/types.h>
#include <gui/widget.h>

namespace engine
{
class ENGINE_API Edit : public Widget
{
public:
	Edit();
	void update(f32 deltaTime) override;
	void handleEvent(WidgetEvent& e) override;
	void draw(Canvas& canvas) override;
	void selectAll() { selectionBegin = 0; selectionEnd = text.size(); selectionActive = true; }
	void deselect() { selectionBegin = 0; selectionEnd = 0; selectionActive = false; }
	void deleteSelection();
	String getSelection();
	void clearText();
	void setDefaultText(const String& value) { defaultText = value; }
	void setMaxTextLength(u32 length) { maxTextLength = length; }
	void setNumeric(bool value) { numericOnly = value; }
	void setText(const String& value) override;

protected:
	void processKeyEvent(WidgetEvent& e);
	void processCurrentKey(f32 deltaTime);
	i32 getTextCharIndexAtX(f32 aX);
	void computeScrollAmount();
	void formatNumeric();

	Array<KeyCode> forbiddenKeys;
	String defaultText;
	KeyModifiers lastKeyModifiers;
	KeyCode lastKeyDown = KeyCode::None;
	bool numericOnly = false;
	u32 decimalPlaces = 2;
	f32 autoRepeatTimer = 0.0f;
	f32 autoRepeatDelay = 0.1f;
	f32 autoRepeatStartDelay = 0.5f;
	f32 autoRepeatStartTimer = 0.0f;
	bool firstCharPress = false;
	i32 selectionBegin = 0;
	i32 selectionEnd = 0;
	f32 scrollOffset = 0;
	i32 caretPosition = 0;
	f32 caretBlinkTimer = 0;
	bool selectionActive = false;
	bool selectingWithMouse = false;
	u32 maxTextLength = 0;
};

}