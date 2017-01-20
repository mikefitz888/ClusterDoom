#include "../include/manager.h"
#include "../include/network/Network.h"
#include "../include/RenderManager.h"
#include "../include/cvInterface.h"
#include <iostream>
#include <thread>
#include "../include/smartpointers.h"
using manager::Manager;
using namespace graphics;


void runCVInterface(cvinterface::ICVInterface* cv){
    cv->init();
}

int main(int argc, char* argv[]){
    Manager model = Manager();
    std::cout << "O:!" << std::endl;

    // Networking
    //NetworkManager nm;
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
    model.initRenderManager(rm);
    
    //TODO: Pass an openCV component to manager
    cvinterface::ICVInterface Icv(model.getGameController());
    std::thread cv_thread = std::thread(runCVInterface, &Icv);

    model.init();
    //rm.setWindowTitle("Clusterdoom");

    

    bool running = true;
    while (running) {
        // Networking
        //nm.networkStep();

        // Step then Render
        running = model.step();
    }

    // Cleanup
    Icv.release();
    cv_thread.join();
    //nm.release();
    model.release();

    return 0;
}