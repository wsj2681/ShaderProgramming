#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define VERTICES_TRIANGLE 3
#define VERTICES_SQUARE 6

struct float3
{
	float3(float x, float y, float z)
		:x(x),
		y(y),
		z(z)
	{
	
	}
	float x;
	float y;
	float z;
};