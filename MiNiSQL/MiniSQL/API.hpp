

#ifndef API_hpp
#define API_hpp

#include <cstdio>
#include "SQLcommand.hpp"
#include "TableInfo.hpp"
#include "CatalogManager.hpp"
#include "Attribute.hpp"
#include <vector>
#include <cstdio>
#include "Table.hpp"
#include "BPTree.hpp"


class API
{
public:
    bool insertRecord(SQLcommand);
    bool createTable(TableInfo);
    bool dropIndex(SQLcommand);
    bool createIndex(SQLcommand);
    bool dropTable(SQLcommand);
    bool selectRecord(SQLcommand);
    bool deleteRecord(SQLcommand);
    bool execFile(SQLcommand);
};
#endif /* API_hpp */
