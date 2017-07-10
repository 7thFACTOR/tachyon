#pragma once

namespace base
{
class Quat;

template <typename Type>
class MatrixTpl
{
public:
	enum class AxisType
	{
		XAxis,
		YAxis,
		ZAxis
	};

	MatrixTpl();
	MatrixTpl(const Type data[16]);
	MatrixTpl(const Type data[4][4]);
	MatrixTpl(const MatrixTpl& other);
	void setIdentity();
	void makeZero();
	void setUnit();
	void multiply(const MatrixTpl& src, MatrixTpl& dst) const;
	void multiply(const MatrixTpl& src);
	void transform(const Vec3Tpl<Type>& src, Vec3Tpl<Type>& dst) const;
	void transform(const Quat& src, Quat& dst) const;
	void setRotationX(f32 angle);
	void setRotationY(f32 angle);
	void setRotationZ(f32 angle);
	void setRotation(AxisType axis, f32 angle);
	void setRotation(f32 eulerX, f32 eulerY, f32 eulerZ);
	void setTranslation(Type x, Type y, Type z);
	void setTranslation(const Vec3Tpl<Type>& trans);
	void setScale(Type x, Type y, Type z);
	void setScale(const Vec3Tpl<Type>& scale);
	void preTranslate(const Vec3Tpl<Type>& pos);
	void fill(Type value);
	MatrixTpl concatenate(const MatrixTpl& matrix);
	void setScale(Type scale);
	void invert();
	Type getDeterminant() const;
	void transpose();
	MatrixTpl getTransposed() const;
	Vec3Tpl<Type> getTranslation() const;
	Vec3Tpl<Type> getScale() const;
	Vec3Tpl<Type> operator * (const Vec3Tpl<Type>& vec) const;
	MatrixTpl operator * (const MatrixTpl& matrix) const;
	MatrixTpl& operator *= (const MatrixTpl& matrix);
	operator Type* ();
	void lookAt(const Vec3Tpl<Type>& from, const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up);
	void lookTowards(const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up);
	void perspective(f32 fov, f32 aspect, Type near, Type far);
	void ortho(Type left, Type right, Type bottom, Type top, Type near, Type far);
	void lookAtLeftHand(const Vec3Tpl<Type>& from, const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up);
	void perspectiveLeftHand(f32 fov, f32 aspect, Type near, Type far);
	void orthoLeftHand(Type left, Type right, Type bottom, Type top, Type near, Type far);
	void lookAtRightHand(const Vec3Tpl<Type>& from, const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up);
	void perspectiveRightHand(f32 fov, f32 aspect, Type near, Type far);
	void orthoRightHand(Type left, Type right, Type bottom, Type top, Type near, Type far);
	void clearTranslation();
	void clearRotation();
	void clearScale();
	MatrixTpl getRotationOnly() const;
	MatrixTpl getInverted() const;
	inline Vec3Tpl<Type> getRightAxis() const;
	inline Vec3Tpl<Type> getUpAxis() const;
	inline Vec3Tpl<Type> getForwardAxis() const;
	static MatrixTpl makeTranslationMatrix(const Vec3Tpl<Type>& trans);

	Type m[4][4];
};

}