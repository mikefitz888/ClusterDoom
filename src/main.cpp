#include "../include/manager.h"
#include "../include/network/Network.h"
#include "../include/RenderManager.h"
#include "../include/cvInterface.h"
#include "../include/smartpointers.h"
using manager::Manager;
using namespace graphics;


void runCVInterface(cvinterface::CvInterface* cv){
    cv->init();
}

int main(int argc, char* argv[]){
    Manager model = Manager();
    std::cout << "O:!" << std::endl;

    // Networking
    //network::NetworkManager nm;
    smartpointers::slave_ptr<int> ptr = nullptr;

    if (ptr) {
        std::cout << "NPT IS VLAID" << std::endl;
    }
    else {
        std::cout << "NPT IS NOT VALID" << std::endl;
    }

    // Rendering
    RenderManager rm;

    //Render Manager must be initialized first to ensure GL context
#ifndef CV_ONLY
    model.initRenderManager(rm);
#endif

    //TODO: Pass an openCV component to manager
    cvinterface::CvInterface cv;
#ifdef CV_ONLY
    runCVInterface(&cv);
#else
    std::thread cv_thread = std::thread(runCVInterface, &cv);
#endif
    

#ifndef CV_ONLY
    model.init();
    //rm.setWindowTitle("Clusterdoom");


	sf::Clock clock = sf::Clock();
    bool running = true;
	int i = 0;
    while (running) {
        // Networking
        //nm.networkStep();

        // Step then Render
        running = model.step();

    }
#endif

    // Cleanup
    cv.release();
#ifndef CV_ONLY
    cv_thread.join();
    //nm.release();
    model.release();
#endif

    return 0;
}
