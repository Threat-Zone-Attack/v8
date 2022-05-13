HWND createControl(HWND hwnd, int controlID, LPCWSTR controlType, DWORD controlStyle, LPCWSTR controlText, int controlX, int controlY, int controlWidth, int controlHeight){
	HWND hControl;
	HFONT hFont;
	LOGFONT lf;

	/* Add the control */
	hControl = CreateWindowEx(WS_EX_WINDOWEDGE, controlType, controlText, controlStyle, controlX, controlY, controlWidth, controlHeight, hwnd, (HMENU)controlID, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

	/* Change its font */
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf); 
	hFont = CreateFont(-11, 0, 0, 0, 400, 0, 0, lf.lfStrikeOut, lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality, lf.lfPitchAndFamily, lf.lfFaceName);
	SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);

	return hControl;
}

int registerClass(HINSTANCE hInstance, LPCWSTR g_szClassName, WNDPROC procName){
	WNDCLASSEX wc;
	int result;

	wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
	wc.lpfnWndProc = procName;
	wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, 0);

	result = RegisterClassEx(&wc);

	return (result != 0);
}