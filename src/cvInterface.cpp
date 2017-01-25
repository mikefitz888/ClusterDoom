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
        findTowers();
        cv::imshow("WebCam", frame);
         // This is essential as if we leave it, the main thread will receive multiple packets of data containing updated lists every frame.
         // (Realistically, this only needs to run at 24 fps, as this is the maximum performance of the camera.)
        cvWaitKey(42);
    }

    void CVInterface::release() {
        //send_buffer.release();
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
        std::cout << "[CV] Sending towers to control thread: Count: " << tower_locations.size() << std::endl;

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
