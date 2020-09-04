

#include "Tuple.hpp"

void Tuple::createlist(string TableName)
{
    CatalogManager cm;
    list = cm.tableInformation(TableName);
}

void Tuple::createPage(string TableName)
{
	BufferManager bm;
	page.tableName = TableName;
	bm.allocatePage(page);
}

void Tuple::convertToRawData()
{
	assert(page.pageType != PageType::UndefinedPage);
	char *cursor = page.pageData;
	cursor += 2 * sizeof(PageIndexType);

	for(unsigned int i = 0; i < list.size(); i++)
	{
		list[i].convertToRawData();
		memcpy(cursor, list[i].rawdata, list[i].getKeyDataLength());
		cursor += list[i].getKeyDataLength();
		assert(list[i].type != AttributeType::UNDEFINED);
	}
}

void Tuple::ParseFromRawData()
{
	assert(page.pageType != PageType::UndefinedPage);
	char *cursor = page.pageData;
	cursor += 2 * sizeof(PageIndexType);

	for(unsigned int i = 0; i < list.size(); i++)
	{
		memcpy(list[i].rawdata, cursor, list[i].getKeyDataLength());
		cursor += list[i].getKeyDataLength();
		list[i].parseFromRawData();
		assert(list[i].type != AttributeType::UNDEFINED);
	}
}
