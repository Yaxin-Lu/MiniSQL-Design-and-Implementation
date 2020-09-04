
#include "BufferManager.hpp"

map<string, PageIndexType>                  BufferManager::tableFileHandles;
map<pair<string, string>, PageIndexType>    BufferManager::indexFileHandles;
map<string, PageIndexType>                  BufferManager::tableCatalogFileHandles;
map<pair<string, string>, PageIndexType>    BufferManager::indexCalalogFileHandles;

const string BufferManager::recordFilesDirectory        = "./RecordFiles";
const string BufferManager::indexFilesDirectory         = "./IndexFiles";
const string BufferManager::recordCatalogFilesDirectory = "./RecordCatalogFiles";
const string BufferManager::indexCatalogFilesDirectory  = "./IndexCatalogFiles";

Page BufferManager::cachePages[CACHECAPACITY];
bool BufferManager::pined[CACHECAPACITY];
bool BufferManager::isDirty[CACHECAPACITY];
int  BufferManager::lruCounter[CACHECAPACITY];

bool BufferManager::openTableFile(string tableName) {
    string filePath = tableFilePath(tableName);
    if (tableFileHandles.find(tableName) == tableFileHandles.end()) {
        int fileHandle = open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fileHandle < 0) return false;
        tableFileHandles[tableName] = fileHandle;
        makeTwoPages(fileHandle);
        return true;
    }
    cout << "File " << filePath << " " << "already opened" << endl;
    return false;
}

bool BufferManager::openIndexFile(string tableName, string attributeName) {
    auto indexPair = make_pair(tableName, attributeName);
    string filePath = indexFilePath(tableName, attributeName);
    if (indexFileHandles.find(indexPair) == indexFileHandles.end()) {
        int fileHandle = open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fileHandle < 0) return false;
        indexFileHandles[indexPair] = fileHandle;
        makeTwoPages(fileHandle);
        return true;
    }
    cout << "File " << filePath << " " << "already opened" << endl;
    return false;
}

bool BufferManager::openTableCatalogFile(string tableName) {
    string filePath = tableCatalogFilePath(tableName);
    if (tableCatalogFileHandles.find(tableName) == tableCatalogFileHandles.end()) {
        int fileHandle = open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fileHandle < 0) return false;
        tableCatalogFileHandles[tableName] = fileHandle;
        makeTwoPages(fileHandle);
        return true;
    }
    cout << "File " << filePath << " " << "already opened" << endl;
    return false;
}

bool BufferManager::openIndexCatalogFile(string tableName, string attributeName) {
    auto indexPair = make_pair(tableName, attributeName);
    string filePath = indexCatalogFilePath(tableName, attributeName);
    if (indexCalalogFileHandles.find(indexPair) == indexCalalogFileHandles.end()) {
        int fileHandle = open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fileHandle < 0) return false;
        indexCalalogFileHandles[indexPair] = fileHandle;
        makeTwoPages(fileHandle);
        return true;
    }
    cout << "File " << filePath << " " << "already opened" << endl;
    return false;
}

string BufferManager::tableFilePath(string tableName) {
    return recordFilesDirectory + "/" + tableName + ".db";
}

string BufferManager::indexFilePath(string tableName, string attributeName) {
    return indexFilesDirectory + "/" + tableName + "_" + attributeName + ".db";
}

string BufferManager::tableCatalogFilePath(string tableName) {
    return recordCatalogFilesDirectory + "/" + tableName + ".db";
}

string BufferManager::indexCatalogFilePath(string tableName, string attributeName) {
    return indexCatalogFilesDirectory + "/" + tableName + "_" + attributeName + ".db";
}

void BufferManager::makeTwoPages(int fileHandle) {
    if (lseek(fileHandle, 0, SEEK_END) / PAGESIZE < 2) {
        char *pageBuffer = new char[PAGESIZE * 2];
        memset(pageBuffer, 0 , PAGESIZE * 2);
        write(fileHandle, pageBuffer, PAGESIZE * 2);
    }
}

bool BufferManager::closeTableFile(string tableName) {
    if (tableFileHandles.find(tableName) == tableFileHandles.end())
        return false;
    int handle = tableFileHandles[tableName];
    tableFileHandles.erase(tableName);
    if (close(handle) != -1) return true;
    return false;
}

bool BufferManager::closeIndexFile(string tableName, string attributeName) {
    auto indexPair = make_pair(tableName, attributeName);
    if (indexFileHandles.find(indexPair) == indexFileHandles.end())
        return false;
    int handle = indexFileHandles[indexPair];
    indexFileHandles.erase(indexPair);
    if (close(handle) != -1) return true;
    return false;
}

bool BufferManager::closeTableCatalogFile(string tableName) {
    if (tableCatalogFileHandles.find(tableName) == tableCatalogFileHandles.end()) {
        return false;
    }
    int handle = tableCatalogFileHandles[tableName];
    tableCatalogFileHandles.erase(tableName);
    if (close(handle) != -1) return true;
    return false;
}

bool BufferManager::closeIndexCatalogFile(string tableName, string attributeName) {
    auto indexPair = make_pair(tableName, attributeName);
    if (indexCalalogFileHandles.find(indexPair) == indexCalalogFileHandles.end()) {
        return false;
    }
    int handle = indexCalalogFileHandles[indexPair];
    indexCalalogFileHandles.erase(indexPair);
    if (close(handle != -1)) return true;
    return false;
}

bool BufferManager::tableFileIsExist(string tableName) {
    auto filePath = tableFilePath(tableName);
    return access(filePath.c_str(), F_OK) != -1;
}

bool BufferManager::indexFileIsExist(string tableName, string attributeName) {
    auto filePath = indexFilePath(tableName, attributeName);
    return access(filePath.c_str(), F_OK) != -1;
}

bool BufferManager::tableCatalogFileIsExist(string tableName) {
    auto filePath = tableCatalogFilePath(tableName);
    return access(filePath.c_str(), F_OK) != -1;
}

bool BufferManager::indexCatalogFileIsExist(string tableName, string attributeName) {
    auto filePath = indexCatalogFilePath(tableName, attributeName);
    return access(filePath.c_str(), F_OK) != -1;
}

bool BufferManager::deleteTableFile(string tableName) {
    writeBackAllCache();
    clearCache();
    auto filePath = tableFilePath(tableName);
    tableFileHandles.erase(tableName);
    return remove(filePath.c_str()) != -1;
}

bool BufferManager::deleteIndexFile(string tableName, string attributeName) {
    writeBackAllCache();
    clearCache();
    auto filePath = indexFilePath(tableName, attributeName);
    indexFileHandles.erase(make_pair(tableName, attributeName));
    return remove(filePath.c_str()) != -1;
}

bool BufferManager::deleteTableCatalogFile(string tableName) {
    writeBackAllCache();
    clearCache();
    auto filePath = tableCatalogFilePath(tableName);
    tableCatalogFileHandles.erase(tableName);
    return remove(filePath.c_str()) != -1;
}

bool BufferManager::deleteIndexCatalogFile(string tableName, string attributeName) {
    writeBackAllCache();
    clearCache();
    auto filePath = indexCatalogFilePath(tableName, attributeName);
    indexCalalogFileHandles.erase(make_pair(tableName, attributeName));
    return remove(filePath.c_str()) != -1;
}

PageIndexType BufferManager::tableFileTotalPages(string tableName) {
    Page page;
    page.tableName = tableName;
    page.pageType = PageType::RecordPage;
    checkPageFile(page);
    return lseek(page.fileHandle, 0, SEEK_END) / PAGESIZE;
}

PageIndexType BufferManager::indexFileTotalPages(string tableName, string attributeName) {
    Page page;
    page.tableName = tableName;
    page.attributeName = attributeName;
    page.pageType = PageType::IndexPage;
    checkPageFile(page);
    return lseek(page.fileHandle, 0, SEEK_END) / PAGESIZE;
}

PageIndexType BufferManager::tableCatalogFileTotalPages(string tableName) {
    Page page;
    page.tableName = tableName;
    page.pageType = PageType::RecordCatalogPage;
    checkPageFile(page);
    return lseek(page.fileHandle, 0, SEEK_END) / PAGESIZE;
}

PageIndexType BufferManager::indexCatalogFileTotalPages(string tableName, string attributeName) {
    Page page;
    page.tableName = tableName;
    page.attributeName = attributeName;
    page.pageType = PageType::IndexCatalogPage;
    checkPageFile(page);
    return lseek(page.fileHandle, 0, SEEK_END) / PAGESIZE;
}

void BufferManager::checkPageFile(Page &page) {
    assert(page.pageType != PageType::UndefinedPage);
    switch (page.pageType) {
        case PageType::RecordPage:
        {
            assert(page.tableName != "");
            if (tableFileHandles.find(page.tableName) == tableFileHandles.end())
                assert(openTableFile(page.tableName) == true);
            page.fileHandle = tableFileHandles[page.tableName];
        }
            break;
        case PageType::IndexPage:
        {
            assert(page.tableName != "");
            assert(page.attributeName != "");
            auto indexPair = make_pair(page.tableName, page.attributeName);
            if (indexFileHandles.find(indexPair) == indexFileHandles.end())
                assert(openIndexFile(page.tableName, page.attributeName) == true);
            page.fileHandle = indexFileHandles[indexPair];
        }
            break;
        case PageType::RecordCatalogPage:
        {
            assert(page.tableName != "");
            if (tableCatalogFileHandles.find(page.tableName) == tableCatalogFileHandles.end())
                assert(openTableCatalogFile(page.tableName) == true);
            page.fileHandle = tableCatalogFileHandles[page.tableName];
        }
            break;
        case PageType::IndexCatalogPage:
        {
//            assert(page.tableName != "");
//            assert(page.attributeName != "");
            auto indexPair = make_pair(page.tableName, page.attributeName);
            if (indexCalalogFileHandles.find(indexPair) == indexCalalogFileHandles.end())
                assert(openIndexCatalogFile(page.tableName, page.attributeName) == true);
            page.fileHandle = indexCalalogFileHandles[indexPair];
        }
            break;
        default:
            break;
    }
}

bool BufferManager::readPage(Page &page) {
//    return forceReadPage(page);
    assert(page.pageType != PageType::UndefinedPage);
    assert(page.pageIndex != UNDEFINEED_PAGE_NUM);
    int pageIndex = findPageInCachePages(page);
    if (pageIndex == -1) {
        forceReadPage(page); // May cause bug
        pageIndex = getUnpinedBiggestCachePage();
        if (pageIndex == -1) {
            return true;
        } else {
            if (isDirty[pageIndex]) {
                isDirty[pageIndex] = false;
                forceWritePage(cachePages[pageIndex]);
            }
            cachePages[pageIndex] = page;
            addCountersExceptCurrent(pageIndex);
            return true;
        }
    } else {
        page = cachePages[pageIndex];
        addCountersExceptCurrent(pageIndex);
        return true;
    }
    assert(false);
    return false;
}

bool BufferManager::forceReadPage(Page &page) {
    assert(page.pageType != PageType::UndefinedPage);
    assert(page.pageIndex != UNDEFINEED_PAGE_NUM);
    checkPageFile(page);
    lseek(page.fileHandle, page.pageIndex * PAGESIZE, SEEK_SET);
    return read(page.fileHandle, page.pageData, PAGESIZE) != -1;
}

bool BufferManager::writePage(Page &page) {
//    return forceWritePage(page);
    assert(page.pageType != PageType::UndefinedPage);
    assert(page.pageIndex != UNDEFINEED_PAGE_NUM);
    int pageIndex = findPageInCachePages(page);
    if (pageIndex == -1) {
        forceWritePage(page);// May cause bug
        pageIndex = getUnpinedBiggestCachePage();
        if (pageIndex == -1) {
            return true;
        } else {
            if (isDirty[pageIndex]) {
                isDirty[pageIndex] = false;
                forceWritePage(cachePages[pageIndex]);
            }
            cachePages[pageIndex] = page;
            addCountersExceptCurrent(pageIndex);
            return true;
        }
    } else {
        cachePages[pageIndex] = page;
        isDirty[pageIndex] = true;
        addCountersExceptCurrent(pageIndex);
        return true;
    }
    assert(false);
    return false;
}

bool BufferManager::forceWritePage(Page &page) {
    assert(page.pageType != PageType::UndefinedPage);
    assert(page.pageIndex != UNDEFINEED_PAGE_NUM);
    checkPageFile(page);
    lseek(page.fileHandle, page.pageIndex * PAGESIZE, SEEK_SET);
    return write(page.fileHandle, page.pageData, PAGESIZE) != -1;
}

bool BufferManager::allocatePage(Page &page) {
    assert(page.pageType != PageType::UndefinedPage);
    checkPageFile(page);
    char pageBuffer[PAGESIZE];
    lseek(page.fileHandle, 0, SEEK_SET);
    read(page.fileHandle, pageBuffer, PAGESIZE);
    
    PageIndexType pageIndex = *((PageIndexType*) pageBuffer);
    if (pageIndex != 0) {
        lseek(page.fileHandle, pageIndex * PAGESIZE, SEEK_SET);
        read(page.fileHandle, pageBuffer, PAGESIZE);
        lseek(page.fileHandle, 0, SEEK_SET);
        write(page.fileHandle, pageBuffer, PAGESIZE);
    } else {
        pageIndex = lseek(page.fileHandle, 0, SEEK_END) / PAGESIZE;
        memset(pageBuffer, 0, PAGESIZE);
        write(page.fileHandle, pageBuffer, PAGESIZE);
    }
    
    page.pageIndex = pageIndex;
    return true;
}

bool BufferManager::deallocatePage(Page &page) {
    assert(page.pageType != PageType::UndefinedPage);
    checkPageFile(page);
    
    char pageBuffer[PAGESIZE];
    lseek(page.fileHandle, 0, SEEK_SET);
    read(page.fileHandle, pageBuffer, PAGESIZE);
    
    lseek(page.fileHandle, page.pageIndex * PAGESIZE, SEEK_SET);
    write(page.fileHandle, pageBuffer, PAGESIZE);
    
    PageIndexType *pageIndex = (PageIndexType*) pageBuffer;
    *pageIndex = page.pageIndex;
    
    lseek(page.fileHandle, 0, SEEK_SET);
    write(page.fileHandle, pageBuffer, PAGESIZE);
    
    page.pageIndex = -1;
    
    return true;
}

void BufferManager::closeAllFiles() {
    for (auto itr: tableFileHandles)
        assert(close(itr.second) != -1);
    
    for (auto itr: indexFileHandles)
        assert(close(itr.second) != -1);
    
    for (auto itr: tableCatalogFileHandles)
        assert(close(itr.second) != -1);
    
    for (auto itr: indexCalalogFileHandles)
        assert(close(itr.second) != -1);
}

void BufferManager::pinPage(Page &page) {
    for (int i = 0; i < CACHECAPACITY; ++i) {
        if (cachePages[i] == page) {
            pined[i] = true;
            break;
        }
    }
}

void BufferManager::unpinPage(Page &page) {
    for (int i = 0; i < CACHECAPACITY; ++i) {
        if (cachePages[i] == page) {
            pined[i] = false;
            break;
        }
    }
}

void BufferManager::addCountersExceptCurrent(int index) {
    for (int i = 0; i < CACHECAPACITY; ++i) {
        lruCounter[i]++;
        if (lruCounter[i] > LRUCOUNTERMAX) lruCounter[i] = LRUCOUNTERMAX;
    }
    lruCounter[index] = 0;
}

int BufferManager::findPageInCachePages(Page &page) {
    int retIndex = -1;
    for (int i = 0; i < CACHECAPACITY; ++i)
        if (cachePages[i] == page) {
            retIndex = i;
            break;
        }
    return retIndex;
}

int BufferManager::getUnpinedBiggestCachePage() {
    int retIndex = -1;
    int bigSaver = -1;
    for (int i = 0; i < CACHECAPACITY; ++i) {
        if ((!pined[i]) && (lruCounter[i] > bigSaver)) {
            retIndex = i;
            bigSaver = lruCounter[i];
        }
    }
    return retIndex;
}

void BufferManager::writeBackAllCache() {
    for (int i = 0; i < CACHECAPACITY; ++i) {
        if (isDirty[i]) {
            forceWritePage(cachePages[i]);
            isDirty[i] = false;
        }
        pined[i] = false;
        lruCounter[i] = 0;
    }
}
void BufferManager::clearCache() {
    for (int i = 0; i < CACHECAPACITY; ++i) {
        cachePages[i].pageType = PageType::UndefinedPage;
        pined[i] = false;
        isDirty[i] = false;
        lruCounter[i] = 0;
    }
}
