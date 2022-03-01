#include "stdafx.h"
#include "Util.h"
#include "gz.h"

Data::Data(const Point3d& p) {
	v[0] = p.x;
	v[1] = p.y;
	v[2] = p.z;
	color[0] = p.r;
	color[1] = p.g;
	color[2] = p.b;
	normal[0] = p.nx;
	normal[1] = p.ny;
	normal[2] = p.nz;
}

Point3d::Point3d(float x, float y, float z) : x(x), y(y), z(z) {
	//this->x = x;
	//this->y = y;
	//this->z = z;
}

Point3d::Point3d(GzCoord c) : x(c[0]), y(c[1]), z(c[2]) {}

Point3d::Point3d(const Point3d& p) : x(p.x), y(p.y), z(p.z) {}

Point3d::Point3d(const Data& d) : x(d.v[0]), y(d.v[1]), z(d.v[2]), 
						r(d.color[0]), g(d.color[1]), b(d.color[2]),
						nx(d.normal[0]), ny(d.normal[1]), nz(d.normal[2]) {}

Edge::Edge(const Point3d& start, const Point3d& end) {
	this->start = start;
	this->end = end;
	this->cur = start;
	this->slopeX = (end.x - start.x) / (end.y - start.y);
	this->slopeZ = (end.z - start.z) / (end.y - start.y);
}

Span::Span(const Point3d& start, const Point3d& end) {
	this->start = start;
	this->end = end;
	this->cur = start;
	this->slopeZ = (end.z - start.z) / (end.x - start.x);
}

Point3d* test() {
	Point3d* p2 = new Point3d(0, 1, 2);
	Point3d p1(0, 1, 2);
	const Point3d& p3 = p1;
	p3.x == p1.x;
	p2->x == p1.x;
	return p2;
}

