#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include <iomanip>

using namespace std;
void myUsage(){cerr<<" USAGE: [feature file] [label file] [label map] <out file>"<<endl;}
typedef vector<double> feature;
typedef pair<string,feature> frame;
bool readFeature(ifstream& inf,vector<frame>* out,vector<size_t>& l_frame);
bool readLabel(ifstream& inf,vector<size_t>* out,const map<string,size_t>& lMap);
bool readMap(ifstream& inf,map<string,size_t>& lMap);
bool parseName(string& str,string& name,char dilimeter=' ');
bool parseFeature(string& str,feature& fvec,char dilimeter=' ');
void write(ofstream& out,vector<frame>* f_ptr,vector<size_t>* l_ptr,const vector<size_t>& fsize);

void svmFeature(ofstream& out,vector<frame>* f_ptr,vector<size_t>* l_ptr,double** ob,int** st,size_t l);
string getFrameName(string str);
void init(double** ob,int** st,size_t l,size_t v);
void mySum(double* val,const feature& v);
void oneFrameOut(ofstream& out,double** ob,int** st,size_t l,size_t v,string name,const vector<size_t>& lab);

int main(int argc,char** argv){
if(argc<4){myUsage();return 1;}

cout<<"reading features...";
ifstream input(argv[1]);
if(!input){cerr<<" ERROR: unable to open feature file: [ "<<argv[1]<<" ]\n ";myUsage();return 1;}
vector<frame>* f_ptr=new vector<frame>;
vector<size_t> l_frame;
if(!readFeature(input,f_ptr,l_frame)){cerr<<"ERROR: feature format not recognized \n";return 1;}
input.close();
cout<<"done!"<<endl;

cout<<"reading labels...";
input.open(argv[3]);
if(!input){cerr<<" ERROR: unable to open label map: [ "<<argv[3]<<" ]\n ";myUsage();return 1;}
map<string,size_t> lmap;
if(!readMap(input,lmap)){cerr<<"ERROR: label map not recognized \n";return 1;}
input.close();

input.open(argv[2]);
if(!input){cerr<<" ERROR: unable to open label file: [ "<<argv[2]<<" ]\n ";myUsage();return 1;}
vector<size_t>* l_ptr=new vector<size_t>;
if(!readLabel(input,l_ptr,lmap)){cerr<<"ERROR: label format not recognized \n";return 1;}
input.close();
cout<<"done!"<<endl;

cout<<"parsing complete!"<<endl;

<<<<<<< HEAD
cout<<"begin to write file..."<<endl;
=======
cout<<"begin to write file...";
>>>>>>> FETCH_HEAD
ofstream out(argv[4]);
if(!out){cerr<<" ERROR: unable to write file: [ "<<argv[4]<<" ]\n ";}
write(out,f_ptr,l_ptr,l_frame);
out.close();
cout<<"done!"<<endl;
/*
cout<<"allocating memeory...";
double** observation=new double*[lmap.size()];
int** state=new int*[lmap.size()];
for(size_t t=0;t<lmap.size();++t){
	observation[t]=new double[(f_ptr->at(0)).second.size()];
	state[t]=new int[lmap.size()];
}
cout<<"done!"<<endl;
cout<<"generating svm features...";
ofstream svm;
if(argc==4)
svm.open("svm.csv");
else
svm.open(argv[4]);
svmFeature(svm,f_ptr,l_ptr,observation,state,lmap.size());
svm.close();
cout<<"done!"<<endl;

for(size_t t=0;t<lmap.size();++t){
	delete [] observation[t];
	delete [] state[t];
}
delete [] observation;
delete [] state;
*/
delete f_ptr;
delete l_ptr;
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
bool readLabel(ifstream& inf, vector<size_t>* out, const map<string,size_t>& lMap){
	string str,hold;
	size_t line;
	map<string,size_t>::const_iterator it;
	for(line=0;getline(inf,str);++line){
		if(!parseName(str,hold,','))
				return false;
		if(!parseName(str,hold,' '))
				return false;
		it=lMap.find(hold);
		if(it==lMap.end())
				return false;
		out->push_back(it->second);
	}
	return true;
}

bool readMap(ifstream& inf,map<string,size_t>& lMap){
	map<string,size_t>::iterator it;
	size_t begin,end,line,index;
	string str,hold,num,tmp;
	for(line=0;getline(inf,str);++line){
		if(!parseName(str,hold,'\t'))
				return false;
		if(!parseName(str,num,' '))
				return false;
				num=num.substr(num.find_first_not_of('\t'));
		index=atoi(num.c_str());
		it=lMap.find(hold);
		if(it==lMap.end())
				lMap.insert(pair<string,size_t>(hold,index));
		else
				return false;
	}
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

void write(ofstream& out,vector<frame>* f_ptr,vector<size_t>* l_ptr,const vector<size_t>& fsize){
	assert(f_ptr->size()==l_ptr->size());
	string part1,part2,hold;
	int acc=0;
	for(size_t t=0;t<fsize.size();++t){
			hold=(f_ptr->at(acc)).first;
			parseName(hold,part1,'_');
			parseName(hold,part2,'_');
			out<<"<"<<part1+part2<<"> "<<fsize[t]<<"\n";
	for(size_t k=0;k<fsize[t];++k){
			out<<"["<<l_ptr->at(acc+k);
		for(size_t l=0;l<(f_ptr->at(acc+k)).second.size();++l)
				out<<" "<<fixed<<setprecision(6)<<(f_ptr->at(acc+k)).second.at(l);
				out<<"]\n";
	}
	acc+=fsize[t];
	}
}

void svmFeature(ofstream& out,vector<frame>* f_ptr,vector<size_t>* l_ptr,double** ob,int** st,size_t l){
	string c_name="",name;
	int c_label=-1,label;
	vector<size_t> labelOneFrame;
	size_t v=(f_ptr->at(0)).second.size();
	for(size_t t=0;t<f_ptr->size();++t){
		name=getFrameName((f_ptr->at(t)).first);
		label=l_ptr->at(t);
		if(c_name.compare(name)!=0){
			if(c_label>=0){
				oneFrameOut(out,ob,st,l,v,c_name,labelOneFrame);
			}
			labelOneFrame.clear();
			init(ob,st,l,v);
			c_name=name;
			c_label=-1;
		}
		mySum(ob[label],(f_ptr->at(t)).second);
		labelOneFrame.push_back(label);
		if(c_label>=0)
				st[c_label][label]++;
		c_label=label;
	}
	oneFrameOut(out,ob,st,l,v,c_name,labelOneFrame);
}

void oneFrameOut(ofstream& out,double** ob,int** st,size_t l,size_t v,string name,const vector<size_t>& lab){
/*		size_t dim=0;
	for(size_t t=0;t<l;++t){
		for(size_t k=0;k<v;++k){
			out<<name<<"_"<<dim<<","<<fixed<<setprecision(7)<<ob[t][k]<<endl;
			dim++;
		}
	}
	for(size_t t=0;t<l;++t){
		for(size_t k=0;k<l;++k){
			out<<name<<"_"<<dim<<","<<st[t][k]<<endl;
			dim++;
		}
	}*/

	out<<"["<<name<<"] "<<lab.size()<<endl;
	for(size_t t=0;t<lab.size();++t)
			out<<" "<<lab[t];
	out<<endl;
	for(size_t t=0;t<l;++t){
		for(size_t k=0;k<v;++k)
				out<<fixed<<setprecision(7)<<ob[t][k]<<endl;
	}
	for(size_t t=0;t<l;++t){
		for(size_t k=0;k<v;++k)
				out<<st[t][k]<<endl;
	}
}

void init(double** ob,int** st,size_t l,size_t v){
	for(size_t t=0;t<l;++t){
			for(size_t k=0;k<v;++k)
					ob[t][k]=0;
			for(size_t k=0;k<l;++k)
					st[t][k]=0;
	}
}

string getFrameName(string str){
	string n1,n2;
	parseName(str,n1,'_');
	parseName(str,n2,'_');
	return n1+n2;
}

void mySum(double* val,const feature& v){
	for(size_t t=0;t<v.size();++t)
			val[t]+=v[t];
}
