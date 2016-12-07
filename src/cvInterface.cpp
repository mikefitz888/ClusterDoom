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
		camera = new cv::VideoCapture(0);
		if(!camera->isOpened()) {
			std::cout << "Cannot open camera!" << std::endl;
		}

		dWidth = camera->get(CV_CAP_PROP_FRAME_WIDTH);
		dHeight = camera->get(CV_CAP_PROP_FRAME_HEIGHT);
		cv::namedWindow("cvDebug", CV_WINDOW_AUTOSIZE);
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

		//TODO: Parse frame data
		cv::imshow("cvDebug", frame);
	}

	void ICVInterface::release() {
		
	}


}