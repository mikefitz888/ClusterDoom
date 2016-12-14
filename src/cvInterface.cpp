#include "../include/cvInterface.h"

namespace cvinterface {
	void ICVInterface::updateTowerList(std::vector<Point<int>>& towerlist) {
		tower_locations = towerlist;
		if(tower_locations.size() > towers.size()) {
			auto tower = game_controller->spawnTowerAt( tower_locations[towers.size()] );
			towers.push_back(tower);
		}else if(tower_locations.size() < towers.size()) {
			//There are less towers in play than in game, find removed tower; For each point find nearest tower and pair
			
		}else {
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

		camera = new cv::VideoCapture(0);
		if(!camera->isOpened()) {
			std::cout << "Cannot open camera!" << std::endl;
		}

		dWidth = camera->get(CV_CAP_PROP_FRAME_WIDTH);
		dHeight = camera->get(CV_CAP_PROP_FRAME_HEIGHT);
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
		cv::imshow("WebCam", frame);
		char keypress = cvWaitKey(20);
	}

	void ICVInterface::release() {
		
	}


}