#include <iostream>
#include <vector>
#include <device_matrix.h>

#include <device_arithmetic.h>
#include <device_math.h>

using namespace std;

typedef device_matrix<float> mat;

template <typename T>
void randomInit(device_matrix<T>& m) {
  T* h_data = new T [m.size()];
  for (int i=0; i<m.size(); ++i)
    h_data[i] = rand() / (T) RAND_MAX;
  cudaMemcpy(m.getData(), h_data, m.size() * sizeof(T), cudaMemcpyHostToDevice);
  delete [] h_data;
}

int main(){

mat A(5,8),B(8,5);
randomInit(A);
randomInit(B);

printf("A=\n");
A.print();
printf("B=\n");
B.print();

printf("A * B= \n"); (A*B).print();

//testing element-wise operation

mat C(8,1), D(8,1);
randomInit(C);
randomInit(D);

printf("C=\n");
C.print();
printf("D=\n");
D.print();

printf("C & D= \n"); (C&D).print();

return 0;
}
