#include "stdafx.h"
#include "Util.h"
#include "gz.h"

Point3d::Point3d(float x, float y, float z) : x(x), y(y), z(z) {
	//this->x = x;
	//this->y = y;
	//this->z = z;
}

Point3d::Point3d(GzCoord c) : x(c[0]), y(c[1]), z(c[2]) {}

Point3d::Point3d(const Point3d& p) : x(p.x), y(p.y), z(p.z) {}

Edge::Edge(const Point3d& start, const Point3d& end) {
	this->start = start;
	this->end = end;
	this->cur = start;
	this->slopeX = (end.x - start.x) / (end.y - start.y);
	this->slopeZ = (end.z - start.z) / (end.y - start.y);
}

Point3d* test() {
	Point3d* p2 = new Point3d(0, 1, 2);
	Point3d p1(0, 1, 2);
	const Point3d& p3 = p1;
	p3.x == p1.x;
	p2->x == p1.x;
	return p2;
}

