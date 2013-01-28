#include "pcx_viewer.h"

HINSTANCE	hInst;
LPTSTR		lpszDialogCaption = "PCX Viewer";
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
        //ExtractArchive(bName);
    }
    DragFinish(wParam);
}
