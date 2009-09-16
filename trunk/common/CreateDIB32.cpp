#include "stdafx.h"
#include "CreateDIB32.h"

HBITMAP CreateDIB32(int width, int height, BITMAPINFO& bmi, void*& pBits)
{
	BITMAPINFOHEADER& header = bmi.bmiHeader;
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 32;
	header.biCompression = BI_RGB;
	header.biSizeImage = width * abs(height) * 4;
	header.biXPelsPerMeter = 0;
	header.biYPelsPerMeter = 0;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	return ::CreateDIBSection(
		(HDC)0,
		&bmi,
		DIB_RGB_COLORS,
		&pBits,
		NULL,
		0
	);
}

