#include "stdafx.h"
#include "MathCore.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

const f64 PI64 = M_PI;
const f32 PI = (f32)M_PI;

const f64 EPSILON64 = 0.00000001;
const f32 EPSILON = 0.000001f;

f32 RandomFloat(f32 min, f32 max)
{
	return ((f32)rand() / (f32)RAND_MAX) * (max - min) + min;
}

//I'm still not sure if these Vector operations
//shouldn't be taking pointers to avoid copying so much data,
//but I'mma go for simplicity first and see how much
//it actually slows things down.
Vector ScalarMultiply(Vector v, float f)
{
	v.x = v.x * f;
	v.y = v.y * f;
	v.z = v.z * f;
	return v;
}

Vector VectorAdd(Vector v1, Vector v2)
{
	v1.x = v1.x + v2.x;
	v1.y = v1.y + v2.y;
	v1.z = v1.z + v2.z;
	return v1;
}

Vector VectorSubtract(Vector v1, Vector v2)
{
	v1.x = v1.x - v2.x;
	v1.y = v1.y - v2.y;
	v1.z = v1.z - v2.z;
	return v1;
}

//takes three inputs, returns either the first or second parameter depending on which one is closest to the third.
float findMinDiff(float first, float second, float check)
{
	float diff1 = abs(check - first);
	float diff2 = abs(check - second);
	return (diff1 < diff2) ? first : second;
}

//takes three inputs, returns either the first or second parameter depending on which one is furthest from the third.
float findMaxDiff(float first, float second, float check)
{
	float diff1 = abs(check - first);
	float diff2 = abs(check - second);
	return (diff1 > diff2) ? first : second;
}