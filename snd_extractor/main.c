#include "main.h"

HINSTANCE	hInst;
LPTSTR		lpszDialogCaption = "Snd Extractor";
struct      file sFile;
HWND        hListView;

LRESULT CALLBACK MainProc(HWND Dlg, UINT message, WPARAM wParam, LPARAM lParam);
void OpenSndFile(HWND hWin);
void SetFileInformation(HWND hWin, OPENFILENAME *ofn);
void AddItem(DWORD dwNum, struct entry_wav *sWav);
void ExtractFile(struct file *sFile, DWORD dwPos);
DWORD GetNbEntry(struct file *sFile);
void MakeMenu(HWND hWin);
void ExtractAction(void);
void PopMenuList(HWND hWin, LPARAM lParam);
void FixWavNameFile(PBYTE Buf);

int APIENTRY WinMainCRTStartup()
{
    //INITCOMMONCONTROLSEX icex;

    hInst = (HINSTANCE)GetModuleHandleA(NULL);
	InitCommonControls();

    //icex.dwICC = ICC_LISTVIEW_CLASSES;
    //InitCommonControlsEx(&icex);

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
            SetDlgItemText(hWin, IDC_STATIC, "No File Loaded");
			//HICON hIcon = LoadIcon (hInst, (LPCSTR)IDI_ICON1);
			//SendMessage (hWin, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            MakeMenu(hWin);
			break;
        case WM_COMMAND:
			dwSelect = LOWORD(wParam);
			switch(dwSelect)
			{
			    case ID_OPEN:
                    OpenSndFile(hWin);
                    break;
                case ID_EXTRACT:
                    ExtractAction();
                    break;
			    case ID_QUIT:
				case IDC_QUIT:
                    SendMessage(hWin, WM_CLOSE, 0, 0);
                    break;
				default:
					break;
			}
			break;
        case WM_NOTIFY :
            PopMenuList(hWin, lParam);
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

void MakeMenu(HWND hWin)
{
    HMENU hMenu, hSubMenu;
    LVCOLUMN lvc = {0};

    hMenu = CreateMenu();
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_OPEN, "O&pen");
    AppendMenu(hSubMenu, MF_STRING, ID_QUIT, "E&xit");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)NULL, "&About");
    SetMenu(hWin, hMenu);
    hListView = CreateWindow(WC_LISTVIEW, NULL,
                        WS_CHILD | LVS_REPORT |WS_VISIBLE | LVS_SINGLESEL,
                        10, 10, 380, 440,
                        hWin, (HMENU) 104, hInst, NULL);
    ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT);
    lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT;
    lvc.fmt  = LVCFMT_CENTER;

    EnableWindow(SendMessage(hListView,LVM_GETHEADER, 0, 0), FALSE);

    lvc.iSubItem = 0;
    lvc.cx       = 150;
    lvc.pszText  = TEXT("Name");
    ListView_InsertColumn(hListView, 0, &lvc);

    lvc.iSubItem = 1;
    lvc.cx       = 100;
    lvc.pszText  = TEXT("Offset");
    ListView_InsertColumn(hListView, 1, &lvc);

    lvc.iSubItem = 2;
    lvc.cx       = 100;
    lvc.pszText  = TEXT("Size");
    ListView_InsertColumn(hListView, 2, &lvc);
}

void ExtractAction(void)
{
    DWORD dwPos;

    dwPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
    if (dwPos == -1)
        MessageBoxA(NULL, "Load a file first", "ERROR", 0);
    else
        ExtractFile(&sFile, dwPos);
}

void PopMenuList(HWND hWin, LPARAM lParam)
{
    HMENU hSubMenu;
    LPNMHDR pnmhdr;
    POINT lpPoint;

    pnmhdr = (LPNMHDR)lParam;
    if (pnmhdr->hwndFrom == hListView && pnmhdr->code == NM_RCLICK)
    {
        hSubMenu = CreatePopupMenu();
        AppendMenu(hSubMenu, MF_STRING, ID_EXTRACT, "E&xtract");
        GetCursorPos(&lpPoint);
        TrackPopupMenu(hSubMenu, TPM_LEFTALIGN , lpPoint.x, lpPoint.y, NULL, hWin, NULL);
    }
}

void OpenSndFile(HWND hWin)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWin;
    ofn.lpstrFilter = "Snd Files (*.snd)\0*.snd\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = "snd";
    ListView_DeleteAllItems(hListView);
    clean_file(&sFile);
    if (GetOpenFileName(&ofn))
    {
        if (open_and_map(ofn.lpstrFile, &sFile) == FALSE)
            return;
        SetFileInformation(hWin, &ofn);
    }
}

void SetFileInformation(HWND hWin, OPENFILENAME *ofn)
{
    BYTE Buf[MAX_PATH];
    DWORD dwSize;
    PBYTE bName;
    struct entry_wav *sWav;
    DWORD  dwCount;

    ZeroMemory(Buf, sizeof(Buf));
    bName = strrchr(ofn->lpstrFile, '\\');
    if (!bName)
        return;
    dwSize = GetFileSize(sFile.hFile, NULL);
    sprintf(Buf, "FileName : %s\nSize : %d bytes (0x%X)", bName + 1, dwSize, dwSize);
    SetDlgItemText (hWin, IDC_STATIC, Buf);
    for (dwCount = 0; dwCount < GetNbEntry(&sFile); dwCount++)
    {
        sWav = (struct entry_wav *)((sFile.bMap + 4) + dwCount * sizeof (struct entry_wav));
        AddItem(dwCount, sWav);
    }
}

void AddItem(DWORD dwNum, struct entry_wav *sWav)
{
    LVITEM lv = {0};
    BYTE bSize[MAX_PATH];
    BYTE bOffset[MAX_PATH];
    BYTE bName[40];

    lv.iItem = 0;
    ListView_InsertItem(hListView, &lv);
    memcpy(bName, sWav->bName, 40);
    FixWavNameFile(bName);
    ListView_SetItemText(hListView, 0, 0, bName);
    sprintf(bOffset, "0x%X", sWav->dwOffset);
    ListView_SetItemText(hListView, 0, 1, bOffset);
    sprintf(bSize, "%d", sWav->dwSize);
    ListView_SetItemText(hListView, 0, 2, bSize);
}


DWORD GetNbEntry(struct file *sFile)
{
    if (sFile->bMap)
        return *(DWORD*)sFile->bMap;
    return 0;
}

void GetValue(DWORD dwItem, DWORD dwSubItem, PBYTE format, void *res)
{
    BYTE buf[256];
    LV_ITEM itemInfo = {0};

    itemInfo.iItem = dwItem;
    itemInfo.iSubItem = dwSubItem;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.cchTextMax = 256 - 1;
    itemInfo.pszText = buf;
    ListView_GetItem(hListView, &itemInfo);
    sscanf(buf, format, res);
}

void ExtractFile(struct file *sFile, DWORD dwPos)
{
    DWORD dwOffset;
    DWORD dwSize;
    BYTE bName[40];

    GetValue(dwPos, 1, "%X", &dwOffset);
    GetValue(dwPos, 2, "%d", &dwSize);
    GetValue(dwPos, 0, "%s", bName);
    save_buf(bName, sFile->bMap + dwOffset, dwSize);
}

void FixWavNameFile(PBYTE Buf)
{
    if (!Buf)
        return;
    Buf[strlen(Buf)] = '.';
}
