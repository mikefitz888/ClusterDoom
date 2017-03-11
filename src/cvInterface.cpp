#include "../include/cvInterface.h"

namespace cvinterface {
    int CVInterface::RED_THRESHOLD = 105;
    int CVInterface::NON_RED_THRESHOLD = 70;

    #define PI 3.14159265359
    #define ANGLE(deg) deg*PI/180

    const int thresh = 100;
    const double maxAllowedAngle = cos(ANGLE(85));
    const double lengthError = 0.15; // 15%
    const int maxCloseness = 40;

    typedef std::vector<cv::Point> Square;

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
        //cv::namedWindow("WebCam", CV_WINDOW_AUTOSIZE);

        networkConnect();

        std::vector<Square> squares;

        while (true) {
            step(squares);
        }
    }

    // finds a cosine of angle between vectors pt0pt1 and pt0pt2
    double angle(cv::Point& pt1, cv::Point& pt2, cv::Point& pt0)
    {
        double dx1 = pt1.x - pt0.x;
        double dy1 = pt1.y - pt0.y;
        double dx2 = pt2.x - pt0.x;
        double dy2 = pt2.y - pt0.y;
        return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2));
    }

    // returns sequence of squares detected on the image.
    // the sequence is stored in the specified memory storage
    void findSquares(const cv::Mat& frame, std::vector<Square>& squares)
    {
        squares.clear();

        cv::Mat pyr, timg;

        // filter noise and blur
        pyrDown(frame, pyr, cv::Size(frame.cols/2, frame.rows/2));
        pyrUp(pyr, timg, frame.size());
        std::vector<std::vector<cv::Point>> contours;

        cv::Mat gray0/*(image.size(), CV_8U)*/, gray;
        /*int ch[] = { 0, 0 };
        mixChannels(&timg, 1, &gray0, 1, ch, 1);*/
        cvtColor(timg, gray0, CV_BGR2GRAY);

        // Perform canny, setting the lower threshold to 0 (which forces edges merging)
        Canny(gray0, gray, 0, thresh);
        // Dilate canny output to remove potential holes between edge segments
        dilate(gray, gray, cv::Mat());

        findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        Square approx;
        for (auto& contour : contours)
        {
            // approximate contour with accuracy proportional
            // to the contour perimeter
            approxPolyDP(contour, approx, arcLength(contour, true)*0.02, true);

            // Squares have four edges
            // Squares should be above a certain size 
            // Squares should be closed (convex)
            if (approx.size() == 4 && isContourConvex(approx) && contourArea(approx) > 1000.0)
            {
                double l1 = std::hypot(approx[0].x - approx[1].x, approx[0].y - approx[1].y);
                double l2 = std::hypot(approx[1].x - approx[2].x, approx[1].y - approx[2].y);
                if (fabs(l1-l2)/l2 < lengthError)
                {
                    double maxCosine = fabs(angle(approx[0], approx[2], approx[1]));
                    double cosine = fabs(angle(approx[0], approx[2], approx[3]));
                    maxCosine = MAX(maxCosine, cosine);

                    // if angles are close to 90 degres
                    if (maxCosine < maxAllowedAngle)
                    {
                        int cx1 = (approx[0].x + approx[1].x + approx[2].x + approx[3].x)/4;
                        int cy1 = (approx[0].y + approx[1].y + approx[2].y + approx[3].y)/4;
                        for (Square& square : squares)
                        {
                            int cx2 = (square[0].x + square[1].x + square[2].x + square[3].x) / 4;
                            int cy2 = (square[0].y + square[1].y + square[2].y + square[3].y) / 4;
                            if (std::hypot(cx2 - cx1, cy2 - cy1) < maxCloseness) goto nopush;
                        }
                        squares.push_back(approx);
                    nopush:;
                    }
                }
            }
        }
    }

    // the function draws all the squares in the image
    void drawSquares(cv::Mat& image, const std::vector<Square>& squares)
    {
        for (size_t i = 0; i < squares.size(); i++)
        {
            const cv::Point* p = &squares[i][0];
            int n = (int) squares[i].size();
            polylines(image, &p, &n, 1, true, cv::Scalar(0, 255, 0), 3);
        }
    }


    void CVInterface::step(std::vector<Square>& squares)
    {
        bool success = camera.read(frame);
        if (!success) {
            // std::cout << "Cannot read frame from video stream!" << std::endl;
            return;
        }

        resize(frame, frame, cv::Size(frame.cols * 2, frame.rows * 2), 0, 0, cv::INTER_NEAREST);

        findSquares(frame, squares);
        std::cout << "There were " << squares.size() << " squares detected\n";

        tower_locations.clear();
        for (Square& square : squares)
        {
            int cx = (square[0].x + square[1].x + square[2].x + square[3].x) / 4;
            int cy = (square[0].y + square[1].y + square[2].y + square[3].y) / 4;
            tower_locations.push_back(gameobject::Point<int>(cx, cy));
        }

        networkSendTowerPositions();

        std::cout << "There were " << squares.size() << " squares detected\n";
        //drawSquares(frame, squares);

        //imshow("Webcam", frame);

        //findTowers2();
        //cv::imshow("WebCam", frame);
         // This is essential as if we leave it, the main thread will receive multiple packets of data containing updated lists every frame.
         // (Realistically, this only needs to run at 24 fps, as this is the maximum performance of the camera.)
        cvWaitKey(42);
    }

/*
std::vector<Square> squares;
    while (true)
    {
        auto t = std::chrono::steady_clock::now();

        if (webcam.read(frame))
        {
            // As we have spare performance, we can work on higher resolution images to get smaller detections
            resize(frame, frame, cv::Size(frame.cols * 2, frame.rows * 2), 0, 0, cv::INTER_NEAREST);

            findSquares(frame, squares);
            std::cout << "There were " << squares.size() << " squares detected\n";
            drawSquares(frame, squares);

            imshow("Webcam", frame);

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t);
            std::cout << "FPS: " << 1000 / duration.count() << "\n";
            cvWaitKey(20); //was 42
        }
    }
*/    

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
