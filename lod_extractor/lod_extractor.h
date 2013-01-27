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

LRESULT CALLBACK MainProc(HWND hWin, UINT message, WPARAM wParam, LPARAM lParam);
void ExtractMap(LPCTSTR FileName);
void HandleFiles(WPARAM wParam);
BOOL FixFileName(LPCTSTR FileName);

#endif // __LOD_EXTRACTOR_H__
