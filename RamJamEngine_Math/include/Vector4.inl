//////////////////////////////////////////////////////////////////////////

#include "MathHelper.h"
#include "Debug.h"

//-------------------------
template <typename Real>
const Vector4_T<Real> Vector4_T<Real>::zero	= Vector4_T<Real>();
//-------------------------
template <typename Real>
const Vector4_T<Real> Vector4_T<Real>::one	= Vector4_T<Real>(1.0f);
//-------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::operator + (const Vector4_T<Real>& v)
{ return Vector4_T<Real>(this->w+v.w, this->x+v.x, this->y+v.y, this->z+v.z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::operator - (const Vector4_T<Real>& v)
{ return Vector4_T<Real>(this->w-v.w, this->x-v.x, this->y-v.y, this->z-v.z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::operator - ()
{ return Vector4_T<Real>(-this->w, -this->x, -this->y, -this->z); }
//---------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::operator / (const Vector4_T<Real>& v)
{ return Vector4_T<Real>(this->w/v.w, this->x/v.x, this->y/v.y, this->z/v.z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::operator / (const Real& f)
{ RJE_ASSERT(f!=0);	return Vector4_T<Real>(this->w/f, this->x/f, this->y/f, this->z/f); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::operator * (const Real& f)
{ return Vector4_T<Real>(this->w*f, this->x*f, this->y*f, this->z*f); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::operator * (const Vector4_T<Real>&v)
{ return Vector4_T<Real>(this->w*v.w, this->x*v.x, this->y*v.y, this->z*v.z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> operator * (const Real& f, const Vector4_T<Real>&v)
{ return Vector4_T<Real>(v.w*f, v.x*f, v.y*f, v.z*f); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> operator * (const int& f, const Vector4_T<Real>&v)
{ return Vector4_T<Real>(v.w*f, v.x*f, v.y*f, v.z*f); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator = (const Vector4_T<Real>& vIn)
{ w = vIn.w; x = vIn.x; y = vIn.y; z = vIn.z; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator = (const DirectX::XMFLOAT4& vIn)
{ w = vIn.x; x = vIn.y; y = vIn.z; z = vIn.w; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator = (const DirectX::PackedVector::XMCOLOR& vIn)
{ w = vIn.a; x = vIn.r; y = vIn.g; z = vIn.b; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator += (const Vector4_T<Real>& v)
{ w += v.w; x += v.x; y += v.y; z += v.z; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator -= (const Vector4_T<Real>& v)
{ w -= v.w; x -= v.x; y -= v.y; z -= v.z; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator /= (const Vector4_T<Real>& v)
{ w /= v.w; x /= v.x; y /= v.y; z /= v.z; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator /= (const Real& f)
{ RJE_ASSERT(f!=0); w /= f; x /= f; y /= f; z /= f; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::operator *= (const Real& f)
{ w *= f; x *= f; y *= f; z *= f; return *this; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE BOOL Vector4_T<Real>::operator == (const Vector4_T<Real>& v)
{ return (w == v.w && x == v.x && y == v.y && z == v.z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE BOOL Vector4_T<Real>::operator != (const Vector4_T<Real>& v)
{ return (w != v.w || x != v.x || y != v.y || z != v.z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
Vector4_T<Real>::operator DirectX::XMFLOAT4 ()
{ return DirectX::XMFLOAT4(w, x, y, z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
Vector4_T<Real>::operator DirectX::PackedVector::XMCOLOR ()
{ return DirectX::PackedVector::XMCOLOR(w, x, y, z); }
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE void Vector4_T<Real>::Set (Real fw, Real fx, Real fy, Real fz)
{ w=fw; x=fx; y=fy; z=fz; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Real Vector4_T<Real>::SqrMagnitude()
{ return w*w + x*x + y*y + z*z }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Real Vector4_T<Real>::Magnitude()
{ return sqrt(w*w + x*x + y*y + z*z); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Real Vector4_T<Real>::Min()
{ return w>x ? (x>y ? (y>z ? z : y) : (x>z ? z : x)) : (w>y ? (y>z ? z : y) : (w>z ? z : w)); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Real Vector4_T<Real>::Max()
{ return w>x ? (w>y ? (w>z ? w : z) : (y>z ? y : z)) : (x>y ? (x>z ? x : z) : (y>z ? y : z)); }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Real Vector4_T<Real>::Dot(const Vector4_T& v1, const Vector4_T& v2)
{ return v1.w*v2.w + v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::Min(const Vector4_T<Real>& v1, const Vector4_T<Real>& v2)
{
	Vector4_T<Real> out;

	out.w = v1.w < v2.w ? v1.w : v2.w;
	out.x = v1.x < v2.x ? v1.x : v2.x;
	out.y = v1.y < v2.y ? v1.y : v2.y;
	out.z = v1.z < v2.z ? v1.z : v2.z;

	return out;
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real> Vector4_T<Real>::Max(const Vector4_T<Real>& v1, const Vector4_T<Real>& v2)
{
	Vector4_T<Real> out;

	out.w = v1.w > v2.w ? v1.w : v2.w;
	out.x = v1.x > v2.x ? v1.x : v2.x;
	out.y = v1.y > v2.y ? v1.y : v2.y;
	out.z = v1.z > v2.z ? v1.z : v2.z;

	return out;
}
//---------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::Normalize()
{
	Real mag = Magnitude(); 
	if (!RJE::Math::IsZero(mag))
		*this /= mag; 
	return *this;
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <typename Real>
FORCEINLINE Vector4_T<Real>& Vector4_T<Real>::Scale(const Vector4_T& v)
{ w *= v.w; x *= v.x; y *= v.y; z *= v.z; return *this; }
//----------------------------------------------------------------------
