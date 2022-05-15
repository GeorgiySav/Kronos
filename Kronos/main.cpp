#include <iostream>
#include <memory>

#include "Kronos_Application.h"
#include "Kronos Engine/Kronos.h"

int main() {
	
 
	static std::unique_ptr<Kronos_Application> app = std::make_unique<Kronos_Application>();

	app->run();
	
	//KRONOS::KronosEngine chess;
	

	return 0;
}