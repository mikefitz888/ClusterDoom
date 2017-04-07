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

#define CALIB_TOPLEFT 0
#define CALIB_TOPRIGHT 1
#define CALIB_BOTTOMRIGHT 2
#define CALIB_BOTTOMLEFT 3

/*struct CalibPoint{
    int x, y;
    CalibPoint(){
        x = 0;
        y = 0;
    }
    CalibPoint(int x, int y){
        this->x = x;
        this->y = y;
    }
};*/

cv::Point calibration_points[4];
int current_calibration_point_id = 0;
    /*struct Marker
    {
        int x, y, marker_type;
        explicit Marker(int x, int y, int marker_type)
        {
            this->x = x;
            this->y = y;
            this->marker_type = marker_type;
        }
    };*/
    void mouseHandle(int event, int x, int y, int flags, void* userdata){
        if( event== cv::EVENT_LBUTTONDOWN){

            // Reset calibration point
            if( flags ==  cv::EVENT_FLAG_CTRLKEY){
                current_calibration_point_id = 0;
            }
            std::cout << "Point: " << current_calibration_point_id << " CALIBRATED AT: " << x << " " << y << std::endl;

            calibration_points[current_calibration_point_id % 4] = cv::Point(x, y);
            current_calibration_point_id ++;
            /*if( current_calibration_point_id >= 4 ){
                current_calibration_point_id = 0;
            }*/
        }
    }

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

        // Detect mouse press
        cv::setMouseCallback("WebCam", mouseHandle, NULL);

        
        networkConnect();

        std::vector<Square> squares;

        while (running) { //Use this so main thread can stop it safely, no while(true) please
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

    double angle(cv::Point& pt1, cv::Point& pt2)
    {
        double dx = std::abs(pt2.x - pt1.x);
        double dy = std::abs(pt2.y - pt1.y);
        return std::abs(std::atan2(dy, dx));
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

    int averageValue(cv::Mat& integral, int x1, int x2, int y1, int y2, int size)
    {
        return (integral.at<int>(x2, y2) 
              - integral.at<int>(x1, y2) 
              - integral.at<int>(x2, y1)
              + integral.at<int>(x1, y1)) / (size*size);
    }
    void decodeSquares(cv::Mat& frame, std::vector<Square>& squares, std::vector<Marker>& markers)
    {
        markers.clear();
        // We know the four points of a square
        // We need to work out the rotation, rotate the image
        // then divide into a grid
        // threshold each segment and perform decoding
        // Outer 8 squares are summed, +8 if centre is black
        for (Square& square : squares)
        {
            // Find center of marker
            int cx = (square[0].x + square[1].x + square[2].x + square[3].x) / 4;
            int cy = (square[0].y + square[1].y + square[2].y + square[3].y) / 4;

            // Find size of marker
            double l1 = std::hypot(square[0].x - square[1].x, square[0].y - square[1].y);
            double l2 = std::hypot(square[1].x - square[2].x, square[1].y - square[2].y);
            double l = (l1 + l2) / 2;

            // Determine bounding box
            int minx = 2147483647;
            int maxx = -1;
            int miny = 2147483647;
            int maxy = -1;
            for (auto& point : square)
            {
                minx = MIN(minx, point.x);
                maxx = MAX(maxx, point.x);
                miny = MIN(miny, point.y);
                maxy = MAX(maxy, point.y);
            }
            cv::Mat bounding_box = frame(cv::Rect(minx, miny, maxx - minx, maxy - miny));

            // Find the rotation matrix and apply it
            cv::Point center(bounding_box.cols / 2, bounding_box.rows / 2);
            double rot = angle(square[0], square[1]);
            cv::Mat rot_mat = cv::getRotationMatrix2D(center, 180*rot/PI, 1.0);
            cv::warpAffine(bounding_box, bounding_box, rot_mat, bounding_box.size());

            // Normalise the points to the rotated bounding box
            int cxb = cx - minx;
            int cyb = cy - miny;
            for (auto& point : square)
            {
                int x = point.x - (minx + cxb);
                int y = point.y - (miny + cyb);
                point.x = (int) (x*std::cos(-rot) - y*std::sin(-rot) + cxb);
                point.y = (int) (x*std::sin(-rot) + y*std::cos(-rot) + cyb);
            }

            // compute the new minimum point
            minx = 2147483647;
            miny = 2147483647;
            for (auto& point : square)
            {
                minx = MIN(minx, point.x);
                miny = MIN(miny, point.y);
            }

            imshow("Marker", bounding_box);

            // Determine characteristics of marker
            int border_offset = (int) (l * 0.125);
            int blob_size = (int) (l * 0.25);

            // Decode
//#define STATIC_THRESHOLD
#ifdef STATIC_THRESHOLD
            const int THRESHOLD = 128; // This could be calibrated based on centre?
#endif
            cv::Mat bounding_box_gray, sum;
            cv::cvtColor(bounding_box, bounding_box_gray, CV_BGR2GRAY);
#ifndef STATIC_THRESHOLD
            int THRESHOLD = 0;
            for (int x = minx + border_offset + blob_size; x < minx + border_offset + blob_size * 2; ++x)
            {
                for (int y = miny + border_offset + blob_size; y < miny + border_offset + blob_size * 2; ++y)
                {
                    THRESHOLD += bounding_box_gray.at<uint8_t>(x, y);
                }
            }
            THRESHOLD /= (blob_size * blob_size);
            //std::cout << "Threshold set at " << THRESHOLD << "\n";
#endif
            cv::threshold(bounding_box_gray, bounding_box_gray, THRESHOLD, 255, CV_THRESH_BINARY);
            cv::integral(bounding_box_gray, sum);

            int blob0 = averageValue(sum, minx + border_offset, 
                                          minx + border_offset + blob_size, 
                                          miny + border_offset, 
                                          miny + border_offset + blob_size, 
                                     blob_size) < 128;
            int blob1 = averageValue(sum, minx + border_offset + blob_size,
                                          minx + border_offset + blob_size * 2,
                                          miny + border_offset,
                                          miny + border_offset + blob_size,
                                     blob_size) < 128;
            int blob2 = averageValue(sum, minx + border_offset + blob_size * 2,
                                          minx + border_offset + blob_size * 3,
                                          miny + border_offset,
                                          miny + border_offset + blob_size,
                                     blob_size) < 128;
            int blob3 = averageValue(sum, minx + border_offset,
                                          minx + border_offset + blob_size,
                                          miny + border_offset + blob_size,
                                          miny + border_offset + blob_size * 2,
                                     blob_size) < 128;
            // center blob
#ifdef STATIC_THRESHOLD
            int blob4 = averageValue(sum, minx + border_offset + blob_size,
                                          minx + border_offset + blob_size * 2,
                                          miny + border_offset + blob_size,
                                          miny + border_offset + blob_size * 2,
                                     blob_size);
#endif
            int blob5 = averageValue(sum, minx + border_offset + blob_size * 2,
                                          minx + border_offset + blob_size * 3,
                                          miny + border_offset + blob_size,
                                          miny + border_offset + blob_size * 2,
                                     blob_size) < 128;
            int blob6 = averageValue(sum, minx + border_offset,
                                          minx + border_offset + blob_size,
                                          miny + border_offset + blob_size * 2,
                                          miny + border_offset + blob_size * 3,
                                     blob_size) < 128;
            int blob7 = averageValue(sum, minx + border_offset + blob_size,
                                          minx + border_offset + blob_size * 2,
                                          miny + border_offset + blob_size * 2,
                                          miny + border_offset + blob_size * 3,
                                     blob_size) < 128;
            int blob8 = averageValue(sum, minx + border_offset + blob_size * 2,
                                          minx + border_offset + blob_size * 3,
                                          miny + border_offset + blob_size * 2,
                                          miny + border_offset + blob_size * 3,
                                     blob_size) < 128;
            int marker_type = blob0 + /*blob1 +*/ blob2 + /*blob3 +*/ /*blob5 +*/ blob6 + /*blob7 +*/ blob8;
           /* std::cout << "Found marker (" 
                      << blob0// << ", " 
                      << blob1// << ", " 
                      << blob2// << ", " 
                      << blob3// << ", " 
#ifdef STATIC_THRESHOLD
                      << blob4// << ", "
#endif
                      << blob5// << ", " 
                      << blob6// << ", " 
                      << blob7// << ", " 
                      << blob8 << ") = " << marker_type << "\n";*/
            markers.push_back(Marker(cx, cy, marker_type));
        }
    }


    void CVInterface::step(std::vector<Square>& squares)
    {
        visited.clear();
        blobs.clear();

        bool success = camera.read(frame);
        if (!success) {
            // std::cout << "Cannot read frame from video stream!" << std::endl;
            return;
        }
        
        //resize(frame, frame, cv::Size(frame.cols * 2, frame.rows * 2), 0, 0, cv::INTER_NEAREST);
        if(current_calibration_point_id >= 4){
            //findSquares(frame, squares);
            // std::cout << "There were " << squares.size() << " squares detected\n";
            //decodeSquares(frame, squares, markers);
            cvtColor(frame, frame, CV_BGR2GRAY);
            cv::threshold(frame, frame, 5, 255, 0);
            for (size_t i = 0; i < (unsigned) frame.rows; i++) {
                for (size_t j = 0; j < (unsigned) frame.cols; j++) {
                    uint32_t x = 0;
                    uint32_t y = 0;
                    uint32_t num_points = 0;
                    flood.push(std::make_pair(i, j));
                    while (!flood.empty()) {
                        auto point = flood.front();
                        flood.pop();
                        if (frame.at<uint8_t>(point.first, point.second) > 0) {
                            if (visited.find(point) == visited.end()) {
                                visited.emplace(point);
                                x += point.first;
                                y += point.second;
                                num_points++;
                                bool right = point.first + 1 < (unsigned) frame.rows;
                                bool left = point.first > 0;
                                bool up = point.second + 1 < (unsigned) frame.cols;
                                bool down = point.second > 0;
                                if (right) {
                                    flood.push(std::make_pair(point.first + 1, point.second));
                                    if (up) flood.push(std::make_pair(point.first + 1, point.second + 1));
                                    if (down) flood.push(std::make_pair(point.first + 1, point.second - 1));
                                }
                                if (left) {
                                    flood.push(std::make_pair(point.first - 1, point.second));
                                    if (up) flood.push(std::make_pair(point.first - 1, point.second + 1));
                                    if (down) flood.push(std::make_pair(point.first - 1, point.second - 1));
                                }
                                if (up) flood.push(std::make_pair(point.first, point.second + 1));
                                if (down) flood.push(std::make_pair(point.first, point.second - 1));
                            }
                        }
                    }

                    if (num_points > 0) blobs.push_back(std::make_pair(y / num_points, x / num_points));
                }
            }

            const int MIN_DIST = 30;
            markers.clear();
            for (auto& blob : blobs) {
                bool merged = false;
                for (auto& marker : markers) {
                    if (std::hypot(marker.x - (int)blob.first, marker.y - (int)blob.second) < MIN_DIST) {
                        marker.x = (marker.x * marker.marker_type + blob.first) / (marker.marker_type + 1);
                        marker.y = (marker.y * marker.marker_type + blob.second) / (marker.marker_type + 1);
                        marker.marker_type++;
                        merged = true;
                        break;
                    }
                }
                if (!merged) {
                    markers.push_back(Marker(blob.first, blob.second, 1));
                }
                //std::cout << "(" << blob.first << ", " << blob.second << ")" << std::endl;
                //cv::circle(frame, cv::Point(blob.first, blob.second), 10, cv::Scalar(128, 128, 128));
            }

            for (auto& marker : markers) {
                std::cout << "(" << marker.x << ", " << marker.y << "): " << marker.marker_type << std::endl;
                cv::circle(frame, cv::Point(marker.x, marker.y), 2, cv::Scalar(128, 128, 128), 2);
            }
            
            networkSendTowerPositions();
        }

      //  std::cout << "There were " << squares.size() << " squares detected\n";
        //drawSquares(frame, squares);

        // DRAW CALIBRATION POINTS
        circle(frame, calibration_points[0], 16, cv::Scalar(255.0,255.0,255.0), 4);
        circle(frame, calibration_points[1], 16, cv::Scalar(255.0,0.0,0.0), 4);
        circle(frame, calibration_points[2], 16, cv::Scalar(0.0,255.0,0.0), 4);
        circle(frame, calibration_points[3], 16, cv::Scalar(0.0,0.0,255.0), 4);


        cv::setMouseCallback("WebCam", mouseHandle, NULL);
        imshow("WebCam", frame);
        
        //findTowers2();
        //cv::imshow("WebCam", frame);
         // This is essential as if we leave it, the main thread will receive multiple packets of data containing updated lists every frame.
         // (Realistically, this only needs to run at 24 fps, as this is the maximum performance of the camera.)
        /*Marker marker1(200, 400, 2);
        Marker marker2(500, 200, 3);
        Marker marker3(800, 400, 2);

        send_buffer.seek(0);
        send_buffer << (int)3;
        send_buffer << (int)marker1.x;
        send_buffer << (int)marker1.y;
        send_buffer << (int)marker1.marker_type;
        send_buffer << (int)marker2.x;
        send_buffer << (int)marker2.y;
        send_buffer << (int)marker2.marker_type;
        send_buffer << (int)marker3.x;
        send_buffer << (int)marker3.y;
        send_buffer << (int)marker3.marker_type;

        socket->send(send_buffer.getPtr(), send_buffer.tell());

        //std::cout << "Sent markers" << std::endl;*/
        cv::waitKey(42); //42
        //while (true);
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
            //TODO: calibrate using corners: calibration_points[0]
            tower_locations.push_back(gameobject::Point<int>(objects[i].x + objects[i].width/2, objects[i].y + objects[i].height/2));
        }
        
        networkSendTowerPositions();
    }

    void CVInterface::networkConnect() {
        socket = new sf::TcpSocket();
        sf::TcpSocket::Status st = socket->connect("127.0.0.1", 31654, sf::seconds(30.0f));
        if (st == sf::TcpSocket::Status::Done) {
            std::cout << "[CV] Connected to CV interface successfully!" << std::endl;
        }
    }

    void CVInterface::networkSendTowerPositions() {
        //std::cout << "[CV] Sending towers to control thread: Count: " << tower_locations.size() << std::endl;

        int marker_count = markers.size();
        if (marker_count > 5) { marker_count = 5; }

        // Write data to buffer
        send_buffer.seek(0);
        send_buffer << marker_count;

        //for (auto& marker : markers) {
        for( int cc=0; cc < marker_count; cc++ ){
            auto marker = markers[cc];

            cv::Point i,j;

            // Translate
            marker.x -= calibration_points[0].x;
            marker.y -= calibration_points[0].y;

            // Calculate calibrated basis vectors
            i =(calibration_points[1] - calibration_points[0]);
            j = (calibration_points[3] - calibration_points[0]);

            //float magA = sqrt((float) (i.x*i.x + i.y*i.y));
            //float magB = sqrt((float) (j.x*j.x + j.y*j.y));
            float magA = std::hypot((float) i.x, (float) i.y);
            float magB = std::hypot((float) j.x, (float) j.y);
            // normalize
            float ifx = (float)i.x/magA;
            float ify = (float)i.y/magA;
            float jfx = (float)j.x/magB;
            float jfy = (float)j.y/magB;

            float mfx = (float)marker.x;
            float mfy = (float)marker.y;

            // Project
            float xfactor = mfx*ifx + mfy*ify;
            float yfactor = mfx*jfx + mfy*jfy;

            float newx = (1280.0f/magA)*(xfactor);
            float newy = (720.0f/magB)*(yfactor);

            marker.x = 1280-(int)newx;
            marker.y = (int)newy;
            //width = frame.cols
            //height = frame.rows

            // Marker type shielding
            if (marker.marker_type > 3 || marker.marker_type < 1) {
                marker.marker_type = 1;
            }

            //send_buffer << marker;
            send_buffer << (int)marker.x;
            send_buffer << (int)marker.y;
            send_buffer << (int)marker.marker_type;

            


            std::cout << "MARKER TYPE: " << marker.marker_type << std::endl;
            cv::circle(frame, cv::Point(marker.x, marker.y), 6, cv::Scalar(0, 128, 0), 5);
        }

        // Send
        socket->send(send_buffer.getPtr(), send_buffer.tell());
    }
}
