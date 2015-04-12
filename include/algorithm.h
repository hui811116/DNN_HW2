#include<iostream>
#include<cassert>
#include<device_matrix.h>

using namespace std;
typedef device_matrix<float> mat;

void viterbi(mat& outputSequence, const mat& information, size_t stateNum);
