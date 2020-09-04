

#ifndef CatalogManager_hpp
#define CatalogManager_hpp

#include <cstdio>
#include <vector>
#include "BufferManager.hpp"
#include "TableInfo.hpp"
#include "Attribute.hpp"
#include <string>
using namespace std;

class CatalogManager
{
public:
    CatalogManager(){};
    ~CatalogManager(){};
    //TableInfo的条目从0开始
    //insertTable可以新建表并判断成功与否并会输出反馈信息
    //如果有primaryKey,会自动在上面建立索引
    void insertTable(TableInfo);        //参数：表名、属性数目、主键、每个属性的类型、每个属性是否unique
    //dropTable可以删除表并判断成功与否并会输出反馈信息
    //会删除这张表上所有的索引
    void dropTable(string);             //参数：表名
    //这个应该api用不上？可以返回一张表是否存在
    bool tableExisted(string);          //参数：表名
    //attrType可以返回attribute的类型
    int attrType(string, string);       //参数：表名、列名；返回值：0-不存在，1-int，2-float，>2-char(返回值-2)（例如5表示char(3)）
    //attrUnique可以返回attribute是否unique
    bool attrUnique(string, string);    //参数：表名、列名
    //primaryKey可以返回一张表上的primary key，如果没有primary key，则返回""
    string primaryKey(string);          //参数：表名；返回值：列名
    //这个应该api用不上？可以返回一个索引名是否已经存在
    bool indexExisted(string);          //参数：索引名
    //这个应该api用不上？可以返回一张表上的一个属性是否存在
    bool attrExisted(string, string);   //参数：表名，列名
    //这个应该api用不上？可以返回某个索引名在哪张表的哪个属性上
    string indexLocation(string);       //参数：索引名；返回值：表名+列名
    //insertIndex可以创建索引并判断是否成功并返回true或false
    bool insertIndex(string, string, string);   //参数：表名，列名，索引名
    //deleteIndex可以删除索引并判断是否成功
    void deleteIndex(string);           //参数：索引名
    //indexNum可以返回某张表的某个属性上创建了多少个索引
    int indexNum(string, string);       //参数：表名，列名；返回值：该属性名上的索引个数
    //tableInformation可以返回一个包含各个属性的vector
    vector<Attribute> tableInformation(string);  //参数：表名
};

#endif /* CatalogManager_hpp */
