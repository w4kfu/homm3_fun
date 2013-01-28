#ifndef __LOD_EXTRACTOR_H__
#define __LOD_EXTRACTOR_H__

#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "Comdlg32.lib")
#pragma comment (lib, "Shell32.lib")

#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <Commdlg.h>
#include <string.h>
#include <Shellapi.h>

#include "file.h"
#include "zlib/zlib.h"
#include "resource.h"

#define CHUNK 0x4000
#define windowBits 15
#define ENABLE_ZLIB_GZIP 32

struct h3File
{
  BYTE		bName[16];
  DWORD	    dwOffset;
  DWORD	    dwRealSize;
  DWORD	    dwType;
  DWORD	    dwCompSize;
};

struct lod_file
{
  DWORD	dwMagic;
  DWORD	dwType;
  DWORD	dwNbFile;
  BYTE	bUnknown[80];
  struct h3File	h3file[10000];
};

LRESULT CALLBACK MainProc(HWND hWin, UINT message, WPARAM wParam, LPARAM lParam);
void ExtractArchive(LPCTSTR FileName);
void HandleFiles(WPARAM wParam);
void ExtractFile(PBYTE pbPath, struct file *sFile, struct h3File* h3file);
void ExtractFile2(PBYTE pbPath, struct file *sFile, struct h3File* h3file);
BOOL FixFileName(LPCTSTR FileName);

#endif // __LOD_EXTRACTOR_H__
