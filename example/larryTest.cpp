#include <random>
#include "myAlgorithm.h"

template <typename T>
void randomInit(device_matrix<T>& m) {
  T* h_data = new T [m.size()];
  for (int i=0; i<m.size(); ++i)
    h_data[i] = rand() / (T) RAND_MAX;
  cudaMemcpy(m.getData(), h_data, m.size() * sizeof(T), cudaMemcpyHostToDevice);
  delete [] h_data;
}

// Simple test
int main(int argc, char** argv){
	mat w(10, 1);
	randomInit(w);
	mat x(1, 5, 0.5);
	randomInit(x);
	mat yOpt(1, 5, 2);
	
	viterbi(yOpt, w, x, 1, 2);
	yOpt.print();
	
	return 0;
}
