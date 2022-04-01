#include	"gz.h"
//#include	"Util.h"
#ifndef GZRENDER_
#define GZRENDER_

/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */


class Point3d;
class Data;

class Data {
public:
	GzCoord position;
	GzCoord normal;
	GzColor color;
	GzTextureIndex texture;

	Data() = default;
	Data(const Point3d& p);
};

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

	float u;
	float v;

	Point3d() = default;
	Point3d(float x, float y, float z);
	Point3d(GzCoord c);
	Point3d(const Point3d& p);
	Point3d(const Data& d);
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


class GzRender {			/* define a renderer */


public:
	unsigned short	xres;
	unsigned short	yres;
	GzPixel* pixelbuffer;		/* frame buffer array */
	char* framebuffer;

	GzCamera		m_camera;
	short		    matlevel;	        /* top of stack - current xform */
	GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
	GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
	GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
	GzColor		flatcolor;          /* color state for flat shaded triangles */
	int			interp_mode;
	int			numlights;
	GzLight		lights[MAX_LIGHTS];
	GzLight		ambientlight;
	GzColor		Ka, Kd, Ks;
	float		    spec;		/* specular power */
	GzTexture		tex_fun;    /* tex_fun(float u, float v, GzColor color) */

	short	normLevel;

	// Constructors
	GzRender(int xRes, int yRes);
	~GzRender();

	// HW1: Display methods
	int GzDefault();
	int GzBeginRender();
	int GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
	int GzGet(int i, int j, GzIntensity* r, GzIntensity* g, GzIntensity* b, GzIntensity* a, GzDepth* z);

	int GzFlushDisplay2File(FILE* outfile);
	int GzFlushDisplay2FrameBuffer();

	// HW2: Render methods
	int scanLineHalf(Edge& edge12, Edge& edge13, bool isV2Left, Point3d v2);

	int GzPutAttribute(int numAttributes, GzToken* nameList, GzPointer* valueList);
	int GzPutTriangle(int numParts, GzToken* nameList, GzPointer* valueList);

	// HW3
	int dotProduct(GzMatrix a, GzMatrix b, GzMatrix res);
	int GzRender::crossProduct(float* a, float* b, float* res);
	int applyTrans(GzMatrix matrix, GzCoord vertex);

	int GzDefaultCamera();
	int GzPutCamera(GzCamera camera);
	int GzPushMatrix(GzMatrix matrix);
	int GzPopMatrix();

	// Extra methods: NOT part of API - just for general assistance */
	inline int ARRAY(int x, int y) { return (x + y * xres); }	/* simplify fbuf indexing */
	inline short	ctoi(float color) { return(short)((int)(color * ((1 << 12) - 1))); }		/* convert float color to GzIntensity short */

	// Object Translation
	int GzRotXMat(float degree, GzMatrix mat);
	int GzRotYMat(float degree, GzMatrix mat);
	int GzRotZMat(float degree, GzMatrix mat);
	int GzTrxMat(GzCoord translate, GzMatrix mat);
	int GzScaleMat(GzCoord scale, GzMatrix mat);

	// HW4
	float GzRender::getProduct(float* a, float* b);
	int GzRender::normToImageSpace(GzMatrix m, GzCoord n);
	int GzRender::GzVecNomalize(float* x);
	int GzRender::lightingEquation(Data& data, GzCoord N);
	int GzRender::GzXnormPush(GzMatrix matrix);
	int GzRender::GzVecMultiplyConst(float* x, float y, float* res);
	int GzRender::GzCoordMinus(GzCoord x, GzCoord y);
	int GzRender::GzVecPlus(float* x, float* y);

	//int GzRender::SLflat(Data data);
	int GzRender::scanLine(Data* data);
	int GzRender::advanceEdge(Edge& e, float delta);
	int GzRender::advanceSpan(Span& s, float delta);

//	int GzRender::interpolate_uv(Point3d start, Point3d end, Point3d& p, float delta);
	void GzRender::worldToScreenSpace(GzTextureIndex& tex, float zs);
	void GzRender::screenToWorldSpace(GzTextureIndex& tex, float zs);
};

#endif
