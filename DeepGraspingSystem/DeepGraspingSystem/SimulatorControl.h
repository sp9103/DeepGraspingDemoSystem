#pragma once
#include "ARMSDK\include\ARMSDK.h"

#include <process.h>
#include <opencv2\opencv.hpp>

#include "RobotVisServer.h"

#define PORT	2252
#define RIGHT_ARM_USE

class SimulatorControl
{
public:
	SimulatorControl();
	~SimulatorControl();

	void Initialize();
	void Deinitialize();
	bool renderData(int *src);

private:
	RobotVisServer robotvisServer;
	bool DeinitCheck;
	RobotInfoData robotDataFormat;
	armsdk::Kinematics kin;
	armsdk::RobotInfo robot;

	static UINT WINAPI simulateThread(LPVOID param); // 쓰레드 함수.
	void fingerTransform(RobotInfoData *src, armsdk::Pose3D *xaxis = NULL, armsdk::Pose3D *yaxis = NULL, armsdk::Pose3D *zaxis = NULL);
	void rot(cv::Mat rotMat, FingerInfo *fin);
};

