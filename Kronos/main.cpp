#include <iostream>
#include <memory>
#include <crtdbg.h>

#include "Kronos_Application.h"
#include "Kronos Engine/Kronos.h"

int main() {
	
 
	std::unique_ptr<Kronos_Application> app = std::make_unique<Kronos_Application>();
	
	app->run();

	return 0;
}