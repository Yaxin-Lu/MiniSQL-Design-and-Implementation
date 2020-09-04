

#ifndef BufferManager_hpp
#define BufferManager_hpp

#include "Global.h"
#include "Page.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <string>

using namespace std;

class BufferManager {
public:
    BufferManager() {
        DIR *tempOpenPointer;
        
        if ((tempOpenPointer = opendir(recordFilesDirectory.c_str())) == NULL) {
//            assert(mkdir(recordFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1);
            mkdir(recordFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (tempOpenPointer != NULL) {
            closedir(tempOpenPointer);
            tempOpenPointer = NULL;
        }
        
        if ((tempOpenPointer = opendir(indexFilesDirectory.c_str())) == NULL) {
//            assert(mkdir(indexFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1);
            mkdir(indexFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (tempOpenPointer != NULL) {
            closedir(tempOpenPointer);
            tempOpenPointer = NULL;
        }
        
        if ((tempOpenPointer = opendir(recordCatalogFilesDirectory.c_str())) == NULL) {
//            assert(mkdir(recordCatalogFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1);
            mkdir(recordCatalogFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (tempOpenPointer != NULL) {
            closedir(tempOpenPointer);
            tempOpenPointer = NULL;
        }
        
        if ((tempOpenPointer = opendir(indexCatalogFilesDirectory.c_str())) == NULL) {
//            assert(mkdir(indexCatalogFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1);
            mkdir(indexCatalogFilesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (tempOpenPointer != NULL) {
            closedir(tempOpenPointer);
            tempOpenPointer = NULL;
        }
        // 初始化存放文件的文件夹，根据表名以及索引键建立文件
    }
    
    bool openTableFile(string tableName);
    bool openIndexFile(string tableName, string attributeName);
    bool openTableCatalogFile(string tableName);
    bool openIndexCatalogFile(string tableName, string attributeName);
    
    void makeTwoPages(int fileHandle);
    
    bool closeTableFile(string tableName);
    bool closeIndexFile(string tableName, string attributeName);
    bool closeTableCatalogFile(string tableName);
    bool closeIndexCatalogFile(string tableName, string attributeName);
    
    bool tableFileIsExist(string tableName);
    bool indexFileIsExist(string tableName, string attributeName);
    bool tableCatalogFileIsExist(string tableName);
    bool indexCatalogFileIsExist(string tableName, string attributeName);
    
    bool deleteTableFile(string tableName);
    bool deleteIndexFile(string tableName, string attributeName);
    bool deleteTableCatalogFile(string tableName);
    bool deleteIndexCatalogFile(string tableName, string attributeName);
    
    PageIndexType tableFileTotalPages(string tableName);
    PageIndexType indexFileTotalPages(string tableName, string attributeName);
    PageIndexType tableCatalogFileTotalPages(string tableName);
    PageIndexType indexCatalogFileTotalPages(string tableName, string attributeName);
    
    inline string tableFilePath(string tableName);
    inline string indexFilePath(string tableName, string attributeName);
    inline string tableCatalogFilePath(string tableName);
    inline string indexCatalogFilePath(string tableName, string attributeName);
    
    bool readPage       (Page &page);
    bool forceReadPage  (Page &page);
    bool writePage      (Page &page);
    bool forceWritePage (Page &page);
    bool allocatePage   (Page &page);
    bool deallocatePage (Page &page);
    
    void checkPageFile  (Page &page);
    
    void closeAllFiles();
    
    void pinPage(Page &page);
    void unpinPage(Page &page);
    void addCountersExceptCurrent(int index);
    int findPageInCachePages(Page &page);
    int getUnpinedBiggestCachePage();
    void writeBackAllCache();
    
    void clearCache();
    
    static map<string, PageIndexType> tableFileHandles;
    static map<pair<string, string>, PageIndexType> indexFileHandles;
    static map<string, PageIndexType> tableCatalogFileHandles;
    static map<pair<string, string>, PageIndexType> indexCalalogFileHandles;
    
    static const string recordFilesDirectory;
    static const string indexFilesDirectory;
    static const string recordCatalogFilesDirectory;
    static const string indexCatalogFilesDirectory;
    
    static Page cachePages[CACHECAPACITY];
    static bool pined[CACHECAPACITY];
    static bool isDirty[CACHECAPACITY];
    static int lruCounter[CACHECAPACITY];
};

#endif /* BufferManager_hpp */
