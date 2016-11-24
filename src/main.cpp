#include "../include/manager.h"
#include "../include/network/Network.h"
#include "../include/RenderManager.h"
#include <iostream>


using manager::Manager;
using namespace graphics;

int main(int argc, char* argv[]){
	Manager model = Manager();
	std::cout << "O:!" << std::endl;

	// Networking
	NetworkManager nm;

	// Rendering
	RenderManager::init();
	RenderManager::setWindowTitle("Clusterdoom");

	bool running = true;
	while (running) {
		// Networking
		nm.networkStep();

		// Render
		running = RenderManager::render();
	}

	// Cleanup
	nm.release();
	RenderManager::release();

	return 0;
}