#pragma once

#include "gz.h"

class Point3d {
public:
	float x;
	float y;
	float z;

	float r;
	float g;
	float b;

	float nx;
	float ny;
	float nz;

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

class Span {
public:
	Point3d start;	// start = L(X,Z)
	Point3d end;	// end = R(X,Z)
	Point3d cur;	// L(X,Z)
	float slopeZ; // dz/dx	(RZ-LZ)/(RX-LX)
	
	Span() = default;
	Span(const Point3d& start, const Point3d& end);

};


//struct Span {
//	Point3d start;	// start = L(X,Z)
//	Point3d end;	// end = R(X,Z)
//	Point3d cur;	// L(X,Z)
//	float slopeZ; // dz/dx	(RZ-LZ)/(RX-LX)
//};

struct Data {
	GzCoord v;
	GzCoord normal;
	GzColor color;
};

