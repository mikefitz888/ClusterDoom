#include "../include/cvInterface.h"

// We aren't robust enough for error correcting right now!
//#define ERROR_CORRECTING
#include<chrono>

namespace cvinterface {
    int CvInterface::RED_THRESHOLD = 105;
    int CvInterface::NON_RED_THRESHOLD = 70;
    uint8_t CvInterface::POWER_THRESHOLD = 240;
    int CvInterface::VOTE_MIN_THRESHOLD = 6000;//275;
    int CvInterface::VOTE_MAX_THRESHOLD = 10000;
    unsigned int CvInterface::NUM_GRAD_ROWS = 4; // <=180
    unsigned int CvInterface::NUM_ROT_ROWS = 30; // <= MAX_ROT
    int CvInterface::MAX_ROT = 90; // <= 90
    int CvInterface::MARKER_SIZE = 85; // length of side
    int CvInterface::EPSILON_SIZE = 5; // ± num pixels allowed for size of marker
    int CvInterface::EPSILON_GRAD = 0; // ± How much we can vary from gradient values of 0, 45, 90, 135, 180, 225, 270, 315
    int CvInterface::BLACK_THRESHOLD = 65;
    int CvInterface::WHITE_THRESHOLD = 65;

    void CvInterface::init() {
        if (!cascade.load("src/Resources/OpenCV/cascade.xml")) {
            std::cout << "Cannot load cascade!" << std::endl;
            // Exit? This is pretty fatal!
        }

        camera = cv::VideoCapture(0);
        if(!camera.isOpened()) {
            std::cout << "Cannot open camera!" << std::endl;
            // Exit? This is pretty fatal!
        }

        dWidth = camera.get(CV_CAP_PROP_FRAME_WIDTH);
        dHeight = camera.get(CV_CAP_PROP_FRAME_HEIGHT);
        std::cout << dWidth << std::endl;
        std::cout << dHeight << std::endl;
        cv::namedWindow("WebCam", CV_WINDOW_AUTOSIZE);

        hough_space = new int**[(size_t) dWidth];
        for (size_t i = 0; i < (size_t) dWidth; ++i)
        {
            hough_space[i] = new int*[(size_t) dHeight];
            for (size_t j = 0; j < (size_t) dHeight; ++j)
            {
                hough_space[i][j] = new int[NUM_ROT_ROWS];
            }
        }

        createRTable();

#ifndef CV_ONLY
        networkConnect();
#endif

        while (true) {
            auto t = std::chrono::steady_clock::now();
            step();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t);
            std::cout << "FPS: " << 1000 / duration.count() << "\n";
        }
    }

    void CvInterface::step()
    {
        bool success = camera.read(frame);
        if (!success) {
            // std::cout << "Cannot read frame from video stream!" << std::endl;
            return;
        }
        //findTowers2();
        //findTowers();
        findMarkers();
        cv::imshow("WebCam", frame);
         // This is essential as if we leave it, the main thread will receive multiple packets of data containing updated lists every frame.
         // (Realistically, this only needs to run at 24 fps, as this is the maximum performance of the camera.)
        cvWaitKey(42);
    }

    void CvInterface::release() {
        for (size_t i = 0; i < dWidth; ++i)
        {
            for (size_t j = 0; j < dHeight; ++j)
            {
                delete[] hough_space[i][j];
            }
            delete[] hough_space[i];
        }
        delete[] hough_space;

        for (size_t theta = 0; theta < NUM_ROT_ROWS; ++theta)
        {
            delete[] table[theta];
        }
        delete[] table;
        //send_buffer.release();
    }

    void CvInterface::findTowers2() {
        cv::Mat frame2 = frame.clone();

        cv::medianBlur(frame2, frame2, 3);
        // Convert input image to HSV
        cv::Mat hsv_image;
        cv::cvtColor(frame2, hsv_image, cv::COLOR_BGR2HSV);

        // Threshold the HSV image, keep only the red pixels
        cv::Mat lower_red_hue_range;
        cv::Mat upper_red_hue_range;
        cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
        cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

        cv::Mat red_hue_image;
        cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
        
        cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);


        std::vector<cv::Vec3f> circles;
        // void HoughCircles(InputArray image, OutputArray circles, int method, double dp, double minDist, double param1=100, double param2=100, int minRadius=0, int maxRadius=0 )
        cv::HoughCircles(red_hue_image, circles, CV_HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 20, 200);

        tower_locations.clear();
        // Next step is to commit the findings to the tower_locations list...
        for (size_t i = 0; i < circles.size(); i++) {
            cv::Point center(std::lround(circles[i][0]), std::lround(circles[i][1]));
            int radius = std::lround(circles[i][2]);
            cv::circle(frame, center, radius, cv::Scalar(0, 255, 0), 5);
            tower_locations.push_back(gameobject::Point<int>((int) circles[i][0], (int) circles[i][1]));

            //cv::rectangle(frame, cv::Point(objects[i].x, objects[i].y), cv::Point(objects[i].x + objects[i].width, objects[i].y + objects[i].height), cv::Scalar(0, 255, 0), 2);
            //tower_locations.push_back(gameobject::Point<int>(objects[i].x + objects[i].width / 2, objects[i].y + objects[i].height / 2));
        }

        networkSendTowerPositions();
    }

    void CvInterface::findTowers() {
        cv::Mat frame2 = frame.clone();
        cv::Mat frame_gray;
        std::vector<cv::Rect> objects;

        for (int i = 0; i < frame2.rows; i++) {
            for (int j = 0; j < frame2.cols; j++) {
                auto& point = frame2.at<cv::Vec3b>(i, j);

                if (!(point[2] > RED_THRESHOLD && point[1] < NON_RED_THRESHOLD && point[0] < NON_RED_THRESHOLD)) {
                    point[0] = 255;
                    point[1] = 255;
                    point[2] = 255;
                }
            }
        }
        //imshow("thresholded", frame2);
        cv::cvtColor(frame2, frame_gray, CV_BGR2GRAY);
        cv::equalizeHist(frame_gray, frame_gray);

        cascade.detectMultiScale(frame_gray, objects, 1.1, 1, CV_HAAR_SCALE_IMAGE, cv::Size(50, 50), cv::Size(500, 500));
        //std::cout << "Objects detected: " << objects.size() << std::endl;

        tower_locations.clear();
        // Next step is to commit the findings to the tower_locations list...
        for (size_t i = 0; i < objects.size(); i++)
        {
            cv::rectangle(frame, cv::Point(objects[i].x, objects[i].y), cv::Point(objects[i].x + objects[i].width, objects[i].y + objects[i].height), cv::Scalar(0, 255, 0), 2);
            tower_locations.push_back(gameobject::Point<int>(objects[i].x + objects[i].width/2, objects[i].y + objects[i].height/2));
        }
        
        networkSendTowerPositions();
    }

    void CvInterface::findMarkers()
    {
        // our frame data is in `frame`
        cv::Mat frame_gray(frame.clone());
        

        /************************************/
        /******* STAGE 0a - THRESHOLD *******/
        /************************************/

        for (int i = 0; i < frame_gray.rows; i++)
        {
            cv::Vec3b* row = frame_gray.ptr<cv::Vec3b>(i);
            for (int j = 0; j < frame_gray.cols; j++)
            {
                auto& px = row[j];
                if (px[0] > WHITE_THRESHOLD && px[1] > WHITE_THRESHOLD && px[2] > WHITE_THRESHOLD)
                {
                    px[0] = 255;
                    px[1] = 255;
                    px[2] = 255;
                }
                //else if (row[j] < BLACK_THRESHOLD) row[j] = 0;
                else 
                {
                    px[0] = 0;
                    px[1] = 0;
                    px[2] = 0;
                }
            }
        }

        cv::cvtColor(frame_gray, frame_gray, CV_BGR2GRAY);

        /************************************/
        /********** STAGE 0b - BLUR *********/
        /************************************/
        cv::GaussianBlur(frame_gray, frame_gray, cv::Size(5, 5), 0);

        imshow("grey frame", frame_gray);

        /************************************/
        /********* STAGE 0c - SOBEL *********/
        /************************************/
        // MIGRATE THIS TO NEW FUNCTION
        cv::Mat grad_x, grad_y;
        cv::Mat power;//(frame.rows, frame.cols, CV_8U);
        cv::Sobel(frame_gray, grad_x, CV_16S, 1, 0);
        cv::Sobel(frame_gray, grad_y, CV_16S, 0, 1);

        cv::Mat direction(frame.size(), CV_16U);
        for (int i = 0; i < frame.rows; i++)
        {
            const int16_t* row_x = grad_x.ptr<int16_t>(i);
            const int16_t* row_y = grad_y.ptr<int16_t>(i);
            for (int j = 0; j < frame.cols; j++)
            {
                int angle = (int) (std::atan2((double) row_y[j], (double) row_x[j]) * 180.0 / CV_PI);
                direction.at<uint16_t>(i, j) = (uint16_t) (angle < 0 ? 360 + 90 + angle : angle + 90) % 360;
                //power.at<uint8_t>(i, j) = (uint8_t) (std::abs(row_y[j]) ^ std::abs(row_x[j]));
            }
        }

        convertScaleAbs(grad_x, grad_x);
        convertScaleAbs(grad_y, grad_y);
        addWeighted(grad_x, 1, grad_y, 1 , -10, power);

        imshow("power", power);

        /************************************/
        /**** STAGE 1 - Marker Detection ****/
        /************************************/

        std::vector<Square> results;
        generalisedHoughTransform(power, direction, results);
        std::cout << "Number of squares found: " << results.size() << "\n";

        for (size_t i = 0; i < results.size(); i++) {
            cv::rectangle(frame, cv::Point(results[i].x - MARKER_SIZE/2, results[i].y - MARKER_SIZE / 2), cv::Point(results[i].x + MARKER_SIZE / 2, results[i].y + MARKER_SIZE / 2), cv::Scalar(0, 0, 255), 2);
        }

        /************************************/
        /*** STAGE 2 - Marker Orientation ***/
        /************************************/

        /************************************/
        /***** STAGE 3 - Marker Reading *****/
        /************************************/

        // This is code 9 with no errors
        uint_fast8_t payload = 73;

        /************************************/
        /***** STAGE 4 - Marker Decoding ****/
        /************************************/

        marker_t code = determineMarker(payload);

        printf("marker #%d found\n", code);
    }

    marker_t CvInterface::determineMarker(uint_fast8_t bits)
    {
        typedef uint_fast8_t bit;
        bit a = (bits & 0b1000000) >> 6;
        bit b = (bits & 0b0100000) >> 5;
        bit c = (bits & 0b0010000) >> 4;
        bit d = (bits & 0b0001000) >> 3;
        bit x = (bits & 0b0000100) >> 2;
        bit y = (bits & 0b0000010) >> 1;
        bit z = (bits & 0b0000001);

        bool ex = x != (a ^ b ^ d);
        bool ey = y != (a ^ c ^ d);
        bool ez = z != (b ^ c ^ d);

#ifdef ERROR_CORRECTING
        if (ex && ey && ez) d = !d;
        else if (ex && ey)  a = !a;
        else if (ex && ez)  b = !b;
        else if (ey && ez)  c = !c;
#else
        // We could do a bit better than this, if only one is wrong then we can assume correct?
        if (ex || ey || ez) return INVALID;
#endif
        // either an error bit is wrong or everythings correct
        return a << 3 | b << 2 | c << 1 | d;
    }

    Square::Square(int x, int y, int theta) : x(x), y(y), theta(theta) {}

    void CvInterface::generalisedHoughTransform(cv::Mat& power, cv::Mat& phase, std::vector<Square>& results)
    {
        // Reset the Hough Space
        for (size_t i = 0; i < (size_t) dWidth; ++i)
        {
            for (size_t j = 0; j < (size_t) dHeight; ++j)
            {
                for (size_t k = 0; k < NUM_ROT_ROWS; ++k)
                {
                    hough_space[i][j][k] = 0;
                }
            }
        }

        for (int y = 0; y < power.rows; y++)
        {
            for (int x = 0; x < power.cols; x++)
            {
                // only sample above threshold
                if (power.at<uint8_t>(y, x) > POWER_THRESHOLD)
                {
                    int theta = phase.at<uint16_t>(y, x) % 90;
                    if ((theta % (MAX_ROT / NUM_ROT_ROWS)) != 0) continue;
                    for (size_t phi = 0; phi < NUM_GRAD_ROWS; ++phi)
                    {
                        for (auto T : table[theta/(MAX_ROT / NUM_ROT_ROWS)][phi])
                        {
                            int cx = x + (int) T.x;
                            int cy = y + (int) T.y;
                            cx = (cx + 1) & ~1;
                            cy = (cy + 1) & ~1;
                            if (cx < 0 || cx >= power.cols || cy < 0 || cy >= power.rows) continue;
                            ++hough_space[cx][cy][theta / (MAX_ROT / NUM_ROT_ROWS)];
                        }
                    }
                }
            }
        }

        int max = 0;
        for (size_t x = 0; x < (size_t) dWidth; ++x)
        {
            for (size_t y = 0; y < (size_t) dHeight; ++y)
            {
                for (size_t theta = 0; theta < NUM_ROT_ROWS; ++theta)
                {
                    if (hough_space[x][y][theta] >= VOTE_MIN_THRESHOLD/* && hough_space[x][y][theta] < VOTE_MAX_THRESHOLD*/)
                    {
                        //std::cout << hough_space[x][y][theta] << "\n";
                        results.push_back(Square(x, y, theta));
                    }
                    if (hough_space[x][y][theta] < VOTE_MAX_THRESHOLD && hough_space[x][y][theta] > max)
                    {
                        max = hough_space[x][y][theta];
                    }
                }
            }
        }
        std::cout << "max: " << max << "\n";
    }

    void CvInterface::createRTable()
    {
        typedef gameobject::Point<float> point_t;
        /* int CVInterface::NUM_GRAD_ROWS = 4; // <=180
         * int CVInterface::NUM_ROT_ROWS = 90; // <= MAX_ROT
         * int CVInterface::MAX_ROT = 90; // <= 90
         * int CVInterface::MARKER_SIZE = 80; // length of side
         * int CVInterface::EPSILON_SIZE = 10; // ± num pixels allowed for size of marker
         * int CVInterface::EPSILON_GRAD = 0;
         */
         // center of the square at (MARKER_SIZE/2, MARKER_SIZE/2)
        int xc = MARKER_SIZE / 2;
        int yc = xc;

        table = new std::vector<point_t>*[NUM_ROT_ROWS];
        int gran = MAX_ROT / NUM_ROT_ROWS;
        for (int theta = 0; theta < MAX_ROT; theta += gran)
        {
            table[theta/gran] = new std::vector<point_t>[NUM_GRAD_ROWS];
            for (int y = -EPSILON_SIZE; y < MARKER_SIZE + EPSILON_SIZE; ++y)
            {
                if (y <= EPSILON_SIZE || y >= MARKER_SIZE - 1 - EPSILON_SIZE)
                {
                    int phi = y <= EPSILON_SIZE ? 2 : 1;
                    for (int x = -EPSILON_SIZE; x < MARKER_SIZE + EPSILON_SIZE; ++x)
                    {
                        float dx = (float)(x - xc);
                        float dy = (float)(y - yc);
                        double r = std::hypot(dx, dy);
                        double beta = std::atan2(dy, dx);
                        float i = (float)(r*std::cos(beta - theta * CV_PI / 180));
                        float j = (float)(r*std::sin(beta - theta * CV_PI / 180));
                        table[theta/gran][phi].push_back(point_t(i, j));
                    }
                }
                else
                {
                    for (int x = -EPSILON_SIZE; x < MARKER_SIZE + EPSILON_SIZE - 1; ++x)
                    {
                        if (x > EPSILON_SIZE && x < MARKER_SIZE - 1 - EPSILON_SIZE) continue;
                        int phi = x <= EPSILON_SIZE ? 3 : 0;
                        float dx = (float)(x - xc);
                        float dy = (float)(y - yc);
                        double r = std::hypot(dx, dy);
                        double beta = std::atan2(dy, dx);
                        float i = (float)(r*std::cos(beta - theta * CV_PI / 180));
                        float j = (float)(r*std::sin(beta - theta * CV_PI / 180));
                        table[theta/gran][phi].push_back(point_t(i, j));
                    }
                }
            }
            //std::cout << "for theta == " << theta << " there are " << table[theta][0].size() + table[theta][1].size() + table[theta][2].size() + table[theta][3].size() << " elements\n";
        }
        std::cout << "table initialised\n";
    }

    void CvInterface::networkConnect() {
        socket = new sf::TcpSocket();
        sf::TcpSocket::Status st = socket->connect("127.0.0.1", 31654, sf::seconds(10.0f));
        if (st == sf::TcpSocket::Status::Done) {
            std::cout << "[CV] Connected to CV interface successfully!" << std::endl;
        }
    }

    void CvInterface::networkSendTowerPositions() {
        //std::cout << "[CV] Sending towers to control thread: Count: " << tower_locations.size() << std::endl;

        // Write data to buffer
        send_buffer.seek(0);
        send_buffer << (int)tower_locations.size();
        for (auto location : tower_locations) {
            send_buffer << location;
        }

        // Send
        socket->send(send_buffer.getPtr(), send_buffer.tell());
    }
}
