#include "../include/manager.h"
#include "../include/network/Network.h"
#include "../include/RenderManager.h"
#include <iostream>
//#include <unistd.h>

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

	//Render Manager must be initialized first to ensure GL context
	model.initRenderManager(rm);
	model.init();
	//rm.init();
	//rm.setWindowTitle("Clusterdoom");

	bool running = true;
	while (running) {
		// Networking
		nm.networkStep();

		// Step then Render
		running = model.step();
		//usleep(200);
	}

	// Cleanup
	nm.release();
	model.releaseRender();

	return 0;
}