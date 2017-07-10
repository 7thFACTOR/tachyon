#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/string.h"
#include "system/array.h"
#include "system/defines.h"
#include "system/types.h"
#include "system/rtti.h"
#include "render/color.h"
#include "math/vec3.h"
#include "math/quat.h"

namespace nytro
{
class FlowGraph;
class Animation;
class AnimationTrack;

//! Looping mode for animation tracks and whole animation
enum EAnimationLoop
{
	//! play once
	eAnimationLoop_None = 0,
	//! play normally, then rewind and play again and repeat
	eAnimationLoop_Repeat,
	//! play normally then inversed and repeat
	eAnimationLoop_PingPong
};

//! The track key interpolation mode
enum EAnimationInterpolation
{
	eAnimationInterpolation_Linear = 0,
	eAnimationInterpolation_Bezier,
	eAnimationInterpolation_TCB
};

//! An animation key value, sort of a variant for keys
struct N_API AnimationKeyValue
{
	//! Union of all possible values for a key value
	union
	{
		i8 m_int8;
		i16 m_int16;
		i32 m_int32;
		i64 m_int64;
		f32 m_float;
		f64 m_double;
		f32 m_vector[3];
		f32 m_quat[4];
		f32 m_color[4];
	};
};

//! A TCB spline animation key point
class N_API AnimationKey
{
public:
	//! Key flags, can be OR'ed
	enum EFlag
	{
		//! is key active
		eFlag_Active = N_BIT(0),
		//! is using m_easeIn and m_easeOut in the curve computations
		eFlag_UseEase = N_BIT(1)
	};

	AnimationKey();
	virtual ~AnimationKey();

	//! set the key value from a pointer to data, WARNING! the pointer must match the key's type
	void set(void* pData);
	//! interpolate this key with rKey at time aT (0.0f ..... 1.0f), into *pData,
	//! which must match the type of the keys
	void interpolate(EAnimationInterpolation aIpolType, const AnimationKey& rKey, f32 aTime, void* pData);

	AnimationKey& operator += (const AnimationKey& rAnimationKey);
	AnimationKey& operator -= (const AnimationKey& rAnimationKey);
	AnimationKey& operator *= (const AnimationKey& rAnimationKey);
	AnimationKey& operator /= (const AnimationKey& rAnimationKey);
	AnimationKey operator + (const AnimationKey& rAnimationKey) const;
	AnimationKey operator - (const AnimationKey& rAnimationKey) const;
	AnimationKey operator * (const AnimationKey& rAnimationKey) const;
	AnimationKey operator / (const AnimationKey& rAnimationKey) const;
	AnimationKey& operator += (f32 rValue);
	AnimationKey& operator -= (f32 rValue);
	AnimationKey& operator *= (f32 rValue);
	AnimationKey& operator /= (f32 rValue);
	AnimationKey operator + (f32 rValue) const;
	AnimationKey operator - (f32 rValue) const;
	AnimationKey operator * (f32 rValue) const;
	AnimationKey operator / (f32 rValue) const;

	//! animation key flags, OR'ed values of AnimationKeyFlagType
	u32 m_flags;
	//! the key's time on track (0..1) where 1 is the max length of the track
	f32 m_time;
	//! the key's type (of type AnimationTrack::EAnimationTrackType)
	u32 m_type;
	//! the TCB tension component of the key (0..1)
	f32 m_tension,
		//! the TCB continuity component of the key (0..1)
		m_continuity,
		//! the TCB bias component of the key (0..1)
		m_bias,
		//! ease in factor (0..1)
		m_easeIn,
		//! ease out factor (0..1)
		m_easeOut;
	//! the key's value
	AnimationKeyValue m_value,
		//! the computed TCB left tangent value
		m_leftTangent,
		//! the computed TCB right tangent value
		m_rightTangent;
};

//! An animation track controller modifies values on an animation track, for example the noise
//! controller
//! adds some noise to the interpolated key values
class N_API AnimationTrackController : public Rtti
{
public:
	//! Sets how the given controller value are merged with the current interpolated track
	//! keys value
	enum EOperand
	{
		//! add controller value to track value
		eOperand_Add,
		//! subtract controller value from track value
		eOperand_Subtract,
		//! multiply controller value with track value
		eOperand_Multiply,
		//! divide track value by controller value
		eOperand_Divide,
		//! replace track value with controller value
		eOperand_Replace
	};

	AnimationTrackController();
	virtual ~AnimationTrackController();

	//! \return the controlled track
	AnimationTrack* getTrack() const;
	//! \return the resulted key after calling controlValue() method
	const AnimationKey& getResultKey() const;
	//! \return the controller's operand
	EOperand getOperand() const;
	//! initialize the controller
	virtual void initialize();
	//! release the controller
	virtual void release();
	//! set the controlled track
	virtual void setTrack(AnimationTrack* pTrack);
	//! set the controller operand type
	virtual void setOperand(EOperand aValue);
	//! control a value pCurrentValue at aTime (0..1), and puts the result in m_resultKey,
	//! obtainable through getResultKey()
	virtual void controlValue(f32 aTime, void* pCurrentValue);

protected:
	//! the controlled / affected track
	AnimationTrack* m_pTrack;
	//! the current result key, computed in controlValue(...) by the user defined method
	AnimationKey m_resultKey;
	//! the operand used to merge the current track's value with the controller's value
	EOperand m_operand;
};

//! An animation track, full with animation key(frames), controlling a user variable,
//! class member, etc.
class N_API AnimationTrack
{
public:
	//! The track's keys data type
	enum EType
	{
		eType_Int8,
		eType_Int16,
		eType_Int32,
		eType_Int64,
		eType_Float,
		eType_Double,
		eType_Vec2,
		eType_Vec3,
		eType_Quat,
		eType_Color
	};

	//! The track's flags
	enum EFlags
	{
		eFlag_Active = N_BIT(0)
	};

	AnimationTrack();
	virtual ~AnimationTrack();

	const String& name() const;
	void setName(const char* pName);
	EType type() const;
	void setType(EType aType);
	//! \return the track's keys
	const Array<AnimationKey*>& keys() const;
	//! \return the track's key value controllers
	const Array<AnimationTrackController*>& controllers() const;
	//! \return the variable address which this track is controlling
	void* variable() const;
	//! \return the loop mode for this track
	EAnimationLoop loopMode() const;
	//! \return the track's speed
	f32 speed() const;
	//! \return the default key variable, used to store result of key interpolation/animation if
	//! track is not linked to user variable
	const AnimationKey& defaultKeyVariable() const;
	u32 trackFlags() const;
	void setTrackFlags(u32 aFlags);
	//! set the current variable (address) which this track will control, the animated/interpolated
	//! value will be set there
	void setVariable(void* pValue);
	//! set the track's looping mode
	void setLoopMode(EAnimationLoop aValue);
	//! set the current speed of the track (f32 value between {0..1}, 1 is normal time, >1 will be
	//! faster, <1 will be slower)
	void setSpeed(f32 aValue);
	//! set interpolation type
	void setInterpolationType(EAnimationInterpolation aType)
	{
		m_interpolationType = aType;
	}

	//! \return interpolation type
	EAnimationInterpolation interpolationType() const
	{
		return m_interpolationType;
	}

	//! process the track, its keys, values, etc., with the final result put in the variable
	//! m_pVariable, given the {0..1} range time f32
	virtual void process(f32 aTime);
	//! bind the track to a user variable to be modified by the track
	//TODO: lets not bind to variable, just return the interpolated value at time T
	virtual void bindToVariable(EType aType, void* pData);
	//! add a new track key
	AnimationKey* addKey(f32 aTime, void* pData);
	//! compute track keys tangent automatically
	virtual void computeTangents();

	//! add a new float32 key (for all the addKey functions, if the type differs from the track's
	//! type, it will not be added)
	AnimationKey* addKey(f32 aTime, f32 aData);
	//! add a new float64 key
	AnimationKey* addKey(f32 aTime, const double& aData);
	//! add a new int64 key
	AnimationKey* addKey(f32 aTime, const i64& aData);
	//! add a new int32 key
	AnimationKey* addKey(f32 aTime, i32 aData);
	//! add a new int16 key
	AnimationKey* addKey(f32 aTime, i16 aData);
	//! add a new int8 key
	AnimationKey* addKey(f32 aTime, i8 aData);
	//! add a new 3D vector key
	AnimationKey* addKey(f32 aTime, const Vec3& aData);
	//! add a new quaternion/rotation key
	AnimationKey* addKey(f32 aTime, const Quat& aData);
	//! add a new color key
	AnimationKey* addKey(f32 aTime, const Color& aData);

protected:
	String m_name;
	EType m_type;
	//! animation tracks
	u32 m_trackFlags;
	//! the track's keys
	Array<AnimationKey*> m_keys;
	//! the curve interpolation type
	EAnimationInterpolation m_interpolationType;
	//! the default resulted key value, if no user variable
	//! was specified when creating track with Animation::addTrack
	AnimationKey m_defaultKeyVariable;
	//! the track's controllers
	Array<AnimationTrackController*> m_controllers;
	//! the current controlled variable
	void* m_pVariable;
	//! the track's looping mode
	EAnimationLoop m_loopMode;
	//! the track's time speed factor (f32 value between {0..1},
	//! 1 is normal time, >1 will be faster, <1 will be slower)
	f32 m_speed;
	//! the animation direction 1 = normal (forward), -1 = reversed, backwards
	f32 m_direction;
};

//! An animation event, triggered when the current animation time reaches it
class N_API AnimationEvent
{
public:
	AnimationEvent();
	virtual ~AnimationEvent();

	//! set the event's time in seconds
	void setTime(f32 aValue);
	//! set the event active or not
	void setActive(bool bActive);
	//! set if this event was triggered or not
	void setTriggered(bool bTrigger);

	//! \return the event's time in seconds
	f32 time() const;
	//! \return true if this event is active
	bool isActive() const;
	//! \return true if the event was triggered already
	bool isTriggered() const;

	//! trigger the event, user defined code
	virtual void onTrigger();

protected:
	f32 m_time;
	bool m_bActive, m_bTriggered;
};

//! An animation object holds several animation tracks in one place, also can play them all at once
class N_API Animation
{
public:
	//! The animation object flags
	enum EFlags
	{
		eFlag_AutoDuration = N_BIT(0)
	};

	Animation();
	virtual ~Animation();

	//! \return total animation duration, if eAnimationFlag_AutoDuration is set in the flags,
	//! then the total animation time is computed from the tracks
	f32 duration() const;
	//! \return the current animation time
	f32 time() const;
	//! \return the time scaling, 1.0f is normal time
	f32 timeScale() const;
	//! \return the animation tracks
	const Array<AnimationTrack*>& tracks() const;
	//! \return the loop mode
	EAnimationLoop loopMode() const;

	//! set the duration of the animation in seconds
	void setDuration(f32 aValue);
	//! set the current animation time, in {0..1} range
	void setTime(f32 aValue);
	//! set the current animation time scale factor, in {0..1} range (f32 value between {0..1},
	//! 1 is normal time, >1 will be faster, <1 will be slower)
	void setTimeScale(f32 aValue);
	//! set the current animation looping mode
	void setLoopMode(EAnimationLoop aValue);
	u32 animationFlags() const;
	void setAnimationFlags(u32 aFlags);

	//! process the animation, given the time delta ( usually the getDeltaTime() )
	virtual void process(f32 aTimeDelta);
	//! add a new named track, for pData user variable, of type aType
	virtual AnimationTrack* addTrack(
								const char* pName,
								AnimationTrack::EType aType,
								void* pData);
	//! start playing the animation
	virtual void play();
	//! find an animation track by name
	virtual AnimationTrack* findTrackByName(const char* pName) const;
	//! free the animation data
	void freeData();
	const String& name() const;
	void setName(const char* pName);

protected:
	String m_name;
	//! animation flags
	u32 m_animationFlags;
	//! the duration in seconds of the animation
	f32 m_duration,
		//! the current time of the animation {0..1}
		m_time;
	//! the current time scale factor of the animation
	f32 m_timeScale,
		//! the current direction of the animation, 1 = normal, -1 = reversed
		m_direction;
	//! the animation looping mode
	EAnimationLoop m_loopMode;
	//! the animation's tracks
	Array<AnimationTrack*> m_tracks;
	//! the animation events
	Array<AnimationEvent*> m_events;
};
}