#include "RobotManager.h"


RobotManager::RobotManager()
{
}


RobotManager::~RobotManager()
{
}


void RobotManager::ControllerInit(int PortNum, int BaudRateNum){
	int robotid[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17 };
	int vel[] = { 2000, 2000, 2000, 2000, 2000, 2000, 50, 50, 50 };
	arm.Init(PortNum, BaudRateNum, robotid);
	arm.SetGoalVelocity(vel);
}

bool RobotManager::robotConnectCheck(){
	veci angi(6);
	arm.Arm_Get_JointValue(&angi);

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

	//¸Æ½Ã¸Ø ¾Þ±Û Ã¼Å© - ¾²·¹±â°ª °É·¯³»±â
	for (int JointNum = 0; JointNum < 6; JointNum++)
	{
		if (abs(angi[JointNum]) > robot.GetJointInfo(JointNum)->GetMaxAngleInValue() + 10)
		{
			cout << "read fail" << endl;
			printf("Data Fail %d\n", angi[JointNum]);
			return false;
		}
	}
}

int RobotManager::Initialize(int PortNum, int BaudRateNum){
	ControllerInit(PortNum, BaudRateNum);
	if (robotConnectCheck()){
		printf("Robot Init fail\n");
		return -1;
	}
}

int RobotManager::DeInitialize(){
	arm.DeInit();

	return 0;
}