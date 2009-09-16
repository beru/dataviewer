#include "stdafx.h"
#include <windows.h>

void Write32(HANDLE fh,const DWORD *lpPixel,int w,int h)
{
	int extra;
	if(w*3%4) extra=4-w*3%4;
	else extra=0;

	DWORD dwWriteSize;
	int zero=0;
	for(int y=0;y<h;y++){
		for(int x=0;x<w;x++)
			WriteFile(fh,lpPixel+x+y*w,3,&dwWriteSize,NULL);
		if(extra) WriteFile(fh,&zero,extra,&dwWriteSize,NULL);
	}	//一行分のバイト数を4の倍数に補正
}

int SaveDIB(LPCTSTR lpFileName,const BYTE *lpPixel,const BITMAPINFO *lpBmpInfo)
{
	BITMAPINFOHEADER bmpInfoH;
	CopyMemory(&bmpInfoH,&lpBmpInfo->bmiHeader,sizeof(BITMAPINFOHEADER));

	int bitCount=bmpInfoH.biBitCount;
	if(bitCount!=32 && bitCount!=24 && bitCount!=8){
		TCHAR str[8];
		_stprintf(str,_T("%d"),bitCount);
		MessageBox(NULL,_T("対応していないビット数です"),str,MB_OK);
		return -1;
	}
	if(bitCount==32) bmpInfoH.biBitCount=24;

	int w=bmpInfoH.biWidth , h=bmpInfoH.biHeight;
	DWORD nColorTable=bmpInfoH.biClrUsed;
	if(bitCount==8 && nColorTable==0) nColorTable=256;

	int len;
	if(w*(bitCount/8)%4) len=w*(bitCount/8)+(4-w*(bitCount/8)%4);
	else len=w*(bitCount/8);

	BITMAPFILEHEADER bmpfh;
	bmpfh.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+len*h;
	bmpfh.bfType=('M'<<8)+'B';
	bmpfh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	if(bitCount==8){
		bmpfh.bfSize+=nColorTable*4;
		bmpfh.bfOffBits+=nColorTable*4;
	}

	HANDLE fh=CreateFile(lpFileName,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	DWORD dwWriteSize;
	WriteFile(fh,&bmpfh,sizeof(BITMAPFILEHEADER),&dwWriteSize,NULL);
	WriteFile(fh,&bmpInfoH,sizeof(BITMAPINFOHEADER),&dwWriteSize,NULL);
	if(bitCount==8)
		WriteFile(fh,lpBmpInfo->bmiColors,nColorTable*4,&dwWriteSize,NULL);

	if(bitCount==32) Write32(fh,(LPDWORD)lpPixel,w,abs(h));
	else WriteFile(fh,lpPixel,len*abs(h),&dwWriteSize,NULL);

	CloseHandle(fh);
	return 0;
}
