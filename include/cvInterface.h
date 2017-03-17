#ifndef CVINTERFACE_H
#define CVINTERFACE_H

#include "gamecore.h"

#include "../include/network/Buffer.h"

namespace cvinterface {

    struct Marker
    {
        int x, y, marker_type;
        explicit inline Marker(int x, int y, int marker_type)
        {
            this->x = x;
            this->y = y;
            this->marker_type = marker_type;
        }
    };

    class CVInterface {
    private:
        std::vector<Marker> markers;
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

        bool running = true;
    public:
        void init();
        inline void stop() { running = false; }
        void step(std::vector<std::vector<cv::Point>>& squares);
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
