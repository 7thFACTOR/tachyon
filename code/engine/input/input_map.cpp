// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <string.h>
#include "base/dictionary.h"
#include "input/input_map.h"
#include "core/globals.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/json.h"
#include "base/math/util.h"
#include "core/application.h"
#include "core/globals.h"
#include "input/input.h"

namespace engine
{
InputMap::InputMap()
{}

InputMap::~InputMap()
{}

void InputMap::free()
{
	actions.clear();
}

void InputMap::update(f32 deltaTime)
{
	// check the actions and add to combo queue
	for (size_t i = 0, iCount = actions.size(); i < iCount; ++i)
	{
		processAction(deltaTime, i);
	}
}

void InputMap::addAction(
	const String& name,
	const InputMapControlCombination& ctrlCombo)
{
	InputAction action;

	for (auto& action : actions)
	{
		if (action.name == name)
		{
			action.controlCombos.append(ctrlCombo);
			return;
		}
	}

	action.controlCombos.append(ctrlCombo);
	action.name = name;
	actions.append(action);
}

bool InputMap::isAction(u32 index)
{
	B_ASSERT(B_INBOUNDS(index, 0, actions.size()));

	if (index < 0 || index >= actions.size())
		return false;

	return actions[index].active;
}

bool InputMap::isActionCombo(const String& name)
{
	InputActionCombo* actionCombo = findActionCombo(name);

	if (!actionCombo)
	{
		B_LOG_ERROR("Unknown action combo name: '" << name << "'");
		return false;
	}

	if (actionCombo->activationStarted)
	{
		auto crtTime = getTimeMilliseconds();

		if (crtTime - actionCombo->lastActionActivatedTime > actionCombo->maxDelay)
		{
			// abort the action combo sequence, too much time passed between the actions
			actionCombo->activationStarted = false;
			actionComboInitiated = false;
		}
		else
		{
			// check to see if the next action was triggered
			size_t index = actionCombo->lastActionActivatedIndex + 1;

			if (index < actionCombo->actions.size())
			{
				if (isAction(actionCombo->actions[index]))
				{
					// alright then, action was done, reset timer
					actionCombo->lastActionActivatedTime = getTimeMilliseconds();
					// go to next action index
					actionCombo->lastActionActivatedIndex++;
				}
				//TODO: ideally we should check if any other key/action was pressed and if so, abort combo
				// because it didn't match the current action
			}
			else
			{
				actionCombo->lastActionActivatedIndex = actionCombo->actions.size();
			}

			if (actionCombo->lastActionActivatedIndex >= actionCombo->actions.size())
			{
				actionCombo->activationStarted = false;
				actionComboInitiated = false;
				// the full action combo chain was triggered!
				return true;
			}
		}
	}
	else
	{
		B_ASSERT(actionCombo->actions.size());

		if (isAction(actionCombo->actions[0]))
		{
			actionComboInitiated = true;
			actionCombo->activationStarted = true;
			actionCombo->lastActionActivatedIndex = 0;
			actionCombo->lastActionActivatedTime = getTimeMilliseconds();
		}
	}

	return false;
}

void InputMap::addActionCombo(const InputActionCombo& actionCombo)
{
	auto actionComboPtr = findActionCombo(actionCombo.name);

	if (actionComboPtr)
	{
		*actionComboPtr = actionCombo;
		return;
	}

	actionCombos.append(actionCombo);
}

void InputMap::processAction(f32 deltaTime, size_t index)
{
	B_ASSERT(B_INBOUNDS(index, 0, actions.size()));

	if (index >= actions.size())
		return;

	InputAction& action = actions[index];

	for (auto& ctrlCombo : action.controlCombos)
	{
		for (auto& ctrl : ctrlCombo.controls)
		{
			bool shift = false;
			bool alt = false;
			bool control = false;

			switch (ctrl.type)
			{
			case InputDeviceType::Keyboard:
			{
				InputKey key = (InputKey)ctrl.keyId;
				InputEvent ev;

				if (getApplication().inputQueue.hasKeyDownEvent(key, &ev))
				{
					if (!ctrl.down)
					{
						ctrl.down = true;
					}

					shift = ev.key.shift;
					alt = ev.key.alt;
					control = ev.key.control;
				}

				// the key can also be UP, in the same queue. lets check!
				if (getApplication().inputQueue.hasKeyUpEvent(key, &ev))
				{
					if (ctrl.down)
					{
						ctrl.down = false;
					}

					shift = ev.key.shift;
					alt = ev.key.alt;
					control = ev.key.control;
				}


				if (key == InputKey::LShift || key == InputKey::RShift)
					ctrl.down = shift;

				if (key == InputKey::LAlt || key == InputKey::RAlt)
					ctrl.down = alt;

				if (key == InputKey::LControl || key == InputKey::RControl)
					ctrl.down = control;

				break;
			}

			case InputDeviceType::Mouse:
			{
				u32 button = 0;

				if (getApplication().inputQueue.hasMouseButtonDown((MouseButton)button))
				{
					ctrl.down = true;
				}

				// the button can also be UP, in the same queue. lets check!
				if (getApplication().inputQueue.hasMouseButtonUp((MouseButton)button))
				{
					ctrl.down = false;
				}

				break;
			}

			case InputDeviceType::Joystick:
			{
				//TODO
				break;
			}
			}
		}
	}

	bool active = false;

	// check to see if any of the key combos are activated
	for (auto& ctrlCombo : action.controlCombos)
	{
		bool comboIsActive = false;

		if (ctrlCombo.controls.notEmpty())
		{
			comboIsActive = ctrlCombo.controls[0].down;
		}

		// check for the remaining controls to see if they are down/pressed
		for (size_t i = 1; i < ctrlCombo.controls.size(); i++)
		{
			auto& ctrl = ctrlCombo.controls[i];

			if (!ctrl.down)
			{
				comboIsActive = false;
				break;
			}
			else
			{
				comboIsActive = true;
			}
		}

		if (comboIsActive)
		{
			active = true;
			break;
		}
	}

	action.active = active;
}

InputAction* InputMap::findAction(const String& name)
{
	for (size_t i = 0, iCount = actions.size(); i < iCount; ++i)
	{
		if (actions[i].name == name)
		{
			return &actions[i];
		}
	}

	return nullptr;
}

InputActionCombo* InputMap::findActionCombo(const String& name)
{
	for (size_t i = 0, iCount = actionCombos.size(); i < iCount; ++i)
	{
		if (actionCombos[i].name == name)
		{
			return &actionCombos[i];
		}
	}

	return nullptr;
}

bool InputMap::isAction(const String& name)
{
	for (size_t i = 0, iCount = actions.size(); i < iCount; ++i)
	{
		if (actions[i].name == name)
		{
			return isAction(i);
		}
	}

	return false;
}

bool InputMap::load(const String& inputMapFilename)
{
	Dictionary<String, InputKey> controlsMap;

	// keep the key as UPPERCASE
	controlsMap["F1"] = InputKey::F1;
	controlsMap["F2"] = InputKey::F2;
	controlsMap["F3"] = InputKey::F3;
	controlsMap["F4"] = InputKey::F4;
	controlsMap["F5"] = InputKey::F5;
	controlsMap["F6"] = InputKey::F6;
	controlsMap["F7"] = InputKey::F7;
	controlsMap["F8"] = InputKey::F8;
	controlsMap["F9"] = InputKey::F9;
	controlsMap["F10"] = InputKey::F10;
	controlsMap["F11"] = InputKey::F11;
	controlsMap["F12"] = InputKey::F12;
	controlsMap["F10"] = InputKey::F10;
	controlsMap["1"] = InputKey::Num1;
	controlsMap["2"] = InputKey::Num2;
	controlsMap["3"] = InputKey::Num3;
	controlsMap["4"] = InputKey::Num4;
	controlsMap["5"] = InputKey::Num5;
	controlsMap["6"] = InputKey::Num6;
	controlsMap["7"] = InputKey::Num7;
	controlsMap["8"] = InputKey::Num8;
	controlsMap["9"] = InputKey::Num9;
	controlsMap["0"] = InputKey::Num0;
	controlsMap["A"] = InputKey::A;
	controlsMap["B"] = InputKey::B;
	controlsMap["C"] = InputKey::C;
	controlsMap["D"] = InputKey::D;
	controlsMap["E"] = InputKey::E;
	controlsMap["F"] = InputKey::F;
	controlsMap["G"] = InputKey::G;
	controlsMap["H"] = InputKey::H;
	controlsMap["I"] = InputKey::I;
	controlsMap["J"] = InputKey::J;
	controlsMap["K"] = InputKey::K;
	controlsMap["L"] = InputKey::L;
	controlsMap["M"] = InputKey::M;
	controlsMap["N"] = InputKey::N;
	controlsMap["O"] = InputKey::O;
	controlsMap["P"] = InputKey::P;
	controlsMap["Q"] = InputKey::Q;
	controlsMap["R"] = InputKey::R;
	controlsMap["S"] = InputKey::S;
	controlsMap["T"] = InputKey::T;
	controlsMap["U"] = InputKey::U;
	controlsMap["V"] = InputKey::V;
	controlsMap["W"] = InputKey::W;
	controlsMap["X"] = InputKey::X;
	controlsMap["Y"] = InputKey::Y;
	controlsMap["Z"] = InputKey::Z;
	controlsMap["NUMPAD1"] = InputKey::NumPad1;
	controlsMap["NUMPAD2"] = InputKey::NumPad2;
	controlsMap["NUMPAD3"] = InputKey::NumPad3;
	controlsMap["NUMPAD4"] = InputKey::NumPad4;
	controlsMap["NUMPAD5"] = InputKey::NumPad5;
	controlsMap["NUMPAD6"] = InputKey::NumPad6;
	controlsMap["NUMPAD7"] = InputKey::NumPad7;
	controlsMap["NUMPAD8"] = InputKey::NumPad8;
	controlsMap["NUMPAD9"] = InputKey::NumPad9;
	controlsMap["NUMPAD0"] = InputKey::NumPad0;
	controlsMap["MULTIPLY"] = InputKey::Multiply;
	controlsMap["ADD"] = InputKey::Add;
	controlsMap["MINUS"] = InputKey::Subtract;
	controlsMap["INSERT"] = InputKey::Insert;
	controlsMap["DIVIDE"] = InputKey::Divide;
	controlsMap["HOME"] = InputKey::Home;
	controlsMap["END"] = InputKey::End;
	controlsMap["PGUP"] = InputKey::PageUp;
	controlsMap["PGDOWN"] = InputKey::PageDown;
	controlsMap["PAGEUP"] = InputKey::PageUp;
	controlsMap["PAGEDOWN"] = InputKey::PageDown;
	controlsMap["UP"] = InputKey::Up;
	controlsMap["DOWN"] = InputKey::Down;
	controlsMap["LEFT"] = InputKey::Left;
	controlsMap["RIGHT"] = InputKey::Right;
	controlsMap["LCONTROL"] = InputKey::LControl;
	controlsMap["RCONTROL"] = InputKey::RControl;
	controlsMap["LCTRL"] = InputKey::LControl;
	controlsMap["RCTRL"] = InputKey::RControl;
	controlsMap["LSHIFT"] = InputKey::LShift;
	controlsMap["RSHIFT"] = InputKey::RShift;
	controlsMap["LALT"] = InputKey::LAlt;
	controlsMap["RALT"] = InputKey::RAlt;
	controlsMap["TAB"] = InputKey::Tab;
	controlsMap["SPACE"] = InputKey::Space;
	controlsMap["ENTER"] = InputKey::Enter;
	controlsMap["RETURN"] = InputKey::Enter;
	controlsMap["ESC"] = InputKey::Escape;
	controlsMap["ESCAPE"] = InputKey::Escape;
	controlsMap["PAUSE"] = InputKey::Pause;
	controlsMap["BACKSPACE"] = InputKey::BackSpace;
	controlsMap["DELETE"] = InputKey::Delete;
	controlsMap["DEL"] = InputKey::Delete;
	controlsMap["QUOTE"] = InputKey::Quote;
	controlsMap["BACKSLASH"] = InputKey::BackSlash;
	controlsMap["PERIOD"] = InputKey::Period;
	controlsMap["COMMA"] = InputKey::Comma;
	controlsMap["EQUALS"] = InputKey::Equal;
	controlsMap["LBRACKET"] = InputKey::LBracket;
	controlsMap["RBRACKET"] = InputKey::RBracket;
	controlsMap["SEMICOLON"] = InputKey::SemiColon;
	controlsMap["SLASH"] = InputKey::Slash;
	controlsMap["TILDE"] = InputKey::Tilde;
	controlsMap["LBUTTON"] = InputKey::LButton;
	controlsMap["MBUTTON"] = InputKey::MButton;
	controlsMap["RBUTTON"] = InputKey::RButton;
	controlsMap["LMOUSEBUTTON"] = InputKey::LButton;
	controlsMap["MMOUSEBUTTON"] = InputKey::MButton;
	controlsMap["RMOUSEBUTTON"] = InputKey::RButton;
	controlsMap["LEFTMOUSEBUTTON"] = InputKey::LButton;
	controlsMap["MIDDLEMOUSEBUTTON"] = InputKey::MButton;
	controlsMap["RIGHTMOUSEBUTTON"] = InputKey::RButton;

	JsonDocument doc;
	
	if (!doc.loadAndParse(inputMapFilename))
	{
		return false;
	}

	if (doc.hasErrors())
	{
		return false;
	}

	mouseSpeed = doc.getF32("mouseSpeed", 1.0f);
	joystickSpeed = doc.getF32("joystickSpeed", 1.0f);

	B_LOG_INFO("Mouse speed multiplier: " << mouseSpeed);
	B_LOG_INFO("Joystick speed multiplier: " << joystickSpeed);

	// read the action to key bindings
	JsonObject* jsnBindings = doc.getObject("bindings");

	if (jsnBindings)
	{
		for (size_t i = 0; i < jsnBindings->getMembers().size(); ++i)
		{
			InputAction action;
			auto pair = jsnBindings->getMembers().at(i);

			action.name = pair.key;

			auto actionJsonObj = pair.value->asObject();
			auto keyCombosJsonArray = actionJsonObj->getArray("keyCombos");

			B_LOG_DEBUG("Combo keys for action: '" << action.name << "'");

			for (size_t j = 0; j < keyCombosJsonArray->size(); j++)
			{
				String combo = keyCombosJsonArray->at(j)->asString();
				Array<String> keyNames;

				B_LOG_DEBUG("\tKeyCombo[" << (u32)j << "]");

				if (explodeString(combo, keyNames, " "))
				{
					InputMapControlCombination cbo;

					for (size_t k = 0; k < keyNames.size(); k++)
					{
						String keyName = keyNames[k];

						toUpperCase(keyName);
						auto iter = controlsMap.find(keyName);

						if (iter != controlsMap.end())
						{
							InputMapControl imc;

							imc.keyId = (u32)iter->value;
							imc.type = InputDeviceType::Keyboard;

							if (iter->value == InputKey::LButton
								|| iter->value == InputKey::MButton
								|| iter->value == InputKey::RButton)
							{
								imc.type = InputDeviceType::Mouse;
							}

							cbo.controls.append(imc);

							B_LOG_DEBUG("\t\tAdded key to combo:" << keyName);
						}
						else
						{
							B_LOG_ERROR("Input map combo control key not known: '" << keyNames[k] << "'");
						}
					}

					action.controlCombos.append(cbo);
				}
			}

			actions.append(action);
		}
	}

	// read the action combos
	JsonObject* jsnActionCombos = doc.getObject("actionCombos");

	if (jsnActionCombos)
	{
		for (size_t i = 0; i < jsnActionCombos->getMembers().size(); ++i)
		{
			InputActionCombo actionCombo;
			auto pair = jsnActionCombos->getMembers().at(i);

			actionCombo.name = pair.key;

			auto jsnActionCombo = pair.value->asObject();
			auto actionsString = jsnActionCombo->getString("actions");
			
			explodeString(actionsString, actionCombo.actions, " ");
			actionCombo.maxDelay = jsnActionCombo->getI32("maxDelay");
			actionCombos.append(actionCombo);

			B_LOG_DEBUG("Added action combo: '" << actionCombo.name << "' actions: '" << actionsString << "' maxDelay: " << actionCombo.maxDelay);
		}
	}

	return true;
}

bool InputMap::save(const String& inputMapFilename)
{
	Dictionary<InputKey, String> controlsMap;

	controlsMap[InputKey::F1] = "F1";
	controlsMap[InputKey::F2] = "F2";
	controlsMap[InputKey::F3] = "F3";
	controlsMap[InputKey::F4] = "F4";
	controlsMap[InputKey::F5] = "F5";
	controlsMap[InputKey::F6] = "F6";
	controlsMap[InputKey::F7] = "F7";
	controlsMap[InputKey::F8] = "F8";
	controlsMap[InputKey::F9] = "F9";
	controlsMap[InputKey::F10] = "F10";
	controlsMap[InputKey::F11] = "F11";
	controlsMap[InputKey::F12] = "F12";
	controlsMap[InputKey::F10] = "F10";
	controlsMap[InputKey::Num1] = "1";
	controlsMap[InputKey::Num2] = "2";
	controlsMap[InputKey::Num3] = "3";
	controlsMap[InputKey::Num4] = "4";
	controlsMap[InputKey::Num5] = "5";
	controlsMap[InputKey::Num6] = "6";
	controlsMap[InputKey::Num7] = "7";
	controlsMap[InputKey::Num8] = "8";
	controlsMap[InputKey::Num9] = "9";
	controlsMap[InputKey::Num0] = "0";
	controlsMap[InputKey::A] = "A";
	controlsMap[InputKey::B] = "B";
	controlsMap[InputKey::C] = "C";
	controlsMap[InputKey::D] = "D";
	controlsMap[InputKey::E] = "E";
	controlsMap[InputKey::F] = "F";
	controlsMap[InputKey::G] = "G";
	controlsMap[InputKey::H] = "H";
	controlsMap[InputKey::I] = "I";
	controlsMap[InputKey::J] = "J";
	controlsMap[InputKey::K] = "K";
	controlsMap[InputKey::L] = "L";
	controlsMap[InputKey::M] = "M";
	controlsMap[InputKey::N] = "N";
	controlsMap[InputKey::O] = "O";
	controlsMap[InputKey::P] = "P";
	controlsMap[InputKey::Q] = "Q";
	controlsMap[InputKey::R] = "R";
	controlsMap[InputKey::S] = "S";
	controlsMap[InputKey::T] = "T";
	controlsMap[InputKey::U] = "U";
	controlsMap[InputKey::V] = "V";
	controlsMap[InputKey::W] = "W";
	controlsMap[InputKey::X] = "X";
	controlsMap[InputKey::Y] = "Y";
	controlsMap[InputKey::Z] = "Z";
	controlsMap[InputKey::NumPad1] = "NumPad1";
	controlsMap[InputKey::NumPad2] = "NumPad2";
	controlsMap[InputKey::NumPad3] = "NumPad3";
	controlsMap[InputKey::NumPad4] = "NumPad4";
	controlsMap[InputKey::NumPad5] = "NumPad5";
	controlsMap[InputKey::NumPad6] = "NumPad6";
	controlsMap[InputKey::NumPad7] = "NumPad7";
	controlsMap[InputKey::NumPad8] = "NumPad8";
	controlsMap[InputKey::NumPad9] = "NumPad9";
	controlsMap[InputKey::NumPad0] = "NumPad0";
	controlsMap[InputKey::Multiply] = "Multiply";
	controlsMap[InputKey::Add] = "Add";
	controlsMap[InputKey::Subtract] = "Subtract";
	controlsMap[InputKey::Insert] = "Insert";
	controlsMap[InputKey::Divide] = "Divide";
	controlsMap[InputKey::Home] = "Home";
	controlsMap[InputKey::End] = "End";
	controlsMap[InputKey::PageUp] = "PageUp";
	controlsMap[InputKey::PageDown] = "PageDown";
	controlsMap[InputKey::Up] = "Up";
	controlsMap[InputKey::Down] = "Down";
	controlsMap[InputKey::Left] = "Left";
	controlsMap[InputKey::Right] = "Right";
	controlsMap[InputKey::LControl] = "LControl";
	controlsMap[InputKey::RControl] = "RControl";
	controlsMap[InputKey::LShift] = "LShift";
	controlsMap[InputKey::RShift] = "RShift";
	controlsMap[InputKey::LAlt] = "LAlt";
	controlsMap[InputKey::RAlt] = "RAlt";
	controlsMap[InputKey::Tab] = "Tab";
	controlsMap[InputKey::Space] = "Space";
	controlsMap[InputKey::Enter] = "Enter";
	controlsMap[InputKey::Escape] = "Escape";
	controlsMap[InputKey::Pause] = "Pause";
	controlsMap[InputKey::BackSpace] = "BackSpace";
	controlsMap[InputKey::Delete] = "Delete";
	controlsMap[InputKey::Quote] = "Quote";
	controlsMap[InputKey::BackSlash] = "BackSlash";
	controlsMap[InputKey::Period] = "Period";
	controlsMap[InputKey::Comma] = "Comma";
	controlsMap[InputKey::Equal] = "Equals";
	controlsMap[InputKey::LBracket] = "LBracket";
	controlsMap[InputKey::RBracket] = "RBracket";
	controlsMap[InputKey::SemiColon] = "SemiColon";
	controlsMap[InputKey::Slash] = "Slash";
	controlsMap[InputKey::Tilde] = "Tilde";
	controlsMap[InputKey::LButton] = "LButton";
	controlsMap[InputKey::MButton] = "MButton";
	controlsMap[InputKey::RButton] = "RButton";

	JsonDocument doc;

	doc.beginDocument();
	doc.addValue("mouseSpeed", mouseSpeed);
	doc.addValue("joystickSpeed", joystickSpeed);
	doc.beginObject("bindings");

	// write actions
	for (size_t i = 0; i < actions.size(); ++i)
	{
		auto& action = actions[i];
		
		doc.beginObject(action.name);
		doc.beginArray("keyCombos");

		for (size_t j = 0; j < action.controlCombos.size(); ++j)
		{
			auto& ctrlCombo = action.controlCombos[j];
			String comboStr;

			for (auto& ctrl : ctrlCombo.controls)
			{
				String nameOfCtrl = controlsMap[(InputKey)ctrl.keyId];

				if (nameOfCtrl.isEmpty())
				{
					continue;
				}

				comboStr += nameOfCtrl;
			}

			doc.addValue(comboStr);
		}

		doc.endArray();
		doc.endObject();
	}

	doc.endObject(); // end bindings
	doc.endDocument();
	auto options = JsonSaveOptions::sjsonOptions();
	doc.save(inputMapFilename, &options);
#ifdef _DEBUG
	doc.debug();
#endif

	return true;
}

}