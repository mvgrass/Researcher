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
	cout<<"Input path "<<endl;
	try
	{
		getline(cin,str);
		path mypath(str);
		if(str == "/help")
		{
			cout<<"1) Input path to directory, which you want to scan."<<endl
				<<"2) Programm will try to find *your directory*/file researcher/statistic.tsv"<<endl
				<<"3.1) If *your directory*/file researcher/statistic.tsv does not exist - programm will create it and write into information (full path, name, size, hash adler-32) about all files in *your directory*"<<endl
				<<"3.2) If *your directory*/file researcher/statistic.tsv exists - programm will scan directory, compare with old information, write into console about changed, removed or displaced files and then update *your directory*/file researcher/statistic.tsv"<<endl<<endl;
			getDirectory(str);
		}
		else
			if(!exists(mypath))
			throw  "Error: try again!";
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

void findChanges(fstream & file, vector <string> & vec)
{
	string temp;
	while(!file.eof())
	{
		getline(file, temp);
		int k =0;
		string dir = "";
		string adlerHash = "";
		for(int i = 0; i<temp.size(); i++)
		{
			if(temp[i]=='\t')
			{
				k++;
				continue;
			}
			switch(k)
			{
			case 0:
				dir = dir + temp[i];
				break;
			case 3:
				adlerHash = adlerHash + temp[i];
				break;
			default:
				break;
			}
		}
		if(dir=="Path")
			continue;
		if(dir==""||adlerHash=="")
		{
			break;
		}
		vec.push_back(dir);
		if(!exists(dir))
		{
			cout<<"File was removed or displaced into another directory: "<<dir<<endl;
		}
		path mp(dir);
		if(exists(dir)&&(toHex(adler(mp.string()+to_string(last_write_time(mp))))!=adlerHash))
		{
			cout<<"File was changed: "<<dir<<endl;
		}
		
	}
}

void writeChanges(fstream & file, const vector <string> & vec, string & directory)
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
			writeChanges(file,vec,temp);
		}
		else
		{
			bool existence = false;
			for(int i = 0; i<vec.size();i++)
			{
				if(itr->path().string() == vec[i])
				{
					existence = true;
					break;
				}
			}
			if(!existence)
			{
				cout<<"File was created or displaced from another directory: "<<itr->path().string()<<endl;
			}
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
	if (!exists(str+"\\file researcher\\statistic.tsv"))
	{
		fstream file(str+"\\file researcher\\statistic.tsv",ios_base::out);
		file << "Path" <<"\t"<<"File name"<<"\t"<<"File size"<<"\t"<<"Hash"<<"\n";
		researchDirectory(str,file);
		file.close();
		cout<<endl<<"File was created and placed into: "<<str<<"\\file researcher\\statistic.tsv"<<endl;
		cin.get();
		return 0;
	}
	else 
	{
		vector <string> myvec;
		fstream file(str+"\\file researcher\\statistic.tsv",ios_base::in);
		findChanges(file,myvec);		
		file.close();
		file.open(str+"\\file researcher\\statistic.tsv",ios_base::out);
		file << "Path" <<"\t"<<"File name"<<"\t"<<"File size"<<"\t"<<"Hash"<<"\n";
		writeChanges(file,myvec,str);
		file.close();
		cout<<endl<<"File was updated: "<<endl<<str<<"\\file researcher\\statistic.tsv"<<endl;
		cin.get();
		return 0;
	}
}