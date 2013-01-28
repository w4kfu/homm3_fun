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
                case IDC_EXTRACT:
                    ExtractFromFolder(hWin);
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

void ExtractFromFolder(HWND hWin)
{
    BROWSEINFO bi;
    BYTE szDisplayName[MAX_PATH];
    LPITEMIDLIST pidl;
    BYTE szPathName[MAX_PATH];

    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = hWin;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szDisplayName;
    bi.lpszTitle = "Please select a folder for storing received files :";
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    bi.lParam = NULL;
    bi.iImage = 0;

    pidl = SHBrowseForFolderA(&bi);
    if (pidl != NULL)
    {
        BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
        strcat(szPathName, "\\*");
        ScanFolder(szPathName);
    }
}

PBYTE strrstr(PBYTE haystack, PBYTE needle)
{
    PBYTE result = NULL;
    PBYTE p;

    if (*needle == '\0')
        return haystack;

    while(1)
    {
        p = strstr(haystack, needle);
        if (p == NULL)
            break;
        result = p;
        haystack = p + 1;
    }
    return result;
}

BOOL FixFileName(LPCTSTR FileName)
{
    PBYTE ph3m = NULL;

    if ((ph3m = strrstr(FileName, "pcx")))
    {
        *ph3m = 'b';
        *(ph3m + 1) = 'm';
        *(ph3m + 2) = 'p';
        return TRUE;
    }
    return FALSE;
}

void ScanFolder(LPCTSTR FolderName)
{
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    BYTE bFilePath[MAX_PATH];
    struct h3pcx homm3pcx;

    hFind = FindFirstFile(FolderName, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        MessageBoxA(NULL, FolderName, "PO", 0);
        return;
    }
    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {

        }
        else
        {
            if (strstr(ffd.cFileName, ".pcx"))
            {
                memset(bFilePath, 0, MAX_PATH);
                strncpy(bFilePath, FolderName, strlen(FolderName) - 2);
                strcat(bFilePath, "\\");
                strcat(bFilePath, ffd.cFileName);
                if (open_and_map(bFilePath, &sFile) == FALSE)
                {
                    clean_file(&sFile);
                    MessageBoxA(NULL, bFilePath, "[-] open_and_map failed", 0);
                }
                else
                {
                    homm3pcx.dwBitmapSize = *(DWORD*)sFile.bMap;
                    homm3pcx.dwWidth = *(DWORD*)(sFile.bMap + 4);
                    homm3pcx.dwHeight = *(DWORD*)(sFile.bMap + 8);
                    homm3pcx.bBitmap = (BYTE*)(sFile.bMap + 3 * sizeof(DWORD));

                    if (homm3pcx.dwBitmapSize != (homm3pcx.dwWidth * homm3pcx.dwHeight * 3))
                    {
                        BMP NewBmp;
                        PBYTE   NewBGR = NULL;
                        struct pcx_color *pal;
                        DWORD x, y;
                        int i;

                        NewBmp.width = homm3pcx.dwWidth;
                        NewBmp.height = homm3pcx.dwHeight;
                        homm3pcx.pal = (struct pcx_color*)(sFile.bMap + (3 * sizeof(DWORD)) + (homm3pcx.dwWidth * homm3pcx.dwHeight));
                        NewBGR = VirtualAlloc(NULL, (homm3pcx.dwWidth * homm3pcx.dwHeight) * 3, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

                        for (y = 0; y < homm3pcx.dwHeight; y++)
                        {
                            for (x = 0; x < homm3pcx.dwWidth; x++)
                            {
                                pal = (struct pcx_color *)&homm3pcx.pal[homm3pcx.bBitmap[x + (y * homm3pcx.dwWidth)]];
                                //SetPixel(hdc, x, y, RGB(pal->r, pal->g, pal->b));
                                i = (x + (y * homm3pcx.dwWidth)) * 3;
                                NewBGR[i + 0] = pal->b;
                                NewBGR[i + 1] = pal->g;
                                NewBGR[i + 2] = pal->r;
                            }
                        }
                        /*for (i = 0; i < NewBmp.width * NewBmp.height; i++)
                        {

                        }

                        pal = (struct pcx_color *)&homm3pcx.pal[NewBmp.data[x + (y * NewBmp.width)]];*/
                        NewBmp.data = NewBGR;
                        FixFileName(bFilePath);
                        bmp_save(&NewBmp, bFilePath);
                        VirtualFree(NewBGR, 0, MEM_RELEASE);
                    }
                    else
                    {
                        BMP NewBmp;

                        NewBmp.width = homm3pcx.dwWidth;
                        NewBmp.height = homm3pcx.dwHeight;
                        NewBmp.data = (BYTE*)(sFile.bMap + 3 * sizeof(DWORD));
                        FixFileName(bFilePath);
                        bmp_save(&NewBmp, bFilePath);
                    }

                    //DrawPCX(NULL, bFilePath);
                }

            }
         //filesize.LowPart = ffd.nFileSizeLow;
         //filesize.HighPart = ffd.nFileSizeHigh;
         //_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
        }
    } while (FindNextFile(hFind, &ffd));
}
