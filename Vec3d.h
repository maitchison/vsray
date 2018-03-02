#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <smmintrin.h>

struct Vec2d
{
	float x;
	float y;
	
	Vec2d()
	{
		x = 0;
		y = 0;
	}

	Vec2d(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	float abs() {
		return sqrtf(abs2());
	}

	float abs2() {
		return x * x + y * y;
	}

	float angle() {
		return atan2(y, x);
	}
	
	Vec2d normalized() {
		float l = abs();
		return Vec2d(x / l, y / l);
	}

	/* Overload + operator to add two Box objects. */
	Vec2d operator+(Vec2d b)
	{
		return Vec2d(this->x + b.x, this->y + b.y);
	}

	Vec2d operator-(Vec2d b)
	{
		return Vec2d(this->x - b.x, this->y - b.y);
	}

	/* Overload + operator to add two Box objects. */
	Vec2d operator*(float b)
	{
		return Vec2d(b * x, b * y);
	}
	
	void rotate(float theta)
	{
		float sn = sinf(theta);
		float cs = cosf(theta);
		float nx = x * cs - y * sn;
		float ny = x * sn + y * cs;
		x = nx; y = ny; 
	}
};


struct Vec3d
{	
	float x, y, z;

	Vec3d()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vec3d(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline float abs() {
		return sqrtf(abs2());
	}

	inline float abs2() {
		return x * x + y * y + z * z;
	}

	inline float xAngle() {
		return atan2(y, z);
	}

	inline float yAngle() {
		return atan2(z, x);
	}

	inline float zAngle() {
		return atan2(y, x);
	}

	inline static float Dot(Vec3d a, Vec3d b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline Vec3d normalized() {
		float l = 1.0/abs();
		return Vec3d(x * l, y * l, z * l);
	}

	inline Vec3d operator+(Vec3d b)
	{
		return Vec3d(this->x + b.x, this->y + b.y, this->z + b.z);
	}

	inline Vec3d operator-(Vec3d b)
	{
		return Vec3d(this->x - b.x, this->y - b.y, this->z - b.z);
	}

	inline Vec3d& operator+=(Vec3d& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
        return *this;
	}

	inline Vec3d& operator*=(Vec3d& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
        return *this;
	}

	inline Vec3d& operator*=(float& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
        return *this;
	}

	inline Vec3d& operator/=(float& rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
        return *this;
	}

	inline Vec3d& operator/=(Vec3d& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
        return *this;
	}

	inline Vec3d& operator-=(Vec3d& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
        return *this;
	}

	inline Vec3d operator*(float b)
	{
		return Vec3d(b * x, b * y, b * z);
	}

	void rotateX(float theta)
	{
		float sn = sinf(theta);
		float cs = cosf(theta);
		float nx = x;
		float ny = y * cs - z * sn;
		float nz = y * sn + z * cs;
		x = nx; y = ny; z = nz;
	}

	void rotateY(float theta)
	{
		float sn = sinf(theta);
		float cs = cosf(theta);
		float nx = x * cs + z * sn;
		float ny = y;
		float nz = x * -sn + z * cs;
		x = nx; y = ny; z = nz;
	}

	void rotateZ(float theta)
	{
		float sn = sinf(theta);
		float cs = cosf(theta);
		float nx = x * cs - y * sn;
		float ny = x * sn + y * cs;
		float nz = z;
		x = nx; y = ny; z = nz;
	}

	// Rotate around x, then y then z
	// could be faster if combined into one matrix
	void rotate(float rx, float ry, float rz)
	{		
		rotateX(rx);
		rotateY(ry);
		rotateZ(rz);
	}

};



// Simple vector class

/*
typedef class vector3 Vec3d;


_MM_ALIGN16 class vector3
{
public:
	// constructors
	inline vector3() : mmvalue(_mm_setzero_ps()) {}
	inline vector3(float x, float y, float z) : mmvalue(_mm_set_ps(0, z, y, x)) {}
	inline vector3(__m128 m) : mmvalue(m) {}

	// arithmetic operators with vector3
	inline vector3 operator+(const vector3& b) const { return _mm_add_ps(mmvalue, b.mmvalue); }
	inline vector3 operator-(const vector3& b) const { return _mm_sub_ps(mmvalue, b.mmvalue); }
	inline vector3 operator*(const vector3& b) const { return _mm_mul_ps(mmvalue, b.mmvalue); }
	inline vector3 operator/(const vector3& b) const { return _mm_div_ps(mmvalue, b.mmvalue); }

	// op= operators
	inline vector3& operator+=(const vector3& b) { mmvalue = _mm_add_ps(mmvalue, b.mmvalue); return *this; }
	inline vector3& operator-=(const vector3& b) { mmvalue = _mm_sub_ps(mmvalue, b.mmvalue); return *this; }
	inline vector3& operator*=(const vector3& b) { mmvalue = _mm_mul_ps(mmvalue, b.mmvalue); return *this; }
	inline vector3& operator/=(const vector3& b) { mmvalue = _mm_div_ps(mmvalue, b.mmvalue); return *this; }

	// arithmetic operators with float
	inline vector3 operator+(float b) const { return _mm_add_ps(mmvalue, _mm_set1_ps(b)); }
	inline vector3 operator-(float b) const { return _mm_sub_ps(mmvalue, _mm_set1_ps(b)); }
	inline vector3 operator*(float b) const { return _mm_mul_ps(mmvalue, _mm_set1_ps(b)); }
	inline vector3 operator/(float b) const { return _mm_div_ps(mmvalue, _mm_set1_ps(b)); }

	// op= operators with float
	inline vector3& operator+=(float b) { mmvalue = _mm_add_ps(mmvalue, _mm_set1_ps(b)); return *this; }
	inline vector3& operator-=(float b) { mmvalue = _mm_sub_ps(mmvalue, _mm_set1_ps(b)); return *this; }
	inline vector3& operator*=(float b) { mmvalue = _mm_mul_ps(mmvalue, _mm_set1_ps(b)); return *this; }
	inline vector3& operator/=(float b) { mmvalue = _mm_div_ps(mmvalue, _mm_set1_ps(b)); return *this; }

	// cross product
	inline vector3 cross(const vector3& b) const
	{
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(mmvalue, mmvalue, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b.mmvalue, b.mmvalue, _MM_SHUFFLE(3, 1, 0, 2))),
			_mm_mul_ps(_mm_shuffle_ps(mmvalue, mmvalue, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b.mmvalue, b.mmvalue, _MM_SHUFFLE(3, 0, 2, 1)))
		);
	}

	// my functions
	inline static float Dot(Vec3d a, Vec3d b)
	{
		return a.dot(b);
	}

	void rotateX(float theta)
	{
		float nx = x;
		float ny = y * cos(theta) - z * sin(theta);
		float nz = y * sin(theta) + z * cos(theta);
		x = nx; y = ny; z = nz;
	}

	void rotateY(float theta)
	{
		float nx = x * cos(theta) + z * sin(theta);
		float ny = y;
		float nz = x * -sin(theta) + z * cos(theta);
		x = nx; y = ny; z = nz;
	}

	void rotateZ(float theta)
	{
		float nx = x * cos(theta) - y * sin(theta);
		float ny = x * sin(theta) + y * cos(theta);
		float nz = z;
		x = nx; y = ny; z = nz;
	}

	inline float abs() const { return length(); }
	inline float abs2() const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, mmvalue, 0x7F)); }
	

	// dot product with another vector
	inline float dot(const vector3& b) const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, b.mmvalue, 0x71)); }
	// length of the vector
	inline float length() const { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0x71))); }
	// 1/length() of the vector
	inline float rlength() const { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0x71))); }
	// returns the vector scaled to unit length
	inline vector3 normalized() const { return _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0x7F))); }

	// overloaded operators that ensure alignment
	inline void* operator new[](size_t x) { return _aligned_malloc(x, 16); }
	inline void operator delete[](void* x) { if (x) _aligned_free(x); }

	// Member variables
	union
	{
		struct { float x, y, z; };
		__m128 mmvalue;
	};
};

inline vector3 operator+(float a, const vector3& b) { return b + a; }
inline vector3 operator-(float a, const vector3& b) { return vector3(_mm_set1_ps(a)) - b; }
inline vector3 operator*(float a, const vector3& b) { return b * a; }
inline vector3 operator/(float a, const vector3& b) { return vector3(_mm_set1_ps(a)) / b; }

*/