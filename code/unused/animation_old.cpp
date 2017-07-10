#include <string.h>
#include "animation.h"
#include "system/assert.h"
#include "system/logger.h"

namespace nytro
{
AnimationKey::AnimationKey()
{
	m_flags = AnimationKey::eFlag_Active | AnimationKey::eFlag_UseEase;
	m_value.m_vector[0] = m_value.m_vector[1] = m_value.m_vector[2] = m_value.m_vector[3] = 0;
	m_time = 0;
	m_tension = 0.5f;
	m_continuity = 0.5f;
	m_bias = 0.5f;
}

AnimationKey::~AnimationKey()
{
}

//---

void AnimationKey::set(void* pData)
{
	N_ASSERT(pData);

	switch (m_type)
	{
	case AnimationTrack::eType_Int8:
		m_value.m_int8 = *(i8*) pData;
		break;
	case AnimationTrack::eType_Int16:
		m_value.m_int16 = *(i16*) pData;
		break;
	case AnimationTrack::eType_Int32:
		m_value.m_int32 = *(i32*) pData;
		break;
	case AnimationTrack::eType_Int64:
		m_value.m_int64 = *(i64*) pData;
		break;
	case AnimationTrack::eType_Float:
		m_value.m_float = *(f32*) pData;
		break;
	case AnimationTrack::eType_Double:
		m_value.m_double = *(double*) pData;
		break;
	case AnimationTrack::eType_Vec2:
		memcpy(m_value.m_vector, pData, sizeof(f32)*2);
		break;
	case AnimationTrack::eType_Vec3:
		memcpy(m_value.m_vector, pData, sizeof(Vec3));
		break;
	case AnimationTrack::eType_Quat:
		memcpy(m_value.m_quat, pData, sizeof(Quat));
		break;
	case AnimationTrack::eType_Color:
		memcpy(m_value.m_color, pData, sizeof(Color));
		break;
	}
}

void AnimationKey::interpolate(
	EAnimationInterpolation aIpolType,
	const AnimationKey& rKey,
	f32 aTime,
	void* pData)
{
	N_ASSERT(pData);

	switch (aIpolType)
	{
	case eAnimationInterpolation_Linear:
		{
			AnimationKey tmpkey;
			Quat q, q1, q2;
			const AnimationKey *k1 = this;
			const AnimationKey *k2 = &rKey;

			switch (m_type)
			{
			case AnimationTrack::eType_Int8:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					*(i8*)pData = tmpkey.m_value.m_int8;
					break;
				}

			case AnimationTrack::eType_Int16:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					*(i16*)pData = tmpkey.m_value.m_int16;
					break;
				}

			case AnimationTrack::eType_Int32:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					*(i32*)pData = tmpkey.m_value.m_int32;
					break;
				}

			case AnimationTrack::eType_Int64:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					*(i64*)pData = tmpkey.m_value.m_int64;
					break;
				}

			case AnimationTrack::eType_Float:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					*(f32*)pData = tmpkey.m_value.m_float;
					break;
				}

			case AnimationTrack::eType_Double:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					*(double*)pData = tmpkey.m_value.m_double;
					break;
				}

			case AnimationTrack::eType_Vec2:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					((Vec3*)pData)->x = tmpkey.m_value.m_vector[0];
					((Vec3*)pData)->y = tmpkey.m_value.m_vector[1];
					break;
				}

			case AnimationTrack::eType_Vec3:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					((Vec3*)pData)->x = tmpkey.m_value.m_vector[0];
					((Vec3*)pData)->y = tmpkey.m_value.m_vector[1];
					((Vec3*)pData)->z = tmpkey.m_value.m_vector[2];
					break;
				}

			case AnimationTrack::eType_Quat:
				{
					q1.x = k1->m_value.m_quat[0];
					q1.y = k1->m_value.m_quat[1];
					q1.z = k1->m_value.m_quat[2];
					q1.w = k1->m_value.m_quat[3];

					q2.x = k2->m_value.m_quat[0];
					q2.y = k2->m_value.m_quat[1];
					q2.z = k2->m_value.m_quat[2];
					q2.w = k2->m_value.m_quat[3];

					q.slerp(q1, q2, aTime);
					*((Quat*)pData) = q;
					break;
				}

			case AnimationTrack::eType_Color:
				{
					tmpkey = *k1 + (*k2 - *k1) * aTime;
					((Color*)pData)->r = tmpkey.m_value.m_color[0];
					((Color*)pData)->g = tmpkey.m_value.m_color[1];
					((Color*)pData)->b = tmpkey.m_value.m_color[2];
					((Color*)pData)->a = tmpkey.m_value.m_color[3];
					break;
				}
			}
			break;
		}

	case eAnimationInterpolation_Bezier:
	case eAnimationInterpolation_TCB:
		{
			f32 t2, t3, t4, t5, ta, tb, tc, td;
			AnimationKey  tmpkey, k1tb, k2ta;
			Quat q, q1, q2;

			t2 = aTime * aTime;
			t3 = aTime * t2;
			t4 = 3.0f * t2;
			t5 = 2.0f * t3;
			ta = t5 - t4 + 1.0f;
			tb = t3 - 2.0f * t2 + aTime;
			tc = -t5 + t4;
			td = t3 - t2;

			const AnimationKey *k1 = this;
			const AnimationKey *k2 = &rKey;

			k1tb = *k1;
			k1tb.m_value = k1->m_rightTangent;
			k2ta = *k2;
			k2ta.m_value = k2->m_leftTangent;

			tmpkey = *k1 * ta + k1tb * tb + *k2 * tc + k2ta * td;

			switch (m_type)
			{
			case AnimationTrack::eType_Int8:
				*(i8*)pData = tmpkey.m_value.m_int8;
				break;

			case AnimationTrack::eType_Int16:
				*(i16*)pData = tmpkey.m_value.m_int16;
				break;

			case AnimationTrack::eType_Int32:
				*(i32*)pData = tmpkey.m_value.m_int32;
				break;

			case AnimationTrack::eType_Int64:
				*(i64*)pData = tmpkey.m_value.m_int64;
				break;

			case AnimationTrack::eType_Float:
				*(f32*)pData = tmpkey.m_value.m_float;
				break;

			case AnimationTrack::eType_Double:
				*(double*)pData = tmpkey.m_value.m_double;
				break;

			case AnimationTrack::eType_Vec2:
				((Vec3*)pData)->x = tmpkey.m_value.m_vector[0];
				((Vec3*)pData)->y = tmpkey.m_value.m_vector[1];
				break;

			case AnimationTrack::eType_Vec3:
				((Vec3*)pData)->x = tmpkey.m_value.m_vector[0];
				((Vec3*)pData)->y = tmpkey.m_value.m_vector[1];
				((Vec3*)pData)->z = tmpkey.m_value.m_vector[2];
				break;

			case AnimationTrack::eType_Quat:
				q1.x = k1->m_value.m_quat[0];
				q1.y = k1->m_value.m_quat[1];
				q1.z = k1->m_value.m_quat[2];
				q1.w = k1->m_value.m_quat[3];

				q2.x = k2->m_value.m_quat[0];
				q2.y = k2->m_value.m_quat[1];
				q2.z = k2->m_value.m_quat[2];
				q2.w = k2->m_value.m_quat[3];

				q.slerp(q1, q2, aTime);
				*((Quat*)pData) = q;
				break;

			case AnimationTrack::eType_Color:
				((Color*)pData)->r = tmpkey.m_value.m_color[0];
				((Color*)pData)->g = tmpkey.m_value.m_color[1];
				((Color*)pData)->b = tmpkey.m_value.m_color[2];
				((Color*)pData)->a = tmpkey.m_value.m_color[3];
				break;
			}

			break;
		}
	}
}

AnimationKey& AnimationKey::operator += (const AnimationKey& rKey)
{
	Vec3 vec;
	Quat quat1, quat2;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 += rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 += rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 += rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 += rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float += rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double += rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] += rKey.m_value.m_vector[0];
		m_value.m_vector[1] += rKey.m_value.m_vector[1];
		m_value.m_vector[2] += rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] += rKey.m_value.m_vector[0];
		m_value.m_vector[1] += rKey.m_value.m_vector[1];
		m_value.m_vector[2] += rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 += quat2;

		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		m_value.m_vector[0] += rKey.m_value.m_vector[0];
		m_value.m_vector[1] += rKey.m_value.m_vector[1];
		m_value.m_vector[2] += rKey.m_value.m_vector[2];
		m_value.m_vector[3] += rKey.m_value.m_vector[3];
		break;
	}

	return *this;
}

AnimationKey& AnimationKey::operator -= (const AnimationKey& rKey)
{
	Vec3 vec;
	Quat quat1, quat2;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 -= rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 -= rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 -= rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 -= rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float -= rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double -= rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] -= rKey.m_value.m_vector[0];
		m_value.m_vector[1] -= rKey.m_value.m_vector[1];
		m_value.m_vector[2] -= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] -= rKey.m_value.m_vector[0];
		m_value.m_vector[1] -= rKey.m_value.m_vector[1];
		m_value.m_vector[2] -= rKey.m_value.m_vector[2];
		break;

	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 -= quat2;

		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;

	case AnimationTrack::eType_Color :
		m_value.m_vector[0] -= rKey.m_value.m_vector[0];
		m_value.m_vector[1] -= rKey.m_value.m_vector[1];
		m_value.m_vector[2] -= rKey.m_value.m_vector[2];
		m_value.m_vector[3] -= rKey.m_value.m_vector[3];
		break;
	}

	return *this;
}

AnimationKey& AnimationKey::operator *= (const AnimationKey& rKey)
{
	Vec3 vec;
	Quat quat1, quat2;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 *= rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 *= rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 *= rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 *= rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float *= rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double *= rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] *= rKey.m_value.m_vector[0];
		m_value.m_vector[1] *= rKey.m_value.m_vector[1];
		m_value.m_vector[2] *= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] *= rKey.m_value.m_vector[0];
		m_value.m_vector[1] *= rKey.m_value.m_vector[1];
		m_value.m_vector[2] *= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 *= quat2;

		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		m_value.m_vector[0] *= rKey.m_value.m_vector[0];
		m_value.m_vector[1] *= rKey.m_value.m_vector[1];
		m_value.m_vector[2] *= rKey.m_value.m_vector[2];
		m_value.m_vector[3] *= rKey.m_value.m_vector[3];
		break;
	}

	return *this;
}

AnimationKey& AnimationKey::operator /= (const AnimationKey& rKey)
{
	Vec3 vec;
	Quat quat1, quat2;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 /= rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 /= rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 /= rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 /= rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float /= rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double /= rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] /= rKey.m_value.m_vector[0];
		m_value.m_vector[1] /= rKey.m_value.m_vector[1];
		m_value.m_vector[2] /= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] /= rKey.m_value.m_vector[0];
		m_value.m_vector[1] /= rKey.m_value.m_vector[1];
		m_value.m_vector[2] /= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 /= quat2;

		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;

	case AnimationTrack::eType_Color :
		m_value.m_vector[0] /= rKey.m_value.m_vector[0];
		m_value.m_vector[1] /= rKey.m_value.m_vector[1];
		m_value.m_vector[2] /= rKey.m_value.m_vector[2];
		m_value.m_vector[3] /= rKey.m_value.m_vector[3];
		break;
	}

	return *this;
}

AnimationKey AnimationKey::operator + (const AnimationKey& rKey) const
{
	Vec3 vec;
	Quat quat1, quat2;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 += rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 += rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 += rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 += rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float += rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double += rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] += rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] += rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] += rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] += rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] += rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] += rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 += quat2;

		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] += rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] += rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] += rKey.m_value.m_vector[2];
		key.m_value.m_vector[3] += rKey.m_value.m_vector[3];
		break;
	}

	return key;
}

AnimationKey AnimationKey::operator - (const AnimationKey& rKey) const
{
	Vec3 vec;
	Quat quat1, quat2;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 -= rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 -= rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 -= rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 -= rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float -= rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double -= rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] -= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] -= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] -= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] -= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] -= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] -= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 -= quat2;

		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] -= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] -= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] -= rKey.m_value.m_vector[2];
		key.m_value.m_vector[3] -= rKey.m_value.m_vector[3];
		break;
	}

	return key;
}

AnimationKey AnimationKey::operator * (const AnimationKey& rKey) const
{
	Vec3 vec;
	Quat quat1, quat2;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 *= rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 *= rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 *= rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 *= rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float *= rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double *= rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] *= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] *= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] *= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] *= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] *= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] *= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 *= quat2;

		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] *= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] *= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] *= rKey.m_value.m_vector[2];
		key.m_value.m_vector[3] *= rKey.m_value.m_vector[3];
		break;
	}

	return key;
}

AnimationKey AnimationKey::operator / (const AnimationKey& rKey) const
{
	Vec3 vec;
	Quat quat1, quat2;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 /= rKey.m_value.m_int8;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 /= rKey.m_value.m_int16;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 /= rKey.m_value.m_int32;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 /= rKey.m_value.m_int64;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float /= rKey.m_value.m_float;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double /= rKey.m_value.m_double;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] /= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] /= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] /= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] /= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] /= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] /= rKey.m_value.m_vector[2];
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];

		quat2.x = rKey.m_value.m_quat[0];
		quat2.y = rKey.m_value.m_quat[1];
		quat2.z = rKey.m_value.m_quat[2];
		quat2.w = rKey.m_value.m_quat[3];
		quat1 /= quat2;

		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] /= rKey.m_value.m_vector[0];
		key.m_value.m_vector[1] /= rKey.m_value.m_vector[1];
		key.m_value.m_vector[2] /= rKey.m_value.m_vector[2];
		key.m_value.m_vector[3] /= rKey.m_value.m_vector[3];
		break;
	}

	return key;
}

AnimationKey& AnimationKey::operator += (f32 aValue)
{
	Vec3 vec;
	Quat quat1;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 += aValue;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 += aValue;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 += aValue;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 += aValue;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float += aValue;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double += aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] += aValue;
		m_value.m_vector[1] += aValue;
		m_value.m_vector[2] += aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] += aValue;
		m_value.m_vector[1] += aValue;
		m_value.m_vector[2] += aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];
		quat1 += aValue;
		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		m_value.m_vector[0] += aValue;
		m_value.m_vector[1] += aValue;
		m_value.m_vector[2] += aValue;
		m_value.m_vector[3] += aValue;
		break;
	}

	return *this;
}

AnimationKey& AnimationKey::operator -= (f32 aValue)
{
	Vec3 vec;
	Quat quat1;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 -= aValue;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 -= aValue;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 -= aValue;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 -= aValue;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float -= aValue;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double -= aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] -= aValue;
		m_value.m_vector[1] -= aValue;
		m_value.m_vector[2] -= aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] -= aValue;
		m_value.m_vector[1] -= aValue;
		m_value.m_vector[2] -= aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];
		quat1 -= aValue;
		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		m_value.m_vector[0] -= aValue;
		m_value.m_vector[1] -= aValue;
		m_value.m_vector[2] -= aValue;
		m_value.m_vector[3] -= aValue;
		break;
	}

	return *this;
}

AnimationKey& AnimationKey::operator *= (f32 aValue)
{
	Vec3 vec;
	Quat quat1;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 *= aValue;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 *= aValue;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 *= aValue;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 *= aValue;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float *= aValue;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double *= aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] *= aValue;
		m_value.m_vector[1] *= aValue;
		m_value.m_vector[2] *= aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] *= aValue;
		m_value.m_vector[1] *= aValue;
		m_value.m_vector[2] *= aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];
		quat1 *= aValue;
		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		m_value.m_vector[0] *= aValue;
		m_value.m_vector[1] *= aValue;
		m_value.m_vector[2] *= aValue;
		m_value.m_vector[3] *= aValue;
		break;
	}

	return *this;
}

AnimationKey& AnimationKey::operator /= (f32 aValue)
{
	Vec3 vec;
	Quat quat1;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		m_value.m_int8 /= aValue;
		break;
	case AnimationTrack::eType_Int16 :
		m_value.m_int16 /= aValue;
		break;
	case AnimationTrack::eType_Int32 :
		m_value.m_int32 /= aValue;
		break;
	case AnimationTrack::eType_Int64 :
		m_value.m_int64 /= aValue;
		break;
	case AnimationTrack::eType_Float :
		m_value.m_float /= aValue;
		break;
	case AnimationTrack::eType_Double :
		m_value.m_double /= aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		m_value.m_vector[0] /= aValue;
		m_value.m_vector[1] /= aValue;
		m_value.m_vector[2] /= aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		m_value.m_vector[0] /= aValue;
		m_value.m_vector[1] /= aValue;
		m_value.m_vector[2] /= aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = m_value.m_quat[0];
		quat1.y = m_value.m_quat[1];
		quat1.z = m_value.m_quat[2];
		quat1.w = m_value.m_quat[3];
		quat1 /= aValue;
		m_value.m_quat[0] = quat1.x;
		m_value.m_quat[1] = quat1.y;
		m_value.m_quat[2] = quat1.z;
		m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		m_value.m_vector[0] /= aValue;
		m_value.m_vector[1] /= aValue;
		m_value.m_vector[2] /= aValue;
		m_value.m_vector[3] /= aValue;
		break;
	}

	return *this;
}

AnimationKey AnimationKey::operator + (f32 aValue) const
{
	Vec3 vec;
	Quat quat1;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 += aValue;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 += aValue;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 += aValue;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 += aValue;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float += aValue;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double += aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] += aValue;
		key.m_value.m_vector[1] += aValue;
		key.m_value.m_vector[2] += aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] += aValue;
		key.m_value.m_vector[1] += aValue;
		key.m_value.m_vector[2] += aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];
		quat1 += aValue;
		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] += aValue;
		key.m_value.m_vector[1] += aValue;
		key.m_value.m_vector[2] += aValue;
		key.m_value.m_vector[3] += aValue;
		break;
	}

	return key;
}

AnimationKey AnimationKey::operator - (f32 aValue) const
{
	Vec3 vec;
	Quat quat1;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 -= aValue;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 -= aValue;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 -= aValue;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 -= aValue;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float -= aValue;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double -= aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] -= aValue;
		key.m_value.m_vector[1] -= aValue;
		key.m_value.m_vector[2] -= aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] -= aValue;
		key.m_value.m_vector[1] -= aValue;
		key.m_value.m_vector[2] -= aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];
		quat1 -= aValue;
		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] -= aValue;
		key.m_value.m_vector[1] -= aValue;
		key.m_value.m_vector[2] -= aValue;
		key.m_value.m_vector[3] -= aValue;
		break;
	}

	return key;
}

AnimationKey AnimationKey::operator * (f32 aValue) const
{
	Vec3 vec;
	Quat quat1;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 *= aValue;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 *= aValue;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 *= aValue;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 *= aValue;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float *= aValue;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double *= aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] *= aValue;
		key.m_value.m_vector[1] *= aValue;
		key.m_value.m_vector[2] *= aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] *= aValue;
		key.m_value.m_vector[1] *= aValue;
		key.m_value.m_vector[2] *= aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];
		quat1 *= aValue;
		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] *= aValue;
		key.m_value.m_vector[1] *= aValue;
		key.m_value.m_vector[2] *= aValue;
		key.m_value.m_vector[3] *= aValue;
		break;
	}

	return key;
}

AnimationKey AnimationKey::operator / (f32 aValue) const
{
	Vec3 vec;
	Quat quat1;
	AnimationKey key;

	key = *this;

	switch (m_type)
	{
	case AnimationTrack::eType_Int8 :
		key.m_value.m_int8 /= aValue;
		break;
	case AnimationTrack::eType_Int16 :
		key.m_value.m_int16 /= aValue;
		break;
	case AnimationTrack::eType_Int32 :
		key.m_value.m_int32 /= aValue;
		break;
	case AnimationTrack::eType_Int64 :
		key.m_value.m_int64 /= aValue;
		break;
	case AnimationTrack::eType_Float :
		key.m_value.m_float /= aValue;
		break;
	case AnimationTrack::eType_Double :
		key.m_value.m_double /= aValue;
		break;
	case AnimationTrack::eType_Vec2 :
		key.m_value.m_vector[0] /= aValue;
		key.m_value.m_vector[1] /= aValue;
		key.m_value.m_vector[2] /= aValue;
		break;
	case AnimationTrack::eType_Vec3 :
		key.m_value.m_vector[0] /= aValue;
		key.m_value.m_vector[1] /= aValue;
		key.m_value.m_vector[2] /= aValue;
		break;
	case AnimationTrack::eType_Quat :
		quat1.x = key.m_value.m_quat[0];
		quat1.y = key.m_value.m_quat[1];
		quat1.z = key.m_value.m_quat[2];
		quat1.w = key.m_value.m_quat[3];
		quat1 /= aValue;
		key.m_value.m_quat[0] = quat1.x;
		key.m_value.m_quat[1] = quat1.y;
		key.m_value.m_quat[2] = quat1.z;
		key.m_value.m_quat[3] = quat1.w;
		break;
	case AnimationTrack::eType_Color :
		key.m_value.m_vector[0] /= aValue;
		key.m_value.m_vector[1] /= aValue;
		key.m_value.m_vector[2] /= aValue;
		key.m_value.m_vector[3] /= aValue;
		break;
	}

	return key;
}

//---

AnimationTrackController::AnimationTrackController()
{
	m_operand = eOperand_Replace;
	m_pTrack = nullptr;
}

AnimationTrackController::~AnimationTrackController()
{
}

AnimationTrack* AnimationTrackController::getTrack() const
{
	return m_pTrack;
}

const AnimationKey& AnimationTrackController::getResultKey() const
{
	return m_resultKey;
}

AnimationTrackController::EOperand AnimationTrackController::getOperand() const
{
	return m_operand;
}

void AnimationTrackController::setOperand(EOperand aValue)
{
	m_operand = aValue;
}

void AnimationTrackController::setTrack(AnimationTrack* pTrack)
{
	m_pTrack = pTrack;
}

void AnimationTrackController::initialize()
{
}

void AnimationTrackController::release()
{
}

void AnimationTrackController::controlValue(f32 aTime, void* pCurrentValue)
{
}

//---

AnimationTrack::AnimationTrack()
{
	m_pVariable = nullptr;
	m_loopMode = eAnimationLoop_None;
	m_speed = m_direction = 1.0f;
	m_interpolationType = eAnimationInterpolation_TCB;
	m_trackFlags = AnimationTrack::eFlag_Active;
}

AnimationTrack::~AnimationTrack()
{
}

const String& AnimationTrack::name() const
{
	return m_name;
}

void AnimationTrack::setName(const char* pName)
{
	m_name = pName;
}

AnimationTrack::EType AnimationTrack::type() const
{
	return m_type;
}

void AnimationTrack::setType(EType aType)
{
	m_type = aType;
}

const Array<AnimationKey*>& AnimationTrack::keys() const
{
	return m_keys;
}

const Array<AnimationTrackController*>& AnimationTrack::controllers() const
{
	return m_controllers;
}

void* AnimationTrack::variable() const
{
	return m_pVariable;
}

EAnimationLoop AnimationTrack::loopMode() const
{
	return m_loopMode;
}

f32 AnimationTrack::speed() const
{
	return m_speed;
}

void AnimationTrack::setVariable(void* pValue)
{
	m_pVariable = pValue;
}

void AnimationTrack::setLoopMode(EAnimationLoop aValue)
{
	m_loopMode = aValue;
}

void AnimationTrack::setSpeed(f32 aValue)
{
	m_speed = aValue;
}

void AnimationTrack::process(f32 aTime)
{
	if (!m_pVariable)
	{
		return;
	}

	aTime *= m_speed;

	if (!(m_trackFlags & eFlag_Active))
	{
		aTime = 0;
	}

	if (m_keys.count() <= 1)
	{
		return;
	}

	if (aTime > m_keys[m_keys.count() - 1]->m_time)
	{
		return;
	}

	AnimationKey *key1, *key2;
	key1 = key2 = nullptr;

	// default first key
	key1 = m_keys[0];

	// search first key
	for (size_t i = 0, iCount = m_keys.count(); i < iCount; ++i)
	{
		if (m_keys[i]->m_time > key1->m_time
			&& m_keys[i]->m_time <= aTime)
		{
			key1 = m_keys[i];
		}
	}

	// default second key
	key2 = m_keys[m_keys.count() - 1];

	// search for the second key
	for (size_t i = 0, iCount = m_keys.count(); i < iCount; ++i)
	{
		if (m_keys[i]->m_time < key2->m_time
			&& m_keys[i]->m_time >= aTime
			&& key1->m_time < m_keys[i]->m_time)
		{
			key2 = m_keys[i];
		}
	}

	// if one of the keys not found, return
	if (!key1 || !key2)
	{
		return;
	}

	// compute the local time between the two keys
	f32 localTime = (f32)(aTime - key1->m_time) / (key2->m_time - key1->m_time);

	// interpolate the keys putting the result into our variable
	key1->interpolate(m_interpolationType, *key2, localTime, m_pVariable);

	Quat q2;
	Color color;

	// now apply the controllers
	for (size_t i = 0, iCount = m_controllers.count(); i < iCount; ++i)
	{
		AnimationTrackController *c = m_controllers[i];

		// control the value at time localTime
		c->controlValue(localTime, m_pVariable);

		switch (m_type)
		{
		case AnimationTrack::eType_Float:
			{
				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*(f32*) m_pVariable = c->getResultKey().m_value.m_float;
					break;
				case AnimationTrackController::eOperand_Add:
					*(f32*) m_pVariable += c->getResultKey().m_value.m_float;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*(f32*) m_pVariable -= c->getResultKey().m_value.m_float;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*(f32*) m_pVariable *= c->getResultKey().m_value.m_float;
					break;
				case AnimationTrackController::eOperand_Divide:
					*(f32*) m_pVariable /= c->getResultKey().m_value.m_float;
					break;
				}
				break;
			}

		case AnimationTrack::eType_Double:
			{
				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*(double*) m_pVariable = c->getResultKey().m_value.m_double;
					break;
				case AnimationTrackController::eOperand_Add:
					*(double*) m_pVariable += c->getResultKey().m_value.m_double;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*(double*) m_pVariable -= c->getResultKey().m_value.m_double;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*(double*) m_pVariable *= c->getResultKey().m_value.m_double;
					break;
				case AnimationTrackController::eOperand_Divide:
					*(double*) m_pVariable /= c->getResultKey().m_value.m_double;
					break;
				}
				break;
			}

		case AnimationTrack::eType_Int8:
			{
				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*(i8*) m_pVariable = c->getResultKey().m_value.m_int8;
					break;
				case AnimationTrackController::eOperand_Add:
					*(i8*) m_pVariable += c->getResultKey().m_value.m_int8;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*(i8*) m_pVariable -= c->getResultKey().m_value.m_int8;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*(i8*) m_pVariable *= c->getResultKey().m_value.m_int8;
					break;
				case AnimationTrackController::eOperand_Divide:
					*(i8*) m_pVariable /= c->getResultKey().m_value.m_int8;
					break;
				}
				break;
			}

		case AnimationTrack::eType_Int16:
			{
				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*(i16*) m_pVariable = c->getResultKey().m_value.m_int16;
					break;
				case AnimationTrackController::eOperand_Add:
					*(i16*) m_pVariable += c->getResultKey().m_value.m_int16;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*(i16*) m_pVariable -= c->getResultKey().m_value.m_int16;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*(i16*) m_pVariable *= c->getResultKey().m_value.m_int16;
					break;
				case AnimationTrackController::eOperand_Divide:
					*(i16*) m_pVariable /= c->getResultKey().m_value.m_int16;
					break;
				}
				break;
			}

		case AnimationTrack::eType_Int32:
			{
				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*(i32*) m_pVariable = c->getResultKey().m_value.m_int32;
					break;
				case AnimationTrackController::eOperand_Add:
					*(i32*) m_pVariable += c->getResultKey().m_value.m_int32;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*(i32*) m_pVariable -= c->getResultKey().m_value.m_int32;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*(i32*) m_pVariable *= c->getResultKey().m_value.m_int32;
					break;
				case AnimationTrackController::eOperand_Divide:
					*(i32*) m_pVariable /= c->getResultKey().m_value.m_int32;
					break;
				}
				break;
			}

		case AnimationTrack::eType_Int64:
			{
				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*(i64*) m_pVariable = c->getResultKey().m_value.m_int64;
					break;
				case AnimationTrackController::eOperand_Add:
					*(i64*) m_pVariable += c->getResultKey().m_value.m_int64;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*(i64*) m_pVariable -= c->getResultKey().m_value.m_int64;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*(i64*) m_pVariable *= c->getResultKey().m_value.m_int64;
					break;
				case AnimationTrackController::eOperand_Divide:
					*(i64*) m_pVariable /= c->getResultKey().m_value.m_int64;
					break;
				}
				break;
			}

		case AnimationTrack::eType_Vec2:
		case AnimationTrack::eType_Vec3:
			{
				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					((Vec3*)m_pVariable)->x = c->getResultKey().m_value.m_vector[0];
					((Vec3*)m_pVariable)->y = c->getResultKey().m_value.m_vector[1];
					((Vec3*)m_pVariable)->z = c->getResultKey().m_value.m_vector[2];
					break;
				case AnimationTrackController::eOperand_Add:
					((Vec3*)m_pVariable)->x += c->getResultKey().m_value.m_vector[0];
					((Vec3*)m_pVariable)->y += c->getResultKey().m_value.m_vector[1];
					((Vec3*)m_pVariable)->z += c->getResultKey().m_value.m_vector[2];
					break;
				case AnimationTrackController::eOperand_Subtract:
					((Vec3*)m_pVariable)->x -= c->getResultKey().m_value.m_vector[0];
					((Vec3*)m_pVariable)->y -= c->getResultKey().m_value.m_vector[1];
					((Vec3*)m_pVariable)->z -= c->getResultKey().m_value.m_vector[2];
					break;
				case AnimationTrackController::eOperand_Multiply:
					((Vec3*)m_pVariable)->x *= c->getResultKey().m_value.m_vector[0];
					((Vec3*)m_pVariable)->y *= c->getResultKey().m_value.m_vector[1];
					((Vec3*)m_pVariable)->z *= c->getResultKey().m_value.m_vector[2];
					break;
				case AnimationTrackController::eOperand_Divide:
					((Vec3*)m_pVariable)->x /= c->getResultKey().m_value.m_vector[0];
					((Vec3*)m_pVariable)->y /= c->getResultKey().m_value.m_vector[1];
					((Vec3*)m_pVariable)->z /= c->getResultKey().m_value.m_vector[2];
					break;
				}
				break;
			}

		case AnimationTrack::eType_Quat:
			{
				q2.x = c->getResultKey().m_value.m_quat[0];
				q2.y = c->getResultKey().m_value.m_quat[1];
				q2.z = c->getResultKey().m_value.m_quat[2];
				q2.w = c->getResultKey().m_value.m_quat[3];

				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*((Quat*)m_pVariable) = q2;
					break;
				case AnimationTrackController::eOperand_Add:
					*((Quat*)m_pVariable) += q2;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*((Quat*)m_pVariable) -= q2;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*((Quat*)m_pVariable) *= q2;
					break;
				case AnimationTrackController::eOperand_Divide:
					*((Quat*)m_pVariable) /= q2;
					break;
				}
				break;
			}

		case AnimationTrack::eType_Color:
			{
				color.set(
						c->getResultKey().m_value.m_color[0],
						c->getResultKey().m_value.m_color[1],
						c->getResultKey().m_value.m_color[2],
						c->getResultKey().m_value.m_color[3]);

				switch (c->getOperand())
				{
				case AnimationTrackController::eOperand_Replace:
					*((Color*)m_pVariable) = color;
					break;
				case AnimationTrackController::eOperand_Add:
					*((Color*)m_pVariable) += color;
					break;
				case AnimationTrackController::eOperand_Subtract:
					*((Color*)m_pVariable) -= color;
					break;
				case AnimationTrackController::eOperand_Multiply:
					*((Color*)m_pVariable) *= color;
					break;
				case AnimationTrackController::eOperand_Divide:
					*((Color*)m_pVariable) /= color;
					break;
				}
				break;
			}
		}
	}
}

void AnimationTrack::bindToVariable(AnimationTrack::EType aType, void* pData)
{
	N_ASSERT(pData);
	m_type = aType;
	m_pVariable = pData;
}

AnimationKey* AnimationTrack::addKey(f32 aTime, void* pData)
{
	N_ASSERT(pData);
	AnimationKey* key = new AnimationKey;

	key->m_time = aTime;
	key->m_type = m_type;
	key->set(pData);
	m_keys.append(key);

	return m_keys[m_keys.count() - 1];
}

AnimationKey* AnimationTrack::addKey(f32 aTime, f32 aData)
{
	return addKey(aTime, &aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, const double& aData)
{
	return addKey(aTime, (void*)&aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, const i64& aData)
{
	return addKey(aTime, (void*)&aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, i32 aData)
{
	return addKey(aTime, (void*)&aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, i16 aData)
{
	return addKey(aTime, (void*)&aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, i8 aData)
{
	return addKey(aTime, (void*)&aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, const Vec3& aData)
{
	return addKey(aTime, (void*)&aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, const Quat& aData)
{
	return addKey(aTime, (void*)&aData);
}

AnimationKey* AnimationTrack::addKey(f32 aTime, const Color& aData)
{
	return addKey(aTime, (void*)&aData);
}

void AnimationTrack::computeTangents()
{
	AnimationKey *h1, *h2;

	if (m_keys.count() <= 1)
	{
		return;
	}

	if (m_keys.count() == 2)
	{
		h1 = m_keys[0];
		h2 = m_keys[1];
		h1->m_rightTangent = ((*h2 - *h1) * (1.0 - h1->m_tension)).m_value;
		h2->m_leftTangent = ((*h2 - *h1) * (1.0 - h2->m_tension)).m_value;

		return;
	}

	AnimationKey *a, *b, *c;
	f32 d1, d2, d;
	f32 adjust1, adjust2;

	for (size_t i = 1, iCount = m_keys.count() - 1; i < iCount; ++i)
	{
		a = m_keys[i - 1];
		b = m_keys[i];
		c = m_keys[i + 1];
		d1 = b->m_time - a->m_time;
		d2 = c->m_time - b->m_time;
		d = d1 + d2;

		if (d == 0.0f)
		{
			d = 1.0f;
		}

		adjust1 = (f32) d1 / d;
		adjust2 = (f32) d2 / d;

		adjust1 = 0.5f + (1.0f - fabsf(b->m_continuity)) * (adjust1 - 0.5f);
		adjust2 = 0.5f + (1.0f - fabsf(b->m_continuity)) * (adjust2 - 0.5f);

		b->m_leftTangent = (((*b - *a) * (1.0f + b->m_bias) * (1.0f - b->m_continuity)
			+ (c - b) * (1.0f - b->m_bias) * (1.0f + b->m_continuity)) * (1.0f - b->m_tension) * adjust1).m_value;
		b->m_rightTangent = (((*b - *a) * (1.0f + b->m_bias) * (1.0f + b->m_continuity)
			+ (c - b) * (1.0f - b->m_bias) * (1.0f - b->m_continuity)) * (1.0f - b->m_tension) * adjust2).m_value;
	}

	AnimationKey* key;
	u32 n = m_keys.count() - 1;

	key = m_keys[1];
	key->m_leftTangent = key->m_value;
	m_keys[0]->m_rightTangent = (((*m_keys[1] - *m_keys[0]) * 1.5f - *key * 0.5f) * (1.0f - m_keys[0]->m_tension)).m_value;

	key = m_keys[n - 1];
	key->m_rightTangent = key->m_value;
	m_keys[n]->m_leftTangent = (((*m_keys[n - 1] - *m_keys[n]) * 1.5f - *key * 0.5f) * (1.0f - m_keys[0]->m_tension)).m_value;
}

const AnimationKey& AnimationTrack::defaultKeyVariable() const
{
	return m_defaultKeyVariable;
}

u32 AnimationTrack::trackFlags() const
{
	return m_trackFlags;
}

void AnimationTrack::setTrackFlags(u32 aFlags)
{
	m_trackFlags;
}

//---

AnimationEvent::AnimationEvent()
{
	m_time = 0;
	m_bActive = true;
	m_bTriggered = false;
}

AnimationEvent::~AnimationEvent()
{
}

void AnimationEvent::setTime(f32 aValue)
{
	m_time = aValue;
}

void AnimationEvent::setActive(bool bActive)
{
	m_bActive = bActive;
}

f32 AnimationEvent::time() const
{
	return m_time;
}

bool AnimationEvent::isActive() const
{
	return m_bActive;
}

void AnimationEvent::onTrigger()
{
}

void AnimationEvent::setTriggered(bool bTrigger)
{
	m_bTriggered = bTrigger;
}

bool AnimationEvent::isTriggered() const
{
	return m_bTriggered;
}

//---

Animation::Animation()
{
	m_direction = 1;
	m_duration = 0;
	m_time = 0;
	m_loopMode = eAnimationLoop_Repeat;
	m_timeScale = 1;
	m_animationFlags = Animation::eFlag_AutoDuration;
}

Animation::~Animation()
{
	freeData();
}

f32 Animation::duration() const
{
	return m_duration;
}

f32 Animation::time() const
{
	return m_time;
}

f32 Animation::timeScale() const
{
	return m_timeScale;
}

const Array<AnimationTrack*>& Animation::tracks() const
{
	return m_tracks;
}

void Animation::setDuration(f32 aValue)
{
	m_duration = aValue;
}

void Animation::setTime(f32 aValue)
{
	m_time = aValue;
}

void Animation::setTimeScale(f32 aValue)
{
	m_timeScale = aValue;
}

u32 Animation::animationFlags() const
{
	return m_animationFlags;
}

void Animation::setAnimationFlags(u32 aFlags)
{
	m_animationFlags = aFlags;
}

void Animation::process(f32 aTimeDelta)
{
	f32 oldTime = m_time;
	m_time += aTimeDelta * m_timeScale * m_direction;

	for (size_t i = 0, iCount = m_events.count(); i < iCount; ++i)
	{
		if (!m_events[i]->isTriggered())
		{
			if (m_events[i]->time() >= oldTime && m_events[i]->time() < m_time)
			{
				m_events[i]->onTrigger();
				m_events[i]->setTriggered(true);
			}
		}
	}

	if (m_time > m_duration)
	{
		for (size_t i = 0, iCount = m_events.count(); i < iCount; ++i)
		{
			m_events[i]->setTriggered(false);
		}

		if (m_loopMode == eAnimationLoop_None)
		{
			return;
		}

		if (m_loopMode == eAnimationLoop_Repeat)
		{
			m_time = 0;
		}

		if (m_loopMode == eAnimationLoop_PingPong)
		{
			m_direction = -m_direction;

			if (m_time > m_duration)
			{
				m_time = m_duration;
			}
		}
	}

	if (m_time <= 0)
	{
		if (m_loopMode == eAnimationLoop_PingPong)
		{
			for (size_t i = 0, iCount = m_events.count(); i < iCount; ++i)
			{
				m_events[i]->setTriggered(false);
			}

			m_direction = -m_direction;
			m_time = 0;
		}
	}

	for (size_t i = 0, iCount = m_tracks.count(); i < iCount; ++i)
	{
		m_tracks[i]->process(m_time);
	}
}

EAnimationLoop Animation::loopMode() const
{
	return m_loopMode;
}

void Animation::setLoopMode(EAnimationLoop aValue)
{
	m_loopMode = aValue;
}

AnimationTrack* Animation::addTrack(
								const char* pName,
								AnimationTrack::EType aType,
								void* pData)
{
	AnimationTrack* pTrack;

	pTrack = new AnimationTrack();
	N_ASSERT(pTrack);

	if (pTrack)
	{
		pTrack->setName(pName);
		pTrack->setType(aType);
		pTrack->bindToVariable(
					aType,
					pData 
						? pData 
						: (void*)&pTrack->defaultKeyVariable().m_value.m_int8);
		m_tracks.append(pTrack);
	}

	return pTrack;
}

void Animation::play()
{
	m_time = 0;

	for (size_t i = 0, iCount = m_events.count(); i < iCount; ++i)
	{
		m_events[i]->setTriggered(false);
	}

	for (size_t i = 0, iCount = m_tracks.count(); i < iCount; ++i)
	{
		m_tracks[i]->computeTangents();
	}

	if (m_animationFlags & Animation::eFlag_AutoDuration)
	{
		m_duration = 0;

		for (size_t i = 0, iCount = m_tracks.count(); i < iCount; ++i)
		{
			for (size_t j = 0, jCount = m_tracks[i]->keys().count(); j < jCount; ++j)
			{
				if (m_tracks[i]->keys()[j]->m_time > m_duration)
				{
					m_duration = m_tracks[i]->keys()[j]->m_time;
				}
			}
		}
	}
}

AnimationTrack* Animation::findTrackByName(const char* pName) const
{
	for (size_t i = 0, iCount = m_tracks.count(); i < iCount; ++i)
	{
		if (m_tracks[i]->name() == pName)
		{
			return m_tracks[i];
		}
	}

	return nullptr;
}

//void Animation::loadFromXmlElements(XmlElement* pElem)
//{
//	Map<String, AnimationTrack::EType> trackTypes;
//	Map<String, EAnimationInterpolation> ipolTypes;
//
//	freeData();
//	trackTypes["int8"] = AnimationTrack::eType_Int8;
//	trackTypes["int16"] = AnimationTrack::eType_Int16;
//	trackTypes["int32"] = AnimationTrack::eType_Int32;
//	trackTypes["int64"] = AnimationTrack::eType_Int64;
//	trackTypes["f32"] = AnimationTrack::eType_Float;
//	trackTypes["double"] = AnimationTrack::eType_Double;
//	trackTypes["vector2"] = AnimationTrack::eType_Vector2;
//	trackTypes["vector3"] = AnimationTrack::eType_Vector3;
//	trackTypes["quat"] = AnimationTrack::eType_Quaternion;
//	trackTypes["color"] = AnimationTrack::eType_Color;
//	
//	ipolTypes["linear"] = eAnimationInterpolation_Linear;
//	ipolTypes["bezier"] = eAnimationInterpolation_Bezier;
//	ipolTypes["tcb"] = eAnimationInterpolation_TCB;
//
//	XmlElement* pAnimationElem = pElem;
//	XmlElement* pTracksElem = pAnimationElem->getChild("tracks");
//
//	for (size_t j = 0, jCount = pTracksElem->getChildren().size(); j < jCount; ++j)
//	{
//		XmlElement* pTrackElem = pTracksElem->getChildren()[j];
//		AnimationTrack* pTrack = addTrack(
//									pTrackElem->getAttributeValue("name").c_str(),
//									trackTypes[pTrackElem->getAttributeValue("type")],
//									nullptr);
//		N_ASSERT(pTrack);
//
//		XmlElement* pKeysElem = pTrackElem->getChild("keys");
//
//		pTrack->setInterpolationType(
//					(EAnimationInterpolation)ipolTypes[pTrackElem->getAttributeValue("interpolation")]);
//
//		for (size_t k = 0, kCount = pKeysElem->getChildren().size(); k < kCount; ++k)
//		{
//			XmlElement* pKeyElem = pKeysElem->getChildren()[k];
//			f32 time = pKeyElem->getAttributeValueAsFloat("time");
//			bool bKeyAdded = true;
//
//			switch (pTrack->getType())
//			{
//			case AnimationTrack::eType_Float:
//				pTrack->addKey(time, (f32)pKeyElem->getAttributeValueAsFloat("value"));
//				break;
//			case AnimationTrack::eType_Double:
//				pTrack->addKey(time, (double)pKeyElem->getAttributeValueAsFloat("value"));
//				break;
//			case AnimationTrack::eType_Int8:
//				pTrack->addKey(time, (i8)pKeyElem->getAttributeValueAsInt("value"));
//				break;
//			case AnimationTrack::eType_Int16:
//				pTrack->addKey(time, (i16)pKeyElem->getAttributeValueAsInt("value"));
//				break;
//			case AnimationTrack::eType_Int32:
//				pTrack->addKey(time, (i32)pKeyElem->getAttributeValueAsInt("value"));
//				break;
//			case AnimationTrack::eType_Int64:
//				pTrack->addKey(time, (i64)pKeyElem->getAttributeValueAsInt("value"));
//				break;
//			case AnimationTrack::eType_Vector2:
//				pTrack->addKey(time, pKeyElem->getAttributeValueAsVector3D("value"));
//				break;
//			case AnimationTrack::eType_Vector3:
//				pTrack->addKey(time, pKeyElem->getAttributeValueAsVector3D("value"));
//				break;
//			case AnimationTrack::eType_Quaternion:
//				pTrack->addKey(time, pKeyElem->getAttributeValueAsQuat("value"));
//				break;
//			case AnimationTrack::eType_Color:
//				pTrack->addKey(time, pKeyElem->getAttributeValueAsColor("value"));
//				break;
//			default:
//				bKeyAdded = false;
//			}
//
//			if (bKeyAdded)
//			{
//				AnimationKey* pKey = pTrack->keys()[pTrack->keys().count() - 1];
//
//				if (pTrack->interpolationType() == eAnimationInterpolation_TCB)
//				{
//					pKey->m_tension = pKeyElem->getAttributeValueAsFloat("tension");
//					pKey->m_continuity = pKeyElem->getAttributeValueAsFloat("continuity");
//					pKey->m_bias = pKeyElem->getAttributeValueAsFloat("bias");
//					pKey->m_easeIn = pKeyElem->getAttributeValueAsFloat("easeIn");
//					pKey->m_easeOut = pKeyElem->getAttributeValueAsFloat("easeOut");
//				}
//				else if (pTrack->interpolationType() == eAnimationInterpolation_Bezier)
//				{
//					//TODO bezier ipol
//				}
//			}
//		}
//	}
//}

void Animation::freeData()
{
	for (size_t i = 0, iCount = m_tracks.count(); i < iCount; ++i)
	{
		delete m_tracks[i];
	}

	for (size_t i = 0, iCount = m_events.count(); i < iCount; ++i)
	{
		delete m_events[i];
	}

	m_tracks.clear();
}

const String& Animation::name() const
{
	return m_name;
}

void Animation::setName(const char* pName)
{
	m_name = pName;
}
}