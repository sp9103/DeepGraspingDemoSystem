#include <stdio.h>
#include <caffe\caffe.hpp>

#include "SimulatorControl.h"
#include "Kinect\KinectMangerThread.h"
#include "Robot\RobotManager.h"
#include "HandTracker\ColorBasedTracker_pregrasp.h"

#define APPROACH_NET_PATH "APPROACH_NET\\deploy_val_app_Net_fc.prototxt"
#define APPROACH_NET_TRAINRESULT "APPROACH_NET\\app_Net_iter_50000.caffemodel"
#define PREGRASP_NET_PATH "PREGRASP_NET\\deploy_val_pregrasp_Net_fc.prototxt"
#define PREGRASP_NET_TRAINRESULT ""

using namespace caffe;

void resultToRobotMotion(const std::vector<caffe::Blob<float>*>& src, int *dst);
void rgbConvertCaffeType(cv::Mat src, cv::Mat *dst);

int main(){
	//0-1. Kinect Initialize
	cv::Rect				RobotROI((KINECT_DEPTH_WIDTH - 160) / 2 + 40, (KINECT_DEPTH_HEIGHT - 160) / 2, 160, 160);
	KinectMangerThread		kinect;
	kinect.Initialize(RobotROI);

	//0-2. Robot Initialze
	RobotManager			robot;
	robot.Initialize(3, 3);

	//robot Initial move
	robot.TorqueOn();
	robot.safeRelease();
	printf("if system ready, press any key to console\n");
	getch();
	cv::Mat RgbBack = kinect.getImg();
	cv::Mat DepthBack = kinect.getDepth();
	cv::cvtColor(RgbBack, RgbBack, CV_BGRA2BGR);
	ColorBasedTracker_pregrasp tracker;
	tracker.InsertBackGround(RgbBack, RgbBack);

	//0-3. Deepnet initialize
	// mode setting - CPU/GPU
	Caffe::set_mode(Caffe::GPU);
	// gpu device number
	int device_id = 0;
	Caffe::SetDevice(device_id);

	//1.Approaching network load 
	Net<float> approach_net(APPROACH_NET_PATH, caffe::TEST);
	approach_net.CopyTrainedLayersFrom(APPROACH_NET_TRAINRESULT);

	//2.Pregrasping network load
	Net<float> pregrasp_net(PREGRASP_NET_PATH, caffe::TEST);
	pregrasp_net.CopyTrainedLayersFrom(PREGRASP_NET_TRAINRESULT);

	//simulator init
	SimulatorControl simul;
	simul.Initialize();

	//RUN
	Blob<float> rgbBlob(1, 3, HEIGHT, WIDTH);
	Blob<float> depthBlob(1, 1, HEIGHT, WIDTH);
	int robotMotion[9];
	float loss;
	printf("Start calculate network output, press 's' key to opencv window\n");
	while (1){
		cv::Mat kinectRGB	= kinect.getImg();
		cv::Mat kinectDEPTH = kinect.getDepth();

		cv::imshow("ROI", kinectRGB);
		char key = cv::waitKey(10);

		if (key == 's'){
			//Approaching network
			//Mat -> Blob
			cv::cvtColor(kinectRGB, kinectRGB, CV_BGRA2BGR);
			cv::Mat caffeRgb;
			rgbConvertCaffeType(kinectRGB, &caffeRgb);
			memcpy(rgbBlob.mutable_cpu_data(), caffeRgb.ptr<float>(0), sizeof(float) * HEIGHT * WIDTH * CHANNEL);
			memcpy(depthBlob.mutable_cpu_data(), kinectDEPTH.ptr<float>(0), sizeof(float) * HEIGHT * WIDTH);
			vector<Blob<float>*> input_vec;				//�Է� RGB, DEPTH
			input_vec.push_back(&rgbBlob);
			input_vec.push_back(&depthBlob);

			//Approaching
			const vector<Blob<float>*>& result_approach = approach_net.Forward(input_vec, &loss);
			resultToRobotMotion(result_approach, robotMotion);
			simul.renderData(robotMotion);
			printf("if u want move, press any key to console\n");
			getch();
			robot.Approaching(robotMotion);

			//Pregrasp
			while (1){
				input_vec.clear();
				cv::Mat kinectRGBPregrasp = kinect.getImg();
				cv::Mat kinectDEPTHPregrasp = kinect.getDepth();
				cv::Mat procImg, procDepth;
				tracker.calcImage(kinectRGBPregrasp, kinectDEPTHPregrasp, &procImg, &procDepth);

				cv::imshow("procImg", procImg);
				cv::waitKey(10);
				rgbConvertCaffeType(procImg, &caffeRgb);
				memcpy(rgbBlob.mutable_cpu_data(), caffeRgb.ptr<float>(0), sizeof(float) * HEIGHT * WIDTH * CHANNEL);
				memcpy(depthBlob.mutable_cpu_data(), procDepth.ptr<float>(0), sizeof(float) * HEIGHT * WIDTH);
				input_vec.push_back(&rgbBlob);
				input_vec.push_back(&depthBlob);
				const vector<Blob<float>*>& result_pregrasp = pregrasp_net.Forward(input_vec, &loss);
				resultToRobotMotion(result_approach, robotMotion);
				simul.renderData(robotMotion);
				printf("Move next step robot motion press any key\n");
				getch();
				robot.Move(robotMotion);
			}

			robot.safeRelease();
		}
		else if (key == 'q')
			break;
	}
	robot.TorqueOff();

	robot.DeInitialize();
	kinect.Deinitialize();
	simul.Deinitialize();

	return 0;
}

void resultToRobotMotion(const std::vector<caffe::Blob<float>*>& src, int *dst){
	int angle_max[9] = { 251000, 251000, 251000, 251000, 151875, 151875, 4095, 4095, 4095 };
	float outputData[9];
	memcpy(outputData, src.at(0)->cpu_data(), sizeof(float) * 9);
	for (int j = 0; j < DATADIM; j++){
		dst[j] = (int)(outputData[j] / 180.f * angle_max[j]);
	}
}

void rgbConvertCaffeType(cv::Mat src, cv::Mat *dst){
	dst->create(src.rows, src.cols, CV_32FC3);
	for (int h = 0; h < src.rows; h++){
		for (int w = 0; w < src.cols; w++){
			for (int c = 0; c < src.channels(); c++){
				dst->at<float>(c*src.rows*src.cols + src.cols*h + w) = (float)src.at<cv::Vec3b>(h, w)[c] / 255.0f;
			}
		}
	}
}