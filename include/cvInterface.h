#ifndef CVINTERFACE_H
#define CVINTERFACE_H

#include "gamecore.h"

#include "gameobject.h"

#include "../include/network/Buffer.h"

namespace cvinterface {
    class CVInterface {
    private:
        std::vector<gameobject::Point<int>> tower_locations;
        cv::VideoCapture camera;
        cv::Mat frame;
        cv::CascadeClassifier cascade;
        double dWidth;
        double dHeight;
        IplImage* pFrame;

		// Network stuff
		sf::TcpSocket *socket;
		Buffer send_buffer;
    public:
        void init();
        void step();
        void release();
        // These are both essentially final! Consider them const!
        static int RED_THRESHOLD;
        static int NON_RED_THRESHOLD;
    private:
        void findTowers();
		void findTowers2();

		// Network stuff
		void networkConnect();
		void networkSendTowerPositions();
    };
}

#endif //CVINTERFACE_H
