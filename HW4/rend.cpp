/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include	"Util.h"

// hw2
#include <iostream>
#include <cstring>
#include <sstream>
#include <algorithm>

#define PI (float) 3.14159265358979323846

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
/* HW 3.1
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
*/
	float r = degree * PI / 180;
	GzMatrix rotX = {
		1,	0,	0,	0,
		0, cos(r), -sin(r), 0,
		0, sin(r), cos(r), 0,
		0,	0,	0,	1
	};
	memcpy(mat, rotX, sizeof(GzMatrix));

	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
/* HW 3.2
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
*/
	float r = degree * PI / 180;
	GzMatrix rotY = {
		cos(r), 0, sin(r), 0,
		0, 1, 0, 0,
		-sin(r), 0, cos(r), 0,
		0, 0, 0, 1
	};
	memcpy(mat, rotY, sizeof(GzMatrix));

	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
/* HW 3.3
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
*/
	float r = degree * PI / 180;
	//mat[0][0] = cos(radius);
	//mat[0][1] = -1 * sin(radius);
	//mat[1][0] = sin(radius);
	//mat[1][1] = cos(radius);
	GzMatrix rotZ = {
	cos(r), -sin(r), 0, 0,
	sin(r), cos(r), 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};
	memcpy(mat, rotZ, sizeof(GzMatrix));

	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
/* HW 3.4
// Create translation matrix
// Pass back the matrix using mat value
*/
	GzMatrix trx = {
	1, 0, 0, translate[0],
	0, 1, 0, translate[1],
	0, 0, 1, translate[2],
	0, 0, 0, 1
	};
	memcpy(mat, trx, sizeof(GzMatrix));

	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
/* HW 3.5
// Create scaling matrix
// Pass back the matrix using mat value
*/
	GzMatrix scaleMat = {
	scale[0], 0, 0, 0,
	0, scale[1], 0 , 0,
	0, 0, scale[2], 0,
	0 ,0 ,0, 1
	};
	memcpy(mat, scaleMat, sizeof(GzMatrix));

	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	if (xRes > MAXXRES) {
		xres = MAXXRES;
	}
	else {
		xres = xRes;
	}
	if (yRes > MAXXRES) {
		yres = MAXXRES;
	}
	else {
		yres = yRes;
	}
	framebuffer = new char[3 * xres * yres];
	pixelbuffer = new GzPixel[xres * yres];
	/*framebuffer = (char*) malloc (3 * sizeof(char) * xRes * yRes);*/

/* HW 3.6
- setup Xsp and anything only done once 
- init default camera 
*/ 
	GzMatrix temp =
	{
		xres / 2,	0,	0,	xres / 2,
		0,	-yres / 2, 0, yres / 2,
		0,	0,	INT_MAX, 0,
		0,	0,	0,	1
	};
	memcpy(Xsp, temp, sizeof(GzMatrix));

	GzCoord tempPos = { DEFAULT_IM_X , DEFAULT_IM_Y, DEFAULT_IM_Z };
	memcpy(m_camera.position, tempPos, sizeof(GzCoord));
	GzCoord tempLookat = { 0, 0, 0 };
	memcpy(m_camera.lookat, tempLookat, sizeof(GzCoord));
	GzCoord tempWorldup = { 0, 1, 0 };
	memcpy(m_camera.worldup, tempWorldup, sizeof(GzCoord));
	m_camera.FOV = DEFAULT_FOV;
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete[]framebuffer;
	delete[]pixelbuffer;

}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */

	for (int i = 0; i < xres * yres; i++) {
		pixelbuffer[i] = { 2000, 555, 555, 1, INT_MAX };	// initailze z to INT_MAX
	}
	// HW 4
	numlights = 0;
	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
/* HW 3.7 
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	float rD = tan((m_camera.FOV * PI / 180) / 2);	// 1/d
	GzMatrix Xpi =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, rD, 0,
		0, 0, rD, 1
	};
	memcpy(m_camera.Xpi, Xpi, sizeof(GzMatrix));

	// compute Z
	GzCoord tmpZ;
	GzCoord tmpC;
	GzCoord tmpL;
	GzCoord tmpCL;
	memcpy(tmpC, m_camera.position, sizeof(GzCoord));
	memcpy(tmpL, m_camera.lookat, sizeof(GzCoord));
	tmpCL[0] = tmpL[0] - tmpC[0];
	tmpCL[1] = tmpL[1] - tmpC[1];
	tmpCL[2] = tmpL[2] - tmpC[2];

	float tmpCLNorm = sqrt(pow(tmpCL[0], 2) + pow(tmpCL[1], 2) + pow(tmpCL[2], 2));
	tmpZ[0] = tmpCL[0] / tmpCLNorm;
	tmpZ[1] = tmpCL[1] / tmpCLNorm;
	tmpZ[2] = tmpCL[2] / tmpCLNorm;

	// compute Y
	GzCoord up;
	memcpy(up, m_camera.worldup, sizeof(GzCoord));
	GzCoord upPrime;
	float upZprodct = up[0] * tmpZ[0] + up[1] * tmpZ[1] + up[2] * tmpZ[2];
	upPrime[0] = up[0] - upZprodct * tmpZ[0];
	upPrime[1] = up[1] - upZprodct * tmpZ[1];
	upPrime[2] = up[2] - upZprodct * tmpZ[2];

	float upPrimeNorm = sqrt(pow(upPrime[0], 2) + pow(upPrime[1], 2) + pow(upPrime[2], 2));
	GzCoord tmpY;
	tmpY[0] = upPrime[0] / upPrimeNorm;
	tmpY[1] = upPrime[1] / upPrimeNorm;
	tmpY[2] = upPrime[2] / upPrimeNorm;

	// compute X
	GzCoord tmpX = { tmpY[1] * tmpZ[2] - tmpY[2] * tmpZ[1],
		tmpY[2] * tmpZ[0] - tmpY[0] * tmpZ[2],
		tmpY[0] * tmpZ[1] - tmpY[1] * tmpZ[0] };

	// compute other
	float XCprod = tmpX[0] * tmpC[0] + tmpX[1] * tmpC[1] + tmpX[2] * tmpC[2];
	float YCprod = tmpY[0] * tmpC[0] + tmpY[1] * tmpC[1] + tmpY[2] * tmpC[2];
	float ZCprod = tmpZ[0] * tmpC[0] + tmpZ[1] * tmpC[1] + tmpZ[2] * tmpC[2];

	GzMatrix Xiw =
	{
		tmpX[0], tmpX[1],tmpX[2], -XCprod,
		tmpY[0], tmpY[1],tmpY[2], -YCprod,
		tmpZ[0], tmpZ[1],tmpZ[2], -ZCprod,
		0, 0, 0, 1
	};
	memcpy(m_camera.Xiw, Xiw, sizeof(GzMatrix));

	// init Ximage
	matlevel = -1;

	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw);
	GzXnormPush(m_camera.Xiw);

	return GZ_SUCCESS;
}


int GzRender::dotProduct(GzMatrix a, GzMatrix b, GzMatrix res)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float temp = 0;
			for (int k = 0; k < 4; k++) {
				temp += a[i][k] * b[k][j];
			}
			res[i][j] = temp;
		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	memcpy(m_camera.position, camera.position, sizeof(GzCoord));
	memcpy(m_camera.lookat, camera.lookat, sizeof(GzCoord));
	memcpy(m_camera.worldup, camera.worldup, sizeof(GzCoord));
	m_camera.FOV = camera.FOV;

	return GZ_SUCCESS;	
}

int GzRender::GzXnormPush(GzMatrix matrix) {
	if (matlevel >= MATLEVELS || matlevel < 0) {
		return GZ_FAILURE;
	}
	else {
		GzMatrix newMatrix;
		dotProduct(Xnorm[matlevel], matrix, newMatrix);
		memcpy(Xnorm[matlevel], newMatrix, sizeof(GzMatrix));
	}

	return GZ_SUCCESS;
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
/* HW 3.9 
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (matlevel >= MATLEVELS) {
		return GZ_FAILURE;
	}

	if (matlevel == -1) {
		memcpy(Ximage[0], matrix, sizeof(GzMatrix));
		matlevel = 0;
	}
	else {
		GzMatrix newMatrix;
		dotProduct(Ximage[matlevel], matrix, newMatrix);
		matlevel++;
		memcpy(Ximage[matlevel], newMatrix, sizeof(GzMatrix));
	}
	
	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (matlevel < 0) return GZ_FAILURE;

	matlevel--;

	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	if (i < 0 || i > xres - 1 || j < 0 || j > yres - 1) {
		return GZ_FAILURE;
	}

	// clamp to 0-4095
	if (r < 0) r = 0;
	if (r > 4095) r = 4095;
	if (g < 0) g = 0;
	if (g > 4095) g = 4095;
	if (b < 0) b = 0;
	if (b > 4095) b = 4095;

	// write pixal
	pixelbuffer[ARRAY(i, j)] = { r, g, b, a, z };

	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
	r = &pixelbuffer[ARRAY(i, j)].red;
	g = &pixelbuffer[ARRAY(i, j)].green;
	b = &pixelbuffer[ARRAY(i, j)].blue;
	a = &pixelbuffer[ARRAY(i, j)].alpha;
	z = &pixelbuffer[ARRAY(i, j)].z;

	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	std::stringstream str;
	str << "P6 " << xres << " " << yres << " 255\r";

	int length = str.str().length();
	char* head = new char[length + 1];

	strcpy(head, str.str().c_str());

	//sprintf(head, "%s%d%s%d%s", "P6 ", xres, " ", yres, " 255\r");

	fputs(head, outfile);
	//std::cout << head << std::endl;	

	char* buffer = new char[3 * xres * yres];

	for (int i = 0; i < 3 * xres * yres; i++) {
		// conversion of GzIntensity(16-bit) to 8-bit rgb component
		// Drop 4-bits by right-shifting and then use low byte of GzIntensity value
		if (i % 3 == 0)
			buffer[i] = (pixelbuffer[i / 3].red >> 4);
		if (i % 3 == 1)
			buffer[i] = (pixelbuffer[i / 3].green >> 4);
		if (i % 3 == 2)
			buffer[i] = (pixelbuffer[i / 3].blue >> 4);
	}
	fputs(buffer, outfile);

	
	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	for (int i = 0; i < 3 * xres * yres; i++) {
		// conversion of GzIntensity(16-bit) to 8-bit rgb component
		// Drop 4-bits by right-shifting and then use low byte of GzIntensity value
		if (i % 3 == 0)
			framebuffer[i] = (pixelbuffer[i / 3].blue >> 4);
		if (i % 3 == 1)
			framebuffer[i] = (pixelbuffer[i / 3].green >> 4);
		if (i % 3 == 2)
			framebuffer[i] = (pixelbuffer[i / 3].red >> 4);
	}

	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++) {
		if (nameList[i] == GZ_RGB_COLOR) {
			flatcolor[0] = ((GzColor*)valueList[i])[0][0];
			flatcolor[1] = ((GzColor*)valueList[i])[0][1];
			flatcolor[2] = ((GzColor*)valueList[i])[0][2];
		}
		if (nameList[i] == GZ_DIRECTIONAL_LIGHT) {
			lights[numlights] = ((GzLight*)valueList[i])[0];
			numlights++;
		}
		if (nameList[i] == GZ_AMBIENT_LIGHT) {
			ambientlight = ((GzLight*)valueList[i])[0];
		}
		if (nameList[i] == GZ_DIFFUSE_COEFFICIENT) {
			Kd[0] = ((GzColor*)valueList[i])[0][0];
			Kd[1] = ((GzColor*)valueList[i])[0][1];
			Kd[2] = ((GzColor*)valueList[i])[0][2];
		}
		if (nameList[i] == GZ_INTERPOLATE) {
			interp_mode = ((int*)valueList[i])[0];
		}
		if (nameList[i] == GZ_AMBIENT_COEFFICIENT) {
			Ka[0] = ((GzColor*)valueList[i])[0][0];
			Ka[1] = ((GzColor*)valueList[i])[0][1];
			Ka[2] = ((GzColor*)valueList[i])[0][2];
		}
		if (nameList[i] == GZ_SPECULAR_COEFFICIENT) {
			Ks[0] = ((GzColor*)valueList[i])[0][0];
			Ks[1] = ((GzColor*)valueList[i])[0][1];
			Ks[2] = ((GzColor*)valueList[i])[0][2];
		}
		if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT) {
			spec = ((float*)valueList[i])[0];
		}

	}
	

	return GZ_SUCCESS;
}

bool GzCoordCmp(const Data d1, const Data d2) {
	return d1.v[1] < d2.v[1];
}

float GzRender::getProduct(float* a, float* b) {
	float res = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return res;
}

int GzRender::normToImageSpace(GzMatrix m, GzCoord n) {
	GzCoord res;
	for (int i = 0; i < 3; i++) {
		res[i] = m[i][0] * n[0] + m[i][1] * n[1] + m[i][2] * n[2];
	}
	memcpy(n, res, sizeof(GzCoord));
	return GZ_SUCCESS;
}

int GzRender::GzVecNomalize(float* x) {
	float base = sqrt(pow(x[0], 2) + pow(x[1], 2) + pow(x[2], 2));
	for (int i = 0; i < 3; i++) {
		x[i] = x[i] / base;
	}
	return GZ_SUCCESS;
}

int GzRender::GzVecMultiplyConst(float* x, float y, float* res) {
	for (int i = 0; i < 3; i++) {
		res[i] = x[i] * y;
	}
	return GZ_SUCCESS;
}

int GzRender::GzCoordMinus(GzCoord x, GzCoord y) {
	for (int i = 0; i < 3; i++) {
		x[i] = x[i] - y[i];
	}
	return GZ_SUCCESS;
}

int GzRender::GzVecPlus(float* x, float* y) {
	for (int i = 0; i < 3; i++) {
		x[i] = x[i] + y[i];
	}
	return GZ_SUCCESS;
}

// calculate lighting equation a.k.a, color each pixel
int GzRender::lightingEquation(Data& data) {
	float E[3] = { 0, 0, 1 };
	GzCoord N;
	memcpy(N, data.normal, sizeof(GzCoord));
	normToImageSpace(Xnorm[matlevel], N);
	GzVecNomalize(N);
	GzCoord L;
	GzCoord R;
	GzColor Ie;
	GzColor Ia;
	memcpy(Ia, ambientlight.color, sizeof(GzColor));

	GzColor sSum;	// sepcular sum
	GzColor dSum;	// diffusion sum

	float NLprod;
	float NEprod = getProduct(N, E);

	for (int i = 0; i < numlights; i++) {
		memcpy(L, lights[i].direction, sizeof(GzCoord));
		GzVecNomalize(L);
		memcpy(Ie, lights[i].color, sizeof(GzColor));

		NLprod = getProduct(N, L);
		if (NLprod < 0 && NEprod < 0) {
			for (int i = 0; i < 3; i++) {
				N[i] = -N[i];
			}
		}
		else if (NLprod > 0 && NEprod > 0) {}
		else {
			continue;
		}

		// calculate R
		GzCoord tmp;
		GzVecMultiplyConst(N, getProduct(N, L) * 2, tmp);
		GzCoordMinus(tmp, L);
		memcpy(R, tmp, sizeof(GzCoord));

		// sepcular
		float REprod = getProduct(R, E);
		if (REprod < 0) REprod = 0;
		GzVecMultiplyConst(Ie, pow(REprod, spec), tmp);
		GzVecPlus(sSum, tmp);
		
		//diffusion
		GzVecMultiplyConst(Ie, getProduct(N, L), tmp);
		GzVecPlus(dSum, tmp);
	}

	GzColor C;
	for (int i = 0; i < 3; i++) {
		C[i] = Ks[i] * sSum[i] + Kd[i] * dSum[i] + Ka[i] * Ia[i];
	}

	memcpy(data.color, C, sizeof(GzColor));

	return GZ_SUCCESS;
}

// implement scanline algo
int GzRender::scanLine(Edge& edgeShort, Edge& edgeLong, bool isV2Left, Point3d stopVertex, int z) {

	SpanXZ L;
	SpanXZ R;

	// span edge12 and edge13
	// include right and bottom
	//while (edge13.current.y <= v3[1])
	while (
		round(edgeShort.cur.y) < 256 && edgeShort.cur.y <= stopVertex.y) {	// include bottom, exclude top pixel

		// span setup and advance
		// span setup
		if (isV2Left) {
			L.x = edgeShort.cur.x;
			L.z = edgeShort.cur.z;
			R.x = edgeLong.cur.x;
			R.z = edgeLong.cur.z;
		}
		else {
			L.x = edgeLong.cur.x;
			L.z = edgeLong.cur.z;
			R.x = edgeShort.cur.x;
			R.z = edgeShort.cur.z;
		}
		Span span = { L, R, L, (R.z - L.z) / (R.x - L.x) };
		//*span.current.y = *edge13.current.y;
		float deltaX = ceil(L.x) - L.x;

		// span advance				
		span.cur.x = span.cur.x + deltaX;
		span.cur.z = span.cur.z + span.slopeZ * deltaX;

		while (round(span.cur.x) < 256 && span.cur.x <= span.end.x) {	// include right, exclude left pixel
			// Test interpolated-Z against FB-Z for each pixel - lowest Z wins
			// Write color value into FB pixel(default or computed color)	??? write into pixelbuffer ???
			//int curX = round(span.current.x);
			//int curY = round(edge13.current.y);
			int position = ARRAY(round(span.cur.x), round(edgeLong.cur.y));
			//if (position >= 256 * 256) continue;
			if (span.cur.x >= 0 && edgeLong.cur.y >= 0 && pixelbuffer[position].z > span.cur.z) {
				// GzPut(span.cur.x, edgeLong.cur.y,
				// 	ctoi(flatcolor[0]), ctoi(flatcolor[1]), ctoi(flatcolor[2]), 1, span.cur.z);


				//pixelbuffer[position].red = ctoi(flatcolor[0]);
				//pixelbuffer[position].green = ctoi(flatcolor[1]);
				//pixelbuffer[position].blue = ctoi(flatcolor[2]);
				//pixelbuffer[position].z = span.cur.z;
			}
			// span advance				
			span.cur.x += 1;
			span.cur.z += span.slopeZ;
		}

		edgeShort.cur.x += edgeShort.slopeX;
		edgeShort.cur.y += 1;
		edgeShort.cur.z += edgeShort.slopeZ;

		edgeLong.cur.x += edgeLong.slopeX;
		edgeLong.cur.y += 1;
		edgeLong.cur.z += edgeLong.slopeZ;
	}
	return GZ_SUCCESS;
}



int GzRender::applyTrans(GzMatrix matrix, GzCoord vertex) {
	float v[4] = { vertex[0], vertex[1], vertex[2], 1 };
	float fourDres[4];
	for (int i = 0; i < 4; i++) {
		float tmp = 0;
		for (int j = 0; j < 4; j++) {
			tmp += matrix[i][j] * v[j];
		}
		fourDres[i] = tmp;
	}
	for (int i = 0; i < 3; i++) {
		vertex[i] = fourDres[i] / fourDres[3];
	}

	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int numParts, GzToken* nameList, GzPointer* valueList)
/* numParts - how many names and values */
{

	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
		  GZ_NULL_TOKEN:		do nothing - no values
		  GZ_POSITION:		3 vert positions in model space
	-- Invoke the rastrizer/scanline framework
	-- Return error code
	*/

	// get the three vertex
	GzCoord v1;
	GzCoord v2;
	GzCoord v3;

	Data data[3];

	for (int i = 0; i < numParts; i++) {
		if (nameList[i] == GZ_NULL_TOKEN) return GZ_FAILURE;

		if (nameList[i] == GZ_POSITION) {
			GzCoord* vertexList = (GzCoord*)valueList[0];
			for (int i = 0; i < 3; ++i) {
				memcpy(data[i].v, vertexList[i], sizeof(GzCoord));
			}
		}

		if (nameList[i] == GZ_NORMAL) {
			GzCoord* normalList = (GzCoord*)valueList[1];
			for (int i = 0; i < 3; ++i) {
				memcpy(data[i].normal, normalList[i], sizeof(GzCoord));
			}
		}
	}

	// HW 3
	// apply the set of transformations to every vertex of every triangle
	for (int i = 0; i < 3; ++i) {
		applyTrans(Ximage[matlevel], data[i].v);
	}

	// ignore triangles that are behind the view plane:
	// skip any triangle with a negative screen-z vertex
	for (int i = 0; i < 3; ++i) {
		if (data[i].v[2] < 0) {
			return  GZ_SUCCESS;
		}
	}

	// select mode
	//switch (interp_mode) {
	//case GZ_FLAT:
	//	SLflat(data);
	//	break;
	//case GZ_COLOR:
	//	SLGouraud();
	//	break;
	//case GZ_NORMALS:
	//	SLPhong();
	//	break;
	//}

	 

	// sort v1, v2, v3 by y in increasing order
	std::sort(data, data + 3, GzCoordCmp);
	memcpy(v1, data[0].v, sizeof(GzCoord));
	memcpy(v2, data[1].v, sizeof(GzCoord));
	memcpy(v3, data[2].v, sizeof(GzCoord));

	Point3d p1(v1);
	Point3d p2(v2);
	Point3d p3(v3);

	//if (interp_mode == GZ_FLAT) {
	//	lightingEquation(data[0]);
	//	memcpy(flatcolor, data[0].color, sizeof(GzColor));
	//	interpolateZ(p1, p2, p3, 0);
	//}
	//else if (interp_mode == GZ_COLOR) {
	//	for (int i = 0; i < 3; i++) {
	//		lightingEquation(data[i]);
	//	}
	//	interpolateZ(p1, p2, p3, 0);
	//	interpolateZ(p1, p2, p3, 1);
	//	interpolateZ(p1, p2, p3, 2);
	//	interpolateZ(p1, p2, p3, 3);
	//}
	//else if (interp_mode == GZ_NORMALS) {

	//}
	 
	 
		// set up edge for three edges
	Edge edge12(p1, p2);
	Edge edge13(p1, p3);
	Edge edge23(p2, p3);


	// sort edges by L or R
	// slope comparision
	bool isV2Left = true;

	float slope12 = edge12.slopeX;
	float slope13 = edge13.slopeX;
	if (slope12 > slope13) isV2Left = false;

	// advance edges
	float deltaY = ceil(p1.y) - p1.y;
	int z = 0;

	// advance edge12 and edge13
	edge12.cur.x = edge12.cur.x + edge12.slopeX * deltaY;
	edge12.cur.y = edge12.cur.y + deltaY;
	edge12.cur.z = edge12.cur.z + edge12.slopeZ * deltaY;

	edge13.cur.x = edge13.cur.x + edge13.slopeX * deltaY;
	edge13.cur.y = edge13.cur.y + deltaY;
	edge13.cur.z = edge13.cur.z + edge13.slopeZ * deltaY;

	scanLine(edge12, edge13, isV2Left, p2, z);

	deltaY = ceil(p2.y) - p2.y;

	// advance edge23 and edge13
	edge23.cur.x = edge23.cur.x + edge23.slopeX * deltaY;
	edge23.cur.y = edge23.cur.y + deltaY;
	edge23.cur.z = edge23.cur.z + edge23.slopeZ * deltaY;

	scanLine(edge23, edge13, isV2Left, p3, z);
	

	return GZ_SUCCESS;
}

int GzRender::interpolateZ(Point3d p1, Point3d p2, Point3d p3, int z) {


	// set up edge for three edges
	Edge edge12(p1, p2);
	Edge edge13(p1, p3);
	Edge edge23(p2, p3);


	// sort edges by L or R
	// slope comparision
	bool isV2Left = true;

	float slope12 = edge12.slopeX;
	float slope13 = edge13.slopeX;
	if (slope12 > slope13) isV2Left = false;

	// advance edges
	float deltaY = ceil(p1.y) - p1.y;


	// advance edge12 and edge13
	edge12.cur.x = edge12.cur.x + edge12.slopeX * deltaY;
	edge12.cur.y = edge12.cur.y + deltaY;
	edge12.cur.z = edge12.cur.z + edge12.slopeZ * deltaY;

	edge13.cur.x = edge13.cur.x + edge13.slopeX * deltaY;
	edge13.cur.y = edge13.cur.y + deltaY;
	edge13.cur.z = edge13.cur.z + edge13.slopeZ * deltaY;

	scanLine(edge12, edge13, isV2Left, p2, z);

	deltaY = ceil(p2.y) - p2.y;

	// advance edge23 and edge13
	edge23.cur.x = edge23.cur.x + edge23.slopeX * deltaY;
	edge23.cur.y = edge23.cur.y + deltaY;
	edge23.cur.z = edge23.cur.z + edge23.slopeZ * deltaY;

	scanLine(edge23, edge13, isV2Left, p3, z);

	return GZ_SUCCESS;

}
