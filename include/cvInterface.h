#ifndef CVINTERFACE_H
#define CVINTERFACE_H

#include "gamecontroller.h"
#include "gameobject.h"
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "../include/network/Buffer.h"
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Time.hpp>

namespace manager {
    class Manager;
}

namespace cvinterface {
    using gamecontroller::GameController;
    using gameobject::Point;
    using gameobject::tower_ptr;
    class ICVInterface {
    private:
        GameController* game_controller;
        std::vector<Point<int>> tower_locations;
        std::vector<tower_ptr> towers; //Up date this list to ensure a correspondance between it and tower_locations
        cv::VideoCapture camera;
        cv::Mat frame;
        cv::CascadeClassifier cascade;
        double dWidth;
        double dHeight;
        IplImage* pFrame;

		// Network stuff
		sf::TcpSocket *socket;
		Buffer        *send_buffer;
    public:
        inline ICVInterface(GameController* gc) : game_controller(gc) {}
        void init();
        void step();
        void release();
        void updateTowerList(/*std::vector<Point<int>> &towers*/);
    private:
        void findTowers();
        // We'll need some others here...
		
		// Network stuff
		void networkConnect();
		void networkSendTowerPositions();
    };
}


#endif //CVINTERFACE_H
