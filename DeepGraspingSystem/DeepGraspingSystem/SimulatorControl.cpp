#include "SimulatorControl.h"


SimulatorControl::SimulatorControl()
{
	DeinitCheck = true;

	robotDataFormat.Thumb.x = 130.0f;
	robotDataFormat.Thumb.y = 0.0f;
	robotDataFormat.Thumb.z = 140.0f;
	robotDataFormat.upperLeft.x = -60.0f;
	robotDataFormat.upperLeft.y = 45.0f;
	robotDataFormat.upperLeft.z = 200.0f;
	robotDataFormat.upperRight.x = -60.0f;
	robotDataFormat.upperRight.y = -45.0f;
	robotDataFormat.upperRight.z = 200.0f;
}


SimulatorControl::~SimulatorControl()
{
	if (!DeinitCheck)
		Deinitialize();
}

UINT WINAPI SimulatorControl::simulateThread(LPVOID param){
	system("..\\robotArmVis\\RobotSimulator.exe");

	return 1;
}

void SimulatorControl::Initialize(){
	DeinitCheck = false;

#ifdef RIGHT_ARM_USE
	//RightArm
	robot.AddJoint(0.0, ML_PI_2, 0.0, 0.0, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 1);
	robot.AddJoint(0.0, -ML_PI_2, 0.0, 0.0, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 3);
	robot.AddJoint(30.0, -ML_PI_2, 246.0, 0.0, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 5);
	robot.AddJoint(-30.0, ML_PI_2, 0.0, ML_PI_2, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 7);
	robot.AddJoint(0.0, -ML_PI_2, 216.0, 0.0, ML_PI, -ML_PI, 151875, -151875, ML_PI, -ML_PI, 9);
	robot.AddJoint(0.0, ML_PI_2, 0.0, 0.0, ML_PI, -ML_PI, 151875, -151875, ML_PI, -ML_PI, 11);
#elif defined LEFT_ARM_USE
	//Leftarm
	robot.AddJoint(0.0, -ML_PI_2, 0.0, 0.0, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 2);
	robot.AddJoint(0.0, ML_PI_2, 0.0, 0.0, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 4);
	robot.AddJoint(30.0, ML_PI_2, 246.0, 0.0, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 6);
	robot.AddJoint(-30.0, -ML_PI_2, 0.0, -ML_PI_2, ML_PI, -ML_PI, 251000, -251000, ML_PI, -ML_PI, 8);
	robot.AddJoint(0.0, ML_PI_2, 216.0, 0.0, ML_PI, -ML_PI, 151875, -151875, ML_PI, -ML_PI, 10);
	robot.AddJoint(0.0, -ML_PI_2, 0.0, 0.0, ML_PI, -ML_PI, 151875, -151875, ML_PI, -ML_PI, 12);
#endif
	kin.InitRobot(&robot);

	HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, simulateThread, NULL, 0, NULL);			//simulator start - TO-DO : How to exit thread safely
	robotvisServer.Init(NULL, PORT);																//server start
}

void SimulatorControl::Deinitialize(){
	HWND handle = FindWindow(NULL, TEXT("robotArmVis"));
	SendMessage(handle, WM_CLOSE, 0, 0);
	DeinitCheck = true;
}

void SimulatorControl::fingerTransform(RobotInfoData *src, armsdk::Pose3D *xaxis, armsdk::Pose3D *yaxis, armsdk::Pose3D *zaxis){
	veci angi(6);
	for (int i = 0; i < 6; i++)	angi[i] = src->Angle[i];
	vecd angd = kin.Value2Rad(angi);
	armsdk::Pose3D CurrentPose, xaxis_, yaxis_, zaxis_;
	kin.EndAxis(angd, &CurrentPose, &xaxis_, &yaxis_, &zaxis_);

	cv::Mat m_RotMat(3, 3, CV_32FC1);

	cv::Mat invR;
	invR.create(3, 3, CV_32FC1);

	invR.at<float>(0, 0) = xaxis_.x;
	invR.at<float>(1, 0) = xaxis_.y;
	invR.at<float>(2, 0) = xaxis_.z;

	invR.at<float>(0, 1) = yaxis_.x;
	invR.at<float>(1, 1) = yaxis_.y;
	invR.at<float>(2, 1) = yaxis_.z;

	invR.at<float>(0, 2) = zaxis_.x;
	invR.at<float>(1, 2) = zaxis_.y;
	invR.at<float>(2, 2) = zaxis_.z;

	if (xaxis != NULL)	*xaxis = xaxis_;
	if (yaxis != NULL)	*yaxis = yaxis_;
	if (zaxis != NULL)	*zaxis = zaxis_;

	//m_RotMat = invR.t();

	rot(invR, &src->Thumb);
	rot(invR, &src->upperLeft);
	rot(invR, &src->upperRight);
}

void SimulatorControl::rot(cv::Mat rotMat, FingerInfo *fin){
	cv::Mat temp, resultMat;
	temp.create(3, 1, CV_32FC1);
	resultMat.create(3, 1, CV_32FC1);

	temp.at<float>(0, 0) = fin->x;
	temp.at<float>(1, 0) = fin->y;
	temp.at<float>(2, 0) = fin->z;

	resultMat = rotMat * temp;

	fin->x = resultMat.at<float>(0, 0);
	fin->y = resultMat.at<float>(1, 0);
	fin->z = resultMat.at<float>(2, 0);
}

void SimulatorControl::renderData(int *src){
	//시뮬레이터 체크
	RobotInfoData sendData = robotDataFormat;
	armsdk::Pose3D xaxis, zaxis;
	for (int i = 0; i < 6; i++)
		sendData.Angle[i] = src[i];
	fingerTransform(&sendData, &xaxis, NULL, &zaxis);

	robotvisServer.SendAndCheck(sendData);
}