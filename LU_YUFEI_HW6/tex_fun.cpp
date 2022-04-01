/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */
  if (u < 0) u = 0;
  if (v < 0) v = 0;
  if (u > 1) u = 1;
  if (v > 1) v = 1;
 /* if (u < 0 || u > 1 || v < 0 || v > 1) {
      return GZ_FAILURE;
  }*/

  float scaledu = u * (xs - 1);
  float scaledv = v * (ys - 1);
  float s = scaledu - floor(scaledu);
  float t = scaledv - floor(scaledv);
  int A = floor(scaledu) + floor(scaledv) * xs;
  int B = A + 1;
  int C = ceil(scaledu) + ceil(scaledv) * xs;
  int D = C - 1;

  for (int i = 0; i < 3; i++) {
      color[i] = s * t * image[C][i] + (1 - s) * t * image[D][i] 
          + s * (1 - t) * image[B][i] + (1 - s) * (1 - t) * image[A][i];
      //color[i] = image[(int)(scaledu + scaledv * xs)][i];
  }  
	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
    xs = 15;
    ys = 15;
    float a = floor(u * (xs - 1));
    float b = floor(v * (ys - 1));
    if (fmod(a + b, 2.0) > 0.5) {  // a+b is odd
        color[0] = 1.0;
        color[1] = 0.6;
        color[2] = 0.6;
    }
    else {  // a+b is even
        color[0] = 0.6;
        color[1] = 0.6;
        color[2] = 1.0;
    }
	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

