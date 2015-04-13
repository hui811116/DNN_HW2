#include <iostream>
#include <cstring>
#include <cfloat>
#include <thrust/device_ptr.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/functional.h>
#include <thrust/transform.h>
#include "myAlgorithm.h"

#define	MAX_FEATURE_SIZE	500
#define MAX_STATE_SIZE   	48


// Debug function
void print(thrust::device_ptr<float> ptr, size_t size){
	for(int i = 0; i < size; i++) std::cout << "D[" << i << "] = " << ptr[i] << std::endl;
}

void print(const thrust::device_vector<float>& D){
	for(int i = 0; i < D.size(); i++) std::cout << "D[" << i << "] = " << D[i] << std::endl;
}

void viterbi(mat& yOpt, const mat& w, const mat& x, size_t inputDim, size_t stateNum = MAX_STATE_SIZE){
	
	//cout << "Enter Viterbi function\n";

	assert( w.getRows() == stateNum*(stateNum+2) + inputDim*stateNum );
	assert( x.getRows() == inputDim );
	assert( stateNum <= MAX_STATE_SIZE);

	size_t featureNum = x.getCols();
	size_t transIdx = inputDim*stateNum;
	yOpt.resize(1, featureNum, 0);

	thrust::device_ptr<float> ptrX(x.getData());
	thrust::device_ptr<float> ptrW(w.getData());
	thrust::device_ptr<float> ptrYOpt(yOpt.getData());

	cout << "x:\n";
	print(ptrX, x.getCols());
	cout << "w:\n";
	print(ptrW, w.getRows());

	float viterbiTemp[MAX_STATE_SIZE][MAX_FEATURE_SIZE];
	size_t viterbiTrack[MAX_STATE_SIZE][MAX_FEATURE_SIZE];

	memset(viterbiTemp, -FLT_MAX, sizeof(viterbiTemp));
	memset(viterbiTrack, -1, sizeof(viterbiTrack));

	thrust::device_vector<float> phi(w.getRows());
	thrust::device_vector<float> temp(w.getRows());
	

	for(size_t k = 0; k < stateNum; k++){
		thrust::fill(phi.begin(), phi.end(), 0);
		thrust::copy(ptrX, ptrX + inputDim, phi.begin() + k*inputDim);
		phi[transIdx + stateNum*stateNum + k] = 1;

		//cout << "phi:\n";
		//print(phi);

		thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<float>());
		float sum = thrust::reduce(temp.begin(), temp.end(), (float) 0, thrust::plus<float>());
		viterbiTemp[k][0] = sum;
	}

	for(size_t i = 1; i < featureNum-1; i++){
		for(size_t k = 0; k < stateNum; k++){
			for(size_t j = 0; j < stateNum; j++){
				thrust::fill(phi.begin(), phi.end(), 0);
				thrust::copy(ptrX+i*inputDim, ptrX+(i+1)*inputDim, phi.begin() + k*inputDim);
				phi[transIdx + j*stateNum + k] = 1;
	
				//cout << "phi:\n";
				//print(phi);

				thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<float>());
				float sum = thrust::reduce(temp.begin(), temp.end(), (float) 0, thrust::plus<float>());
				if( viterbiTemp[k][i] < sum + viterbiTemp[j][i-1] ){
					viterbiTemp[k][i] = sum + viterbiTemp[j][i-1];
					viterbiTrack[k][i] = j;
				}
			}
		}
	}

	for(size_t k = 0; k < stateNum; k++){
		for(size_t j = 0; j < stateNum; j++){
			thrust::fill(phi.begin(), phi.end(), 0);
			thrust::copy(ptrX + (featureNum-1)*inputDim, ptrX + featureNum*inputDim, phi.begin() + k*inputDim);
			phi[transIdx + (stateNum+1)*stateNum + k] = 1;
			thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<float>());
			float sum = thrust::reduce(temp.begin(), temp.end(), (float) 0, thrust::plus<float>());
			if( viterbiTemp[k][featureNum-1] < sum + viterbiTemp[j][featureNum-2] ){
				viterbiTemp[k][featureNum-1] = sum + viterbiTemp[j][featureNum-2];
				viterbiTrack[k][featureNum-1] = j;
			}
		}
	}

	// Back tracking
	float maxValue = viterbiTemp[0][featureNum-1];
	size_t idx = 0;
	for(size_t j = 0; j < stateNum; j++){
		if(maxValue < viterbiTemp[j][featureNum-1]){
			maxValue = viterbiTemp[j][featureNum-1];
			idx = j;
		}
	}
	ptrYOpt[featureNum-1] = idx;
	for(int i = featureNum-1; i > 0; i--){
		idx = viterbiTrack[idx][i];
		ptrYOpt[i-1] = idx;
	}
	//cout << "End Viterbi function.\n";
}




