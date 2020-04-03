/* iNES Header Editor
 * Copyright (C) 2006-2011 QMT Productions
 * Command line support and minor fix by Brad Smith, 2017
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>
#include <tchar.h>

#include "resource.h"

HWND hWnd;
HINSTANCE hInst;
BOOL                 InitInstance(HINSTANCE, int);
INT_PTR CALLBACK     dlgProc(HWND, UINT, WPARAM, LPARAM);

TCHAR filename[MAX_PATH];
BOOL fileOpen;
BOOL modified;
unsigned char header[16], header_orig[16];
BOOL isExtended;
BOOL Mask = FALSE;

BOOL OpenROM (HWND hDlg); // forward declaration


int APIENTRY _tWinMain(HINSTANCE hInstance, 
                     HINSTANCE hPrevInstance, 
                     LPTSTR    lpCmdLine, 
                     int       nCmdShow)
{
	MSG msg;
	if (!InitInstance (hInstance, nCmdShow)) 
		return FALSE;

	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLine(),&argc);
	if (argc >= 2)
	{
		_tcsncpy(filename,argv[1],MAX_PATH);
		filename[MAX_PATH-1] = 0;
		OpenROM(hWnd);
	}

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (IsDialogMessage(hWnd, &msg))
			continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_INESHEADER), NULL, dlgProc);
	if (!hWnd)
	{
		int err = GetLastError();
		TCHAR error[256];
		_stprintf(error, _T("Error %08X"), err);
		MessageBox(NULL, error, _T("iNES Header Editor"), MB_OK);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

void UpdateNum (HWND hDlg, int Control, int num)
{
	BOOL success;
	int oldnum = GetDlgItemInt(hDlg, Control, &success, FALSE);
	if ((num == oldnum) && success)
		return;
	SetDlgItemInt(hDlg, Control, num, FALSE);
}

void UpdateDialog (HWND hDlg)
{
	const int standard[] = { IDC_INES_VER1, IDC_INES_VER2, IDC_INES_MAP, IDC_INES_PRG, IDC_INES_CHR, IDC_INES_HORIZ, IDC_INES_VERT, IDC_INES_VS, IDC_INES_PC10, IDC_INES_4SCR, IDC_INES_BATT, IDC_INES_TRAIN, 0 };
	const int extended[] = { IDC_INES_SUBMAP, IDC_INES_PRAM, IDC_INES_PSAV, IDC_INES_CRAM, IDC_INES_CSAV, IDC_INES_NTSC, IDC_INES_PAL, IDC_INES_DUAL, IDC_INES_VSPPU, IDC_INES_VSFLAG, 0 };
	int i;
	Mask = TRUE; 
	if (!fileOpen)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_INES_VER1), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_INES_VER2), FALSE);

		CheckRadioButton(hDlg, IDC_INES_VER1, IDC_INES_VER2, IDC_INES_VER1);

		for (i = 0; standard[i] != 0; i++)
			EnableWindow(GetDlgItem(hDlg, standard[i]), FALSE);
		for (i = 0; extended[i] != 0; i++)
			EnableWindow(GetDlgItem(hDlg, extended[i]), FALSE);

		SetDlgItemText(hDlg, IDC_INES_MAP, _T(""));
		SetDlgItemText(hDlg, IDC_INES_PRG, _T(""));
		SetDlgItemText(hDlg, IDC_INES_CHR, _T(""));

		CheckRadioButton(hDlg, IDC_INES_HORIZ, IDC_INES_VERT, IDC_INES_HORIZ);

		CheckDlgButton(hDlg, IDC_INES_BATT, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_INES_TRAIN, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_INES_4SCR, BST_UNCHECKED);

		CheckDlgButton(hDlg, IDC_INES_VS, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_INES_PC10, BST_UNCHECKED);

		SetDlgItemText(hDlg, IDC_INES_SUBMAP, _T(""));
		SendDlgItemMessage(hDlg, IDC_INES_PRAM, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_PSAV, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_CRAM, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_CSAV, CB_SETCURSEL, 0xF, 0);
		CheckRadioButton(hDlg, IDC_INES_NTSC, IDC_INES_DUAL, IDC_INES_DUAL);
		SendDlgItemMessage(hDlg, IDC_INES_VSPPU, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_VSFLAG, CB_SETCURSEL, 0xF, 0);

		EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDCLOSE), FALSE);
		Mask = FALSE;
		return;
	}

	EnableWindow(GetDlgItem(hDlg, IDC_INES_VER1), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_INES_VER2), TRUE);

	for (i = 0; standard[i] != 0; i++)
		EnableWindow(GetDlgItem(hDlg, standard[i]), TRUE);

	if ((header[7] & 0x0C) == 0x08)
		isExtended = TRUE;
	else	isExtended = FALSE;
	CheckRadioButton(hDlg, IDC_INES_VER1, IDC_INES_VER2, isExtended ? IDC_INES_VER2 : IDC_INES_VER1);

	for (i = 0; extended[i] != 0; i++)
		EnableWindow(GetDlgItem(hDlg, extended[i]), isExtended);

	if (isExtended)
	{
		UpdateNum(hDlg, IDC_INES_MAP, ((header[6] & 0xF0) >> 4) | (header[7] & 0xF0) | ((header[8] & 0x01) << 8));
		UpdateNum(hDlg, IDC_INES_PRG, header[4] | ((header[9] & 0x0F) << 8));
		UpdateNum(hDlg, IDC_INES_CHR, header[5] | ((header[9] & 0xF0) << 4));
	}
	else
	{
		UpdateNum(hDlg, IDC_INES_MAP, ((header[6] & 0xF0) >> 4) | (header[7] & 0xF0));
		UpdateNum(hDlg, IDC_INES_PRG, header[4]);
		UpdateNum(hDlg, IDC_INES_CHR, header[5]);
	}

	CheckDlgButton(hDlg, IDC_INES_BATT, (header[6] & 0x02) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDlg, IDC_INES_TRAIN, (header[6] & 0x04) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDlg, IDC_INES_4SCR, (header[6] & 0x08) ? BST_CHECKED : BST_UNCHECKED);

	if (header[6] & 0x01)
		CheckRadioButton(hDlg, IDC_INES_HORIZ, IDC_INES_VERT, IDC_INES_VERT);
	else	CheckRadioButton(hDlg, IDC_INES_HORIZ, IDC_INES_VERT, IDC_INES_HORIZ);

	CheckDlgButton(hDlg, IDC_INES_VS, (header[7] & 0x01) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDlg, IDC_INES_PC10, (header[7] & 0x02) ? BST_CHECKED : BST_UNCHECKED);

	if (IsDlgButtonChecked(hDlg, IDC_INES_4SCR))
	{
		EnableWindow(GetDlgItem(hDlg, IDC_INES_HORIZ), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_INES_VERT), FALSE);
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg, IDC_INES_HORIZ), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_INES_VERT), TRUE);
	}

	if (IsDlgButtonChecked(hDlg, IDC_INES_VS))
		EnableWindow(GetDlgItem(hDlg, IDC_INES_PC10), FALSE);
	else if (IsDlgButtonChecked(hDlg, IDC_INES_PC10))
		EnableWindow(GetDlgItem(hDlg, IDC_INES_VS), FALSE);
	else
	{
		EnableWindow(GetDlgItem(hDlg, IDC_INES_VS), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_INES_PC10), TRUE);
	}

	if (isExtended)
	{
		UpdateNum(hDlg, IDC_INES_SUBMAP, ((header[8] & 0xF0) >> 4));
		SendDlgItemMessage(hDlg, IDC_INES_PRAM, CB_SETCURSEL, header[10] & 0x0F, 0);
		SendDlgItemMessage(hDlg, IDC_INES_CRAM, CB_SETCURSEL, header[11] & 0x0F, 0);


		if (header[6] & 0x02)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_INES_PSAV), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_INES_CSAV), TRUE);
			SendDlgItemMessage(hDlg, IDC_INES_PSAV, CB_SETCURSEL, (header[10] & 0xF0) >> 4, 0);
			SendDlgItemMessage(hDlg, IDC_INES_CSAV, CB_SETCURSEL, (header[11] & 0xF0) >> 4, 0);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDC_INES_PSAV), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_INES_CSAV), FALSE);
			SendDlgItemMessage(hDlg, IDC_INES_PSAV, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_INES_CSAV, CB_SETCURSEL, 0, 0);
		}

		if (header[12] & 0x02)
			CheckRadioButton(hDlg, IDC_INES_NTSC, IDC_INES_DUAL, IDC_INES_DUAL);
		else if (header[12] & 0x01)
			CheckRadioButton(hDlg, IDC_INES_NTSC, IDC_INES_DUAL, IDC_INES_PAL);
		else	CheckRadioButton(hDlg, IDC_INES_NTSC, IDC_INES_DUAL, IDC_INES_NTSC);

		if (header[7] & 0x01)
		{
			SendDlgItemMessage(hDlg, IDC_INES_VSPPU, CB_SETCURSEL, header[13] & 0x0F, 0);
			SendDlgItemMessage(hDlg, IDC_INES_VSFLAG, CB_SETCURSEL, (header[13] & 0xF0) >> 4, 0);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDC_INES_VSPPU), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_INES_VSFLAG), FALSE);
			SendDlgItemMessage(hDlg, IDC_INES_VSPPU, CB_SETCURSEL, 0xF, 0);
			SendDlgItemMessage(hDlg, IDC_INES_VSFLAG, CB_SETCURSEL, 0xF, 0);
		}
	}
	else
	{
		SetDlgItemText(hDlg, IDC_INES_SUBMAP, _T(""));
		SendDlgItemMessage(hDlg, IDC_INES_PRAM, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_PSAV, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_CRAM, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_CSAV, CB_SETCURSEL, 0xF, 0);
		CheckRadioButton(hDlg, IDC_INES_NTSC, IDC_INES_DUAL, IDC_INES_DUAL);
		SendDlgItemMessage(hDlg, IDC_INES_VSPPU, CB_SETCURSEL, 0xF, 0);
		SendDlgItemMessage(hDlg, IDC_INES_VSFLAG, CB_SETCURSEL, 0xF, 0);
	}

	if (memcmp(header_orig, header, 16))
		EnableWindow(GetDlgItem(hDlg, IDOK), modified = TRUE);
	else	EnableWindow(GetDlgItem(hDlg, IDOK), modified = FALSE);
	EnableWindow(GetDlgItem(hDlg, IDCLOSE), TRUE);
	Mask = FALSE;
}

BOOL CheckHeader (BOOL fix)
{
	BOOL needfix = FALSE;
	// Check for NES 2.0
	if ((header[7] & 0x0C) == 0x08)
	{
		// If the SRAM flag is cleared, make sure the extended battery-RAM sizes are zero
		if (!(header[6] & 0x02))
		{
			if ((header[10] & 0xF0) || (header[11] & 0xF0))
			{
				needfix = TRUE;
				if (fix)
				{
					header[10] &= 0x0F;
					header[11] &= 0x0F;
				}
			}
		}
		// If the VS flag is cleared, make sure the Vs. PPU fields are zero
		if (!(header[7] & 0x01))
		{
			if (header[13])
			{
				needfix = TRUE;
				if (fix)
					header[13] = 0;
			}
		}
		// Make sure no reserved bits are set in the TV system byte
		if (header[12] & 0xFC)
		{
			needfix = TRUE;
			if (fix)
				header[12] &= 0x03;
		}
		// Make sure other reserved bytes are cleared
		if (header[14] || header[15])
		{
			needfix = TRUE;
			if (fix)
			{
				header[14] = 0;
				header[15] = 0;
			}
		}
	}
	else
	{
		if (header[7] & 0x0C)
		{
			needfix = TRUE;
			if (fix)
				header[7] = 0;
		}
		for (int i = 8; i < 16; i++)
		{
			if (header[i])
			{
				needfix = TRUE;
				if (fix)
					header[i] = 0;
			}
		}
	}
	return needfix;
}

BOOL SaveROM (HWND hDlg)
{
	FILE *ROM = _tfopen(filename, _T("r+b"));
	if (ROM == NULL)
	{
		int result = MessageBox(hDlg, _T("Unable to reopen file! Discard changes?"), _T("iNES Header Editor"), MB_YESNO | MB_ICONQUESTION);
		if (result == IDYES)
		{
			fileOpen = FALSE;
			SetDlgItemText(hDlg, IDC_INES_NAME, _T(""));
			UpdateDialog(hDlg);
			return TRUE;
		}
		return FALSE;
	}
	// ensure the header is in a consistent state - zero out all unused bits
	CheckHeader(TRUE);
	memcpy(header_orig, header, 16);
	fseek(ROM, 0, SEEK_SET);
	fwrite(header, 1, 16, ROM);
	fclose(ROM);
	modified = FALSE;
	EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
	return TRUE;
}

BOOL CloseROM (HWND hDlg)
{
	if (fileOpen)
	{
		if (modified)
		{
			int result = MessageBox(hDlg, _T("Save changes?"), _T("iNES Header Editor"), MB_YESNOCANCEL | MB_ICONQUESTION);
			if (result == IDCANCEL)
				return FALSE;
			if (result == IDYES)
				return SaveROM(hDlg);
		}
		fileOpen = FALSE;
		SetDlgItemText(hDlg, IDC_INES_NAME, _T(""));
		UpdateDialog(hDlg);
	}
	return TRUE;
}

BOOL OpenROM (HWND hDlg)
{
	FILE *ROM = _tfopen(filename, _T("r+b"));
	if (ROM == NULL)
	{
		MessageBox(hDlg, _T("Unable to open file for read/write access!"), _T("iNES Header Editor"), MB_OK | MB_ICONERROR);
		UpdateDialog(hDlg);
		return FALSE;
	}
	fseek(ROM, 0, SEEK_SET);
	fread(header, 1, 16, ROM);
	fclose(ROM);

	if (memcmp(header, "NES\x1A", 4))
	{
		MessageBox(hDlg, _T("Selected file is not a valid iNES ROM image!"), _T("iNES Header Editor"), MB_OK | MB_ICONERROR);
		UpdateDialog(hDlg);
		return FALSE;
	}
	fileOpen = TRUE;

	SetDlgItemText(hDlg, IDC_INES_NAME, filename);
	memcpy(header_orig, header, 16);
	Mask = TRUE;	// prevent the radio button selections from cascading through to the rest
	if ((header[7] & 0x0C) == 0x04)
	{
		MessageBox(hDlg, _T("Selected ROM appears to have been corrupted by \"DiskDude!\" - cleaning..."), _T("iNES Header Editor"), MB_OK | MB_ICONWARNING);
		memset(header+7, 0, 9);
	}
	else if (CheckHeader(FALSE) && 
		(MessageBox(hDlg, _T("Unrecognized or inconsistent data detected in ROM header! Do you wish to clean it?"), _T("iNES Header Editor"), MB_YESNO | MB_ICONQUESTION) == IDYES))
	{
		CheckHeader(TRUE);
	}
	UpdateDialog(hDlg);
	return TRUE;
}

INT_PTR CALLBACK dlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	const TCHAR *RAMsizes[16] = {_T("None"), _T("128"), _T("256"), _T("512"), _T("1K"), _T("2K"), _T("4K"), _T("8K"), _T("16K"), _T("32K"), _T("64K"), _T("128K"), _T("256K"), _T("512K"), _T("1MB"), _T("N/A") };
	const TCHAR *VSPPUs[16] = {_T("RP2C03B"), _T("RP2C03G"), _T("RP2C04-0001"), _T("RP2C04-0002"), _T("RP2C04-0003"), _T("RP2C04-0004"), _T("RC2C03B"), _T("RC2C03C"), _T("RC2C05-01"), _T("RC2C05-02"), _T("RC2C05-03"), _T("RC2C05-04"), _T("RC2C05-05"), _T("N/A"), _T("N/A"), _T("N/A") };
	const TCHAR *VSFlags[16] = {_T("Normal"), _T("RBI Baseball"), _T("TKO Boxing"), _T("Super Xevious"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A"), _T("N/A") };

	OPENFILENAME ofn;
	int i, n;

	switch (message) 
	{
	case WM_INITDIALOG:
		fileOpen = FALSE;
		modified = FALSE;
		EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
		for (i = 0; i < 16; i++)
		{
			SendDlgItemMessage(hDlg, IDC_INES_PRAM, CB_ADDSTRING, 0, (LPARAM)RAMsizes[i]);
			SendDlgItemMessage(hDlg, IDC_INES_PSAV, CB_ADDSTRING, 0, (LPARAM)RAMsizes[i]);
			SendDlgItemMessage(hDlg, IDC_INES_CRAM, CB_ADDSTRING, 0, (LPARAM)RAMsizes[i]);
			SendDlgItemMessage(hDlg, IDC_INES_CSAV, CB_ADDSTRING, 0, (LPARAM)RAMsizes[i]);
			SendDlgItemMessage(hDlg, IDC_INES_VSPPU, CB_ADDSTRING, 0, (LPARAM)VSPPUs[i]);
			SendDlgItemMessage(hDlg, IDC_INES_VSFLAG, CB_ADDSTRING, 0, (LPARAM)VSFlags[i]);
		}
		UpdateDialog(hDlg);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_INES_BROWSE:
			if (!CloseROM(hDlg))
				break;

			filename[0] = 0;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.hInstance = hInst;
			ofn.lpstrFilter = _T("iNES ROM Images (*.NES)\0") _T("*.NES\0") _T("\0");
			ofn.lpstrCustomFilter = NULL;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = _T("Open ROM Image");
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrDefExt = NULL;
			ofn.lCustData = 0;
			ofn.lpfnHook = NULL;
			ofn.lpTemplateName = NULL;

			SetDlgItemText(hDlg, IDC_INES_NAME, _T(""));
			if (GetOpenFileName(&ofn))
				OpenROM(hDlg);
			else	UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_VER1:
			if (Mask)
				break;
			header[7] &= 0xF3;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_VER2:
			if (Mask)
				break;
			header[7] &= 0xF3;
			header[7] |= 0x08;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_MAP:
			if (Mask)
				break;
			n = GetDlgItemInt(hDlg, IDC_INES_MAP, NULL, FALSE);
			header[6] &= 0x0F;
			header[6] |= (n & 0x00F) << 4;
			header[7] &= 0x0F;
			header[7] |= (n & 0x0F0) << 0;
			if (isExtended)
			{
				header[8] &= 0x0F;
				header[8] |= (n & 0xF00) >> 8;
			}
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_PRG:
			if (Mask)
				break;
			n = GetDlgItemInt(hDlg, IDC_INES_PRG, NULL, FALSE);
			header[4] = n & 0xFF;
			if (isExtended)
			{
				header[9] &= 0xF0;
				header[9] |= (n & 0xF00) >> 8;
			}
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_CHR:
			if (Mask)
				break;
			n = GetDlgItemInt(hDlg, IDC_INES_CHR, NULL, FALSE);
			header[5] = n & 0xFF;
			if (isExtended)
			{
				header[9] &= 0x0F;
				header[9] |= (n & 0xF00) >> 4;
			}
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_HORIZ:
			if (Mask)
				break;
			header[6] &= ~0x01;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_VERT:
			if (Mask)
				break;
			header[6] |= 0x01;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_4SCR:
			if (Mask)
				break;
			if (IsDlgButtonChecked(hDlg, IDC_INES_4SCR))
				header[6] |= 0x08;
			else	header[6] &= ~0x08;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_BATT:
			if (Mask)
				break;
			if (IsDlgButtonChecked(hDlg, IDC_INES_BATT))
				header[6] |= 0x02;
			else	header[6] &= ~0x02;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_TRAIN:
			if (Mask)
				break;
			if (IsDlgButtonChecked(hDlg, IDC_INES_TRAIN))
				header[6] |= 0x04;
			else	header[6] &= ~0x04;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_VS:
			if (Mask)
				break;
			if (IsDlgButtonChecked(hDlg, IDC_INES_VS))
				header[7] |= 0x01;
			else	header[7] &= ~0x01;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_PC10:
			if (Mask)
				break;
			if (IsDlgButtonChecked(hDlg, IDC_INES_PC10))
				header[7] |= 0x02;
			else	header[7] &= ~0x02;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_SUBMAP:
			if (Mask)
				break;
			n = GetDlgItemInt(hDlg, IDC_INES_SUBMAP, NULL, FALSE);
			header[8] &= 0x0F;
			header[8] |= (n & 0xF) << 4;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_PRAM:
			if (Mask)
				break;
			n = (int)SendDlgItemMessage(hDlg, IDC_INES_PRAM, CB_GETCURSEL, 0, 0);
			header[10] &= 0xF0;
			header[10] |= n & 0xF;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_PSAV:
			if (Mask)
				break;
			n = (int)SendDlgItemMessage(hDlg, IDC_INES_PSAV, CB_GETCURSEL, 0, 0);
			header[10] &= 0x0F;
			header[10] |= (n & 0xF) << 4;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_CRAM:
			if (Mask)
				break;
			n = (int)SendDlgItemMessage(hDlg, IDC_INES_CRAM, CB_GETCURSEL, 0, 0);
			header[11] &= 0xF0;
			header[11] |= n & 0xF;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_CSAV:
			if (Mask)
				break;
			n = (int)SendDlgItemMessage(hDlg, IDC_INES_CSAV, CB_GETCURSEL, 0, 0);
			header[11] &= 0x0F;
			header[11] |= (n & 0xF) << 4;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_NTSC:
			if (Mask)
				break;
			header[12] &= ~0x03;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_PAL:
			if (Mask)
				break;
			header[12] &= ~0x03;
			header[12] |= 0x01;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_DUAL:
			if (Mask)
				break;
			header[12] &= ~0x03;
			header[12] |= 0x02;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_VSPPU:
			if (Mask)
				break;
			n = (int)SendDlgItemMessage(hDlg, IDC_INES_VSPPU, CB_GETCURSEL, 0, 0);
			header[13] &= 0xF0;
			header[13] |= n & 0xF;
			UpdateDialog(hDlg);
			return TRUE;

		case IDC_INES_VSFLAG:
			if (Mask)
				break;
			n = (int)SendDlgItemMessage(hDlg, IDC_INES_VSFLAG, CB_GETCURSEL, 0, 0);
			header[13] &= 0x0F;
			header[13] |= (n & 0xF) << 4;
			UpdateDialog(hDlg);
			return TRUE;

		case IDOK:
			SaveROM(hDlg);
			return TRUE;
		case IDCLOSE:
			CloseROM(hDlg);
			return TRUE;
		case IDCANCEL:
			if (CloseROM(hDlg))
				DestroyWindow(hDlg);
			return TRUE;
		}
		break;
	case WM_DROPFILES:
		if (!CloseROM(hDlg))
			break;
		DragQueryFile((HDROP)wParam, 0, filename, MAX_PATH);
		DragFinish((HDROP)wParam);
		OpenROM(hDlg);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}