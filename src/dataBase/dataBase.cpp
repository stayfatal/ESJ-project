#include "dataBase.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <stdio.h>
#include <cstdio>

using std::string;
using std::vector;

DataBase::DataBase(string fileName){ this->fileName=fileName; this->path=folder+fileName; }

void DataBase::editDb(int line,string newLine,bool overwrite){
    std::ifstream in(path);
    vector<string> vec;
    if(in.is_open()){
        string temp;
        while(std::getline(in,temp)){
            vec.push_back(temp);
        }
        if(overwrite){
            vec[line]=newLine;
        }else{
            vec[line]+=newLine;
        }
        in.close();
    }

    std::ofstream out(path);
    if(out.is_open()){
        for(auto i:vec){
            out << i << std::endl;
        }
        out.close();
    }
}

int DataBase::findLine(string strLine){
    std::ifstream in(path);
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

RegistrationDataBase::RegistrationDataBase(string fileName) : DataBase(fileName){}

int RegistrationDataBase::findId(int64_t id){
    std::ifstream in(path);
    if(in.is_open()){
        string temp;
        string strId=std::to_string(id);
        int curLine=0;
        while(std::getline(in,temp)){
            curLine++;
            size_t iter=0;
            int counter=0;
            while(counter<3 and iter!=std::string::npos){
                iter=temp.find(":",iter+1);
                counter++;
            }
            if(strId==temp.substr(iter+1,temp.length())){
                in.close();
                return curLine;
            }
        }
        in.close();
    }
    return -1;
}

bool RegistrationDataBase::registration(string name,int64_t chatId){
    if(findLine(name)!=-1){
        editDb(findLine(name)-1,":"+std::to_string(chatId),false);
        return true;
    }else{
        return false;
    }
}

bool RegistrationDataBase::isRegistered(int64_t chatId){
    if(findId(chatId)!=-1){
        return true;
    }else{
        return false;
    }
}