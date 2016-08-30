#include <stdio.h>
#include <caffe\caffe.hpp>

#include "SimulatorControl.h"
#include "Kinect\KinectMangerThread.h"
#include "Robot\RobotManager.h"

using namespace caffe;

int main(){
	//0-1. Kinect Initialize
	cv::Rect				RobotROI((KINECT_DEPTH_WIDTH - 160) / 2 + 40, (KINECT_DEPTH_HEIGHT - 160) / 2, 160, 160);
	KinectMangerThread		kinect;
	kinect.Initialize(RobotROI);

	//0-2. Robot Initialze
	RobotManager			robot;

	//0-3. Deepnet initialize

	// mode setting - CPU/GPU
	Caffe::set_mode(Caffe::GPU);
	// gpu device number
	int device_id = 0;
	Caffe::SetDevice(device_id);

	//1.Approaching network load 

	//2.Pregrasping network load

	//simulator init
	SimulatorControl simul;
	simul.Initialize();

	//RUN

	robot.DeInitialize();
	kinect.Deinitialize();
	simul.Deinitialize();

	return 0;
}