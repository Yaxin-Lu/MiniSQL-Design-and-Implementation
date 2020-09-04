

#include "RecordPage.hpp"

void RecordPage::writenext(PageIndexType next)
{
	memcpy(pageData, &next, sizeof(PageIndexType));
}

void RecordPage::writebefore(PageIndexType before)
{
	memcpy(pageData + sizeof(PageIndexType), &before, sizeof(PageIndexType));
}

PageIndexType RecordPage::readnext()
{
	PageIndexType next;
	memcpy(&next, pageData, sizeof(PageIndexType));
	return next;
}

PageIndexType RecordPage::readbefore()
{
	PageIndexType before;
	memcpy(&before, pageData + sizeof(PageIndexType), sizeof(PageIndexType));
	return before;
}
