#include "lod_extractor.h"

HINSTANCE	hInst;
LPTSTR		lpszDialogCaption = "Homm3 LOD Archive Extractor";

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
            SetDlgItemText(hWin, IDC_STATIC, "Drop .lod file here");
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
        ExtractArchive(bName);
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

void ExtractArchive(LPCTSTR FileName)
{
    struct file sFile;
    struct lod_file *lod = NULL;
    DWORD dwCount;
    BYTE  bPath[MAX_PATH];
    PBYTE pbLast = NULL;

    if (open_and_map(FileName, &sFile) == FALSE)
    {
        clean_file(&sFile);
        MessageBoxA(NULL, "[-] open_and_map failed", "error", 0);
        return;
    }
    lod = (struct lod_file*)sFile.bMap;
    if (lod->dwMagic != 0x00444f4c)
    {
        clean_file(&sFile);
        MessageBoxA(NULL, "[-] Appears to not be a homm3 lod (archive) file", "error", 0);
        return;
    }
    strncpy(bPath, FileName, MAX_PATH);
    pbLast = strrchr(bPath, '\\');
    if (!pbLast)
        return;
    *pbLast = 0;
    strcat(bPath, "\\Extracted");
    CreateDirectory (bPath, NULL);
    for (dwCount = 0; dwCount < lod->dwNbFile; dwCount++)
    {
        if (lod->h3file[dwCount].dwCompSize == 0 && *(sFile.bMap + lod->h3file[dwCount].dwOffset) == 0x1F && *(sFile.bMap + lod->h3file[dwCount].dwOffset + 1) == 0x8B)
            ExtractFile2(bPath, &sFile, &lod->h3file[dwCount]);
        else
            ExtractFile(bPath, &sFile, &lod->h3file[dwCount]);
    }
    clean_file(&sFile);
}

void ExtractFile(PBYTE pbPath, struct file *sFile, struct h3File* h3file)
{
    BYTE    bPath[MAX_PATH];
    z_stream strm = {0};
    PBYTE pbOut;
    int    err;

    strncpy(bPath, pbPath, MAX_PATH);
    strcat(bPath, "\\");
    strcat(bPath, h3file->bName);

    pbOut = VirtualAlloc(NULL, h3file->dwRealSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (h3file->dwCompSize == 0)
    {
        memcpy(pbOut, sFile->bMap + h3file->dwOffset, h3file->dwRealSize);
        goto save_and_clean;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;

    strm.next_in = sFile->bMap + h3file->dwOffset;
    strm.avail_in = h3file->dwCompSize;

    strm.next_out = pbOut;
    strm.avail_out = h3file->dwRealSize;

    if (inflateInit(&strm) != Z_OK)
    {
        MessageBoxA(NULL, "[-] inflateInit failed", "error", 0);
        VirtualFree(pbOut, 0, MEM_RELEASE);
        return;
    }

    err = inflate(&strm, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&strm);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && strm.avail_in == 0))
        {
            MessageBoxA(NULL, h3file->bName, "error", 0);
            VirtualFree(pbOut, 0, MEM_RELEASE);
            return;
        }
        MessageBoxA(NULL, h3file->bName, "error", 0);
        return;
    }
    inflateEnd(&strm);
save_and_clean:
    save_buf(bPath, pbOut, h3file->dwRealSize);
    VirtualFree(pbOut, 0, MEM_RELEASE);
}

void ExtractFile2(PBYTE pbPath, struct file *sFile, struct h3File* h3file)
{
    z_stream strm = {0};
    PBYTE pbOut;
    HANDLE hAppend;
    DWORD dwBytesWritten;
    BYTE   bPath[MAX_PATH];

    strncpy(bPath, pbPath, MAX_PATH);
    strcat(bPath, "\\");
    strcat(bPath, h3file->bName);

    pbOut = VirtualAlloc(NULL, CHUNK, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = sFile->bMap + h3file->dwOffset;
    strm.avail_in = 0;

    strm.next_out = pbOut;
    if (inflateInit2(&strm, windowBits | ENABLE_ZLIB_GZIP) < 0)
    {
        VirtualFree(pbOut, 0, MEM_RELEASE);
        MessageBoxA(NULL, "[-] inflateInit2 failed", "error", 0);
        return;
    }
    hAppend = CreateFileA(bPath,
              GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ | FILE_SHARE_WRITE,
              NULL,
              CREATE_ALWAYS,
              FILE_ATTRIBUTE_NORMAL,
              NULL);
    strm.avail_in = h3file->dwRealSize;
    do
    {
        strm.avail_out = CHUNK;
        inflate(&strm, Z_NO_FLUSH);
        WriteFile(hAppend, pbOut, CHUNK - strm.avail_out, &dwBytesWritten, NULL);
        strm.next_out = pbOut;
    }
    while (strm.avail_out == 0);
    VirtualFree(pbOut, 0, MEM_RELEASE);
    inflateEnd(&strm);
    CloseHandle(hAppend);
}
