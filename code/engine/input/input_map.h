#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "input/input.h"

namespace engine
{
using namespace base;

//! A class used for input mapping, holding the device type and a value for the key/control/knob
struct InputMapControl
{
	InputDeviceType type = InputDeviceType::None;
	u32 keyId = 0;
	bool down = false;
};

struct InputMapControlCombination
{
	Array<InputMapControl> controls;
};

//! An input action like "fire","left","right"
struct InputAction
{
	//! true if action is ON
	bool active = false;
	//! the action name
	String name;
	Array<InputMapControlCombination> controlCombos;
};

struct InputActionCombo
{
	String name;
	Array<String> actions;
	u32 maxDelay = 5; //!< max delay between actions, in milliseconds

	// used internally
	bool activationStarted = false;
	u32 lastActionActivatedTime = 0;
	size_t lastActionActivatedIndex = 0;
};

//! An input map stores the actions available in the game, can be loaded from a file
class E_API InputMap
{
public:
	InputMap();
	~InputMap();

	bool load(const String& inputMapFilename);
	bool save(const String& inputMapFilename);
	void free();
	//! must be called to prepare the actions, before any isAction() call, each frame or when an event occurs
	void update(f32 deltaTime);
	//! check if the action was triggered, by action name ex.: "fire"
	bool isAction(const String& name);
	void addAction(const String& name, const InputMapControlCombination& ctrlCombo);
	inline const Array<InputAction>& getActions() const { return actions; }
	bool isActionCombo(const String& name);
	void addActionCombo(const InputActionCombo& actionCombo);
	inline const Array<InputActionCombo>& getActionCombos() const { return actionCombos; }
	inline bool isAnyActionComboInitiated() const { return actionComboInitiated; }

public:
	f32 mouseSpeed = 1;
	f32 joystickSpeed = 1;

protected:
	bool isAction(u32 index);
	void processAction(f32 deltaTime, size_t index);
	InputAction* findAction(const String& name);
	InputActionCombo* findActionCombo(const String& name);

	Array<InputAction> actions;
	Array<InputActionCombo> actionCombos;
	bool actionComboInitiated = false;
};

}