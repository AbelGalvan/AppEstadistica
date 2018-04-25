#include <windows.h>
#include <CommCtrl.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";
const char g_szChildClassName[] = "myChildrenClass";

HWND g_hMDIClient = NULL;
HWND g_hMainWindow = NULL;
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
//Falta corregir
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName){
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwTextLength;

		dwTextLength = GetWindowTextLength(hEdit);
		// No need to bother if there's no text.
		if (dwTextLength > 0)
		{
			LPSTR pszText;
			DWORD dwBufferSize = dwTextLength + 1;

			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if (pszText != NULL)
			{
				if (GetWindowText(hEdit, pszText, dwBufferSize))
				{
					DWORD dwWritten;

					if (WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
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
//Falta corregir
void DoFileSave(HWND hwnd){
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_EDITBOX);
		if (SaveTextFileFromEdit(hEdit, szFileName))
		{
			SetWindowText(hwnd, szFileName);
		}
	}
}

HWND CreateNewMDIChild(HWND hMDIClient){
	MDICREATESTRUCT mcs;
	HWND hChild;
	
	mcs.szTitle = "Nuevo archivo";
	mcs.szClass = g_szChildClassName;
	mcs.hOwner = GetModuleHandle(NULL);
	mcs.x = CW_USEDEFAULT;
	mcs.y = CW_USEDEFAULT;
	mcs.cx = 410;
	mcs.cy = 350;
	mcs.style = MDIS_ALLCHILDSTYLES;
	
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
	static HINSTANCE hInstance;

	switch (msg){
	case WM_CREATE: {
		
		HFONT hfDefault;
		HWND hEdit, hListbox, hButtonAdd, hButtonDel;
		HWND hBtnProm, hTextProm, hBtnMediana, hTextMediana, hBtnModa, hTextModa, hBtnDAM, hTextDAM,
			hBtnVarianza, hTextVarianza, hBtnStandard, hTextStandard;

		hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		
		//Create ListBox Control
		hListbox = CreateWindowEx(0, "LISTBOX", "",
			WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL | WS_TABSTOP,
			10, 10, 150, 280, hwnd, (HMENU)IDC_LISTBOX_EDIT, GetModuleHandle(NULL), NULL);
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
	case WM_MDIACTIVATE: {
		HMENU hMenu, hFileMenu;
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
		EnableMenuItem(hFileMenu, ID_ARCHIVO_GUARDAR, MF_BYCOMMAND | EnableFlag);

		EnableMenuItem(hFileMenu, ID_ARCHIVO_CERRAR, MF_BYCOMMAND | EnableFlag);
		EnableMenuItem(hFileMenu, ID_ARCHIVO_CERRARTODO, MF_BYCOMMAND | EnableFlag);

		DrawMenuBar(g_hMainWindow);
	}break;
	/*case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;

		GetClientRect(hwnd, &rcClient);

		hEdit = GetDlgItem(hwnd, IDC_EDITBOX);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
	}break;*/
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_ARCHIVO_GUARDAR: {
			DoFileSave(hwnd);
		}break;
		case IDC_BUTTON_ADD: {

			HWND hEdit;
			DWORD dwTextLength;
			
			hEdit = GetDlgItem(hwnd, IDC_EDITBOX);
			dwTextLength = GetWindowTextLength(hEdit);
			// No need to bother if there's no text.
			if (dwTextLength > 0){
				LPSTR pszText;
				DWORD dwBufferSize = dwTextLength + 1;

				pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
				if (pszText != NULL){
					if (GetWindowText(hEdit, pszText, dwBufferSize)){
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
			if (count != LB_ERR){
				if (count != 0){
					// And then allocate room to store the list of selected items.
					int i;
					int *buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
					SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);

					// Now we loop through the list and remove each item that was selected.  
					for (i = count - 1; i >= 0; i--)
						SendMessage(hList, LB_DELETESTRING, (WPARAM)buf[i], 0);
					GlobalFree(buf);
				}else{
					MessageBox(hwnd, "Items no seleccionado", "Warning", MB_OK);
				}
			}else{
				MessageBox(hwnd, "Error contando items :(", "Warning", MB_OK);
			}
		}break;
		case IDC_BUTTON_PROM: {
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_EDIT);
			int all = SendMessage(hList, LB_GETCOUNT, 0, 0);
			if (all != LB_ERR) {
				SendMessage(hList, LB_SELITEMRANGEEX, 0, (LPARAM)all);
				int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
				if (count != LB_ERR) {
					if (count != 0) {
						// And then allocate room to store the list of selected items.
						int i;
						int *buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
						SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);
						// Now we loop through the list and remove each item that was selected.  
						float total=0;
						for (i = count - 1; i >= 0; i--) {
							int range = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)i, 0);
							LPSTR dato = (LPSTR)GlobalAlloc(GPTR, range+1);
							SendMessage(hList, LB_GETTEXT, (WPARAM)buf[i], (LPARAM)dato);
							int num = 0, lim = strlen(dato);
							for (int i = 0; i < lim; i++) {
								switch (dato[i]) {
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
							total = total + num;
							GlobalFree(dato);
						}
						GlobalFree(buf);
						float count2 = count;
						float prom = total / count2;
						LPSTR stri = itoa(prom, 10);
						SetDlgItemText(hwnd, IDC_TEXT_PROM, stri);
					}else {
						MessageBox(hwnd, "Items no seleccionado", "Warning", MB_OK);
					}
				}else {
					MessageBox(hwnd, "Error contando items :(", "Warning", MB_OK);
				}
				SendMessage(hList, LB_SELITEMRANGEEX, all, 0);
			}else{
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

// Window Procedure para el padre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
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
	}
		break;
	case WM_COMMAND: {
		switch (LOWORD(wParam)){
		case ID_ARCHIVO_NUEVO: {
			//codigo para Archivo/Nuevo
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_DIALOG_OPEN), hwnd, DlgProc);
			CreateNewMDIChild(g_hMDIClient);
		}break;
		case ID_ARCHIVO_ABRIR: {
			//Codigo para archivo/Abrir
			HWND hChild = CreateNewMDIChild(g_hMDIClient);
			if (hChild)
			{
				DoFileOpen(hChild);
			}
		}break;
		case ID_ARCHIVO_CERRAR: {
			//Codigo para Archivo/Cerrar
			HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
			if (hChild)
			{
				SendMessage(hChild, WM_CLOSE, 0, 0);
			}
		}break;
		case ID_OPCIONES_CASCADA:
			SendMessage(g_hMDIClient, WM_MDICASCADE, 0, 0);
			break;
		case ID_SALIR: {
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}break;
		default: {
			if (LOWORD(wParam) >= ID_MDI_FIRSTCHILD){
				DefFrameProc(hwnd, g_hMDIClient, WM_COMMAND, wParam, lParam);
			}else{
				HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
				if (hChild)
					SendMessage(hChild, WM_COMMAND, wParam, lParam);
			}
		}
		}
	}
		break;
	case WM_CLOSE: {
		DestroyWindow(hwnd);
	}break;
	case WM_DESTROY: {
		PostQuitMessage(0);
	}break;
	default:
		return DefFrameProc(hwnd, g_hMDIClient, msg, wParam, lParam);
	}
	return 0;
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
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
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