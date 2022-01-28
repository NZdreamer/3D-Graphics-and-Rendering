#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <algorithm>


/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

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
}

GzRender::~GzRender()
{
	/* HW1.2 clean up, free buffer memory */
	delete[]framebuffer;
	delete[]pixelbuffer;
	//free(framebuffer);
	//free(pixelbuffer);
}

int GzRender::GzDefault()
{
	/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int i = 0; i < xres * yres; i++) {
		pixelbuffer[i] = { 2000, 555, 555, 1, INT_MAX };	// initailze z to INT_MAX
	}
	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	/* HW1.4 write pixel values into the buffer */
		// ignore off-screen coordinate commands
		// pixels are between 0 and Xres-1, 0 and Yres-1
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


int GzRender::GzGet(int i, int j, GzIntensity* r, GzIntensity* g, GzIntensity* b, GzIntensity* a, GzDepth* z)
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
	//char head[30];
	//for (int i = 0; i < 30; i++) {
	//	head[i] = '\0';
	//}

	//std::string string = "P6 " + xres;
	//string.append(" ").append(std::to_string(yres)).append(" 255\r");
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

	//char buffer[3];
	//for (int i = 0; i < xres * yres; i++) {
	//	// conversion of GzIntensity(16-bit) to 8-bit rgb component
	//	// Drop 4-bits by right-shifting and then use low byte of GzIntensity value
	//	buffer[0] = pixelbuffer[i].red >> 4;
	//	buffer[1] = pixelbuffer[i].green >> 4;
	//	buffer[2] = pixelbuffer[i].blue >> 4;
	//	fputs(buffer, outfile);		
	//}


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

int GzRender::GzPutAttribute(int numAttributes, GzToken* nameList, GzPointer* valueList)
{
	/* HW 2.1
	-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	-- In later homeworks set shaders, interpolaters, texture maps, and lights
	*/
	if (nameList[0] == GZ_RGB_COLOR) {
		flatcolor[0] = ((GzColor*)valueList[0])[0][0];
		flatcolor[1] = ((GzColor*)valueList[0])[0][1];
		flatcolor[2] = ((GzColor*)valueList[0])[0][2];
	}

	return GZ_SUCCESS;
}

bool GzCoordCmp(const GzCoord* v1, const GzCoord* v2) {
	return (*v1)[1] < (*v2)[1];
}

// implement scanline algo
int GzRender::scanLine(Edge& edgeShort, Edge& edgeLong, bool isV2Left, GzCoord stopVertex) {

	SpanXZ L;
	SpanXZ R;

	// span edge12 and edge13
	// include right and bottom
	//while (edge13.current[1] <= v3[1])
	while (round(edgeShort.current[1]) < 256 && edgeShort.current[1] <= stopVertex[1]) {	// include bottom, exclude top pixel

		// span setup and advance
		// span setup
		if (isV2Left) {
			L.x = edgeShort.current[0];
			L.z = edgeShort.current[2];
			R.x = edgeLong.current[0];
			R.z = edgeLong.current[2];
		}
		else {
			L.x = edgeLong.current[0];
			L.z = edgeLong.current[2];
			R.x = edgeShort.current[0];
			R.z = edgeShort.current[2];
		}
		Span span = { L, R, L, (R.z - L.z) / (R.x - L.x) };
		//*span.current[1] = *edge13.current[1];
		float deltaX = ceil(L.x) - L.x;

		// span advance				
		span.current.x = span.current.x + deltaX;
		span.current.z = span.current.z + span.slopeZ * deltaX;

		while (round(span.current.x) < 256 && span.current.x <= span.end.x) {	// include right, exclude left pixel
			// Test interpolated-Z against FB-Z for each pixel - lowest Z wins
			// Write color value into FB pixel(default or computed color)	??? write into pixelbuffer ???
			//int curX = round(span.current.x);
			//int curY = round(edge13.current[1]);
			int position = ARRAY(round(span.current.x), round(edgeLong.current[1]));
			//if (position >= 256 * 256) continue;
			if (span.current.x >= 0 && pixelbuffer[position].z > span.current.z) {
				pixelbuffer[position].red = ctoi(flatcolor[0]);
				pixelbuffer[position].green = ctoi(flatcolor[1]);
				pixelbuffer[position].blue = ctoi(flatcolor[2]);
				pixelbuffer[position].z = span.current.z;
			}
			// span advance				
			span.current.x += 1;
			span.current.z += span.slopeZ;
		}

		edgeShort.current[0] += edgeShort.slopeX;
		edgeShort.current[1] += 1;
		edgeShort.current[2] += edgeShort.slopeZ;

		edgeLong.current[0] += edgeLong.slopeX;
		edgeLong.current[1] += 1;
		edgeLong.current[2] += edgeLong.slopeZ;
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int	numParts, GzToken* nameList, GzPointer* valueList)
/* numParts - how many names and values */
{
	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
		  GZ_NULL_TOKEN:		do nothing - no values
		  GZ_POSITION:		3 vert positions in model space
	-- Invoke the rastrizer/scanline framework
	-- Return error code
	*/

	if (nameList[0] == GZ_NULL_TOKEN) return GZ_FAILURE;
	if (nameList[0] == GZ_POSITION) {

		// get the three vertex
		GzCoord v1;
		GzCoord v2;
		GzCoord v3;

		GzCoord* v[3];
		GzCoord vVal[3];

		GzCoord* vertexList = (GzCoord*)valueList[0];
		
		for (int i = 0; i < 3; ++i) {
			memcpy(vVal[i], vertexList[i], sizeof(GzCoord));
			v[i] = &(vVal[i]);
		}

		//v[0][0] = vertexList[0][0];
		//v[0][1] = vertexList[0][1];
		//v[0][2] = vertexList[0][2];
		//v[1][0] = vertexList[1][0];
		//v[1][1] = vertexList[1][1];
		//v[1][2] = vertexList[1][2];
		//v[2][0] = vertexList[2][0];
		//v[2][1] = vertexList[2][1];
		//v[2][2] = vertexList[2][2];
		/*memcpy(v1, vertexlist[0], sizeof(gzcoord));
		memcpy(v2, vertexlist[1], sizeof(gzcoord));
		memcpy(v3, vertexlist[2], sizeof(gzcoord));*/
		//v1[0] = vertexList[0][0];
		//v1[1] = vertexList[0][1];
		//v1[2] = vertexList[0][2];
		//v2[0] = vertexList[1][0];
		//v2[1] = vertexList[1][1];
		//v2[2] = vertexList[1][2];
		//v3[0] = vertexList[2][0];
		//v3[1] = vertexList[2][1];
		//v3[2] = vertexList[2][2];


		// sort v1, v2, v3 by y in increasing order
		std::sort(v, v + 3, GzCoordCmp);
		memcpy(v1, *(v[0]), sizeof(GzCoord));
		memcpy(v2, *(v[1]), sizeof(GzCoord));
		memcpy(v3, *(v[2]), sizeof(GzCoord));
		/*GzCoord temp;
		if (v1[1] > v2[1]) {
			temp[0] = v1[0]; temp[1] = v1[1]; temp[2] = v1[2];
			v1[0] = v2[0]; v1[1] = v2[1]; v1[2] = v2[2];
			v2[0] = temp[0]; v2[1] = temp[1]; v2[2] = temp[2];
		}
		if (v1[1] > v3[1]) {
			temp[0] = v1[0]; temp[1] = v1[1]; temp[2] = v1[2];
			v1[0] = v3[0]; v1[1] = v3[1]; v1[2] = v3[2];
			v3[0] = temp[0]; v3[1] = temp[1]; v3[2] = temp[2];
		}
		if (v2[1] > v3[1]) {
			temp[0] = v2[0]; temp[1] = v2[1]; temp[2] = v2[2];
			v2[0] = v3[0]; v2[1] = v3[1]; v2[2] = v3[2];
			v3[0] = temp[0]; v3[1] = temp[1]; v3[2] = temp[2];
		}*/


		// set up edge for three edges
		Edge edge12 = { {v1[0], v1[1], v1[2]}, {v2[0], v2[1], v2[2]}, {v1[0], v1[1], v1[2]}, (v2[0] - v1[0]) / (v2[1] - v1[1]), (v2[2] - v1[2]) / (v2[1] - v1[1]) };
		Edge edge13 = { {v1[0], v1[1], v1[2]}, {v3[0], v3[1], v3[2]}, {v1[0], v1[1], v1[2]}, (v3[0] - v1[0]) / (v3[1] - v1[1]), (v3[2] - v1[2]) / (v3[1] - v1[1]) };
		Edge edge23 = { {v2[0], v2[1], v2[2]}, {v3[0], v3[1], v3[2]}, {v2[0], v2[1], v2[2]}, (v3[0] - v2[0]) / (v3[1] - v2[1]), (v3[2] - v2[2]) / (v3[1] - v2[1]) };
		
		// sort edges by L or R
		// slope comparision
		bool isV2Left = true;

		// TODO: if v2[1] == v1[1]
		float slope12 = (v2[0] - v1[0]) / (v2[1] - v1[1]);
		float slope13 = (v3[0] - v1[0]) / (v3[1] - v1[1]);
		if (slope12 > slope13) isV2Left = false;

		// advance edges
		float deltaY = ceil(v1[1]) - v1[1];		


		// advance edge12 and edge13
		edge12.current[0] = edge12.current[0] + edge12.slopeX * deltaY;
		edge12.current[1] = edge12.current[1] + deltaY;
		edge12.current[2] = edge12.current[2] + edge12.slopeZ * deltaY;

		edge13.current[0] = edge13.current[0] + edge13.slopeX * deltaY;
		edge13.current[1] = edge13.current[1] + deltaY;
		edge13.current[2] = edge13.current[2] + edge13.slopeZ * deltaY;

		scanLine(edge12, edge13, isV2Left, v2);

		deltaY = ceil(v2[1]) - v2[1];

		// advance edge23 and edge13
		edge23.current[0] = edge23.current[0] + edge23.slopeX * deltaY;
		edge23.current[1] = edge23.current[1] + deltaY;
		edge23.current[2] = edge23.current[2] + edge23.slopeZ * deltaY;

		scanLine(edge23, edge13, isV2Left, v3);

	}

	return GZ_SUCCESS;
}


