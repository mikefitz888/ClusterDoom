#include "../include/cvInterface.h"

namespace cvinterface {
	void ICVInterface::updateTowerList(/*std::vector<Point<int>>& towerlist*/) {
		//tower_locations = towerlist;
		if (tower_locations.size() > towers.size()) {
			auto tower = game_controller->spawnTowerAt(tower_locations[towers.size()]);
			towers.push_back(tower);
		} else if (tower_locations.size() < towers.size()) {
			//There are less towers in play than in game, find removed tower; For each point find nearest tower and pair

		} else {
			//Tower count unchanged
		}

		//Deal with any movement
	}

	void ICVInterface::init() {
		//CvCapture* pCapture;

		// Start capturing data from the webcam
		//pCapture = cvCaptureFromCAM(CV_CAP_V4L2);

		// Display image properties
		//std::cout << "Width of frame: " << cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_WIDTH) << std::endl; 		// Width of the frames in the video stream
		//std::cout << "Height of frame: " << cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_HEIGHT) << std::endl; 	// Height of the frames in the video stream
		//std::cout << "Image brightness: " << cvGetCaptureProperty(pCapture, CV_CAP_PROP_BRIGHTNESS) << std::endl; 	// Brightness of the image (only for cameras)
		//std::cout << "Image contrast: " << cvGetCaptureProperty(pCapture, CV_CAP_PROP_CONTRAST) << std::endl; 		// Contrast of the image (only for cameras)
		//std::cout << "Image saturation: " << cvGetCaptureProperty(pCapture, CV_CAP_PROP_SATURATION) << std::endl;		// Saturation of the image (only for cameras)
		//std::cout << "Image hue: " << cvGetCaptureProperty(pCapture, CV_CAP_PROP_HUE) << std::endl;			// Hue of the image (only for cameras)

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
		//pFrame = cvQueryFrame(pCapture);
		//cvShowImage("WebCam", pFrame);
		//char keypress = cvWaitKey(20);

		while(true) {
			step();
		}
	}

	void ICVInterface::step()
	{
		bool success = camera->read(frame);
		if(!success) {
			std::cout << "Cannot read frame from video stream!" << std::endl;
			return;
		}
		//std::cout << "Stepping" << std::endl;
		//TODO: Parse frame data
		findTowers();
		cv::imshow("WebCam", frame);
		char keypress = cvWaitKey(20);
	}

	void ICVInterface::release() {

	}

	void ICVInterface::findTowers() {
		cv::Mat frame2 = frame.clone();
	    cv::Mat frame_gray;
	    std::vector<cv::Rect> objects;

	    for (int i = 0; i < frame2.rows; i++) {
	        for (int j = 0; j < frame2.cols; j++) {
	            auto& point = frame2.at<cv::Vec3b>(i, j);

	            int greenblue = 70;
	            int red = 105;
	            if (!(point[2] > red && point[1] < greenblue && point[0] < greenblue)) {
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
		cout << "Objects detected: " << objects.size() << endl;

		tower_locations.clear();
		// Next step is to commit the findings to the tower_locations list...
	    for (size_t i = 0; i < objects.size(); i++)
	    {
	        cv::rectangle(frame, cv::Point(objects[i].x, objects[i].y), cv::Point(objects[i].x + objects[i].width, objects[i].y + objects[i].height), cv::Scalar(0, 255, 0), 2);
			tower_locations.push_back(Point<int>(objects[i].x + objects[i].width/2, objects[i].y + objects[i].height/2));
	    }
		updateTowerList();
	}
}
