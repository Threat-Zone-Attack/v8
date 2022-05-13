#include <windows.h>
#include "resource.h"

#include "globals.h"
#include "commonFunctions.h"
#include "createDialog.h"
#include "createWindow.h"

#include "fileDump.h"
#include "build.h"

/* Treasure Hunter version 0.1 Alpha - Builder
 * Created by Jolly Roger (jollyroger@prv.name)
 * Все комментарии идут на английском, потому что мне (опять же) было лень переключать раскладку.
 * 
 * This code is written in pure C. No C++ features were used.
 *
 * TO DO for the next version of the builder (0.2 Beta):
 * Replace all Unicode versions of functions with ANSI versions.
 * Improve the code structure:
 * - Replace all the if - else constructs that are rendered needless by return commands;
 * - Organize the includes;
 * - Give the code proper commenting so that I am able to modify and improve it after not having seen it for some time (if such a thing happens).
 * Make scan exceptions and service codes configurable.
 * Add a logo to the GUI.
 * Add a cryptor and/or packer to crypt and/or reduce the size of the build.
 */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	createWindow(hInstance, nCmdShow);
}