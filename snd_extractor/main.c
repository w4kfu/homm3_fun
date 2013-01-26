#include "main.h"

HINSTANCE	hInst;
LPTSTR		lpszDialogCaption = "Snd Extractor";
struct      file sFile;
HWND        hListView;

LRESULT CALLBACK MainProc(HWND Dlg,UINT message,WPARAM wParam,LPARAM lParam);
void open_snd(HWND hWin);
void SetFileInformation(HWND hWin, OPENFILENAME *ofn);
void add_item(DWORD dwNum, struct entry_wav *sWav);

int APIENTRY WinMainCRTStartup()
{
   INITCOMMONCONTROLSEX icex;

    hInst = (HINSTANCE)GetModuleHandleA(NULL);
	InitCommonControls();

    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

	DialogBoxParam(hInst, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)MainProc, 0);
	ExitProcess (0);
    return 0;
}

LRESULT CALLBACK MainProc(HWND hWin,UINT message,WPARAM wParam,LPARAM lParam)
{
    int Select;
    HMENU hMenu, hSubMenu;
    LVCOLUMN lvc = {0};


    switch(message)
    {
        case WM_INITDIALOG:
            hMenu = CreateMenu();
            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_OPEN, "O&pen");
            AppendMenu(hSubMenu, MF_STRING, ID_QUIT, "E&xit");
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)NULL, "&About");

            SetMenu(hWin, hMenu);

			SendMessage (hWin, WM_SETTEXT, 0, (LPARAM)lpszDialogCaption);
			//HICON hIcon = LoadIcon (hInst, (LPCSTR)IDI_ICON1);
			//SendMessage (hWin, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SetDlgItemText (hWin, IDC_STATIC, "No File Loaded");

            //SendMessage(hWin, LB_SETCOLUMNWIDTH, (WPARAM) 100, (LPARAM) 0);
            hListView = CreateWindow(WC_LISTVIEW, NULL,
                                 WS_CHILD | LVS_REPORT |WS_VISIBLE,
                                 10, 10, 380, 440,
                                 hWin, (HMENU) 104, hInst, NULL);
            ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
            //ListView_SetExtendedListViewStyle(hListView, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
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
			break;
        case WM_COMMAND:
			Select = LOWORD(wParam);
			switch(Select)
			{
			    case ID_OPEN:
                    open_snd(hWin);
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
        {


            LPNMHDR pnmhdr;
            pnmhdr=(LPNMHDR)lParam;
            if(pnmhdr->hwndFrom == hListView && pnmhdr->code==NM_RCLICK)
            {
                MessageBoxA(NULL, "dsd", "sdsd", 0);
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_CHOIX), hDlg,About);
                return 0;
            }
            break;
        }
        case WM_CLOSE:
        case WM_DESTROY:
			ShowWindow (hWin, SW_MINIMIZE);
			EndDialog (hWin,0);
			break;
        default:
            return FALSE;
   }
   return TRUE;
}

void open_snd(HWND hWin)
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
        return;
    }
    return;
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
        add_item(dwCount, sWav);
    }
}

void add_item(DWORD dwNum, struct entry_wav *sWav)
{
    LVITEM lv = {0};
    BYTE bSize[MAX_PATH];
    BYTE bOffset[MAX_PATH];

    lv.iItem = 0;
    ListView_InsertItem(hListView, &lv);
    ListView_SetItemText(hListView, 0, 0, sWav->bName);
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
