add three struct in 'gz.h'
POINT_XYZ, Edge, SpanXZ, Span

add a function def in 'rend,h'
int scanLine(Edge& edge12, Edge& edge13, bool isV2Left, GzCoord v2);

add two functions in 'rend.cpp'
1. bool GzCoordCmp(const GzCoord* v1, const GzCoord* v2)	-- to implement std::sort funciton
2. int GzRender::scanLine(Edge& edgeShort, Edge& edgeLong, bool isV2Left, GzCoord stopVertex)

