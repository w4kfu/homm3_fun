#ifndef __PCX_VIEWER_H__
#define __PCX_VIEWER_H__

#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "Comdlg32.lib")

#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <Commdlg.h>
#include <string.h>
#include <Shellapi.h>

#include "file.h"
#include "resource.h"

LRESULT CALLBACK MainProc(HWND hWin, UINT message, WPARAM wParam, LPARAM lParam);
void HandleFiles(WPARAM wParam);

#endif // __PCX_VIEWER_H__
