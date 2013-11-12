#pragma once

#include "Types.h"
#include "Matrix44.h"
#include "Vector3.h"
#include "Vector4.h"

template <typename Real>
struct Quaternion_T
{
	union
	{
		struct
		{
			Real w;
			Real x;
			Real y;
			Real z;
		};
		struct
		{
			Real			mReal;
			Vector3_T<Real>	mImaginary;
		};
		struct
		{
			Vector4_T<Real>	mVectorial;
		};
	};

	static const Quaternion_T identity;

	Quaternion_T()                               : w(0.0), x(0.0), y(0.0), z(0.0)	{};
	Quaternion_T(Real w, Real x, Real y, Real z) { Set(w,x,y,z); }
	Quaternion_T(const Quaternion_T& q)          { Set(q.w, q.x, q.y, q.z); }
	Quaternion_T(Real pitch, Real yaw, Real roll);
	Quaternion_T(Matrix44_T<Real>& rotation);

	//-------------------------

	Quaternion_T	operator +  (const Quaternion_T&);
	Quaternion_T	operator -  (const Quaternion_T&);
	Quaternion_T	operator *  (const Quaternion_T&);
	Quaternion_T	operator /  (const Real&);
	Quaternion_T&	operator =  (const Quaternion_T&);
	Quaternion_T&	operator += (const Quaternion_T&);
	Quaternion_T&	operator -= (const Quaternion_T&);
	BOOL			operator == (const Quaternion_T&);
	BOOL			operator != (const Quaternion_T&);
	//-------------------------
	operator Matrix44_T<Real>();
	//-------------------------
	void			Set(Real w, Real x, Real y, Real z);
	Real			Magnitude();
	Real			SqrMagnitude();
	BOOL			IsNormalized();
	//-------------------------
	Quaternion_T&	Conjugate();
	Quaternion_T&	Normalize();
	Quaternion_T&	Inverse();
	//-------------------------
	Vector3_T<Real>	ToEulerAngles();
	//-------------------------
	static void		Slerp(OUT Quaternion_T& qOut, const IN Quaternion_T& qStart, const IN Quaternion_T& qEnd, Real factor);
};

typedef Quaternion_T<f32> Quaternion;

#include "Quaternion.inl"
