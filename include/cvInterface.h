#ifndef CVINTERFACE_H
#define CVINTERFACE_H

#include "gamecore.h"

#include "../include/network/Buffer.h"

namespace cvinterface {

    /* This class encapsulates the parameters of a detected marker. */
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

    typedef std::vector<cv::Point> Square;

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

        std::set<std::pair<uint32_t, uint32_t>> visited;
        std::vector<std::pair<uint32_t, uint32_t>> blobs;
        std::queue<std::pair<uint32_t, uint32_t>> flood;

        // Network stuff
        sf::TcpSocket *socket;
        Buffer send_buffer;

        bool running = true;
    public:
        void init();
        inline void stop() { running = false; }
        // This method runs the new detection algorithm
        void step(std::vector<std::vector<cv::Point>>& squares);
        void release();
        // These are both essentially final! Consider them const!
        static int RED_THRESHOLD;
        static int NON_RED_THRESHOLD;
    private:
        // DEPRECATED
        void findTowers();
        void findTowers2();
        void findSquares(const cv::Mat& frame, std::vector<Square>& squares);
        void decodeSquares(cv::Mat& frame, std::vector<Square>& squares, std::vector<Marker>& markers);

        // Network stuff (for sending the towers to the main server)
        void networkConnect();
        void networkSendTowerPositions();
    };
}

#endif //CVINTERFACE_H
