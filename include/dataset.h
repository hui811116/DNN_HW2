#ifndef DATASET_H_
#define DATASET_H_

#include <string>
#include <vector>
#include <fstream>
#include "device_matrix.h"
using namespace std;
typedef device_matrix<float> mat;
typedef struct{
	
	const char* trainPath;
	size_t trainDataNum;
	const char* testPath;
	size_t testDataNum;
	const char* labelPath;
	size_t labelDataNum;
	size_t labelNum;
	size_t inputDim;
	size_t outputDim;
	size_t phonemeNum;
	int frameRange;
}Data;

class Dataset{
	public:
	Dataset();
	Dataset(const char* trainPath, size_t trainDataNum, const char* testPath, size_t testDataNum, const char* labelPath, size_t labelDataNum, size_t labelNum, size_t inputDim, size_t outputDim, size_t phonemeNum);
	Dataset(Data data, char mode);
	Dataset(const Dataset& data);
	~Dataset();
	
	
	mat getTestSet();
	mat getTestSet(float** testData,size_t frameRan, size_t testNum);
	size_t getNumOfTrainData();
	size_t getNumOfTestData();
	size_t getNumOfLabel();
	size_t getNumOfPhoneme();
	size_t getInputDim();
	size_t getOutputDim();
	int    getTrainSize();
	int    getValidSize();

	string* getTrainDataNameMatrix();
	string* getTestDataNameMatrix();
	float** getTrainDataMatrix();
	float** getTestDataMatrix();
	map<string, int> getLabelMap();
	map<string, string> getTo39PhonemeMap();
	void   getBatch(int batchSize, mat& batch, mat& batchLabel);
	void   getTrainSet(int trainSize, mat& trainData, vector<size_t>& trainLabel);
	void   getValidSet(int validSize, mat& validData, vector<size_t>& validLabel);
	void   dataSegment( float trainProp);
	void   printLabelMap(map<string, int> labelMap);
	void   printTo39PhonemeMap(map<string, string>);
	void   prtPointer(float** input, int r, int c);
	void   loadTo39PhonemeMap(const char*);
	void   saveCSV(vector<size_t> testResult);

private:
	// dataset parameters
	size_t _featureDimension; //input Dim
	size_t _stateDimension;   //output Dim
	size_t _numOfTrainData;
	size_t _numOfTestData;
	size_t _numOfLabel;
	size_t _numOfPhoneme;
	int    _trainSize;
	int    _validSize;
	int _frameRange; //Used for frame
	
	// datasetJason private functions
	mat    outputNumtoBin(int* outputVector, int vectorSize);
	// change 0~47 to a 48 dim mat
	mat    inputFtreToMat(float** input, int r, int c);	
       // void   prtPointer(float** input, int r, int c);	
	// original data
	string* _trainDataNameMatrix; // frame name
	string* _testDataNameMatrix;

	float** _trainDataMatrix; // input MFCC features
	float** _testDataMatrix;

	int* _labelMatrix; // output phoneme changed to integer

	map<string, int> _labelMap; //Map phoneme to int
	map<string, string> _To39PhonemeMap; //Map the output to 39 dimension
	
	// storing training matrix
	mat trainMat;
	mat validMat;
	float** _trainX;
	float** _validX;
	int* _trainY;
	int* _validY;
	bool _trainSetFlag;
	bool _validSetFlag;
	long long int _batchCtr;
};

#endif 
