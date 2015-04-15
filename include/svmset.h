#ifndef SVMSET_H
#define SVMSET_H

#include <string>
#include <vector>
#include <fstream>
#include <utility>
using namespace std;

typedef pair<string,double*> svec;

class Svmset{
	public:
		Svmset();
		Svmset(const Svmset& s);
		Svmset(char* argvx);
		Svmset(string file);
		~Svmset();
		bool read(char* argvx);
		bool read(string file);
		size_t size()const;
		size_t svmdim()const;
		string getFrameName(size_t idx)const;
		double* getFeature(size_t idx)const;
	private:
		//*********************
		//*  HELPER FUNCTIONS *
		//*********************
		bool parse(ifstream& in);
		void reset();
		void insertOne(const vector<double>& dvec,string name);
		//*********************
		//*  DATA MEMBERS     *
		//*********************
		size_t _svmdim;
		vector<svec>* _vsptr;

};


#endif
