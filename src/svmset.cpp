#include "svmset.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
using namespace std;


//******************************
//*		HELPER FUNCTION		   *
//******************************
string parseOne(string& str,char dilimeter=' '){
	size_t begin=str.find_first_not_of(' '),end=str.find_first_of(dilimeter,begin);
	string hold="";
	if(end==string::npos){
		end=str.find_last_not_of(' ');
		if(end==begin)
			hold=str.substr(begin,1);
		else
			hold=str.substr(begin,end-begin);
		str="";
	}
	else if(end==begin){
		begin=str.find_first_not_of(dilimeter,end);
		str=(begin==string::npos)? "" : str.substr(begin);
	}
	else{
		if(end==begin)
			hold=str.substr(begin,1);
		else
			hold=str.substr(begin,end-begin);
		begin=str.find_first_not_of(dilimeter,end);
		str=str.substr(begin);
	}
	return hold;
}
string parseFrameName(string str){
	string n1,n2;
	n1=parseOne(str,'_');
	n2=parseOne(str,'_');
	return n1+'_'+n2;
}


//******************************
//*		MEMBER FUNCTION		   *
//******************************

Svmset::Svmset(){
	_svmdim=0;
	_vsptr=new vector<svec>;
}

Svmset::Svmset(const Svmset& s){
	_svmdim=s._svmdim;
	_vsptr->operator=(*(s._vsptr));
	//this is not complete! 
	// should new double*  
}

Svmset::Svmset(char* argvx){
	_vsptr=new vector<svec>;
	_svmdim=0;
	ifstream in(argvx);
	if(!in){cerr<<"no such file!"<<endl;reset();}
	if(!parse(in)){cerr<<"Svm feature not recognized!"<<endl;reset();}	
}

Svmset::Svmset(string file){
	_vsptr=new vector<svec>;
	_svmdim=0;
	ifstream in(file.c_str());
	if(!in){cerr<<"no such file!"<<endl;reset();}
	if(!parse(in)){cerr<<"Svm feature not recognized!"<<endl;reset();}	
}
Svmset::~Svmset(){
	for(size_t t=0;t<_vsptr->size();++t)
		delete [] (_vsptr->at(t)).second;
	_vsptr->clear();
	delete _vsptr;
}
bool Svmset::read(char* argvx){
	ifstream in(argvx);
	if(!in){return false;}
	return parse(in);
}
bool Svmset::read(string file){
	ifstream in(file.c_str());
	if(!in){return false;}
	return parse(in);
}
size_t Svmset::size()const{
	return _vsptr->size();
}
size_t Svmset::svmdim()const{
	return _svmdim;
}
string Svmset::getFrameName(size_t idx)const{
	assert(idx<size());
	return (_vsptr->at(idx)).first;
}
double* Svmset::getFeature(size_t idx)const{
	assert(idx<size());
	return (_vsptr->at(idx)).second;
}
bool Svmset::parse(ifstream& in){
	string str,hold;
	string c_name="",name;
	size_t line;
	vector<double> dataset;
	for(line=0;getline(in,str);++line){
		name=parseFrameName(parseOne(str,','));
		hold=parseOne(str,' ');
		if(c_name.compare(name)!=0){
			if(line!=0){
				if(_svmdim==0)
					_svmdim=line;
				if(dataset.size()==_svmdim)
					insertOne(dataset,c_name);	
				else{
					cerr<<"feature dimension not matched"<<endl;
					return false;
				}
			}
				dataset.clear();
				c_name=name;
		}	
		dataset.push_back(atof(hold.c_str()));
	}
	if(!dataset.empty()){
		if(dataset.size()==_svmdim)
			insertOne(dataset,c_name);
		else{
			cerr<<"last feature missing some terms"<<endl;
			return false;
			}
	}
	return true;		
}
void Svmset::insertOne(const vector<double>& dvec,string name){
	double* set=new double[dvec.size()];
	for(size_t t=0;t<dvec.size();++t)
		set[t]=dvec[t];
	_vsptr->push_back(svec(name,set));
}
void Svmset::reset(){
	for(size_t t=0;t<_vsptr->size();++t)
		delete [] (_vsptr->at(t)).second;
	_vsptr->clear();
	_svmdim=0;
}

