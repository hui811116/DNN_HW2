#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <cmath>

using namespace std;

enum ERROR{
	MISSINGARG,
	FALSEARG,
	OPENFAIL,
	WRITEFAIL,
	WRONGFORMAT,
	COMPLETE
};
void myUsage(){
	cerr<<" USAGE: [feature file] [output file] (--gender)"<<endl;
}
void errorMsg(ERROR msg){
	switch(msg){
		case MISSINGARG:
			cerr<<"ERROR: Missing argument."<<endl;
			myUsage();
		break;
		case FALSEARG:
			cerr<<"ERROR: Unknown arguments."<<endl;
			myUsage();
		break;
		case OPENFAIL:
		case WRITEFAIL:
			cerr<<"ERROR: Fail when opening file"<<endl;
		break;
		case WRONGFORMAT:
			cerr<<"ERROR: File format not recognized"<<endl;
		break;
		case COMPLETE:
		default:
		break;
	}
}
typedef vector<double> feature;
typedef pair<string,feature> frame;

bool readFeature(ifstream& inf,vector<frame>* out,vector<size_t>& l_frame);

bool parseName(string& str,string& name,char dilimeter=' ');
bool parseFeature(string& str,feature& fvec,char dilimeter=' ');
string getFrameName(string str);

bool option(char* op);
void normalize(vector<frame>* fin,vector<frame>* fout);
void separateGender(vector<frame>* fin,vector<frame>* fout);
ERROR write(vector<frame>* fin,char* op);

int main(int argc,char** argv){
if(argc<3){errorMsg(MISSINGARG);return 1;}
if(argc>4){errorMsg(FALSEARG);return 1;}
bool gen=false;
if(argc==4){
if(option(argv[3]))
	gen=true;
else
	errorMsg(FALSEARG);
}
ifstream file(argv[1]);
if(!file){errorMsg(OPENFAIL);return 1;}
vector<frame>* f_vec=new vector<frame>;
vector<size_t> frame_num;
if(!readFeature(file,f_vec,frame_num)){errorMsg(WRONGFORMAT);return 1;}
//different type
	vector<frame>* norm=new vector<frame>;
//1.all
	if(gen)
		separateGender(f_vec,norm);
	else
		normalize(f_vec,norm);
//2.gender
//done
	errorMsg(write(norm,argv[2]));

f_vec->clear();
delete f_vec;
norm->clear();
delete norm;
return 0;
}

bool readFeature(ifstream& inf, vector<frame>* out,vector<size_t>& l_frame){
	string str,hold,part1,part2,c_hold;
	feature fvec;
	size_t line,frame_size=0;
	for(line=0;getline(inf,str);++line){
		if(!parseName(str,hold,' '))
				return false;
		if(!parseFeature(str,fvec))
				return false;
		out->push_back(frame(hold,fvec));
		parseName(hold,part1,'_');
		parseName(hold,part2,'_');
		hold=part1+part2;
		if(c_hold.compare(hold)!=0){
			if(frame_size!=0)
			l_frame.push_back(frame_size);
			frame_size=0;
			c_hold=hold;
		}
		frame_size++;
	}
	if(frame_size!=0)
	l_frame.push_back(frame_size);
	return true;
}

bool parseName(string& str,string& name,char dilimeter){
	size_t begin=str.find_first_not_of(' '),end=str.find_first_of(dilimeter,begin);
	if(end!=string::npos)
		name=str.substr(begin,end-begin);
	else
		name=str.substr(begin,str.length());
	if(begin==str.find_last_not_of(' '))
			name=str[begin];
	begin=str.find_first_not_of(dilimeter,end);
	if(begin!=string::npos)
		str=str.substr(begin);
	else
		str=str.substr(str.find_first_not_of(' '));
	return true;
}

bool parseFeature(string& str,feature& fvec,char dilimeter){
	fvec.clear();
	size_t begin=str.find_first_not_of(' '),end;
	string hold;
	while(begin!=string::npos){
		end=str.find_first_of(dilimeter,begin);
		if(end==string::npos)
				hold=str.substr(begin,str.find_last_not_of(' ')-begin);
		else
				hold=str.substr(begin,end-begin);
		if(!hold.empty())
				fvec.push_back(atof(hold.c_str()));
		else{
				if(begin==str.find_last_not_of(' ')){
						hold=str.substr(begin,1);
						fvec.push_back(atof(hold.c_str()));
				}
				else
				return false;
		}
		begin=str.find_first_not_of(dilimeter,end);
	}
	return true;
}
string getFrameName(string str){
	string n1,n2;
	parseName(str,n1,'_');
	parseName(str,n2,'_');
	return n1+n2;
}
bool option(char* op){
	string str(op);
	string hold;
	hold=str.substr(str.find_first_of('-'));
	return hold.compare(0,2,"-g");
}
void separateGender(vector<frame>* fin,vector<frame>* fout){
	vector<frame>* male=new vector<frame>;
	vector<frame>* female=new vector<frame>;
	vector<frame>* nor_male=new vector<frame>;
	vector<frame>* nor_female=new vector<frame>;
	string check;
	bool formatcheck=true;
// how to separate gender
	for(size_t t=0;t<fin->size();++t){
		check=(fin->at(t)).first;
		if(check.compare(0,1,"f"))
			female->push_back(fin->at(t));
		else if(check.compare(0,1,"m"))
			male->push_back(fin->at(t));
		else{formatcheck=false;}
	}
//
	if(!formatcheck){cerr<<"WARNING: Gender separator found unknown format"<<endl;}
	normalize(female,nor_female);
	female->clear();
	delete female;
	for(size_t t=0;t<nor_female->size();++t)
		fout->push_back(nor_female->at(t));
	nor_female->clear();
	delete nor_female;
	normalize(male,nor_male);
	male->clear();
	delete male;
	for(size_t t=0;t<nor_male->size();++t)
		fout->push_back(nor_male->at(t));
	nor_male->clear();
	delete nor_male;

}
void normalize(vector<frame>* fin,vector<frame>* fout){
	if(fin->size()!=0){
		int dim=(fin->at(0)).second.size();
		double* sum=new double[dim];
		double* var=new double[dim];
		double* std=new double[dim];
		for(size_t t=0;t<dim;++t){
			sum[t]=0.00;
			var[t]=0.00;
		}
		for(size_t t=0;t<fin->size();++t){
			for(size_t k=0;k<dim;++k)
				sum[k]+=(fin->at(t)).second.at(k);
		}
		for(size_t t=0;t<dim;++t){
			sum[t]/=(double)fin->size(); //mean
		}
		for(size_t t=0;t<fin->size();++t){
			for(size_t k=0;k<dim;++k)
				var[k]+=pow((fin->at(t)).second.at(k)-sum[k],2.0);
		}
		for(size_t t=0;t<dim;++t){
			var[t]/=(double)fin->size(); //varance
			std[t]=sqrt(var[t]);
		}
		feature normf;
		for(size_t t=0;t<fin->size();++t){
			normf.clear();
			for(size_t k=0;k<dim;++k){
				normf.push_back(((fin->at(t)).second.at(k)-sum[k])/(double)std[k]); // normalize to 1
			}
			fout->push_back(frame((fin->at(t)).first,normf));
		}
		delete [] sum;
		delete [] var;
		delete [] std;
	}
	else{
		cerr<<"WARNING:recognized no feature"<<endl;
	}
}
ERROR write(vector<frame>* fin,char* op){
	ofstream out(op);
	if(!out){return OPENFAIL;}
	for(size_t t=0;t<fin->size();++t){
		out<<(fin->at(t)).first;
		out<<fixed<<setprecision(7);
		for(size_t k=0;k<(fin->at(t)).second.size();++k)
			out<<" "<<(fin->at(t)).second.at(k);
		out<<endl;
	}
	return COMPLETE;
}
