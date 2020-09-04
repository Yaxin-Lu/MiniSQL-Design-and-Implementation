

#include "Table.hpp"

BufferManager  bm;

Table::Table(string name)
{
	TableName = name;
	RecordPage page;
	page.pageIndex = 1;
	page.tableName = name;
	bm.readPage(page);
	head = page.readnext();
	if(head == 0)
        head = UNDEFINEED_PAGE_NUM;


}

Table::~Table()
{
	RecordPage page;
	page.pageIndex = 1;
	page.tableName = TableName;
	bm.readPage(page);
	page.writenext(head);
	bm.writePage(page);
}

PageIndexType Table::insertTuple(vector<Attribute> list)
{
	Tuple tuple;
    tuple.list = list;
	tuple.createPage(TableName);

	tuple.page.writebefore(1);
	tuple.page.writenext(head);
	if(head != UNDEFINEED_PAGE_NUM)
	{
		RecordPage page;
		page.pageIndex = head;
		page.tableName = TableName;
		bm.readPage(page);
		page.writebefore(tuple.page.pageIndex);
        bm.writePage(page);
	}
	head = tuple.page.pageIndex;
	tuple.convertToRawData();
	bm.writePage(tuple.page);

	return tuple.page.pageIndex;
}

void Table::deleteTuple(PageIndexType index)
{
	RecordPage page;
	page.pageIndex = index;
	page.tableName = TableName;
	bm.readPage(page);

	PageIndexType next = page.readnext();
	PageIndexType before = page.readbefore();
    if (before == 1) {
        head = next;
    }
	if(next == UNDEFINEED_PAGE_NUM)
	{
		RecordPage beforepage;
		beforepage.pageIndex = before;
		beforepage.tableName = TableName;
		bm.readPage(beforepage);
		beforepage.writenext(UNDEFINEED_PAGE_NUM);
		bm.writePage(beforepage);
	}
	else
	{
		RecordPage nextpage;
		nextpage.pageIndex = next;
		nextpage.tableName = TableName;
		bm.readPage(nextpage);
		RecordPage beforepage;
		beforepage.pageIndex = before;
		beforepage.tableName = TableName;
		bm.readPage(beforepage);

		beforepage.writenext(nextpage.pageIndex);
		nextpage.writebefore(beforepage.pageIndex);

		bm.writePage(beforepage);
		bm.writePage(nextpage);
	}

	bm.deallocatePage(page);

}

vector<PageIndexType> Table::scanEqual(int attrnum, Attribute attribute)
{
	vector<PageIndexType> result;
	PageIndexType i = head;

	while(i != UNDEFINEED_PAGE_NUM)
	{
		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);

		Tuple tuple;
		tuple.page = page;
		tuple.createlist(TableName);
		tuple.ParseFromRawData();

		if(tuple.list[attrnum] == attribute)
			result.push_back(i);

		i = page.readnext();
	}

	return result;
}

vector<PageIndexType> Table::scanNotEqual(int attrnum, Attribute attribute)
{
	vector<PageIndexType> result;
	PageIndexType i = head;

	while (i != UNDEFINEED_PAGE_NUM)
	{
		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);

		Tuple tuple;
		tuple.page = page;
		tuple.createlist(TableName);
		tuple.ParseFromRawData();

		if (tuple.list[attrnum] != attribute)
			result.push_back(i);

		i = page.readnext();
	}

	return result;
}

vector<PageIndexType> Table::scanLess(int attrnum, Attribute attribute)
{
	vector<PageIndexType> result;
	PageIndexType i = head;
	while(i != UNDEFINEED_PAGE_NUM)
	{
		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);

		Tuple tuple;
		tuple.page = page;
		tuple.createlist(TableName);
		tuple.ParseFromRawData();
		if(tuple.list[attrnum] < attribute)
			result.push_back(i);

		i = page.readnext();
	}

	return result;
}

vector<PageIndexType> Table::scanGreater(int attrnum, Attribute attribute)
{
	vector<PageIndexType> result;
	PageIndexType i = head;
	while(i != UNDEFINEED_PAGE_NUM)
	{
		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);

		Tuple tuple;
		tuple.page = page;
		tuple.createlist(TableName);
		tuple.ParseFromRawData();
		if(tuple.list[attrnum] > attribute)
			result.push_back(i);

		i = page.readnext();
	}

	return result;
}

vector<PageIndexType> Table::scanLessEqual(int attrnum, Attribute attribute)
{
	vector<PageIndexType> result;
	PageIndexType i = head;
	while(i != UNDEFINEED_PAGE_NUM)
	{

		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);

		Tuple tuple;
		tuple.page = page;
		tuple.createlist(TableName);
		tuple.ParseFromRawData();
		if(tuple.list[attrnum] <= attribute)
			result.push_back(i);

		i = page.readnext();
	}

	return result;
}

vector<PageIndexType> Table::scanGreaterEqual(int attrnum, Attribute attribute)
{
	vector<PageIndexType> result;
	PageIndexType i = head;
	while(i != UNDEFINEED_PAGE_NUM)
	{
		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);

		Tuple tuple;
		tuple.page = page;
		tuple.createlist(TableName);
		tuple.ParseFromRawData();
		if(tuple.list[attrnum] >= attribute)
			result.push_back(i);

		i = page.readnext();
	}

	return result;
}

vector<PageIndexType> Table::getAll()
{
	vector<PageIndexType> result;
	PageIndexType i = head;
	while (i != UNDEFINEED_PAGE_NUM)
	{
		result.push_back(i);

		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);		
		i = page.readnext();
	}

	return result;
}

vector<pair<Attribute, PageIndexType>> Table::getAll(int attrnum)
{
	vector<pair<Attribute, PageIndexType>> result;
	PageIndexType i = head;
	while(i != UNDEFINEED_PAGE_NUM)
	{
		RecordPage page;
		page.tableName = TableName;
		page.pageIndex = i;
		bm.readPage(page);

		Tuple tuple;
		tuple.page = page;
		tuple.createlist(TableName);
		tuple.ParseFromRawData();
		pair<Attribute, PageIndexType> p;
		p.first = tuple.list[attrnum];
		p.second = i;
		result.push_back(p);

		i = page.readnext();
	}

	return result;
}

vector<Attribute> Table::getTupleAtPage(PageIndexType num)
{
	RecordPage page;
	page.tableName = TableName;
	page.pageIndex = num;
	bm.readPage(page);

	Tuple tuple;
	tuple.page = page;
	tuple.createlist(TableName);
	tuple.ParseFromRawData();

	return tuple.list;
}

void Table::printinfo(PageIndexType index)
{
    RecordPage page;
    page.pageIndex = index;
    page.tableName = TableName;
    bm.readPage(page);

    Tuple tuple;
    tuple.page = page;
    tuple.createlist(TableName);
    tuple.ParseFromRawData();

    for(int i = 0; i < tuple.list.size(); i++)
    {
        switch(tuple.list[i].type)
        {
        case AttributeType::CHAR:
            for (int j = 0; j < tuple.list[i].length; j++)
                cout << tuple.list[i].chardata[j];
            break;
        case AttributeType::FLOAT:
            printf("%.2f", tuple.list[i].floatdata);
            break;
        case AttributeType::INT:
            cout << tuple.list[i].intdata;
            break;
        case AttributeType::UNDEFINED:;
        default:
            cout << "Type error!" ;
            break;
        }
        cout << "\t\t\t";
    }
    cout << endl;

}
