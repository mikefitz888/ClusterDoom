#include "../include/manager.h"
#include "../include/network/Network.h"
#include "../include/RenderManager.h"
#include "../include/cvInterface.h"
#include <iostream>
#include <thread>

using manager::Manager;
using namespace graphics;
using namespace network;

void runCVInterface(cvinterface::ICVInterface* cv){
	cv->init();
}

int main(int argc, char* argv[]){
	Manager model = Manager();
	std::cout << "O:!" << std::endl;

	// Networking
	NetworkManager nm;

	// Rendering
	RenderManager rm;

	//Render Manager must be initialized first to ensure GL context
	model.initRenderManager(rm);
	
	//TODO: Pass an openCV component to manager
	//cvinterface::ICVInterface Icv(model.getGameController());
	//std::thread cv_thread = std::thread(runCVInterface, &Icv);

	model.init();
	//rm.setWindowTitle("Clusterdoom");

	bool running = true;
	while (running) {
		// Networking
		nm.networkStep();

		// Step then Render
		running = model.step();
	}

	// Cleanup
	//Icv.release();
	//cv_thread.join();
	nm.release();
	model.releaseRender();

	return 0;
}