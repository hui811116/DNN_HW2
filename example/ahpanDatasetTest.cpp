#include <iostream>
#include "dataset.h"

using namespace std;



typedef device_matrix<float> mat;


int main(int argc, char**argv){
	
	size_t labelNum = 48;
	size_t phonemeNum = 39;
	size_t trainDataNum =1; //5000;
//	size_t trainDataNum = 1124823;

	size_t testDataNum =180406;
	size_t labelDataNum = 1124823;
	size_t inFtreDim = 39;
	size_t outFtreDim = 48;
	const char* trainFilename = "/home/larry/Documents/data/MLDS_HW1_RELEASE_v1/mfcc/train.ark";	
//	const char* testFilename = "/home/larry/Documents/data/MLDS_HW1_RELEASE_v1/fbank/test.ark";
	
	//const char* testFilename1 = "data/mfcc/test1_351.ark";
	//const char* testFilename2 = "data/mfcc/test2_351.ark";
	//const char* testFilename = "data/mfcc/test_351.ark";
	
	const char* testFilename = "data/mfcc/test.ark";
	const char* labelFilename = "/home/larry/Documents/data/MLDS_HW1_RELEASE_v1/label/train.lab2";
	const char* labelMapFilename = "/home/larry/Documents/data/MLDS_HW1_RELEASE_v1/phones/48_39.map";
	
	int frameRange=4;	
	
	Dataset dataset=Dataset();
	dataset.loadData("fbank", "trainWithLabel", "/home/ahpan/DeepLearningHW2/DNN_HW2/Data/MLDS_HW1_RELEASE_v1/fbank/trainWithLabel.ark"," ");
	dataset.buildFeatureVector();	
	dataset.outputFeatureVector();
	return 0;
}
