#include <stdio.h>
#include <caffe\caffe.hpp>

#include "SimulatorControl.h"
#include "Kinect\KinectMangerThread.h"


#ifdef _DEBUG
#pragma comment(lib, "ARMSDKd.lib")
#endif
#ifdef NDEBUG
#pragma comment(lib, "ARMSDK.lib") 
#endif

using namespace caffe;

int main(){
	//0. Deepnet initialize

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

	simul.Deinitialize();

	return 0;
}