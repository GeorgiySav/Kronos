#include <iostream>
#include <memory>
#include <crtdbg.h>
#define NOMINMAX
#include <Windows.h>

#include "Kronos_Application.h"
#include "Kronos Engine/Kronos.h"

// entry point of the application
int main() {

	// hides the console window
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
 
	std::unique_ptr<Kronos_Application> app = std::make_unique<Kronos_Application>();	
	app->run();

	return 0;
}