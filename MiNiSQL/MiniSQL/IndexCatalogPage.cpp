

#include "BufferManager.hpp"
#include "IndexCatalogPage.hpp"
#include <string>
#include "Global.h"
using namespace std;

//由于index信息只存在一个文件内，因此用第一页来存放总体信息
//pageData[0..3]表示当前index条目总数，pageData[4..7]表示最后一个被删除的条目编号，如果没有则为-1
void IndexCatalogPage::writeInitialPage()
{
    BufferManager buffer;
    
    *(int*)pageData=0;
    *(int*)(pageData+4)=-1;
    buffer.writePage(*this);
}

int IndexCatalogPage::readPrevDel(int indexPos)
{
    int i,j,x,rec;
    BufferManager buffer;
    
    rec=pageIndex;              //由于这个函数是临时调用，因此先记录下此前所在页面
    i=(indexPos-1)/recordLimit+2;
    j=(indexPos-1)%recordLimit+1;
    pageIndex=i;                //改为要读取的页号，并且读取内容
    buffer.readPage(*this);
    x=*(int*)(pageData+(j-1)*400+300);
    pageIndex=rec;              //读完内容，改回原页号，并且重新读取内容
    buffer.readPage(*this);
    return x;
}

string IndexCatalogPage::readIndexName(int indexPos)
{
    int i,j,k;
    string s="";
    BufferManager buffer;
    
    i=(indexPos-1)/recordLimit+2;
    j=(indexPos-1)%recordLimit+1;
    pageIndex=i;
    buffer.readPage(*this);
    for (k=(j-1)*400+200; pageData[k]!=0; k++)
        s=s+pageData[k];
    
    return s;
}

string IndexCatalogPage::readTableName(int indexPos)
{
    int i,j,k;
    string s="";
    BufferManager buffer;
    
    i=(indexPos-1)/recordLimit+2;
    j=(indexPos-1)%recordLimit+1;
    pageIndex=i;
    buffer.readPage(*this);
    for (k=(j-1)*400; pageData[k]!=0; k++)
        s=s+pageData[k];
    
    return s;
}

string IndexCatalogPage::readAttrName(int indexPos)
{
    int i,j,k;
    string s="";
    BufferManager buffer;
    
    i=(indexPos-1)/recordLimit+2;
    j=(indexPos-1)%recordLimit+1;
    pageIndex=i;
    buffer.readPage(*this);
    for (k=(j-1)*400+100; pageData[k]!=0; k++)
        s=s+pageData[k];
    
    return s;
}

//在本页写
void IndexCatalogPage::writeCont(int start, string cont)
{
    int i,len;
    
    len=(int)cont.length();
    for (i=0; i<len; i++)
        pageData[start+i]=cont[i];
    
    pageData[start+i]=0;
}

//储存格式：表名、属性名、索引名、上一个被删除的位置（如果当前条目并未被删除，则此项为0）
int IndexCatalogPage::writeIndex(string tableName, string attrName, string indexName)
{
    int n,m,i,j,x,target;
    BufferManager buffer;
    
    pageIndex=1;
    buffer.readPage(*this);
    n = *(int*)pageData;
    n++;
    *(int*)pageData = n;
    m = *(int*)(pageData+4);
    if (m==-1)                  //没有删除的条目，即目前为满排列，直接插到最后
    {
        target=n;
        i=(n-1)/recordLimit+2;  //i为页数
        j=(n-1)%recordLimit+1;  //j为行数
    }
    else                        //有删除的条目，插到最后一次删除的位置，并更改删除信息
    {
        target=m;
        i=(m-1)/recordLimit+2;
        j=(m-1)%recordLimit+1;
        buffer.writePage(*this);//由于下一行要调用一个更改页的函数，这里必须先write一次以免pageData丢失
        x=readPrevDel(m);
        *(int*)(pageData+4)=x;
    }
    
    buffer.writePage(*this);    //改首页信息，包括：总数增加了1，以及如果占用了已删除位置，还需要更改这项信息
    pageIndex=i;                //强行改成第i页并读第i页数据
    buffer.readPage(*this);
    writeCont((j-1)*400, tableName);
    writeCont((j-1)*400+100, attrName);
    writeCont((j-1)*400+200, indexName);
    *(int*)(pageData+(j-1)*400+300) = 0;
    buffer.writePage(*this);
    
    return target;
}

void IndexCatalogPage::deleteIndex(int indexPos)
{
    int m,n,i,j;
    BufferManager buffer;
    
    pageIndex=1;                //读取最后被删除的位置编号
    buffer.readPage(*this);
    n = *(int*)pageData;
    m = *(int*)(pageData+4);
    *(int*)pageData = n-1;
    *(int*)(pageData+4) = indexPos;
    buffer.writePage(*this);    //更改首页信息
    i=(indexPos-1)/recordLimit+2;
    j=(indexPos-1)%recordLimit+1;
    pageIndex=i;                //跳到当前删除的位置，并修改删除信息
    buffer.readPage(*this);
    *(int*)(pageData+(j-1)*400+300) = m;
    buffer.writePage(*this);
}
