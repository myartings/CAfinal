// cyCodeBase by Cem Yuksel
// [www.cemyuksel.com]
//-------------------------------------------------------------------------------
///
/// \file		Point.h 
/// \author		Cem Yuksel
/// \version	1.3
/// \date		October 6, 2007
///
/// \brief 2D, 3D and 4D point classes.
///
///
/// @copydoc Point3f
///
/// Point3f is 3D point class
///
/// You can use point classes with OpenGL as
///
/// glVertex3f( myvector.x, myvector.y, myvector.z );
///
/// or
///
/// glVertex3fv( &myvector.x );
///
//-------------------------------------------------------------------------------

#ifndef _POINT_H_INCLUDED_
#define _POINT_H_INCLUDED_

//-------------------------------------------------------------------------------

#include <math.h>

//-------------------------------------------------------------------------------

/// 2D point class

class Point2f
{
	friend Point2f operator+( const float v, const Point2f &pt ) { return pt+v; }		///< Addition with a constant
	friend Point2f operator-( const float v, const Point2f &pt ) { return -(pt-v); }	///< Subtraction from a constant
	friend Point2f operator*( const float v, const Point2f &pt ) { return pt*v; }		///< Multiplication with a constant

public:

	float x, y;

	///@name Constructors
	Point2f() {}
	Point2f( float _x, float _y ) { x=_x; y=_y; }
	Point2f( const float *pt ) { x=pt[0]; y=pt[1]; }
	Point2f( const Point2f &pt ) { x=pt.x; y=pt.y; }

	///@name Set & Get value functions
	Point2f& Zero() { x=0; y=0; return *this; }						///< Sets x and y coordinates as zero
	Point2f& Set( float _x, float _y ) { x=_x; y=_y; return *this; }	///< Sets x and y coordinates as given
	Point2f& Set( const float *v ) { x=v[0]; y=v[1]; return *this; }	///< Sets x and y coordinates using the values in the given array
	void GetValue( float *v ) const { v[0]=x; v[1]=y; }					///< Puts x and y values into the array

	///@name Length and Normalize functions
	void		Normalize()		{ float s = (1.0f)/Length(); *this *= s; }
	Point2f	GetNormalized() const { float s = (1.0f)/Length(); return *this * s; }
	float		LengthSquared()	const { return x*x + y*y; }
	float		Length()		const { return (float) sqrt(LengthSquared()); }

	///@name Limit functions
	void ClampMinMax( float minValue, float maxValue ) { ClampMin(minValue); ClampMax(maxValue); }
	void ClampMin( float n ) { if(x<n)x=n; if(y<n)y=n; }
	void ClampMax( float n ) { if(x>n)x=n; if(y>n)y=n; }

	///@name Unary operators
	Point2f operator-() const { return Point2f(-x,-y); } 
	Point2f operator+() const { return *this; }

	///@name Binary operators
	Point2f operator+( const Point2f &pt ) const { return Point2f(x+pt.x, y+pt.y); }
	Point2f operator-( const Point2f &pt ) const { return Point2f(x-pt.x, y-pt.y); }
	Point2f operator*( const Point2f &pt ) const { return Point2f(x*pt.x, y*pt.y); }
	Point2f operator/( const Point2f &pt ) const { return Point2f(x/pt.x, y/pt.y); }
	Point2f operator+(float n) const { return Point2f(x+n, y+n); }
	Point2f operator-(float n) const { return Point2f(x-n, y-n); }
	Point2f operator*(float n) const { return Point2f(x*n, y*n); }
	Point2f operator/(float n) const { return Point2f(x/n, y/n); }

	///@name Assignment operators
	Point2f& operator+=( const Point2f &pt ) { x+=pt.x; y+=pt.y; return *this; }
	Point2f& operator-=( const Point2f &pt ) { x-=pt.x; y-=pt.y; return *this; }
	Point2f& operator*=( const Point2f &pt ) { x*=pt.x; y*=pt.y; return *this; }
	Point2f& operator/=( const Point2f &pt ) { x/=pt.x; y/=pt.y; return *this; }
	Point2f& operator+=(float n) { x+=n; y+=n; return *this; }
	Point2f& operator-=(float n) { x-=n; y-=n; return *this; }
	Point2f& operator*=(float n) { x*=n; y*=n; return *this; }
	Point2f& operator/=(float n) { x/=n; y/=n; return *this; }

	///@name Test operators
	int operator==( const Point2f& pt ) const { return ( (pt.x==x) && (pt.y==y) ); }
	int operator!=( const Point2f& pt ) const { return ( (pt.x!=x) || (pt.y!=y) ); }

	///@name Access operators
	float& operator[]( int i ) { return (&x)[i]; }
	float  operator[]( int i ) const { return (&x)[i]; }

	///@name Cross product and dot product
	float Cross	   ( const Point2f &pt ) const { return x*pt.y-y*pt.x; }		///< Cross product
	float operator^( const Point2f &pt ) const { return Cross(pt); }			///< Cross product operator
	float Dot	   ( const Point2f &pt ) const { return x*pt.x + y*pt.y; }	///< Dot product
	float operator%( const Point2f &pt ) const { return Dot(pt); }			///< Dot product operator

};




//-------------------------------------------------------------------------------


/// 3D point class

class Point3f
{
	friend Point3f operator+( const float v, const Point3f &pt ) { return pt+v; }		///< Addition with a constant
	friend Point3f operator-( const float v, const Point3f &pt ) { return -(pt-v); }	///< Subtraction from a constant
	friend Point3f operator*( const float v, const Point3f &pt ) { return pt*v; }		///< Multiplication with a constant

public:

	float x, y, z;

	///@name Constructors
	Point3f() { }
	Point3f( float _x, float _y, float _z ) { x=_x; y=_y; z=_z; }
	Point3f( const float *pt ) { x=pt[0]; y=pt[1]; z=pt[2]; }
	Point3f( const Point3f &pt ) { x=pt.x; y=pt.y; z=pt.z; }
	Point3f( const Point2f &pt ) { x=pt.x; y=pt.y; z=0.0f; }
	Point3f( const Point2f &pt, float _z ) { x=pt.x; y=pt.y; z=_z; }

	///@name Set & Get value functions
	Point3f& Zero() { x=0; y=0; z=0; return *this; }									///< Sets x, y and z coordinates as zero
	Point3f& Set( float _x, float _y, float _z ) { x=_x; y=_y; z=_z; return *this; }	///< Sets x, y and z coordinates as given
	Point3f& Set( const float *v ) { x=v[0]; y=v[1]; z=v[2]; return *this; }			///< Sets x, y and z coordinates using the values in the given array
	void GetValue( float *v ) const { v[0]=x; v[1]=y; v[2]=z; }							///< Puts x, y and z values into the array

	///@name Length and Normalize functions
	void		Normalize()		{ float s = (1.0f)/Length(); *this *= s; }
	Point3f	GetNormalized()	const { float s = (1.0f)/Length(); return *this * s; }
	float		LengthSquared()	const { return x*x + y*y + z*z; }
	float		Length()		const { return (float) sqrt(LengthSquared()); }

	///@name Limit functions
	void ClampMinMax( float min, float max ) { ClampMin(min); ClampMax(max); }
	void ClampMin( float n ) { if(x<n)x=n; if(y<n)y=n; if(z<n)z=n; }
	void ClampMax( float n ) { if(x>n)x=n; if(y>n)y=n; if(z>n)z=n; }

	///@name Unary operators
	Point3f operator-() const { return Point3f(-x,-y,-z); } 
	Point3f operator+() const { return *this; }

	///@name Binary operators
	Point3f operator+( const Point3f &pt ) const { return Point3f(x+pt.x, y+pt.y, z+pt.z); }
	Point3f operator-( const Point3f &pt ) const { return Point3f(x-pt.x, y-pt.y, z-pt.z); }
	Point3f operator*( const Point3f &pt ) const { return Point3f(x*pt.x, y*pt.y, z*pt.z); }
	Point3f operator/( const Point3f &pt ) const { return Point3f(x/pt.x, y/pt.y, z/pt.z); }
	Point3f operator+(float n) const { return Point3f(x+n, y+n, z+n); }
	Point3f operator-(float n) const { return Point3f(x-n, y-n, z-n); }
	Point3f operator*(float n) const { return Point3f(x*n, y*n, z*n); }
	Point3f operator/(float n) const { return Point3f(x/n, y/n, z/n); }

	///@name Assignment operators
	Point3f& operator+=( const Point3f &pt ) { x+=pt.x; y+=pt.y; z+=pt.z; return *this; }
	Point3f& operator-=( const Point3f &pt ) { x-=pt.x; y-=pt.y; z-=pt.z; return *this; }
	Point3f& operator*=( const Point3f &pt ) { x*=pt.x; y*=pt.y; z*=pt.z; return *this; }
	Point3f& operator/=( const Point3f &pt ) { x/=pt.x; y/=pt.y; z/=pt.z; return *this; }
	Point3f& operator+=(float n) { x+=n; y+=n; z+=n; return *this; }
	Point3f& operator-=(float n) { x-=n; y-=n; z-=n; return *this; }
	Point3f& operator*=(float n) { x*=n; y*=n; z*=n; return *this; }
	Point3f& operator/=(float n) { x/=n; y/=n; z/=n; return *this; }

	///@name Test operators
	int operator==( const Point3f& pt ) const { return ( (pt.x==x) && (pt.y==y) && (pt.z==z) ); }
	int operator!=( const Point3f& pt ) const { return ( (pt.x!=x) || (pt.y!=y) || (pt.z!=z) ); }

	///@name Access operators
	float& operator[]( int i ) { return (&x)[i]; }
	float  operator[]( int i ) const { return (&x)[i]; }

	///@name Cross product and dot product
	Point3f	Cross	 ( const Point3f pt ) const { return Point3f(y*pt.z-z*pt.y, z*pt.x-x*pt.z, x*pt.y-y*pt.x); }	///< Cross product
	Point3f	operator^( const Point3f pt ) const { return Cross(pt); }					///< Cross product
	float		Dot		 ( const Point3f pt ) const { return x*pt.x + y*pt.y + z*pt.z; }	///< Dot product
	float		operator%( const Point3f pt ) const { return Dot(pt); }					///< Dot product

	///@name Conversion Methods
	Point2f	XY() const { return Point2f(x,y); }
};

//-------------------------------------------------------------------------------


/// 4D point class
class Point4f
{
	friend Point4f operator+( const float v, const Point4f &pt ) { return pt+v; }		///< Addition with a constant
	friend Point4f operator-( const float v, const Point4f &pt ) { return -(pt-v); }	///< Subtraction from a constant
	friend Point4f operator*( const float v, const Point4f &pt ) { return pt*v; }		///< Multiplication with a constant

public:

	float x, y, z, w;

	///@name Constructors
	Point4f() { }
	Point4f( float _x, float _y, float _z, float _w ) { x=_x; y=_y; z=_z; w=_w; }
	Point4f( const float *pt ) { x=pt[0]; y=pt[1]; z=pt[2]; w=pt[3]; }
	Point4f( const Point4f &pt ) { x=pt.x; y=pt.y; z=pt.z; w=pt.w; }
	Point4f( const Point3f &pt ) { x=pt.x; y=pt.y; z=pt.z; w=1.0f; }
	Point4f( const Point3f &pt, float _w ) { x=pt.x; y=pt.y; z=pt.z; w=_w; }

	///@name Set & Get value functions
	Point4f& Zero() { x=0; y=0; z=0; w=0; return *this; }												///< Sets x, y, z and w coordinates as zero
	Point4f& Set( float _x, float _y, float _z, float _w ) { x=_x; y=_y; z=_z; w=_w; return *this; }	///< Sets x, y, z and w coordinates as given
	Point4f& Set( const float *v ) { x=v[0]; y=v[1]; z=v[2]; w=v[3]; return *this; }					///< Sets x, y, z and w coordinates using the values in the given array
	void GetValue( float *v ) const { v[0]=x; v[1]=y; v[2]=z; v[3]=w; }									///< Puts x, y, z and w values into the array

	///@name Length and Normalize functions
	void		Normalize()		{ float s = (1.0f)/Length(); *this *= s; }
	Point4f	GetNormalized() const { float s = (1.0f)/Length(); return *this * s; }
	float		LengthSquared()	const { return x*x + y*y + z*z + w*w; }
	float		Length()		const { return (float) sqrt(LengthSquared()); }

	///@name Limit functions
	void ClampMinMax( float min, float max ) { ClampMin(min); ClampMax(max); }
	void ClampMin( float n ) { if(x<n)x=n; if(y<n)y=n; if(z<n)z=n; if(w<n)w=n; }
	void ClampMax( float n ) { if(x>n)x=n; if(y>n)y=n; if(z>n)z=n; if(w>n)w=n; }

	///@name Unary operators
	Point4f operator-() const { return Point4f(-x,-y,-z,-w); } 
	Point4f operator+() const { return *this; }

	///@name Binary operators
	Point4f operator+( const Point4f &pt ) const { return Point4f(x+pt.x, y+pt.y, z+pt.z, w+pt.w); }
	Point4f operator-( const Point4f &pt ) const { return Point4f(x-pt.x, y-pt.y, z-pt.z, w-pt.w); }
	Point4f operator*( const Point4f &pt ) const { return Point4f(x*pt.x, y*pt.y, z*pt.z, w*pt.w); }
	Point4f operator/( const Point4f &pt ) const { return Point4f(x/pt.x, y/pt.y, z/pt.z, w/pt.w); }
	Point4f operator+(float n) const { return Point4f(x+n, y+n, z+n, w+n); }
	Point4f operator-(float n) const { return Point4f(x-n, y-n, z-n, w-n); }
	Point4f operator*(float n) const { return Point4f(x*n, y*n, z*n, w*n); }
	Point4f operator/(float n) const { return Point4f(x/n, y/n, z/n, w/n); }

	///@name Assignment operators
	Point4f& operator+=( const Point4f &pt ) { x+=pt.x; y+=pt.y; z+=pt.z; w+=pt.w; return *this; }
	Point4f& operator-=( const Point4f &pt ) { x-=pt.x; y-=pt.y; z-=pt.z; w-=pt.w; return *this; }
	Point4f& operator*=( const Point4f &pt ) { x*=pt.x; y*=pt.y; z*=pt.z; w*=pt.w; return *this; }
	Point4f& operator/=( const Point4f &pt ) { x/=pt.x; y/=pt.y; z/=pt.z; w/=pt.w; return *this; }
	Point4f& operator+=(float n) { x+=n; y+=n; z+=n; w+=n; return *this; }
	Point4f& operator-=(float n) { x-=n; y-=n; z-=n; w-=n; return *this; }
	Point4f& operator*=(float n) { x*=n; y*=n; z*=n; w*=n; return *this; }
	Point4f& operator/=(float n) { x/=n; y/=n; z/=n; w/=n; return *this; }

	///@name Test operators
	int operator==( const Point4f& pt ) const { return ( (pt.x==x) && (pt.y==y) && (pt.z==z) && (pt.w==w) ); }
	int operator!=( const Point4f& pt ) const { return ( (pt.x!=x) || (pt.y!=y) || (pt.z!=z) || (pt.w!=w) ); }

	///@name Access operators
	float& operator[]( int i ) { return (&x)[i]; }
	float  operator[]( int i ) const { return (&x)[i]; }

	///@ Dot product
	float		Dot		 ( const Point4f pt ) const { return x*pt.x + y*pt.y + z*pt.z + w*pt.w; }	///< Dot product
	float		operator%( const Point4f pt ) const { return Dot(pt); }							///< Dot product

	///@name Conversion Methods
	Point2f	XY() const { return Point2f(x,y); }
	Point3f	XYZ() const { return Point3f(x,y,z); }
};

//-------------------------------------------------------------------------------

#endif

