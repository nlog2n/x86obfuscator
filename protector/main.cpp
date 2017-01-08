/* x86.Virtualizer
 *  rev 20101005: 
 *      vm_protect_vm(), doProtect(), which both call vm_init().
 *      doProtect() calls vm_protect() in protect.cpp and vm_protect_vm() also.
 */


#include <windows.h>
#include <cstdio>
#include <ctime>

#include "protect.h"
#include "resource.h"


//-----------------------------------------------------------------------------
DWORD ddFrom;
DWORD ddTo;
int WINAPI AddDialogProc(HWND hDlg, UINT uMSg, WPARAM wParam, LPARAM lParam)
{
	switch (uMSg)
	{
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;
		case WM_COMMAND:
			{
				switch (wParam & 0xFFFF)
				{
					case BTN_ADD_NO:						
						EndDialog(hDlg, 0);
						break;
					case BTN_ADD_YES:
						char temp[20];
						ddFrom = 0;
						ddTo = 0;
						GetDlgItemText(hDlg, EDT_FROM, temp, 20);
						sscanf(temp, "%x", &ddFrom);
						GetDlgItemText(hDlg, EDT_TO, temp, 20);
						sscanf(temp, "%x", &ddTo);
						if (ddTo && ddFrom) EndDialog(hDlg, 1);
						else MessageBox(hDlg, "Error", "Error", MB_ICONERROR);
						break;
				}
			}
			break;
	}
	return 0;
}



int WINAPI DialogProc(HWND hDlg, UINT uMSg, WPARAM wParam, LPARAM lParam)
{
	switch (uMSg)
	{
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;
		case WM_COMMAND:
			{
				switch (wParam & 0xFFFF)
				{
					case BTN_EXIT:
						EndDialog(hDlg, 0);
						break;
					case BTN_ADD:
						if (DialogBoxParam((HINSTANCE)0x400000, (LPCSTR)IDD_DIALOG2, hDlg, AddDialogProc, 0))
						{
							char temp[50];
							sprintf(temp, "%08X - %08X", ddFrom, ddTo);
							SendDlgItemMessage(hDlg, LB_LIST, LB_ADDSTRING, 0, (LPARAM)temp);
						}
						break;
					case BTN_OPEN:
						{
							OPENFILENAME ofn;
							memset(&ofn, 0, sizeof(OPENFILENAME));
							ofn.lStructSize = sizeof(OPENFILENAME);
							ofn.hwndOwner = hDlg;
							char fileName[MAX_PATH] = {0};
							ofn.lpstrFile = fileName;
							ofn.nMaxFile = MAX_PATH;
							ofn.Flags = OFN_FILEMUSTEXIST;
							GetOpenFileName(&ofn);
							SetDlgItemText(hDlg, EDT_FILE, fileName);
							SendDlgItemMessage(hDlg, LB_LIST, LB_RESETCONTENT, 0, 0);
						}
						break;
					case BTN_PROTECT:
						{
							char fileName[MAX_PATH];
							char tmp[128] ={0};
							if (GetDlgItemText(hDlg, EDT_FILE, fileName, MAX_PATH))
							{
								bool vmovm = false;
								if (SendDlgItemMessage(hDlg, CHK_VMOVM, BM_GETCHECK, 0, 0) == BST_CHECKED) {
									vmovm = true;
								}

                                MyObfuscator obf(fileName);
								obf.do_protect(GetDlgItem(hDlg, LB_LIST), vmovm);
								sprintf(tmp, "Finished, err code= %08X\n by fanghui", obf.m_errno);
                                //MessageBox(0, a, "Error", MB_ICONERROR); 
							}
							MessageBox(hDlg, tmp, "Info", MB_ICONINFORMATION);
						}
						break;
				}
			}
			break;
	}
	return 0;
}

int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	DialogBoxParam(hInstance, (LPCSTR)IDD_DIALOG1, 0, DialogProc, 0);

	return (int)VirtualAlloc;
}