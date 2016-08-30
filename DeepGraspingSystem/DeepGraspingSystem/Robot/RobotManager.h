#pragma once
#include "..\ARMSDK\include\ARMSDK.h"
#include "..\stdafx.h"
#include "RobotArm.h"

#ifdef _DEBUG
#pragma comment(lib, "ARMSDKd.lib")
#endif
#ifdef NDEBUG
#pragma comment(lib, "ARMSDK.lib") 
#endif

class RobotManager
{
public:
	RobotManager();
	~RobotManager();

	int Initialize(int PortNum, int BaudRateNum);
	int DeInitialize();

private:
	armsdk::RobotInfo robot;
	armsdk::Kinematics kin;
	RobotArm arm;

	void ControllerInit(int PortNum, int BaudRateNum);
	bool robotConnectCheck();
};
