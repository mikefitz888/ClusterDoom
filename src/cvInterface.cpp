#include "../include/cvInterface.h"

namespace cvinterface {
    int CVInterface::RED_THRESHOLD = 105;
    int CVInterface::NON_RED_THRESHOLD = 70;

    void CVInterface::init() {
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

        networkConnect();

        while (true) {
            step();
        }
    }

    void CVInterface::step()
    {
        bool success = camera.read(frame);
        if (!success) {
            // std::cout << "Cannot read frame from video stream!" << std::endl;
            return;
        }
        findTowers2();
        cv::imshow("WebCam", frame);
         // This is essential as if we leave it, the main thread will receive multiple packets of data containing updated lists every frame.
         // (Realistically, this only needs to run at 24 fps, as this is the maximum performance of the camera.)
        cvWaitKey(42);
    }

    void CVInterface::release() {
        //send_buffer.release();
    }

    void CVInterface::findTowers2() {
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

    void CVInterface::findTowers() {
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

    void CVInterface::networkConnect() {
        socket = new sf::TcpSocket();
        sf::TcpSocket::Status st = socket->connect("127.0.0.1", 31654, sf::seconds(10.0f));
        if (st == sf::TcpSocket::Status::Done) {
            std::cout << "[CV] Connected to CV interface successfully!" << std::endl;
        }
    }

    void CVInterface::networkSendTowerPositions() {
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
