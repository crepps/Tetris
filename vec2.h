#pragma once

struct vec2
{
	float x, y;

	vec2() {}

	vec2(float a, float b)
		:x(a), y(b) {}

	bool operator==(const vec2 &arg)
	{
		return x == arg.x && y == arg.y ? true : false;
	}

	bool operator!=(const vec2 &arg)
	{
		return x != arg.x || y != arg.y ? true : false;
	}

	vec2 operator+(const vec2 &arg)
	{
		return vec2(x + arg.x, y + arg.y);
	}

	vec2 operator-(const vec2 &arg)
	{
		return vec2(x - arg.x, y - arg.y);
	}

	vec2 operator*(const vec2 &arg)
	{
		return vec2(x * arg.x, y * arg.y);
	}

	vec2 operator/(const vec2 &arg)
	{
		return vec2(x / arg.x, y / arg.y);
	}

	void operator+=(const vec2 &arg)
	{
		x += arg.x;
		y += arg.y;
	}

	void operator-=(const vec2 &arg)
	{
		x -= arg.x;
		y -= arg.y;
	}

	void operator*=(const vec2 &arg)
	{
		x *= arg.x;
		y *= arg.y;
	}

	void operator/=(const vec2 &arg)
	{
		x /= arg.x;
		y /= arg.y;
	}
};