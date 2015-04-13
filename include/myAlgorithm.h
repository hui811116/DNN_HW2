#include<iostream>
#include<cassert>
#include<device_matrix.h>

using namespace std;
typedef device_matrix<float> mat;

void viterbi(mat& yOpt, const mat& w, const mat& x, size_t inputDim, size_t stateNum);
