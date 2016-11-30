#include "../include/manager.h"
#include "../include/network/Network.h"
#include "../include/RenderManager.h"
#include <iostream>

using manager::Manager;
using namespace graphics;
using namespace network;

int main(int argc, char* argv[]){
	Manager model = Manager();
	std::cout << "O:!" << std::endl;

	// Networking
	NetworkManager nm;

	// Rendering
	RenderManager rm;
	rm.init();
	rm.setWindowTitle("Clusterdoom");

	bool running = true;
	while (running) {
		// Networking
		nm.networkStep();

		// Render
		running = rm.render();
	}

	// Cleanup
	nm.release();
	rm.release();

	return 0;
}