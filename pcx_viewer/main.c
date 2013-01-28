#include "pcx_viewer.h"

HINSTANCE	hInst;
LPTSTR		lpszDialogCaption = "PCX Viewer";
struct      file sFile;
HWND        picBoxDisp;
struct file sFile;

int APIENTRY WinMainCRTStartup()
{
    hInst = (HINSTANCE)GetModuleHandleA(NULL);
	InitCommonControls();
	DialogBoxParam(hInst, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)MainProc, 0);
	ExitProcess (0);
    return 0;
}

LRESULT CALLBACK MainProc(HWND hWin, UINT message, WPARAM wParam, LPARAM lParam)
{
    DWORD dwSelect;

    switch(message)
    {
        case WM_INITDIALOG:
            SendMessage(hWin, WM_SETTEXT, 0, (LPARAM)lpszDialogCaption);
            SetDlgItemText(hWin, IDC_STATIC, "Drop .pcx file here");
			break;
        case WM_COMMAND:
			dwSelect = LOWORD(wParam);
			switch(dwSelect)
			{
				case IDC_QUIT:
                    SendMessage(hWin, WM_CLOSE, 0, 0);
                    break;
				default:
					break;
			}
			break;
        case WM_DROPFILES:
            HandleFiles(hWin, wParam);
            break;
        case WM_CLOSE:
        case WM_DESTROY:
			ShowWindow(hWin, SW_MINIMIZE);
			EndDialog(hWin,0);
			break;
        default:
            return FALSE;
   }
   return TRUE;
}

HBITMAP LoadBitmapFromBuffer(char *buffer, int width, int height)
{
    return CreateBitmap(width, height, 3, 12, buffer);
}

void HandleFiles(HWND hWin, WPARAM wParam)
{
    BYTE bName[MAX_PATH];
    DWORD dwNbDrop;
    DWORD dwCount;

    dwNbDrop = DragQueryFile(wParam, 0xFFFFFFFF, bName, MAX_PATH);
    for (dwCount = 0; dwCount < dwNbDrop; dwCount++)
    {
        clean_file(&sFile);
        DragQueryFile(wParam, dwCount, bName, MAX_PATH);
        if (open_and_map(bName, &sFile) == FALSE)
        {
            clean_file(&sFile);
            MessageBoxA(NULL, "[-] open_and_map failed", "error", 0);
            return;
        }
        DrawPCX(hWin, bName);
    }
    DragFinish(wParam);
}

void GetRGB(struct h3pcx *homm3pcx, DWORD x, DWORD y, DWORD *r, DWORD *g, DWORD *b)
{
    int i;

    if ((x < homm3pcx->dwWidth) && (y < homm3pcx->dwHeight))
    {
        i = (x + (y * homm3pcx->dwWidth)) * 3;
        *b = homm3pcx->bBitmap[i + 0];
        *g = homm3pcx->bBitmap[i + 1];
        *r = homm3pcx->bBitmap[i + 2];
    }
    else
    {
        *r = 0;
        *g = 0;
        *r = 0;
    }
}

void DrawPCX(HWND hWin, LPCTSTR FileName)
{
    struct h3pcx homm3pcx;
    DWORD x, y;
    HDC hdc = GetDC(hWin);
    DWORD r, g, b;
    DWORD dwIsPalette = 0;

    homm3pcx.dwBitmapSize = *(DWORD*)sFile.bMap;
    homm3pcx.dwWidth = *(DWORD*)(sFile.bMap + 4);
    homm3pcx.dwHeight = *(DWORD*)(sFile.bMap + 8);
    homm3pcx.bBitmap = (BYTE*)(sFile.bMap + 3 * sizeof(DWORD));

    if (homm3pcx.dwBitmapSize != (homm3pcx.dwWidth * homm3pcx.dwHeight * 3))
    {
        dwIsPalette = 1;
        homm3pcx.pal = (struct pcx_color*)(sFile.bMap + (3 * sizeof(DWORD)) + (homm3pcx.dwWidth * homm3pcx.dwHeight));
    }

    for (y = 0; y < homm3pcx.dwHeight; y++)
    {
        for (x = 0; x < homm3pcx.dwWidth; x++)
        {
            if (dwIsPalette)
            {
                struct pcx_color *pal;

                pal = (struct pcx_color *)&homm3pcx.pal[homm3pcx.bBitmap[x + (y * homm3pcx.dwWidth)]];
                SetPixel(hdc, x, y, RGB(pal->r, pal->g, pal->b));
            }
            else
            {
                /* BGR -> RGB */
                GetRGB(&homm3pcx, x, y, &b, &g, &r);
                SetPixel(hdc, x, y, RGB(r, g, b));
            }
        }
    }
    ReleaseDC(hWin, hdc);
}
