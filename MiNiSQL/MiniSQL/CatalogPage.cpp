

#include "CatalogPage.hpp"
#include <string>
using namespace std;

void CatalogPage::writeAttr(int start, string cont)
{
    int i,len;
    
    len=(int)cont.length();
    for (i=0; i<len; i++)
        pageData[start+i]=cont[i];
    
    pageData[start+i]=0;
}

string CatalogPage::readAttrName(int num)
{
    int i;
    string s="";
    
    for (i=num*100+2; pageData[i]!=0; i++)
        s = s+pageData[i];
    
    return s;
}

string CatalogPage::readAttrType(int num)
{
    int i;
    string s="";
    
    for (i=num*100+22; pageData[i]!=0; i++)
        s = s+pageData[i];
    
    return s;
}

string CatalogPage::readAttrIndex(int num)
{
    int i;
    string s="";
    
    for (i=num*100+42; pageData[i]!=0; i++)
        s = s+pageData[i];
    
    return s;
}

char CatalogPage::readAttrUnique(int num)
{
    return pageData[num*100+62];
}

int CatalogPage::readAttrIndexNum(int num)
{
    return *(int*)(pageData+num*100+82);
}

void CatalogPage::modifyAttrIndexNum(int num, int incr)
{
    int x;
    x = *(int*)(pageData+num*100+82);
    *(int*)(pageData+num*100+82)=x+incr;
}
