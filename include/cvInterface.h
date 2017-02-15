#ifndef CVINTERFACE_H
#define CVINTERFACE_H

#include "gamecore.h"

#include "../include/network/Buffer.h"

#define CV_ONLY

namespace cvinterface {
    // Good markers are 5, 6, 7, 10?
    enum Codes : marker_t {INVALID = -1,
                           TOWER_BASIC = 6, 
                           TOWER_BASE = 7};

    struct Square
    {
        explicit Square(int x, int y, int theta);
        int x;
        int y;
        int theta;
    };

    class CvInterface {
        std::vector<gameobject::Point<int>> tower_locations;
        std::vector<std::pair<gameobject::Point<int>, marker_t>> marker_locations;

        // Augmented R-Table
        // In order to improve CPU time, we will make a 3d R-table as opposed to usual 2d
        // This allows us to do all the calculations only once!
        // The R-Table is generated for us during initialisation
        // indexed first on a rotation of a square then on the offset gradient
        // provides a pair of floats to add to the pixel value to get center point of square
        std::vector<gameobject::Point<float>>** table;

        // Hough space
        // We are using a 3 dimensional Generalised Hough Space
        // The three dimensions are center x, center y and rotation theta
        // The length of a size is fixed, but has a certain slack on it
        // if this proves insufficient we will move to 4d space, RIP
        // Size of this space is dHeight*dWidth*NUM_ROT_ROWS
        int*** hough_space;

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
        static uint8_t POWER_THRESHOLD;
        static int VOTE_MIN_THRESHOLD;
        static int VOTE_MAX_THRESHOLD;
        static unsigned int NUM_GRAD_ROWS;
        static unsigned int NUM_ROT_ROWS;
        static int MAX_ROT;
        static int MARKER_SIZE;
        static int EPSILON_SIZE;
        static int EPSILON_GRAD;
        static int BLACK_THRESHOLD;
        static int WHITE_THRESHOLD;
    private:
        // These are formally being deprecated in favour of new detection markers
        void findTowers();
        void findTowers2();

        void findMarkers();
        static marker_t determineMarker(uint_fast8_t bits);

        // This is for the GHT
        void generalisedHoughTransform(cv::Mat& power, cv::Mat& phase, std::vector<Square>& results);
        void createRTable();

        // Network stuff
        void networkConnect();
        void networkSendTowerPositions();
        //void networkSendMarkers();
    };
}

#endif //CVINTERFACE_H
