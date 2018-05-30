#include <windows.h>
#include <CommCtrl.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include "resource.h"
using namespace std;

const char g_szClassName[] = "myWindowClass";
char g_szChildClassName[] = "myChildrenClass";
char g_szGraficClassName[] = "myGraficClass";

HWND g_hMDIClient = NULL;
HWND g_hMainWindow = NULL;
HWND hListGlobal = NULL;

LPSTR dataEditDialog = NULL;

#define IDC_MAIN_MDI		101
#define ID_MDI_FIRSTCHILD	50000

LPSTR itoa(int val, int base) {
	static char buf[32] = { 0 };
	int i = 30;
	for (; val && i; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i + 1];
}

int atoi(LPSTR texto) {
	int num = 0, lim = strlen(texto);
	for (int i = 0; i < lim; i++) {
		switch (texto[i]) {
		case '1':
			num = num * 10 + 1;
			break;
		case '2':
			num = num * 10 + 2;
			break;
		case '3':
			num = num * 10 + 3;
			break;
		case '4':
			num = num * 10 + 4;
			break;
		case '5':
			num = num * 10 + 5;
			break;
		case '6':
			num = num * 10 + 6;
			break;
		case '7':
			num = num * 10 + 7;
			break;
		case '8':
			num = num * 10 + 8;
			break;
		case '9':
			num = num * 10 + 9;
			break;
		case '0':
			num = num * 10;
			break;
		}
	}
	return num;
}

BOOL putAListBox(HWND hListbox, LPSTR strin){
	BOOL point = TRUE, firstCheck = FALSE;
	LPSTR data;
	int subs = 0;
	int lim = strlen(strin) + 1;
	for (int i = 0; i < lim; i++) {
		if (isdigit(strin[i])) {
			if (firstCheck == FALSE) {
				int tam = 0, j = i;
				do {
					tam++;
					j++;
				} while (isdigit(strin[j]));
				data = (LPSTR)GlobalAlloc(GPTR, tam + 1);
				firstCheck = TRUE;
			}
			point = FALSE;
			switch (strin[i]) {
			case '1':
				subs = subs * 10 + 1;
				break;
			case '2':
				subs = subs * 10 + 2;
				break;
			case '3':
				subs = subs * 10 + 3;
				break;
			case '4':
				subs = subs * 10 + 4;
				break;
			case '5':
				subs = subs * 10 + 5;
				break;
			case '6':
				subs = subs * 10 + 6;
				break;
			case '7':
				subs = subs * 10 + 7;
				break;
			case '8':
				subs = subs * 10 + 8;
				break;
			case '9':
				subs = subs * 10 + 9;
				break;
			case '0':
				subs = subs * 10;
				break;
			}
		}
		else if (point == FALSE) {
			data = itoa(subs, 10);
			int index = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)data);
			SendMessage(hListbox, LB_SETITEMDATA, (WPARAM)index, NULL);
			data = NULL;
			GlobalFree(data);
			subs = 0;
			firstCheck = FALSE;
			point = TRUE;
		}
	}
	return TRUE;
}

BOOL LoadTextFileToList(HWND hList, LPCTSTR pszFileName){
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if (pszFileText != NULL)
			{
				DWORD dwRead;

				if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0; // Add null terminator
					if(putAListBox(hList, pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

void DoFileOpen(HWND hwnd){
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if (GetOpenFileName(&ofn))
	{
		HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
		if (LoadTextFileToList(hList, szFileName))
		{
			SetWindowText(hwnd, szFileName);
		}
	}
}

HWND CreateNewMDIChild(HWND hMDIClient, int tipo){

	MDICREATESTRUCT mcs;
	HWND hChild;
	
	if (1==tipo) {
		mcs.szTitle = "Nuevo archivo";
		mcs.szClass = g_szChildClassName;
		mcs.hOwner = GetModuleHandle(NULL);
		mcs.x = CW_USEDEFAULT;
		mcs.y = CW_USEDEFAULT;
		mcs.cx = 410;
		mcs.cy = 380;
		mcs.style = MDIS_ALLCHILDSTYLES;
	}
	else if (2==tipo) {
		mcs.szTitle = "Distribucion de Frecuencias";
		mcs.szClass = g_szGraficClassName;
		mcs.hOwner = GetModuleHandle(NULL);
		mcs.x = CW_USEDEFAULT;
		mcs.y = CW_USEDEFAULT;
		mcs.cx = 730;
		mcs.cy = 530;
		mcs.style = MDIS_ALLCHILDSTYLES;
	}
	
	hChild = (HWND)SendMessage(hMDIClient, WM_MDICREATE, 0, (LONG)&mcs);
	if (!hChild)
	{
		MessageBox(hMDIClient, "NO SE CREO EL HIJO", "Oh Oh...",
			MB_ICONEXCLAMATION | MB_OK);
	}
	return hChild;
}

//Procedure para el dialogo
BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
	switch (Message)
	{
	case WM_INITDIALOG:

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:

			HWND hEditdlg;
			DWORD dwTextLength;

			hEditdlg = GetDlgItem(hwnd, IDC_EDIT_DATOS);
			dwTextLength = GetWindowTextLength(hEditdlg);
			// No need to bother if there's no text.
			if (dwTextLength > 0) {
				dataEditDialog = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
				if (dataEditDialog != NULL) {
					GetWindowText(hEditdlg, dataEditDialog, dwTextLength + 1);
				}
			}
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

//Window Procedure para el hijo
LRESULT CALLBACK MDIChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch (msg){
	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit, hListbox, hButtonAdd, hButtonDel;
		HWND hBtnProm, hTextProm, hBtnMediana, hTextMediana, hBtnModa, hTextModa, hBtnDAM, hTextDAM,
			hBtnVarianza, hTextVarianza, hBtnStandard, hTextStandard;

		hfDefault = (HFONT)GetStockObject(DEFAULT_PALETTE);
		
		//Create ListBox Control
		hListbox = CreateWindowEx(0, "LISTBOX", "",
			WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL,
			10, 10, 150, 1000, hwnd, (HMENU)IDC_LISTBOX_EDIT, GetModuleHandle(NULL), NULL);
		if (hListbox == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la lista", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hListbox, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Button Control Add
		hButtonAdd = CreateWindowEx(0, "BUTTON", "Agregar",
			WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER | WS_TABSTOP,
			170, 10, 100, 25, hwnd, (HMENU)IDC_BUTTON_ADD, GetModuleHandle(NULL), NULL);
		if (hButtonAdd == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el button ADD", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hButtonAdd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		// Create Edit Control Add
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
			WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_TABSTOP,
			280, 10, 100, 25, hwnd, (HMENU)IDC_EDITBOX, GetModuleHandle(NULL), NULL);
		if (hEdit == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el Edit.", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Button Control Delete
		hButtonDel = CreateWindowEx(0, "BUTTON", "Borrar Item",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			170, 45, 100, 25, hwnd, (HMENU)IDC_BUTTON_DELETE, GetModuleHandle(NULL), NULL);
		if (hButtonDel == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el button Delete", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hButtonDel, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		
		//Create Boton Promedio
		hBtnProm = CreateWindowEx(0, "BUTTON", "Media",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			170, 90, 100, 25, hwnd, (HMENU)IDC_BUTTON_PROM, GetModuleHandle(NULL), NULL);
		if (hBtnProm == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el button Promedio", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hBtnProm, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Etiqueta Promedio
		hTextProm = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			280, 95, 100, 25, hwnd, (HMENU)IDC_TEXT_PROM, GetModuleHandle(NULL), NULL);
		if (hTextProm == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la etiqueta promedio", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hTextProm, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Boton Mediana
		hBtnMediana = CreateWindowEx(0, "BUTTON", "Mediana",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			170, 125, 100, 25, hwnd, (HMENU)IDC_BUTTON_MEDIANA, GetModuleHandle(NULL), NULL);
		if (hBtnMediana == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el boton Mediana", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hBtnMediana, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Etiqueta Mediana
		hTextMediana = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			280, 130, 100, 25, hwnd, (HMENU)IDC_TEXT_MEDIANA, GetModuleHandle(NULL), NULL);
		if (hTextMediana == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la etiqueta Mediana", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hTextMediana, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Button Control Moda
		hBtnModa = CreateWindowEx(0, "BUTTON", "Moda",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			170, 160, 100, 25, hwnd, (HMENU)IDC_BUTTON_MODA, GetModuleHandle(NULL), NULL);
		if (hBtnModa == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el button Moda", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hBtnModa, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Etiqueta Moda
		hTextModa = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			280, 165, 100, 25, hwnd, (HMENU)IDC_TEXT_MODA, GetModuleHandle(NULL), NULL);
		if (hTextModa == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la etiqueta Moda", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hTextModa, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Button Control DAM
		hBtnDAM= CreateWindowEx(0, "BUTTON", "DAM",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			170, 195, 100, 25, hwnd, (HMENU)IDC_BUTTON_DAM, GetModuleHandle(NULL), NULL);
		if (hBtnDAM == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el button DAM", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hBtnDAM, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Etiqueta DAM
		hTextDAM = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			280, 200, 100, 25, hwnd, (HMENU)IDC_TEXT_DAM, GetModuleHandle(NULL), NULL);
		if (hTextDAM == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la etiqueta DAM", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hTextDAM, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Button Control Varianza
		hBtnVarianza = CreateWindowEx(0, "BUTTON", "Varianza",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			170, 230, 100, 25, hwnd, (HMENU)IDC_BUTTON_VARIANZA, GetModuleHandle(NULL), NULL);
		if (hBtnVarianza == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el button Varianza", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hBtnVarianza, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Etiqueta Varianza
		hTextVarianza = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			280, 235, 100, 25, hwnd, (HMENU)IDC_TEXT_VARIANZA, GetModuleHandle(NULL), NULL);
		if (hTextVarianza == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la etiqueta Varianza", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hTextVarianza, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Button Control Standard
		hBtnStandard = CreateWindowEx(0, "BUTTON", "Desv Estandar",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			170, 265, 100, 25, hwnd, (HMENU)IDC_BUTTON_STANDARD, GetModuleHandle(NULL), NULL);
		if (hBtnStandard == NULL)
			MessageBox(hwnd, "Puede que no se haya creado el button Standard", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hBtnStandard, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Create Etiqueta Standard
		hTextStandard= CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			280, 270, 100, 25, hwnd, (HMENU)IDC_TEXT_STANDARD, GetModuleHandle(NULL), NULL);
		if (hTextStandard == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la etiqueta Standard", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hTextStandard, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		if (dataEditDialog != NULL) {
			//Se envian los datos del dialogo al ListBox
			putAListBox(hListbox, dataEditDialog);
			dataEditDialog = NULL;
			GlobalFree(dataEditDialog);
		}

	}break;
	/*case WM_ACTIVATE: {
		HMENU hMenu, hFileMenu;
		UINT EnableFlag;

		hMenu = GetMenu(g_hMainWindow);
		if (hwnd == (HWND)lParam) {
			//being activated, enable the menus
			EnableFlag = MF_ENABLED;
		}
		else {
			//being de-activated, gray the menus
			EnableFlag = MF_GRAYED;
		}
		hFileMenu = GetSubMenu(hMenu, 0);
		EnableMenuItem(hFileMenu, ID_OPCIONES_GRAFICA, MF_BYCOMMAND | EnableFlag);
	}break;*/
	case WM_MDIACTIVATE: {
		HMENU hMenu, hFileMenu, hOptionMenu;
		UINT EnableFlag;

		hMenu = GetMenu(g_hMainWindow);
		if (hwnd == (HWND)lParam) {
			//being activated, enable the menus
			EnableFlag = MF_ENABLED;
		}else{
			//being de-activated, gray the menus
			EnableFlag = MF_GRAYED;
		}

		EnableMenuItem(hMenu, 1, MF_BYPOSITION | EnableFlag);
		EnableMenuItem(hMenu, 2, MF_BYPOSITION | EnableFlag);

		hFileMenu = GetSubMenu(hMenu, 0);
		EnableMenuItem(hFileMenu, ID_ARCHIVO_CERRAR, MF_BYCOMMAND | EnableFlag);
		EnableMenuItem(hFileMenu, ID_ARCHIVO_GUARDAR, MF_BYCOMMAND | EnableFlag);

		hOptionMenu = GetSubMenu(hMenu, 1);
		EnableMenuItem(hOptionMenu, ID_OPCIONES_GRAFICA, MF_BYCOMMAND | EnableFlag);

		DrawMenuBar(g_hMainWindow);
	}break;
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_ADD: {

			HWND hEdit;
			DWORD dwTextLength;

			hEdit = GetDlgItem(hwnd, IDC_EDITBOX);
			dwTextLength = GetWindowTextLength(hEdit);
			// No need to bother if there's no text.
			if (dwTextLength > 0) {
				LPSTR pszText;
				DWORD dwBufferSize = dwTextLength + 1;

				pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
				if (pszText != NULL) {
					if (GetWindowText(hEdit, pszText, dwBufferSize)) {
						SendDlgItemMessage(hwnd, IDC_LISTBOX_EDIT, LB_ADDSTRING, 0, (LPARAM)pszText);
					}
					GlobalFree(pszText);
				}
			}
		}break;
		case IDC_BUTTON_DELETE: {
			// When the user clicks the Remove button, we first get the number of selected items
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
			if (count != LB_ERR) {
				if (count != 0) {
					// And then allocate room to store the list of selected items.
					int i;
					int *buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
					SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);

					// Now we loop through the list and remove each item that was selected.  
					for (i = count - 1; i >= 0; i--)
						SendMessage(hList, LB_DELETESTRING, (WPARAM)buf[i], 0);
					GlobalFree(buf);
				}else {
					MessageBox(hwnd, "Items no seleccionado", "Warning", MB_OK);
				}
			}else {
				MessageBox(hwnd, "Error contando items :(", "Warning", MB_OK);
			}
		}break;
		case IDC_BUTTON_PROM: {
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				float total = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					int num = atoi(text);
					total += num;
					GlobalFree(text);
				}
				float promedio = total / all;
				LPSTR stri = itoa(promedio, 10);
				SetDlgItemText(hwnd, IDC_TEXT_PROM, stri);
			}
			else {
				MessageBox(hwnd, "Error select items", "Warning", MB_OK);
			}
		}break;
		case IDC_BUTTON_MEDIANA: {
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				int arreglo[255] = { 0 };
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					int num = atoi(text);
					GlobalFree(text);
					arreglo[i] = num;
				}
				for (int i = 0; i < all; i++) {
					for (int j = all - 1; j >= i; j--) {
						if (arreglo[j] > arreglo[j + 1]) {
							int temp = arreglo[j];
							arreglo[j] = arreglo[j + 1];
							arreglo[j + 1] = temp;
						}
					}
				}
				if (all % 2 == 1) {
					int mitad = ((all - 1) / 2) + 1;
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)mitad - 1, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)mitad - 1, (LPARAM)text);
					SetDlgItemText(hwnd, IDC_TEXT_MEDIANA, text);
					GlobalFree(text);
				}
				else {
					int mitad = all / 2;
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)mitad - 1, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)mitad - 1, (LPARAM)text);
					int num1 = atoi(text);
					GlobalFree(text);
					range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)mitad, 0);
					text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)mitad, (LPARAM)text);
					int num2 = atoi(text);
					GlobalFree(text);
					int mediana = (num1 + num2) / 2;
					SetDlgItemInt(hwnd, IDC_TEXT_MEDIANA, mediana, false);
				}
			}
			else {
				MessageBox(hwnd, "Error select items", "Warning", MB_OK);
			}
		}break;
		case IDC_BUTTON_MODA: {
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				int repeMaximas = 0, aidi = 0, repetidas;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					int num1 = atoi(text);
					repetidas = 0;
					for (int j = i; j < all; j++) {
						range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)j, 0);
						LPSTR text2 = (LPSTR)GlobalAlloc(GPTR, range + 1);
						SendMessage(hList, LB_GETTEXT, (WPARAM)j, (LPARAM)text2);
						int num2 = atoi(text2);
						if (num1 == num2)
							repetidas++;
						GlobalFree(text2);
					}
					GlobalFree(text);
					if (repetidas > repeMaximas) {
						repeMaximas = repetidas;
						aidi = i;
					}
				}
				int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)aidi, 0);
				LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
				SendMessage(hList, LB_GETTEXT, (WPARAM)aidi, (LPARAM)text);
				SetDlgItemText(hwnd, IDC_TEXT_MODA, text);
				GlobalFree(text);
			}
			else {
				MessageBox(hwnd, "Error select items", "Warning", MB_OK);
			}
		}break;
		case IDC_BUTTON_DAM: {
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				float total = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					float num = atoi(text);
					total += num;
					GlobalFree(text);
				}
				float promedio = total / all;
				int da = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					float num = atoi(text);
					int result = num - promedio;
					float absoluto = abs(result);
					da += absoluto;
				}
				float dam = da / all;
				SetDlgItemInt(hwnd, IDC_TEXT_DAM, dam, false);
			}
			else {
				MessageBox(hwnd, "Error select items", "Warning", MB_OK);
			}
		}break;
		case IDC_BUTTON_VARIANZA: {
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				float total = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					float num = atoi(text);
					total += num;
					GlobalFree(text);
				}
				float promedio = total / all;
				int var = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					float num = atoi(text);
					float result = pow(num - promedio, 2);
					var += result;
				}
				float varianza = var / all;
				SetDlgItemInt(hwnd, IDC_TEXT_VARIANZA, varianza, false);
			}
			else {
				MessageBox(hwnd, "Error select items", "Warning", MB_OK);
			}
		}break;
		case IDC_BUTTON_STANDARD: {
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				float total = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					float num = atoi(text);
					total += num;
					GlobalFree(text);
				}
				float promedio = total / all;
				int var = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					float num = atoi(text);
					float result = pow(num - promedio, 2);
					var += result;
				}
				float desviacion = pow(var / all, 0.5);
				SetDlgItemInt(hwnd, IDC_TEXT_STANDARD, desviacion, false);
			}
			else {
				MessageBox(hwnd, "Error select items", "Warning", MB_OK);
			}
		}break;
		}
	}break;
	default:
		return DefMDIChildProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//Window Procedure para la grafica (Toda la creacion de la grafica)
LRESULT CALLBACK WinGraficProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	HDC hDC;
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_CREATE: {
		HFONT hfDefault;
		HWND hListbox, hEtiquetaH, hArreglo;
		HWND htext1, htext2, htext3, htext4, htext5, htext6, htext7, htext8, htext9, htext10, htext11, htext12;
		HWND htext13, htext14, htext15, htext16, htext17, htext18, htext19, htext20, htext21, htext22, htext23;
		HWND htext24, htext25, htext26, htext27, htext28, htext29, htext30;
		HWND hcant1, hcant2, hcant3, hcant4, hcant5, hcant6, hcant7, hcant8, hcant9, hcant10;
		HWND hEtiqAbajo, hEtiqIz1, hEtiqIz2, hEtiqIz3, hEtiqIz4, hEtiqIz5, hEtiqIz6, hEtiqIz7, hEtiqIz8;

		hfDefault = (HFONT)GetStockObject(DEFAULT_PALETTE);

		//Etiquetas de Ayuda
		//Aqui el listbox es creado para transferir los datos de la ventana seleccionada
		hListbox = CreateWindowEx(0, "LISTBOX", "",
			WS_CHILD | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL | WS_TABSTOP,
			10, 10, 150, 315, hwnd, (HMENU)IDC_LISTBOX_EDIT, GetModuleHandle(NULL), NULL);
		if (hListbox == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la lista", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hListbox, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		//ListBox usado como arreglo
		hArreglo = CreateWindowEx(0, "LISTBOX", "",
			WS_CHILD | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL | WS_TABSTOP,
			200, 10, 150, 315, hwnd, (HMENU)IDC_ARREGLO, GetModuleHandle(NULL), NULL);
		if (hArreglo == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la lista del arreglo", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hArreglo, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		//Etiqueta de ayuda
		hEtiquetaH = CreateWindowEx(0, "STATIC", "nada",
			WS_CHILD | WS_TABSTOP,
			300, 10, 50, 30, hwnd, (HMENU)IDC_TEXT_AYUDA, GetModuleHandle(NULL), NULL);
		if (hEtiquetaH == NULL)
			MessageBox(hwnd, "Puede que no se haya creado la etiqueta ayuda", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiquetaH, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Etiquetas para diseño
		htext1 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT1, GetModuleHandle(NULL), NULL);
		if (htext1 == NULL)
			MessageBox(hwnd, "Etiqueta 1 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext1, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext2 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT2, GetModuleHandle(NULL), NULL);
		if (htext2 == NULL)
			MessageBox(hwnd, "Etiqueta 2 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext2, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext3 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT3, GetModuleHandle(NULL), NULL);
		if (htext3 == NULL)
			MessageBox(hwnd, "Etiqueta 3 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext3, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext4 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT4, GetModuleHandle(NULL), NULL);
		if (htext4 == NULL)
			MessageBox(hwnd, "Etiqueta 4 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext4, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext5 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT5, GetModuleHandle(NULL), NULL);
		if (htext5 == NULL)
			MessageBox(hwnd, "Etiqueta 5 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext5, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext6 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT6, GetModuleHandle(NULL), NULL);
		if (htext6 == NULL)
			MessageBox(hwnd, "Etiqueta 6 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext6, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext7 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT7, GetModuleHandle(NULL), NULL);
		if (htext7 == NULL)
			MessageBox(hwnd, "Etiqueta 7 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext7, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext8 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT8, GetModuleHandle(NULL), NULL);
		if (htext8 == NULL)
			MessageBox(hwnd, "Etiqueta 8 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext8, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext9 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT9, GetModuleHandle(NULL), NULL);
		if (htext9 == NULL)
			MessageBox(hwnd, "Etiqueta 9 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext9, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext10 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT10, GetModuleHandle(NULL), NULL);
		if (htext10 == NULL)
			MessageBox(hwnd, "Etiqueta 10 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext10, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext11 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT11, GetModuleHandle(NULL), NULL);
		if (htext11 == NULL)
			MessageBox(hwnd, "Etiqueta 11 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext11, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext12 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT12, GetModuleHandle(NULL), NULL);
		if (htext12 == NULL)
			MessageBox(hwnd, "Etiqueta 12 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext12, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext13 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT13, GetModuleHandle(NULL), NULL);
		if (htext13 == NULL)
			MessageBox(hwnd, "Etiqueta 13 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext13, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext14 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT14, GetModuleHandle(NULL), NULL);
		if (htext14 == NULL)
			MessageBox(hwnd, "Etiqueta 14 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext14, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext15 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT15, GetModuleHandle(NULL), NULL);
		if (htext15 == NULL)
			MessageBox(hwnd, "Etiqueta 15 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext15, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext16 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT16, GetModuleHandle(NULL), NULL);
		if (htext16 == NULL)
			MessageBox(hwnd, "Etiqueta 16 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext16, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext17 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT17, GetModuleHandle(NULL), NULL);
		if (htext17 == NULL)
			MessageBox(hwnd, "Etiqueta 17 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext17, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext18 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT18, GetModuleHandle(NULL), NULL);
		if (htext18 == NULL)
			MessageBox(hwnd, "Etiqueta 18 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext18, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext19 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT19, GetModuleHandle(NULL), NULL);
		if (htext19 == NULL)
			MessageBox(hwnd, "Etiqueta 19 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext19, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext20 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT20, GetModuleHandle(NULL), NULL);
		if (htext20 == NULL)
			MessageBox(hwnd, "Etiqueta 20 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext20, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext21 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT21, GetModuleHandle(NULL), NULL);
		if (htext21 == NULL)
			MessageBox(hwnd, "Etiqueta 21 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext21, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext22 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT22, GetModuleHandle(NULL), NULL);
		if (htext22 == NULL)
			MessageBox(hwnd, "Etiqueta 22 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext22, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext23 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT23, GetModuleHandle(NULL), NULL);
		if (htext23 == NULL)
			MessageBox(hwnd, "Etiqueta 23 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext23, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext24 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT24, GetModuleHandle(NULL), NULL);
		if (htext24 == NULL)
			MessageBox(hwnd, "Etiqueta 24 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext24, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext25 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT25, GetModuleHandle(NULL), NULL);
		if (htext25 == NULL)
			MessageBox(hwnd, "Etiqueta 25 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext25, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext26 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT26, GetModuleHandle(NULL), NULL);
		if (htext26 == NULL)
			MessageBox(hwnd, "Etiqueta 26 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext26, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext27 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT27, GetModuleHandle(NULL), NULL);
		if (htext27 == NULL)
			MessageBox(hwnd, "Etiqueta 27 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext27, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext28 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT28, GetModuleHandle(NULL), NULL);
		if (htext28 == NULL)
			MessageBox(hwnd, "Etiqueta 28 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext28, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext29 = CreateWindowEx(0, "STATIC", "a",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT29, GetModuleHandle(NULL), NULL);
		if (htext29 == NULL)
			MessageBox(hwnd, "Etiqueta 29 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext29, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		htext30 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_TEXT30, GetModuleHandle(NULL), NULL);
		if (htext30 == NULL)
			MessageBox(hwnd, "Etiqueta 30 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(htext30, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Etiquetas para las cantidades de los intervalos
		hcant1 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT1, GetModuleHandle(NULL), NULL);
		if (hcant1 == NULL)
			MessageBox(hwnd, "Etiqueta cant1 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant1, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant2 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT2, GetModuleHandle(NULL), NULL);
		if (hcant2 == NULL)
			MessageBox(hwnd, "Etiqueta cant2 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant2, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant3 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT3, GetModuleHandle(NULL), NULL);
		if (hcant3 == NULL)
			MessageBox(hwnd, "Etiqueta cant3 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant3, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant4 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT4, GetModuleHandle(NULL), NULL);
		if (hcant4 == NULL)
			MessageBox(hwnd, "Etiqueta cant4 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant4, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant5 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT5, GetModuleHandle(NULL), NULL);
		if (hcant5 == NULL)
			MessageBox(hwnd, "Etiqueta cant5 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant5, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant6 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT6, GetModuleHandle(NULL), NULL);
		if (hcant6 == NULL)
			MessageBox(hwnd, "Etiqueta cant6 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant6, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant7 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT7, GetModuleHandle(NULL), NULL);
		if (hcant7 == NULL)
			MessageBox(hwnd, "Etiqueta cant7 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant7, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant8 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT8, GetModuleHandle(NULL), NULL);
		if (hcant8 == NULL)
			MessageBox(hwnd, "Etiqueta cant8 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant8, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant9 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT9, GetModuleHandle(NULL), NULL);
		if (hcant9 == NULL)
			MessageBox(hwnd, "Etiqueta cant9 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant9, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hcant10 = CreateWindowEx(0, "STATIC", "-",
			WS_CHILD | WS_TABSTOP,
			50, 700, 50, 30, hwnd, (HMENU)ID_CANT10, GetModuleHandle(NULL), NULL);
		if (hcant10 == NULL)
			MessageBox(hwnd, "Etiqueta cant10 -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hcant10, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Etiqueta de Abajo
		hEtiqAbajo = CreateWindowEx(0, "STATIC", "I N T E R V A L O S",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			300, 450, 150, 15, hwnd, (HMENU)ID_STATICABAJO, GetModuleHandle(NULL), NULL);
		if (hEtiqAbajo == NULL)
			MessageBox(hwnd, "Etiqueta statica abajo -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqAbajo, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Etiquetas de Izquierda
		hEtiqIz1 = CreateWindowEx(0, "STATIC", "#",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 100, 15, 15, hwnd, (HMENU)ID_STATICIZ1, GetModuleHandle(NULL), NULL);
		if (hEtiqIz1 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz1, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hEtiqIz2 = CreateWindowEx(0, "STATIC", "D",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 140, 20, 15, hwnd, (HMENU)ID_STATICIZ2, GetModuleHandle(NULL), NULL);
		if (hEtiqIz2 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz2, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hEtiqIz3 = CreateWindowEx(0, "STATIC", "E",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 160, 20, 15, hwnd, (HMENU)ID_STATICIZ3, GetModuleHandle(NULL), NULL);
		if (hEtiqIz3 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz3, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hEtiqIz4 = CreateWindowEx(0, "STATIC", "I",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 200, 20, 15, hwnd, (HMENU)ID_STATICIZ4, GetModuleHandle(NULL), NULL);
		if (hEtiqIz4 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz4, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hEtiqIz5 = CreateWindowEx(0, "STATIC", "T",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 220, 20, 15, hwnd, (HMENU)ID_STATICIZ5, GetModuleHandle(NULL), NULL);
		if (hEtiqIz5 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz5, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hEtiqIz6 = CreateWindowEx(0, "STATIC", "E",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 240, 20, 15, hwnd, (HMENU)ID_STATICIZ6, GetModuleHandle(NULL), NULL);
		if (hEtiqIz6 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz6, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hEtiqIz7 = CreateWindowEx(0, "STATIC", "M",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 260, 20, 15, hwnd, (HMENU)ID_STATICIZ7, GetModuleHandle(NULL), NULL);
		if (hEtiqIz7 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz7, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		hEtiqIz8 = CreateWindowEx(0, "STATIC", "S",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			25, 280, 20, 15, hwnd, (HMENU)ID_STATICIZ8, GetModuleHandle(NULL), NULL);
		if (hEtiqIz8 == NULL)
			MessageBox(hwnd, "Etiqueta statica izquierda -problema-", "Error", MB_OK | MB_ICONERROR);
		SendMessage(hEtiqIz8, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Aqui se agregan los datos al list box
		int all = SendMessage(hListGlobal, LB_GETCOUNT, 0, 0);
		if (all != LB_ERR) {
			for (int i = 0; i < all; i++) {
				int range = SendMessage(hListGlobal, LB_GETTEXTLEN, (WPARAM)i, 0);
				LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
				SendMessage(hListGlobal, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
				SendDlgItemMessage(hwnd, IDC_LISTBOX_EDIT, LB_ADDSTRING, 0, (LPARAM)text);
				GlobalFree(text);
			}
		}

		int n = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
		if (n != LB_ERR) {
			//agrega el primer item para maximo y minimo
			int range = SendMessage(hListbox, LB_GETTEXTLEN, 0, 0);
			LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
			SendMessage(hListbox, LB_GETTEXT, 0, (LPARAM)text);
			int maxim = atoi(text);
			int minim = atoi(text);
			GlobalFree(text);

			for (int i = 0; i < n; i++) {
				range = SendMessage(hListbox, LB_GETTEXTLEN, (WPARAM)i, 0);
				text = (LPSTR)GlobalAlloc(GPTR, range + 1);
				SendMessage(hListbox, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
				int numer = atoi(text);
				if (maxim < numer) {
					maxim = numer;
				}
				if (minim > numer) {
					minim = numer;
				}
				GlobalFree(text);
			}
			float rango = maxim - minim;
			int numInterv = round(1 + 3.333*log10(n));
			int amplitud = ceil(rango / numInterv);

			int acum = minim;
			int cont;
			//Agrega la cantidad de datos por barra a la lista Arreglo
			for (int j = 0; j < numInterv; j++) {
				cont = 0;
				for (int i = 0; i < n; i++) {
					range = SendMessage(hListbox, LB_GETTEXTLEN, (WPARAM)i, 0);
					text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hListbox, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					int numero = atoi(text);
					GlobalFree(text);
					if ((numero >= acum) && (numero < (acum + amplitud))) {
						cont++;
					}
				}
				//Escriben los textos de las etiquetas para las amplitudes
				int idInicio=ID_TEXT1+j*3, idFinal=ID_TEXT3+j*3;
				SetDlgItemInt(hwnd, idInicio, acum, false);
				SetDlgItemInt(hwnd, idFinal, acum+amplitud-1, false);
				//Se agregan la cantidad  de datos por barra en cada etiqueta de la barra
				int idBarra = ID_CANT1+j;
				SetDlgItemInt(hwnd, idBarra, cont, false);

				acum += amplitud;
				LPSTR txtArreglo = itoa(cont, 10);
				//Aqui se agregan los valores al arreglo
				SendDlgItemMessage(hwnd, IDC_ARREGLO, LB_ADDSTRING, 0, (LPARAM)txtArreglo);
			}
		}
		else {
			MessageBox(hwnd, "Error select items", "Warning", MB_OK);
		}
	}break;
	case WM_SIZE: {
		HWND hArreglo = GetDlgItem(hwnd, IDC_ARREGLO);

		int n = SendMessage(hArreglo, LB_GETCOUNT, 0, 0);
		if (n != LB_ERR) {

			int range = SendMessage(hArreglo, LB_GETTEXTLEN, 0, 0);
			LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
			SendMessage(hArreglo, LB_GETTEXT, 0, (LPARAM)text);
			int maximo = atoi(text);
			GlobalFree(text);

			for (int i = 0; i < n; i++) {
				range = SendMessage(hArreglo, LB_GETTEXTLEN, (WPARAM)i, 0);
				text = (LPSTR)GlobalAlloc(GPTR, range + 1);
				SendMessage(hArreglo, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
				int numer = atoi(text);
				if (maximo < numer) {
					maximo = numer;
				}
				GlobalFree(text);
			}
			float demas = maximo / 5;
			if (demas < 1) {demas = 1;}
			SetDlgItemInt(hwnd, IDC_TEXT_AYUDA, demas, false);
			int total = ceil(maximo + demas);
			int altura = 400 / total;
			int largo = 600 / n;
			int base = 0;
			for (int i = 0; i < n; i++) {
				range = SendMessage(hArreglo, LB_GETTEXTLEN, (WPARAM)i, 0);
				text = (LPSTR)GlobalAlloc(GPTR, range + 1);
				SendMessage(hArreglo, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
				int num = atoi(text);
				GlobalFree(text);
				//Posicion y visibilidad de las etiquetas
				int idText1 = ID_TEXT1 + i * 3;
				int idText2 = ID_TEXT2 + i * 3;
				int idText3 = ID_TEXT3 + i * 3;
				int idBarra = ID_CANT1 + i;
				HWND text1 = GetDlgItem(hwnd, idText1);
				HWND text2 = GetDlgItem(hwnd, idText2);
				HWND text3 = GetDlgItem(hwnd, idText3);
				HWND barra = GetDlgItem(hwnd, idBarra);
				SetWindowPos(text1, HWND_TOP, 0 + base + largo, 420, 15, 15, SWP_SHOWWINDOW);
				SetWindowPos(text2, HWND_TOP, 20 + base + largo, 420, 15, 15, SWP_SHOWWINDOW);
				SetWindowPos(text3, HWND_TOP, 40 + base + largo, 420, 15, 15, SWP_SHOWWINDOW);
				SetWindowPos(barra, HWND_TOP, 65, 400 - altura * num, 15, 15, SWP_SHOWWINDOW);
				base += largo;
			}
			UpdateWindow(hwnd);
		}
		else {
			MessageBox(hwnd, "Error select items x2 :'v", "Warning", MB_OK);
		}
	}break;
	case WM_PAINT: {
		hDC = BeginPaint(hwnd, &ps);
		HWND hArreglo = GetDlgItem(hwnd, IDC_ARREGLO);
		//Rectangulo mayor y el pintor
		Rectangle(hDC, 90, 10, 691, 410);
		CONST LOGBRUSH palette = { BS_SOLID, COLORREF(RGB(0,100,255)) , 0 };
		HBRUSH pincel = CreateBrushIndirect(&palette);
		if (pincel == NULL) {
			MessageBox(hwnd, "Error para crear el pincel :'v", "Warning", MB_OK);
		}
		SelectObject(hDC, pincel);

		int n = SendMessage(hArreglo, LB_GETCOUNT, 0, 0);
		if (n != LB_ERR) {

			int range = SendMessage(hArreglo, LB_GETTEXTLEN, 0, 0);
			LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
			SendMessage(hArreglo, LB_GETTEXT, 0, (LPARAM)text);
			int maximo = atoi(text);
			GlobalFree(text);

			for (int i = 0; i < n; i++) {
				range = SendMessage(hArreglo, LB_GETTEXTLEN, (WPARAM)i, 0);
				text = (LPSTR)GlobalAlloc(GPTR, range + 1);
				SendMessage(hArreglo, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
				int numer = atoi(text);
				if (maximo < numer) {
					maximo = numer;
				}
				GlobalFree(text);
			}
			float demas = maximo / 5;
			if (demas < 1) { demas = 1; }
			SetDlgItemInt(hwnd, IDC_TEXT_AYUDA, demas, false);
			int total = ceil(maximo + demas);
			int altura = 400 / total;
			int largo = 600 / n;
			int base = 0;
			for (int i = 0; i < n; i++) {
				range = SendMessage(hArreglo, LB_GETTEXTLEN, (WPARAM)i, 0);
				text = (LPSTR)GlobalAlloc(GPTR, range + 1);
				SendMessage(hArreglo, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
				int num = atoi(text);
				GlobalFree(text);
				//Dibujos de los Rectangulos;
				Rectangle(hDC, 90 + base, 410 - altura * num, 91 + base + largo, 411);
				base += largo;
			}
		}
		else {
			MessageBox(hwnd, "Error select items x2 :'v", "Warning", MB_OK);
		}
		DeleteObject(pincel);
		EndPaint(hwnd, &ps);
	}break;
	case WM_MDIACTIVATE: {
		HMENU hMenu, hFileMenu, hOptionsMenu;
		UINT EnableFlag;

		hMenu = GetMenu(g_hMainWindow);
		if (hwnd == (HWND)lParam) {
			//being activated, enable the menus
			EnableFlag = MF_ENABLED;
		}
		else {
			//being de-activated, gray the menus
			EnableFlag = MF_GRAYED;
		}

		EnableMenuItem(hMenu, 1, MF_BYPOSITION | EnableFlag);
		EnableMenuItem(hMenu, 2, MF_BYPOSITION | EnableFlag);

		hFileMenu = GetSubMenu(hMenu, 0);

		EnableMenuItem(hFileMenu, ID_ARCHIVO_CERRAR, MF_BYCOMMAND | EnableFlag);
		EnableMenuItem(hFileMenu, ID_ARCHIVO_GUARDAR, MF_BYCOMMAND | MF_GRAYED);

		hOptionsMenu = GetSubMenu(hMenu, 1);

		EnableMenuItem(hOptionsMenu, ID_OPCIONES_GRAFICA, MF_BYCOMMAND | MF_GRAYED);

		DrawMenuBar(g_hMainWindow);
	}break;
	default:
		return DefMDIChildProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// Window Procedure para el padre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg) {
	case WM_CREATE: {
		CLIENTCREATESTRUCT ccs;

		ccs.hWindowMenu = GetSubMenu(GetMenu(hwnd), 2);
		ccs.idFirstChild = ID_MDI_FIRSTCHILD;

		g_hMDIClient = CreateWindowEx(WS_EX_CLIENTEDGE, "mdiclient", NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hwnd, (HMENU)IDC_MAIN_MDI, GetModuleHandle(NULL), (LPVOID)&ccs);
		if (g_hMDIClient == NULL) {
			MessageBox(hwnd, "Puede que no se haya creado el Cliente MDI", "Error", MB_OK | MB_ICONERROR);
		}
	}break;
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_ARCHIVO_NUEVO: {
			//codigo para Archivo/Nuevo
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_DIALOG_OPEN), hwnd, DlgProc);
			CreateNewMDIChild(g_hMDIClient, 1);
		}break;
		case ID_ARCHIVO_ABRIR: {
			//Codigo para archivo/Abrir
			HWND hChild = CreateNewMDIChild(g_hMDIClient, 1);
			if (hChild)
			{
				DoFileOpen(hChild);
			}
		}break;
		case ID_ARCHIVO_CERRAR: {
			//Codigo para Archivo/Cerrar
			HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
			if (hChild) {
				SendMessage(hChild, WM_CLOSE, 0, 0);
			}
		}break;
		case ID_ARCHIVO_GUARDAR: {
			HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
			HWND hList = GetDlgItem(hChild, IDC_LISTBOX_EDIT);
			ofstream archivo;
			archivo.open("DatosGuardados.txt");
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				float total = 0;
				for (int i = 0; i < all; i++) {
					int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
					LPSTR text = (LPSTR)GlobalAlloc(GPTR, range + 1);
					SendMessage(hList, LB_GETTEXT, (WPARAM)i, (LPARAM)text);
					int num = atoi(text);
					archivo << num << endl;
					GlobalFree(text);
				}
			}
			else {
				MessageBox(hwnd, "Error select items", "Warning", MB_OK);
			}
		}break;
		case ID_OPCIONES_GRAFICA: {
			//Codigo para la ventana Grafica de los datos (Creada en el MDI)
			HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
			hListGlobal = GetDlgItem(hChild, IDC_LISTBOX_EDIT);
			CreateNewMDIChild(g_hMDIClient, 2);
		}break;
		case ID_OPCIONES_CASCADA: {
			SendMessage(g_hMDIClient, WM_MDICASCADE, 0, 0);
		}break;
		case ID_INFORMACION: {
			MessageBox(hwnd, "Paul Vitela Payan\nAbel Galvan Enriquez", "Desarrolladores", MB_OK);
		}break;
		case ID_SALIR: {
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}break;
		default: {
			if (LOWORD(wParam) >= ID_MDI_FIRSTCHILD) {
				DefFrameProc(hwnd, g_hMDIClient, WM_COMMAND, wParam, lParam);
			}
			else {
				HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
				if (hChild)
					SendMessage(hChild, WM_COMMAND, wParam, lParam);
			}
		}
		}
	}break;
	case WM_CLOSE: {
		DestroyWindow(hwnd);
	}break;
	case WM_DESTROY: {
		PostQuitMessage(0);
	}break;
	default:
		return DefFrameProc(hwnd, g_hMDIClient, msg, wParam, lParam);
	return 0;
	}
}

//Creacion de la clase ventana Child
BOOL SetUpMDIChildWindowClass(HINSTANCE hInstance){
	//Creando el hijo

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MDIChildWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szChildClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)){
		MessageBox(0, "Puede que no se haya registrado el hijo", "Oh Oh...",
			MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}else
		return TRUE;
}

//Creacion de la clase ventana Grafica
BOOL SetUpMDIGraficWindowClass(HINSTANCE hInstance) {

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WinGraficProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szGraficClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(0, "Puede que no se haya registrado la ventana para la Grafica", "Oh Oh...",
			MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}
	else
		return TRUE;
}

//Creacion de la ventana padre
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow){
	
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	//Step 1: Registrando la clase ventana
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if (!SetUpMDIChildWindowClass(hInstance))
		return 0;
	if (!SetUpMDIGraficWindowClass(hInstance))
		return 0;

	// Step 2: Creating the Window
	
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Calculadora de Estadistica",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 700, 500,
		NULL, NULL, hInstance, NULL);
		
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	g_hMainWindow = hwnd;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Step 3: The Message Loop
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		if (!TranslateMDISysAccel(g_hMDIClient, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	return Msg.wParam;
}