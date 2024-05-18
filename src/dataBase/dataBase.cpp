#include "dataBase.h"
#include <fstream>
#include <vector>
#include <iterator>
using std::string;
using std::vector;

DataBase::DataBase(string fileName){ this->fileName=fileName;}

void DataBase::editDb(int line,string newLine,bool overwrite){
    std::ifstream in(fileName);
    vector<string> vec;
    if(in.is_open()){
        string temp;
        while(std::getline(in,temp)){
            vec.push_back(temp);
        }
        if(overwrite){
            vec[line]=newLine;
        }else{
            vec[line]+=":"+newLine;
        }
        in.close();
    }

    std::ofstream out(fileName);
    if(out.is_open()){
        for(auto i:vec){
            out << i;
        }
        out.close();
    }
}

int DataBase::findLine(string strLine){
    std::ifstream in(fileName);
    if(in.is_open()){
        std::string temp;
        int curLine=0;
        while(std::getline(in,temp)){
            curLine++;
            if(strLine==temp){
                in.close();
                return curLine;
            }
        }
        in.close(); 
    }
    return -1;
}