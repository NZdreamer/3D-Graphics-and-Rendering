#pragma once

#include "gz.h"

class Point3d {
public:
	float x;
	float y;
	float z;

	Point3d() = default;
	Point3d(float x, float y, float z);
	Point3d(GzCoord c);
	Point3d(const Point3d& p);
};

class Edge
{
public:
	Point3d start;	// start vertex
	Point3d end;	// end vertex
	Point3d cur;
	float slopeX;
	float slopeZ;

	Edge(const Point3d& start, const Point3d& end);
};

// Span struct defination
struct SpanXZ {
	float x;
	float z;
};

struct Span {
	SpanXZ start;	// start = L(X,Z)
	SpanXZ end;	// end = R(X,Z)
	SpanXZ cur;	// L(X,Z)
	float slopeZ; // dz/dx	(RZ-LZ)/(RX-LX)
};

struct Data {
	GzCoord v;
	GzCoord normal;
	GzColor color;
};

