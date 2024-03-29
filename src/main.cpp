#include "../include/manager.h"
#include "../include/network/Network.h"
#include "../include/RenderManager.h"
#include "../include/cvInterface.h"
#include "../include/smartpointers.h"
using manager::Manager;
using namespace graphics;


void runCVInterface(cvinterface::CVInterface* cv){
    cv->init();
}

int main(int argc, char* argv[]){
    Manager model = Manager();
    std::cout << "O:!" << std::endl;

    // Networking
    //NetworkManager nm;
    smartpointers::slave_ptr<int> ptr = nullptr;

    if (ptr) {
        std::cout << "NPT IS VALID" << std::endl;
    }
    else {
        std::cout << "NPT IS NOT VALID" << std::endl;
    }

    // Rendering
    RenderManager rm;

    //Render Manager must be initialized first to ensure GL context
    model.initRenderManager(rm);
    



    model.init();
    //rm.setWindowTitle("Clusterdoom");

    //TODO: Pass an openCV component to manager
    //cvinterface::CVInterface cv;
    //std::thread cv_thread = std::thread(runCVInterface, &cv);

    sf::Clock clock = sf::Clock();
    bool running = true;
    int i = 0;
    while (running) {
        // Networking
        //nm.networkStep();

        // Step then Render
        running = model.step();

    }

    //Tell CV thread to stop what it's doing, this gives it a chance to free resources etc.
    //cv.stop();

    // Cleanup
    //cv.release();
    //cv_thread.join();
    //nm.release();
    model.release();

    return 0;
}
