#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

unsigned long int adler(const string & str)
{
	unsigned int s1 = 1;
	unsigned int s2 = 0;
	for (unsigned int i = 0; i<str.size(); i++)
	{
		s1 = (s1 + str[i])%65521;
		s2 = (s2+ s1)%65521;
	}
	return (s2 << 16) + s1;
}

string toHex(unsigned long int a)
{
	stringstream ss;
	string s;
	ss <<hex<< a;
	ss >> s;
	return s;
}

void getDirectory (string & str)
{
	cout<<"Введите путь "<<endl;
	try
	{
		getline(cin,str);
		path mypath(str);
		if(!exists(mypath))
			throw "Неправидьный ввод, попробуйте еще раз";
	}
	catch (const char * error)
	{
		cout<<error<<endl<<endl;
		getDirectory(str);
	}
}

void setDirectory(const string & str)
{
	if(!exists(str+"\file researcher"))
		create_directory(str+"\\file researcher");
}

void researchDirectory(string & directory, fstream & file)
{
	path mypath(directory);
	directory_iterator end_itr;
	for(directory_iterator itr(mypath);itr!=end_itr;++itr)
	{
		if(itr->path().string()==(directory+"\\file researcher"))
		{
			continue;
		}
		if(is_directory(itr->status()))
		{
			string temp = itr->path().string();
			researchDirectory(temp, file);
		}
		else
		{
			file << itr->path().string() <<"\t"<<itr->path().filename()<<"\t"<<file_size(itr->path())<<"\t"<<toHex(adler(itr->path().string()+to_string(last_write_time(itr->path()))))<<"\n";
		}
	}
}


int main()
{
	SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
	cout<<"Change font to Lucida Console"<<endl;
	string str;
	getDirectory(str);
	setDirectory(str);
	fstream file(str+"\\file researcher\\statistic.tsv",ios_base::out);
	file << "Path" <<"\t"<<"File name"<<"\t"<<"File size"<<"\t"<<"Hash"<<"\n";
	researchDirectory(str,file);
	file.close();
	cout<<endl<<"Файл был создан, находится: "<<str<<"\\file researcher\\statistic.tsv"<<endl;
	cin.get();
	return 0;
}