#include "h3m_map.h"

HINSTANCE	hInst;
LPTSTR		lpszDialogCaption = "Homm3 Map Extractor";

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
            SetDlgItemText(hWin, IDC_STATIC, "Drop .h3m file here");
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
            HandleFiles(wParam);
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

void HandleFiles(WPARAM wParam)
{
    BYTE bName[MAX_PATH];
    DWORD dwNbDrop;
    DWORD dwCount;

    dwNbDrop = DragQueryFile(wParam, 0xFFFFFFFF, bName, MAX_PATH);
    for (dwCount = 0; dwCount < dwNbDrop; dwCount++)
    {
        DragQueryFile(wParam, dwCount, bName, MAX_PATH);
        ExtractMap(bName);
    }
    DragFinish(wParam);
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

    if ((ph3m = strrstr(FileName, "h3m")))
    {
        *ph3m = 't';
        *(ph3m + 1) = 'x';
        *(ph3m + 2) = 't';
        return TRUE;
    }
    return FALSE;
}

void ExtractMap(LPCTSTR FileName)
{
    z_stream strm = {0};
    PBYTE pbOut;
    struct file sFile;
    DWORD dwLastError = 0;
    HANDLE hAppend;
    DWORD dwBytesWritten;
    DWORD dwSize;

    if (open_and_map(FileName, &sFile) == FALSE)
    {
        clean_file(&sFile);
        MessageBoxA(NULL, "[-] open_and_map failed", "error", 0);
        return;
    }
    dwSize = GetFileSize(sFile.hFile, NULL);
    pbOut = VirtualAlloc(NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = sFile.bMap;
    strm.avail_in = 0;
    strm.next_out = pbOut;
    if (inflateInit2(&strm, windowBits | ENABLE_ZLIB_GZIP) < 0)
    {
        clean_file(&sFile);
        MessageBoxA(NULL, "[-] inflateInit2 failed", "error", 0);
        return;
    }
    if (FixFileName(FileName) == FALSE)
    {
        clean_file(&sFile);
        MessageBoxA(NULL, "[-] Can't find h3m extension", "error", 0);
        return;
    }
    hAppend = CreateFileA(FileName,
              GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ | FILE_SHARE_WRITE,
              NULL,
              CREATE_ALWAYS,
              FILE_ATTRIBUTE_NORMAL,
              NULL);
    strm.avail_in = dwSize;
    do
    {
        strm.avail_out = dwSize;
        inflate(&strm, Z_NO_FLUSH);
        WriteFile(hAppend, pbOut, dwSize - strm.avail_out, &dwBytesWritten, NULL);
        strm.next_out = pbOut;
    }
    while (strm.avail_out == 0);
    VirtualFree(pbOut, 0, MEM_RELEASE);
    inflateEnd(&strm);
    CloseHandle(hAppend);
    clean_file(&sFile);
}
