#pragma once

#include <Util.h>

const float EPSILON = 0.00001;

struct Color
{
	float r;
	float g;
	float b;
	float a;

	Color()
	{
		r = g = b = 0.0f;
	}

	Color(float _r, float _g, float _b, float _a = 1.0)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	int int24()
	{
		int _r = clip(int(r * 255), 0, 255);
		int _g = clip(int(g * 255), 0, 255);
		int _b = clip(int(b * 255), 0, 255);
		return (_r)+(_g << 8) + (_b << 16);
	}

	/* Overload + operator to add two Box objects. */
	Color operator+(Color other)
	{
		return Color(this->r + other.r, this->g + other.g, this->b + other.b);
	}

	Color operator-(Color other)
	{
		return Color(this->r - other.r, this->g - other.g, this->b - other.b);
	}

	Color operator*(Color other)
	{
		return Color(this->r * other.r, this->g * other.g, this->b * other.b);
	}

	/* Overload + operator to add two Box objects. */
	Color operator*(float factor)
	{
		return Color(r * factor, g * factor, b * factor);
	}


};
