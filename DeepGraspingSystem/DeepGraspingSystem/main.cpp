#include <stdio.h>
#include <caffe\caffe.hpp>

#include "SimulatorControl.h"
#include "Kinect\KinectMangerThread.h"
#include "Robot\RobotManager.h"

#define APPROACH_NET_PATH "APPROACH_NET\\deploy_val_app_Net_fc.prototxt"
#define APPROACH_NET_TRAINRESULT "APPROACH_NET\\app_Net_iter_50000.caffemodel"
#define PREGRASP_NET_PATH ""
#define PREGRASP_NET_TRAINRESULT

using namespace caffe;

int main(){
	//0-1. Kinect Initialize
	cv::Rect				RobotROI((KINECT_DEPTH_WIDTH - 160) / 2 + 40, (KINECT_DEPTH_HEIGHT - 160) / 2, 160, 160);
	KinectMangerThread		kinect;
	kinect.Initialize(RobotROI);

	//0-2. Robot Initialze
	RobotManager			robot;
	robot.Initialize(3, 3);

	//robot Initial move
	robot.safeRelease();
	printf("if system ready, press any key to console\n");
	getch();
	cv::Mat RgbBack = kinect.getImg();
	cv::Mat DepthBack = kinect.getDepth();

	//0-3. Deepnet initialize
	// mode setting - CPU/GPU
	Caffe::set_mode(Caffe::GPU);
	// gpu device number
	int device_id = 0;
	Caffe::SetDevice(device_id);

	//1.Approaching network load 
	Net<float> caffe_test_net(APPROACH_NET_PATH, caffe::TEST);
	caffe_test_net.CopyTrainedLayersFrom(APPROACH_NET_TRAINRESULT);

	//2.Pregrasping network load

	//simulator init
	SimulatorControl simul;
	simul.Initialize();

	//RUN
	Blob<float> rgbBlob(1, 3, HEIGHT, WIDTH);
	Blob<float> depthBlob(1, 1, HEIGHT, WIDTH);

	printf("Start calculate network output, press 's' key to opencv window\n");
	while (1){
		cv::Mat kinectRGB	= kinect.getImg();
		cv::Mat kinectDEPTH = kinect.getDepth();

		cv::imshow("ROI", kinectRGB);
		char key = cv::waitKey(10);

		if (key == 's'){
			//Mat -> Blob
			memcpy(rgbBlob.mutable_cpu_data(), kinectRGB.ptr<float>(0), sizeof(float) * HEIGHT * WIDTH * CHANNEL);
			memcpy(depthBlob.mutable_cpu_data(), kinectDEPTH.ptr<float>(0), sizeof(float) * HEIGHT * WIDTH);
			//입력 형식 맞춰주기
			vector<Blob<float>*> input_vec;				//입력 RGB, DEPTH
			input_vec.push_back(&rgbBlob);
			input_vec.push_back(&depthBlob);
		}
	}

	robot.DeInitialize();
	kinect.Deinitialize();
	simul.Deinitialize();

	return 0;
}