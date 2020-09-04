
#include "CatalogManager.hpp"
#include "BufferManager.hpp"
#include "Page.hpp"
#include "CatalogPage.hpp"
#include "IndexCatalogPage.hpp"
#include "Attribute.hpp"
#include "TableInfo.hpp"
#include <vector>
#include <cstdio>
#include <string>
using namespace std;

void CatalogManager::insertTable(TableInfo table)
{
    CatalogPage page;
    BufferManager buffer;
    int i,j;
    string s;
    
    if (tableExisted(table.tableName))
    {
        printf("Failed to create table %s. Table already existed.\n",table.tableName.c_str());
        return;
    }

    for (i=0; i<table.attrNum-1; i++)
        for (j=i+1; j<table.attrNum; j++)
            if (table.attrName[i]==table.attrName[j])
            {
                printf("Failed to create table %s. Attributes with the same name have been found.\n",table.tableName.c_str());
                return;
            }
    
    if (table.primaryKey=="")
        table.primaryKeyLoc=-1;
    else
    {
        for (i=0; i<table.attrNum; i++)
            if (table.attrName[i]==table.primaryKey)
            {
                table.primaryKeyLoc=i;
                table.attrUnique[i]='Y';
            }
    }

    page.tableName = table.tableName;
    page.pageData[0] = (char)table.attrNum;
    page.pageData[1] = (char)table.primaryKeyLoc;
    for (i=0; i<table.attrNum; i++)
    {
        page.writeAttr(i*100+2, table.attrName[i]);
        page.writeAttr(i*100+22, table.attrType[i]);
//        page.writeAttr(i*100+42, table.attrIndex[i]);
        page.pageData[i*100+62] = table.attrUnique[i];
        *(int*)(page.pageData+(i*100+82)) = 0;      //该属性上的索引总数，初始为0
    }
    
    buffer.writePage(page);
    printf("Created table %s successfully!\n",table.tableName.c_str());
    if (table.primaryKeyLoc>=0)
    {
        printf("Trying to build index on primary key automatelly...\n");
        insertIndex(table.tableName, table.attrName[table.primaryKeyLoc], table.tableName+table.attrName[table.primaryKeyLoc]);
    }
}

void CatalogManager::dropTable(string tableName)
{
    if (!tableExisted(tableName))
    {
        printf("Table does not exist!\n");
    }
    else
    {
        BufferManager buffer;
        IndexCatalogPage indexPage;
        int n,nn,i,x,k;
        string s, attrName;
        
        indexPage.pageIndex=1;
        buffer.readPage(indexPage);
        n=*(int*)indexPage.pageData;
        nn=n;
        i=1;
        k=0;
        while (i<=n)                                //开始逐条检查，删除这张表上所有的索引
        {
            x=indexPage.readPrevDel(i);
            if (x==0)                               //如果当前条未被删除
            {
                s=indexPage.readTableName(i);
                if (s==tableName)                   //如果是这张表的索引，删掉
                {
                    k++;                            //k用来记录删除了多少条索引
                    indexPage.deleteIndex(i);
                    attrName=indexPage.readAttrName(i);
                    BufferManager bm;
                    string filePath=bm.indexFilePath(tableName, attrName);
                    if (bm.indexFileIsExist(tableName, attrName))
                        bm.deleteIndexFile(tableName, attrName);
                }
            }
            else                                    //如果当前条已被删除，则最后一条位置后移
                n++;
            
            i++;
        }
        
        indexPage.pageIndex=1;                      //记录索引信息首页的索引总数也需要改
        buffer.readPage(indexPage);
        *(int*)indexPage.pageData=nn-k;
        buffer.writePage(indexPage);
        buffer.deleteTableCatalogFile(tableName);   //删除当前表的文件
        buffer.deleteTableFile(tableName);          //删除当前表的record文件
        printf("Dropped table %s successfully!\n",tableName.c_str());
    }
}

bool CatalogManager::tableExisted(string tableName)
{
    BufferManager buffer;
    
    if (buffer.tableCatalogFileIsExist(tableName))
        return 1;
    else
        return 0;
}

bool CatalogManager::attrExisted(string tableName, string attrName)
{
    if (!tableExisted(tableName))
    {
        printf("Failed to find Attribute %s on Table %s. Table %s does not existed.", attrName.c_str(),tableName.c_str(),tableName.c_str());
        
        return 0;
    }
    else
    {
        CatalogPage page;
        BufferManager buffer;
        int num,i;
        
        page.tableName = tableName;
        buffer.readPage(page);
        num = (int)page.pageData[0];
        for (i=0; i<num; i++)
            if (page.readAttrName(i) == attrName)
                return 1;

        return 0;
    }
}

int CatalogManager::attrType(string tableName, string attrName)
{
    int i,j,num;
    string s="";
    
    CatalogPage page;
    BufferManager buffer;
    
    page.tableName = tableName;
    buffer.readPage(page);
    num = (int)page.pageData[0];
    for (i=0; i<num; i++)
        if (page.readAttrName(i) == attrName)
            s = page.readAttrType(i);
    
    if (s == "")
        return 0;
    else
    if (s == "int")
        return 1;
    else
    if (s == "float")
        return 2;
    else
    {
        j=0;
        for (i=5; s[i]!=')'; i++)
            j = j*10+s[i]-'0';
        
        return j+2;
    }
}

bool CatalogManager::attrUnique(string tableName, string attrName)
{
    int i,num;
    char c = '\0';
    CatalogPage page;
    BufferManager buffer;
    
    page.tableName = tableName;
    buffer.readPage(page);
    num = (int)page.pageData[0];
    for (i=0; i<num; i++)
        if (page.readAttrName(i) == attrName)
            c = page.readAttrUnique(i);

    if (c=='Y')
        return 1;
    else
        return 0;
}

string CatalogManager::primaryKey(string tableName)
{
    CatalogPage page;
    BufferManager buffer;
    
    page.tableName = tableName;
    buffer.readPage(page);
    if ((int)page.pageData[1]==-1)
        return "";
    else
        return page.readAttrName((int)page.pageData[1]);
}

bool CatalogManager::indexExisted(string indexName)
{
    string s;
    
    s=indexLocation(indexName);
    if (s=="")
        return 0;
    else
        return 1;
}

string CatalogManager::indexLocation(string indexName)
{
    BufferManager buffer;
    IndexCatalogPage indexPage;
    int n,i,k,x;
    string s,ans;
    
    ans="";
    indexPage.pageIndex=1;
    buffer.readPage(indexPage);
    n=*(int*)indexPage.pageData;
    i=1;
    k=0;
    while (i<=n)                                //开始逐条检查，看看这个索引名有没有
    {
        x=indexPage.readPrevDel(i);
        if (x==0)                               //如果当前条未被删除
        {
            s=indexPage.readIndexName(i);
            if (s==indexName)                   //如果找到了
            {
                ans=indexPage.readTableName(i)+" "+indexPage.readAttrName(i);
                return ans;
            }
        }
        else                                    //如果当前条已被删除，则最后一条位置后移
            n++;
        
        i++;
    }

    return "";
}

bool CatalogManager::insertIndex(string tableName, string attrName, string indexName)
{
    IndexCatalogPage indexPage;
    if (indexExisted(indexName))
    {
        printf("Failed to insert index %s. Index name already existed!\n", indexName.c_str());
        return 0;
    }
    else
    if (!tableExisted(tableName))
    {
        printf("Failed to insert index %s. Table %s does not exist!\n", indexName.c_str(),tableName.c_str());
        return 0;
    }
    else
    if (!attrExisted(tableName, attrName))
    {
        printf("Failed to insert index %s. Attribute %s on Table %s does not exist!\n", indexName.c_str(), attrName.c_str(),tableName.c_str());
        return 0;
    }
    else
    if (!attrUnique(tableName, attrName))
    {
        printf("Failed to insert index %s. Attribute %s on Table %s is not unique!\n", indexName.c_str(), attrName.c_str(),tableName.c_str());
        return 0;
    }
    else
    {
        indexPage.writeIndex(tableName, attrName, indexName);
        BufferManager buffer;
        CatalogPage catalog;
        int num,i;
        
        catalog.tableName = tableName;
        buffer.readPage(catalog);
        num = (int)catalog.pageData[0];
        for (i=0; i<num; i++)
            if (catalog.readAttrName(i) == attrName)
            {
                catalog.modifyAttrIndexNum(i,1);
                buffer.writePage(catalog);
                break;
            }
       
        printf("Inserted index %s(Attribute %s on Table %s) successfully!\n", indexName.c_str(), attrName.c_str(),tableName.c_str());
        return 1;
    }
}

void CatalogManager::deleteIndex(string indexName)
{
    if (!indexExisted(indexName))                   //如果没有这个索引
    {
        printf("Failed to delete index %s. Index does not existed.\n", indexName.c_str());
        return;
    }
    else                                            //有这个索引
    {
        BufferManager buffer;
        CatalogPage catalog;
        IndexCatalogPage indexPage;
        string s,tableName,attrName;
        int i,num,n,x;
        
        s=indexLocation(indexName);                 //找到这个索引
        tableName="";                               //分割字符串
        attrName="";
        i=0;
        while (s[i]!=' ')
        {
            tableName=tableName+s[i];
            i++;
        }
        
        while (s[i]==' ')
            i++;
        
        while (s[i]!=0)
        {
            attrName=attrName+s[i];
            i++;
        }
        
        printf("#%s#  @%s@  *%s*\n",indexName.c_str(), tableName.c_str(), attrName.c_str());
        printf("~%s~\n",primaryKey(tableName).c_str());
        if (indexName == tableName+attrName && primaryKey(tableName) == attrName)
        {
            printf("Failed to delete index %s. Default index on primary key cannot be deleted.\n",indexName.c_str());
            return;
        }
        
        indexPage.pageIndex=1;
        buffer.readPage(indexPage);
        n=*(int*)indexPage.pageData;
        i=1;
        while (i<=n)                                //开始在indexCatalogPage里逐条检查，删除这个索引
        {
            x=indexPage.readPrevDel(i);
            if (x==0)                               //如果当前条未被删除
            {
                s=indexPage.readIndexName(i);
                if (s==indexName)                   //如果找到要删的这条索引
                {
                    indexPage.deleteIndex(i);       //删掉它
                    break;
                }
            }
            else                                    //如果当前条已被删除，则最后一条位置后移
                n++;

            i++;
        }
        
        buffer.writePage(indexPage);
        catalog.tableName = tableName;              //在catalogPage里面的对应属性上修改索引总数
        buffer.readPage(catalog);
        num = (int)catalog.pageData[0];
        for (i=0; i<num; i++) {
            if (catalog.readAttrName(i) == attrName)
            {
                catalog.modifyAttrIndexNum(i,-1);
                buffer.writePage(catalog);
                if (catalog.readAttrIndexNum(i)==0)
                {
                    BufferManager bm;
                    string filePath=bm.indexFilePath(tableName, attrName);
                    if (bm.indexFileIsExist(tableName, attrName))
                        bm.deleteIndexFile(tableName, attrName);
                }

                break;
            }
        }

        printf("Deleted index %s successfully!\n",indexName.c_str());
    }
}

int CatalogManager::indexNum(string tableName, string attrName)
{
    CatalogPage catalog;
    BufferManager buffer;
    int num,i,x=-1;
    
    catalog.tableName = tableName;
    buffer.readPage(catalog);
    num = (int)catalog.pageData[0];
    for (i=0; i<num; i++)
        if (catalog.readAttrName(i) == attrName)
        {
            x = catalog.readAttrIndexNum(i);
            return x;
        }

    return x;
}

vector<Attribute> CatalogManager::tableInformation(string tableName)
{
    int i,j,num;
    string s;
    vector<Attribute> table;
    Attribute attr;
    CatalogPage catalog;
    BufferManager buffer;
    
    catalog.pageIndex=1;
    catalog.tableName=tableName;
    buffer.readPage(catalog);
    num=(int)catalog.pageData[0];
    table.clear();
    for (i=0; i<num; i++)
    {
        attr.attrName=catalog.readAttrName(i);
        j=attrType(tableName, attr.attrName);
        if (j==1)
        {
            attr.type = AttributeType::INT;
            attr.length = sizeof(int);
        }
        else
        if (j==2)
        {
            attr.type = AttributeType::FLOAT;
            attr.length = sizeof(float);
        }
        else
        {
            attr.type = AttributeType::CHAR;
            attr.length = j-2;
        }
        
        table.push_back(attr);
    }
    
    return table;
}
