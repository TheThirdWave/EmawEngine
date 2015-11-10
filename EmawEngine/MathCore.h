#pragma once

#include "Types.h"

const extern f64 PI64;
const extern f32 PI;
const extern f64 EPSILON64;
const extern f32 EPSILON;

f32 RandomFloat(f32 min = 0.f, f32 max = 1.f);
Vector ScalarMultiply(Vector, float);
Vector VectorAdd(Vector, Vector);
Vector VectorSubtract(Vector, Vector);
float findMinDiff(float, float, float);
float findMaxDiff(float, float, float);