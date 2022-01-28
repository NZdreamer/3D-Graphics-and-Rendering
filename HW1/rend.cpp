#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"

#include <iostream>
#include <cstring>
#include <sstream>



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
	delete []framebuffer;
	delete []pixelbuffer;
	//free(framebuffer);
	//free(pixelbuffer);
}

int GzRender::GzDefault()
{
	/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int i = 0; i < xres * yres; i++) {
		pixelbuffer[i] = { 2000, 555, 555, 1, 0 };
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