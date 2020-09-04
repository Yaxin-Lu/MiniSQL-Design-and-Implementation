
#ifndef RecordPage_hpp
#define RecordPage_hpp

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Page.hpp"
using namespace std;

class RecordPage: public Page
{
public:
    RecordPage(){pageType = PageType::RecordPage;}
    ~RecordPage(){};
	
	void writenext(PageIndexType);
	void writebefore(PageIndexType);
	PageIndexType readnext();
	PageIndexType readbefore();

};

#endif /* RecordPage_hpp */
