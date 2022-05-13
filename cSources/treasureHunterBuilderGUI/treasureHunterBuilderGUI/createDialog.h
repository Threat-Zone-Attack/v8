void populateDialog(HWND hwnd){
	HWND hButton, hLabel, hGroupbox;

	/* Create group box */
	hGroupbox = createControl(hwnd, IDC_DLGGROUPBOX, L"Button", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, L"About this program", 10, 10, 410, 90);
	
	/* Create label */
	hLabel = createControl(hwnd, IDC_DLGTEXT, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"This is a builder for the Treasure Hunter RAMScraper v0.8 Alpha\r\n\r\n© 2014 Jolly Roger (jollyroger@prv.name)", 20, 30, 250, 60);

	/* Create button */
	hButton = createControl(hwnd, IDC_CLOSEDLG, L"Button",  BS_TEXT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, L"Close", 300, 30, 100, 30);
}

LRESULT CALLBACK aboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
    switch(Message){
        case WM_CREATE:
			populateDialog(hwnd);
			return DefWindowProc(hwnd, Message, wParam, lParam);

        case WM_COMMAND:
            switch(LOWORD(wParam)){
                case IDC_CLOSEDLG:
					EnableWindow(hwndParent, TRUE);
                    DestroyWindow(hwnd);
					UnregisterClass(g_szDialogClassName, ghInstance);
                break;
            }
        break;

		case WM_CLOSE:
			EnableWindow(hwndParent, TRUE);
			DestroyWindow(hwnd);
			UnregisterClass(g_szDialogClassName, ghInstance);
        break;

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return DefWindowProc(hwnd, Message, wParam, lParam);
}

int createDialogBox(HWND hwnd, HINSTANCE hInstance){
	if (registerClass(hInstance, g_szDialogClassName, (WNDPROC)aboutDlgProc) == 0){
        MessageBoxA(NULL, "Dialog Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	EnableWindow(hwnd, FALSE);
	CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, g_szDialogClassName, L"About", WS_VISIBLE | WS_CAPTION | WS_POPUP | WS_SYSMENU, 100, 100, 450, 150, hwnd, NULL, hInstance, NULL);

	hwndParent = hwnd;

	return 1;
}