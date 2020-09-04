
#ifndef SQLcommand_hpp
#define SQLcommand_hpp

#include <cstdio>
#include <string>
#include "TableInfo.hpp"
#include "ConditionContent.hpp"
using namespace std;

class SQLcommand
{
public:
    int commandType;
    //0-错误命令, 参数errorInfo
    //1-Create table, 参数tableInfo
    //2-Drop table, 参数tableName
    //3-Create index, 参数indexName, tableName, attrName
    //4-Drop index, 参数indexName
    //5-Select, 参数tableName, condNum, condCont[1..condNum]
    //6-Insert, 参数tableName, attrNum, condCont[1..attrNum]
    //7-Delete, 参数tableName, condNum, condCont[1..condNum]
    //8-Quit
    //9-Execfile, 参数fileName
    string tableName;
    string attrName;
    string indexName;
    TableInfo tableInfo;
    int condNum;
    int attrNum;
    ConditionContent condCont[50];
    string fileName;
    void reset() {tableName=""; attrName=""; indexName=""; tableInfo.reset(); condNum=0; attrNum=0; fileName="";}
};

#endif /* SQLcommand_hpp */
