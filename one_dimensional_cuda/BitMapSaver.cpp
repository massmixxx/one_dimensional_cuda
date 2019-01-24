#pragma once
#include "BitMapSaver.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdexcept>



BitMapSaver::BitMapSaver()
{
}


BitMapSaver::~BitMapSaver()
{
}


using namespace std;
 void BitMapSaver::SaveBitmap(unsigned char* matrix, int rowSize, int numRows, std::string name) {

	int w = rowSize;
	int h = numRows;

	FILE *f;
	unsigned char *img = NULL;
	int filesize = 54 + 3 * w*h;

	img = (unsigned char *)malloc(3 * w*h);
	memset(img, 0, 3 * w*h);

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			int x = i; int y = (h - 1) - j;
			int color = (matrix[((rowSize * numRows) - 1) - ((j*rowSize) + i)] ? 255 : 0);

			img[(x + y * w) * 3 + 2] = (unsigned char)(color);
			img[(x + y * w) * 3 + 1] = (unsigned char)(color);
			img[(x + y * w) * 3 + 0] = (unsigned char)(color);
		}
	}

	unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
	unsigned char bmppad[3] = { 0,0,0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(w);
	bmpinfoheader[5] = (unsigned char)(w >> 8);
	bmpinfoheader[6] = (unsigned char)(w >> 16);
	bmpinfoheader[7] = (unsigned char)(w >> 24);
	bmpinfoheader[8] = (unsigned char)(h);
	bmpinfoheader[9] = (unsigned char)(h >> 8);
	bmpinfoheader[10] = (unsigned char)(h >> 16);
	bmpinfoheader[11] = (unsigned char)(h >> 24);

	string filename = "img" + name + ".bmp";

	f = fopen(filename.c_str(), "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	for (int i = 0; i < h; i++)
	{
		fwrite(img + (w*(h - i - 1) * 3), 3, w, f);
		fwrite(bmppad, 1, (4 - (w * 3) % 4) % 4, f);
	}

	free(img);
	fclose(f);
}
