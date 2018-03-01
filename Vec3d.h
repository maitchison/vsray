#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

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
		float nx = x * cos(theta) - y * sin(theta);
		float ny = x * sin(theta) + y * cos(theta);		
		x = nx; y = ny; 
	}
};



struct Vec3d
{
	float x;
	float y;
	float z;

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

	Vec3d normalized() {
		float l = abs();
		return Vec3d(x / l, y / l, z / l);
	}

	Vec3d operator+(Vec3d b)
	{
		return Vec3d(this->x + b.x, this->y + b.y, this->z + b.z);
	}

	Vec3d operator-(Vec3d b)
	{
		return Vec3d(this->x - b.x, this->y - b.y, this->z - b.z);
	}

	Vec3d& operator+=(Vec3d& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
        return *this;
	}

	Vec3d& operator*=(Vec3d& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
        return *this;
	}

	Vec3d& operator*=(float& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
        return *this;
	}

	Vec3d& operator/=(float& rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
        return *this;
	}

	Vec3d& operator/=(Vec3d& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
        return *this;
	}

	Vec3d& operator-=(Vec3d& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
        return *this;
	}

	Vec3d operator*(float b)
	{
		return Vec3d(b * x, b * y, b * z);
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


	/** Rotate around x, then y then z */
	void rotate(float rx, float ry, float rz)
	{
		rotateX(rx);
		rotateY(ry);
		rotateZ(rz);
	}

};

