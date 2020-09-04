
#ifndef Table_hpp
#define Table_hpp

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <utility>
#include "BufferManager.hpp"
#include "Attribute.hpp"
#include "RecordPage.hpp"
#include "Tuple.hpp"

class Table{
public:
	Table(string);
	~Table();

	PageIndexType insertTuple(vector<Attribute>);				//参数：插入的Tuple的Attribute组成的vector 返回：插入page的PageIndex
	void deleteTuple(PageIndexType);							//参数：所删除的Page的页号

	vector<PageIndexType> scanEqual(int , Attribute);			//参数：第几个属性，所需比较的参数组成的Attribute
	vector<PageIndexType> scanNotEqual(int, Attribute);			//返回：由页号所组成的vector
	vector<PageIndexType> scanLess(int , Attribute);
	vector<PageIndexType> scanGreater(int , Attribute);
	vector<PageIndexType> scanLessEqual(int , Attribute);
	vector<PageIndexType> scanGreaterEqual(int , Attribute);

	vector<PageIndexType> getAll();								//参数：无	返回：所有在这个表中的page的页号
	vector<pair<Attribute, PageIndexType>> getAll(int);			//参数：第几个属性 返回：由Attribute和页号所组成的pair的vector
	vector<Attribute> getTupleAtPage(PageIndexType);			//参数：页号 返回：这一页的所有的Attribute所组成的vector

    void printinfo(PageIndexType); 

	PageIndexType head;
	string TableName;

};

#endif /* Table_hpp */
