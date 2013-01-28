#ifndef __PCX_VIEWER_H__
#define __PCX_VIEWER_H__

#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "Comdlg32.lib")

#include <windows.h>
#include <commctrl.h>
#include <Commdlg.h>
#include <string.h>
#include <Shellapi.h>
#include <Shlobj.h>

#include "file.h"
#include "resource.h"
#include "bmp.h"

struct pcx_color
{
	BYTE r;
	BYTE g;
	BYTE b;
};

struct h3pcx
{
  DWORD dwBitmapSize;
  DWORD dwWidth;
  DWORD dwHeight;
  BYTE *bBitmap;
  struct pcx_color *pal;
};

LRESULT CALLBACK MainProc(HWND hWin, UINT message, WPARAM wParam, LPARAM lParam);
void HandleFiles(HWND hWin, WPARAM wParam);
void DrawPCX(HWND hWin, LPCTSTR FileName);
void GetRGB(struct h3pcx *homm3pcx, DWORD x, DWORD y, DWORD *r, DWORD *g, DWORD *b);
void ExtractFromFolder(HWND hWin);
void ScanFolder(LPCTSTR FolderName);

#endif // __PCX_VIEWER_H__
