#include "bmp.h"

/*BMP *bmp_create(int x, int y)
{
    BMP *b = NULL;

    b = VirtualAlloc(NULL, sizeof(BMP), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!b)
    {
        return NULL;
    }
    b->width  = x;
    b->height = y;
    //b->data = VirtualAlloc(NULL, (x * y) * 3, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return b;
}*/

void bmp_getcolor(BMP *bmp, int x, int y, pixel *p)
{
  if((x < bmp->width) && (y < bmp->height))
    {
      int a = (x + (y * bmp->width)) * 3;
      /* p->r = bmp->data[a + 0]; */
      p->b = bmp->data[a + 0];
      p->g = bmp->data[a + 1];
      /* p->b = bmp->data[a + 2]; */
      p->r = bmp->data[a + 2];
    }
}

void	make_bmpfileheader(long bitsize, bmpFHEAD *filehead)
{
  filehead->bfType = 0x4d42;	/* 'MB'; */
  filehead->bfSize = sizeof(bmpFHEAD) - 2 + sizeof(bmpIHEAD) + bitsize; /* -2 due to byte alignment */
  filehead->bfReserved1 = 0;
  filehead->bfReserved2 = 0;
  filehead->bfOffBits = sizeof(bmpFHEAD)-2 + sizeof(bmpIHEAD);	/* -2 due to bytealignment */
}

void	make_bmpinfoheader(BMP *bmp, long bitsize, bmpIHEAD *infohead)
{
  infohead->biSize = sizeof (bmpIHEAD);
  infohead->biWidth = bmp->width;
  infohead->biHeight = bmp->height * -1;
  infohead->biPlanes = 1;
  infohead->biBitCount = 24; /* 24bit */
  infohead->biCompression = 0;  /* RGB */
  infohead->biSizeImage = bitsize;
  infohead->biXPelsPerMeter = 2800;
  infohead->biYPelsPerMeter = 2800;
  infohead->biClrUsed = 0;
  infohead->biClrImportant = 0;
}

int bmp_save(BMP *bmp, char *bPath)
{
    bmpFHEAD filehead; /* File Header */
    bmpIHEAD infohead; /* Info Header */
    unsigned char swp;
    long bitsize;
    int x, y, a;
    pixel p;
    //FILE *fp;
    HANDLE hBmp;
    DWORD dwBytesWritten;

    hBmp = CreateFileA(bPath,
              GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ | FILE_SHARE_WRITE,
              NULL,
              CREATE_ALWAYS,
              FILE_ATTRIBUTE_NORMAL,
              NULL);
  //fp = fopen(filename, "wb");
  /*if (!bmp->data || !fp)
    {
      fclose (fp);
      return (0);
    }*/
    if (hBmp == INVALID_HANDLE_VALUE)
        return;
    bitsize  = bmp->width * bmp->height * 3;
    bitsize += bitsize % 4; /* and round to next 4 bytes (see byte alignement in compiler options!) */
    make_bmpfileheader(bitsize, &filehead);

    WriteFile(hBmp, &filehead.bfType, sizeof(filehead.bfType), &dwBytesWritten, NULL);
    WriteFile(hBmp, &filehead.bfSize, sizeof(filehead.bfSize), &dwBytesWritten, NULL);
    WriteFile(hBmp, &filehead.bfReserved1, sizeof(filehead.bfReserved1), &dwBytesWritten, NULL);
    WriteFile(hBmp, &filehead.bfReserved2, sizeof(filehead.bfReserved2), &dwBytesWritten, NULL);
    WriteFile(hBmp, &filehead.bfOffBits, sizeof(filehead.bfOffBits), &dwBytesWritten, NULL);

    //fwrite(&filehead.bfType,sizeof(filehead.bfType), 1, fp);
    //fwrite(&filehead.bfSize,sizeof(filehead.bfSize), 1, fp);
    //fwrite(&filehead.bfReserved1,sizeof(filehead.bfReserved1), 1, fp);
    //fwrite(&filehead.bfReserved2,sizeof(filehead.bfReserved2), 1, fp);
    //fwrite(&filehead.bfOffBits, sizeof(filehead.bfOffBits),1, fp);

    make_bmpinfoheader(bmp, bitsize, &infohead);

    WriteFile(hBmp, &infohead, sizeof (infohead), &dwBytesWritten, NULL);
    //fwrite(&infohead, sizeof (infohead), 1, fp);

    /* BITMAP */
    for(y = 0; y < bmp->height; y++)
    {
        for(x = 0; x < bmp->width; x++)
        {
            bmp_getcolor(bmp, x, y, &p);
            swp = p.r; /* Swap BGR to RGB */
            p.r = p.b;
            p.b = swp;
            WriteFile(hBmp, &p, sizeof (p), &dwBytesWritten, NULL);
            //fwrite (&p, sizeof (p), 1, fp);
        }
        /* Add padding bytes */
        swp = 0;
        for (a = 0; a < (4 - ((3 * bmp->width ) % 4)) % 4; a++)
            WriteFile(hBmp, &swp, sizeof(char), &dwBytesWritten, NULL);
            //fwrite (&swp, sizeof(char), 1, fp);
    }
    CloseHandle(hBmp);
    //fclose (fp);
    return 1;
}
