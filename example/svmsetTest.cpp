#include "svmset.h"
#include <iostream>

using namespace std;

int main(int argc,char** argv){
	Svmset s1(argv[1]);
	cout<<"number of features:"<<s1.size()<<endl;
	cout<<"dimension of features:"<<s1.svmdim()<<endl;
	return 0;
}
