#ifndef BMP_H_
# define BMP_H_

#include <windows.h>

typedef struct
{
  WORD	bfType;
  DWORD	bfSize;
  WORD	bfReserved1;
  WORD	bfReserved2;
  DWORD	bfOffBits;
}		bmpFHEAD;

typedef struct bmpIHEAD
{
  DWORD	biSize;
  int	biWidth;
  int	biHeight;
  WORD	biPlanes;
  WORD	biBitCount;
  DWORD	biCompression;
  DWORD	biSizeImage;
  int	biXPelsPerMeter;
  int	biYPelsPerMeter;
  DWORD	biClrUsed;
  DWORD	biClrImportant;
}		bmpIHEAD;

typedef struct
{
  BYTE	r;
  BYTE	g;
  BYTE	b;
}		pixel;

typedef struct
{
  int	width;
  int	height;
  BYTE	*data;
}		BMP;

#endif /* !BMP_H_ */
