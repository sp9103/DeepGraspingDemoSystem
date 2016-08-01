#include <stdio.h>
#include <caffe\caffe.hpp>

#include "SimulatorControl.h"

int main(){
	//Deepnet initialize

	//simulator init
	SimulatorControl simul;
	simul.Initialize();

	simul.Deinitialize();

	return 0;
}