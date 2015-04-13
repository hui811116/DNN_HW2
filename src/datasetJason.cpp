#include "dataset.h"
#include <algorithm>
#include <vector>
#include <cstdlib> // rand()
#include <ctime>
using namespace std;
typedef device_matrix<float> mat;
void Dataset::getBatch(int batchSize, mat& batch, mat& batchLabel){
	// use shuffled trainX to get batch sequentially
	float** batchFtre = new float*[batchSize];
	int*    batchOutput = new int[batchSize];
	for (int i = 0; i < batchSize; i++){
		batchFtre[i] = _trainX[ _batchCtr % _trainSize ];
		batchOutput[i] = _trainY[ _batchCtr % _trainSize ];
		_batchCtr ++;
	}

	
	
	// random initialize indices for this batch	
	/*
	int* randIndex = new int [batchSize];
	for (int i = 0; i < batchSize; i++){
		randIndex[i] = rand() % _trainSize; 
	}
	float** batchFtre = new float*[batchSize];
	int*    batchOutput = new int[batchSize];
	for (int i = 0; i < batchSize; i++){
		batchFtre[i] = _trainX[ randIndex[i] ];
		batchOutput[i] = _trainY[ randIndex[i] ];
	}
	delete[] randIndex;
	randIndex = NULL;
	*/
	// convert them into mat format
	batch = inputFtreToMat( batchFtre, getInputDim(), batchSize);
	batchLabel = outputNumtoBin( batchOutput, batchSize );
	// free tmp pointers
	delete[] batchOutput;
	delete[] batchFtre;
	batchOutput = NULL;
	batchFtre = NULL;
	// for debugging, print both matrices
	/*
	cout << "This is the feature matrix\n";
	batch.print();
	cout << "from trainX pointer:\n";
	prtPointer(batchFtre, _numOfLabel, batchSize);
	cout << "This is the label matrix\n";
	batchLabel.print();
	*/
}

void Dataset::getTrainSet(int trainSize, mat& trainData, vector<size_t>& trainLabel){
	if (_trainSetFlag == true){
		trainData = trainMat;
		return;
	}
	if (trainSize > _trainSize){
		cout << "requested training set size overflow, will only output "
		     << _trainSize << " training sets.\n";
		trainSize = _trainSize;
	}
	trainLabel.clear();
	// random initialize
		
	int* randIndex = new int [trainSize];
	for (int i = 0; i < trainSize; i++){
		if (trainSize == _trainSize)
			randIndex[i] = i;
		else
			randIndex[i] = rand() % _trainSize; 
	}
	float** trainFtre = new float*[trainSize];
	for (int i = 0; i < trainSize; i++){
		trainFtre[i] = _trainX[ randIndex[i] ];
		trainLabel.push_back( _trainY[ randIndex[i] ] );
	}
	trainData = inputFtreToMat(trainFtre, getInputDim(), trainSize);
	
	_trainSetFlag = true;
	trainMat = trainData;
	//cout << "get Train Set:\n";
	//trainData.print();
	delete[] randIndex;
	delete[] trainFtre;
	randIndex = NULL;
	trainFtre = NULL;
}

void Dataset::getValidSet(int validSize, mat& validData, vector<size_t>& validLabel){
	if (_validSetFlag == true){
		validData = validMat;
		return;
	}
	if (validSize > _validSize){
		cout << "requested valid set size is too big, can only feed in " << _validSize << " data.\n";
	validSize = _validSize;
	}
	validLabel.clear();
	// random choose index
	cout << "validate size is : " << validSize << " " << _validSize << endl;
	int* randIndex = new int [validSize];
	for (int i = 0; i < validSize; i++){
		if (validSize == _validSize)
			randIndex[i] = i;
		else
			randIndex[i] = rand() % _validSize; 
	}
	float** validFtre = new float*[validSize];
	for (int i = 0; i < validSize; i++){
		validFtre[i] = _validX[ randIndex[i] ];
		validLabel.push_back( _validY[ randIndex[i] ] );
	}
	validData = inputFtreToMat(validFtre, getInputDim(), validSize);
	
	_validSetFlag = true;
	validMat = validData;
	delete[] validFtre;
	delete[] randIndex;
	validFtre = NULL;
	randIndex = NULL;
}




void Dataset::dataSegment( float trainProp ){
	if (_trainX != NULL){
		delete _trainX;
		delete _trainY;
		delete _validX;
		delete _validY;
	}
	cout << "start data segmenting:\n";
	cout << "num of data is "<< getNumOfTrainData() << endl;
	// segment data into training and validating set
	_trainSize = trainProp*getNumOfTrainData();
	_validSize = getNumOfTrainData() - _trainSize;
	
	//create random permutation
	vector<int> randIndex;
	
	for (int i = 0; i < getNumOfTrainData(); i++){
		randIndex.push_back( i );
	}
	random_shuffle(randIndex.begin(), randIndex.end());
	// print shuffled data
	cout << "start shuffling:\n";
	/*
	for (int i = 0; i < getNumOfTrainData(); i++){
		cout << randIndex[i] <<" ";
	}
	*/
	// 
	cout << "put feature into training set\n";
	cout << "trainingsize = " << _trainSize <<endl;
	_trainX = new float*[_trainSize];
	_trainY = new int[_trainSize];
	for (int i = 0; i < _trainSize; i++){
		_trainX[i] = _trainDataMatrix[ randIndex[i] ]; 
		_trainY[i] = _labelMatrix[ randIndex[i] ];  // depends on ahpan
	}
	cout << "put feature into validating set\n";
	cout << "validatingsize = " << _validSize <<endl;
	_validX = new float*[_validSize];
	_validY = new int[_validSize];
	for (int i = 0; i < _validSize; i++){
		_validX[i] = _trainDataMatrix [ randIndex[_trainSize + i] ];
		_validY[i] = _labelMatrix[ randIndex[_trainSize + i] ];
	}
	// debugging, print out train x y valid x y
	/*
	prtPointer(_trainX, _numOfLabel, _trainSize);
	prtPointer(_validX, _numOfLabel, _validSize);
	cout << "print train phoneme:\n";
	for (int i = 0; i < _trainSize; i++)
		cout << _trainY[i] << " ";
	cout << "print valid phoneme:\n";
	for (int i = 0; i < _validSize; i++)
		cout << _validY[i] << " ";
	*/
}
mat Dataset::outputNumtoBin(int* outputVector, int vectorSize)
{
	float* tmpVector = new float[ vectorSize * _numOfLabel ];
	for (int i = 0; i < vectorSize; i++){
		for (int j = 0; j < _numOfLabel; j++){
			*(tmpVector + i*_numOfLabel + j) = (outputVector[i] == j)?1:0;
		}
	}

	mat outputMat(tmpVector, _numOfLabel, vectorSize);
	delete[] tmpVector;
	tmpVector = NULL;
	return outputMat;
}
mat Dataset::inputFtreToMat(float** input, int r, int c){
	// r shall be the number of Labels
	// c shall be the number of data
	//cout << "Ftre to Mat size is : " << r << " " << c<<endl;
	//cout << "size is : " << r << " " << c<<endl;
	float* inputReshaped = new float[r * c];
	for (int i = 0; i < c; i++){
		for (int j = 0; j < r; j++){
			//*(inputReshaped + i*r + j) = *(*(input + i) +j);
			*(inputReshaped + i*r + j) = input[i][j];
		}
	}
	mat outputMat(inputReshaped, r, c);
	delete[] inputReshaped;
	inputReshaped = NULL;
	return outputMat;
}
void Dataset::prtPointer(float** input, int r, int c){
	//cout << "this prints the pointer of size: " << r << " " << c << endl;
	for (int i = 0; i < c; i++){
		cout << i << endl;
		for(int j = 0; j < r; j++){
			cout <<input[i][j]<<" ";
			if ((j+1)%5 == 0) cout <<endl;
		}
		cout <<endl;
	}
	return;
}
