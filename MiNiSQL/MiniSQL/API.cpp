
#include "API.hpp"

bool API::insertRecord(SQLcommand sql)
{
//        printf("%d\n",sql.attrNum);
//        printf("%f\n",sql.condCont[2].attrValueFlo);
    clock_t begin = clock();
    printf("----API::insertRecord----\n");
    /* sql使用方法：
        sql.tableName为用户想要插入的表名
        sql.attrNum为用户输入的数据个数
        sql.condCont[1..attrNum]里面为每一个数据的情况，其中有如下参数可以使用
        condCont[i].attrType 为 "INT" "FLOAT" "CHAR" 中的一种，要注意当用户输入"INT"时，表的属性是"FLOAT"也是合法的
        对应的，根据condCont[i].attrType，以下三个里面会有一个有值，要注意当attrType为"INT"时，attrValueFlo里面也会有一份
            condCont[i].attrValueFlo / condCont[i].attrValueInt / condCond[i].attrValueStr
    */
    CatalogManager catalog;
    if (!catalog.tableExisted(sql.tableName)) {
        printf("Table %s doesn't exist! Insertion failed!\n", sql.tableName.c_str());
    }
    vector<Attribute> vec=catalog.tableInformation(sql.tableName);
    int i;
    if (vec.size()!=sql.attrNum)
    {
        printf("Failed to insert record. Wrong number of attributes.\n");
        return 0;
    }
    
    for (i=1; i<=sql.attrNum; i++)
    {
        if (vec[i-1].type==AttributeType::CHAR)
        {
            if (sql.condCont[i].attrType!="CHAR")
            {
                printf("Failed to insert record. Wrong type of value.\n");
                return 0;
            }
            
            if (sql.condCont[i].attrValueStr.length()>vec[i-1].length)
            {
                printf("Failed to insert record. Value is too long.\n");
                return 0;
            }
            memset(vec[i - 1].chardata, 0, vec[i - 1].length);
            memcpy(vec[i - 1].chardata, sql.condCont[i].attrValueStr.c_str(), sql.condCont[i].attrValueStr.length());
        }
        else
        if (vec[i-1].type==AttributeType::FLOAT)
        {
            if (sql.condCont[i].attrType=="CHAR")
            {
                printf("Failed to insert record. Wrong type of value.\n");
                return 0;
            }
            if (sql.condCont[i].attrType == "INT") {
                vec[i - 1].floatdata = (float) sql.condCont[i].attrValueInt;
            } else {
                vec[i - 1].floatdata = sql.condCont[i].attrValueFlo;
            }
        }
        else
        {
            if (sql.condCont[i].attrType!="INT")
            {
                printf("Failed to insert record. Wrong type of value.\n");
                return 0;
            }
            vec[i - 1].intdata = sql.condCont[i].attrValueInt;
        }
    }
    
    //现在数量和类型都是对的了，就等插入了
    //注意判unique的属性不能和表中已有的重复
    //判一个属性是否unique可以调用catalogManager的函数
    //bool attrUnique(string, string);  参数：表名、列名
    //vec[i].attrName可以读列名
    
    Table table(sql.tableName);
    
    for (int i = 0; i < vec.size(); ++i) {
        if (catalog.attrUnique(sql.tableName, vec[i].attrName)) {
            if (catalog.indexNum(sql.tableName, vec[i].attrName) > 0) {
                BPTree *indexTree;
                if (vec[i].type == AttributeType::INT) {
                    indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::INT, vec[i].length);
                } else if (vec[i].type == AttributeType::FLOAT) {
                    indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::FLOAT, vec[i].length);
                } else {
                    indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::CHAR, vec[i].length);
                }
                if ((*indexTree).searchKeyForPagePointer(vec[i]) != UNDEFINEED_PAGE_NUM) {
                    printf("Conflict on attribute %s, this attribute should be unique!\n", vec[i].attrName.c_str());
                    delete indexTree;
                    return 0;
                }
                delete indexTree;
            } else {
                if (!table.scanEqual(i, vec[i]).empty()) {
                    printf("Conflict on attribute %s, this attribute should be unique!\n", vec[i].attrName.c_str());
                    return 0;
                }
            }
        }
    }
    
    
    PageIndexType insertedPage = table.insertTuple(vec);
    
    for (int i = 0; i < vec.size(); ++i) {
        if (catalog.indexNum(sql.tableName, vec[i].attrName) > 0) {
            BPTree *indexTree;
            if (vec[i].type == AttributeType::INT) {
                indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::INT, vec[i].length);
            } else if (vec[i].type == AttributeType::FLOAT) {
                indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::FLOAT, vec[i].length);
            } else {
                indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::CHAR, vec[i].length);
            }
            (*indexTree).insertKeyPointerPair(vec[i], insertedPage);
            delete indexTree;
        }
    }
    
    printf("Command running time: %f second\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
    return 1;
}

bool API::createTable(TableInfo tableInfo)
{
//    printf("@@@%s\n",tableInfo.tableName.c_str());
//    printf("---%d\n",tableInfo.attrNum);
//    printf("===%s\n",tableInfo.attrType[2].c_str());
//    printf("~~~%s\n",tableInfo.primaryKey.c_str());
    clock_t begin = clock();
    printf("----API::createTable----\n");
    CatalogManager catalog;
    catalog.insertTable(tableInfo);
    printf("Command running time: %f second\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
    return 1;
}

bool API::dropIndex(SQLcommand sql)
{
//    printf("---%s\n",sql.indexName.c_str());
    printf("----API::dropIndex----\n");
    CatalogManager catalog;
    catalog.deleteIndex(sql.indexName);
    return 1;
}

bool API::createIndex(SQLcommand sql)
{
//    printf("---%s %s %s\n",sql.indexName.c_str(), sql.tableName.c_str(), sql.attrName.c_str());
    
    clock_t begin = clock();
    printf("----API::createIndex----\n");
    CatalogManager catalog;
    if (catalog.insertIndex(sql.tableName, sql.attrName, sql.indexName))
    {
        //catalog创建成功，调用indexManager的接口真正建索引
        if (catalog.indexNum(sql.tableName, sql.attrName) == 1) {
            Table table(sql.tableName);
            BPTree *indexTree;
            int i;
            auto vec = catalog.tableInformation(sql.tableName);
            for (i = 0; i < vec.size(); ++i) {
                if (vec[i].attrName == sql.attrName) break;
            }
            if (vec[i].type == AttributeType::INT) {
                indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::INT, vec[i].length);
            } else if (vec[i].type == AttributeType::FLOAT) {
                indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::FLOAT, vec[i].length);
            } else {
                indexTree = new BPTree(sql.tableName, vec[i].attrName, BPTreeKeyType::CHAR, vec[i].length);
            }
            for (auto itr: table.getAll(i)) {
                indexTree->insertKeyPointerPair(itr.first, itr.second);
            }
            delete indexTree;
        }
    }
    printf("Command running time: %f second\n", (double)(clock() - begin) / CLOCKS_PER_SEC);

    return 1;
}

bool API::dropTable(SQLcommand sql)
{
//    printf("---%s\n",sql.tableName.c_str());
    
    printf("----API::dropTable----\n");
    CatalogManager catalog;
    catalog.dropTable(sql.tableName);
    return 1;
}

bool API::selectRecord(SQLcommand sql)
{
//    printf("---%d\n",sql.condNum);
//    printf("---%s %s %f\n", sql.condCont[1].attrName.c_str(), sql.condCont[1].op.c_str(), sql.condCont[1].attrValueFlo);
    
    clock_t begin = clock();
    printf("----API::selectRecord----\n");
    
    CatalogManager cm;
    if (!cm.tableExisted(sql.tableName)) {
        printf("Table %s doesn't exist! Select failed!\n", sql.tableName.c_str());
        return 0;
    }
    
    vector<Attribute> conditionList;
    conditionList.clear();
    vector<string> relationList;
    relationList.clear();
    
    for (int i = 1; i <= sql.condNum; i++) {
        bool exist = false;
        for (auto attribute: cm.tableInformation(sql.tableName)) {
            if (attribute.attrName == sql.condCont[i].attrName) {
                exist = true;
                if (sql.condCont[i].attrType == "INT") {
                    if (attribute.type == AttributeType::CHAR) {
                        printf("In where clause, the argument provided for attribute %s doesn't match its type, select failed!\n", attribute.attrName.c_str());
                        return 0;
                    } else if (attribute.type == AttributeType::INT) {
                        attribute.intdata = sql.condCont[i].attrValueInt;
                    } else if (attribute.type == AttributeType::FLOAT) {
                        attribute.floatdata = (float)sql.condCont[i].attrValueInt;
                    }
                } else if (sql.condCont[i].attrType == "FLOAT") {
                    if (attribute.type != AttributeType::FLOAT) {
                        printf("In where clause, the argument provided for attribute %s doesn't match its type, select failed!\n", attribute.attrName.c_str());
                        return 0;
                    }
                    attribute.floatdata = sql.condCont[i].attrValueFlo;
                } else if (sql.condCont[i].attrType == "CHAR") {
                    if (attribute.type != AttributeType::CHAR) {
                        printf("In where clause, the argument provided for attribute %s doesn't match its type, select failed!\n", attribute.attrName.c_str());
                        return 0;
                    }
                    if (attribute.length < sql.condCont[i].attrValueStr.length()) {
                        printf("In where clause, the argument provided for attribute %s is too long, select failed!\n", attribute.attrName.c_str());
                        return 0;
                    }
                    memset(attribute.chardata, 0, attribute.length);
                    memcpy(attribute.chardata, sql.condCont[i].attrValueStr.c_str(), sql.condCont[i].attrValueStr.length());
                }
                conditionList.push_back(attribute);
                relationList.push_back(sql.condCont[i].op);
            }
        }
        if (!exist) {
            printf("Attribute named %s doesn't exist! Select failed!\n", sql.condCont[i].attrName.c_str());
            return 0;
        }
    }
    
    
    for (auto itr: cm.tableInformation(sql.tableName)) {
        printf("%s\t\t\t", itr.attrName.c_str());
    }
    cout << endl;
    
    Table table(sql.tableName);
    
    if (sql.condNum == 0) {
        auto result = table.getAll();
        for (auto itr: result) {
            table.printinfo(itr);
        }
        printf("Total %lu records selected\n", result.size());
    } else {
        vector<PageIndexType> result = table.getAll();
        
        for (int i = 0; i < conditionList.size(); ++i) {
            if ((relationList[i] == "=") && (cm.indexNum(sql.tableName, conditionList[i].attrName) > 0)) {
                BPTree *indexTree;
                if (conditionList[i].type == AttributeType::INT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::INT, conditionList[i].length);
                } else if (conditionList[i].type == AttributeType::FLOAT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::FLOAT, conditionList[i].length);
                } else {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::CHAR, conditionList[i].length);
                }
                auto searchResult = indexTree->searchKeyForPagePointer(conditionList[i]);
                if (searchResult != UNDEFINEED_PAGE_NUM) {
                    result.clear();
                    result.push_back(searchResult);
                    break;
                }
                delete indexTree;
            }
        }
        
        for (int i = 0; i < conditionList.size(); ++i) {
            vector<PageIndexType> nextResult;
            nextResult.clear();
            
            auto tableInfo = cm.tableInformation(sql.tableName);
            int attributeIndex;
            for (attributeIndex = 0; attributeIndex < tableInfo.size(); ++attributeIndex)
                if (tableInfo[attributeIndex].attrName == conditionList[i].attrName) break;
            
            if ((relationList[i] == "=") && (cm.indexNum(sql.tableName, conditionList[i].attrName) > 0)) {
                BPTree *indexTree;
                if (conditionList[i].type == AttributeType::INT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::INT, conditionList[i].length);
                } else if (conditionList[i].type == AttributeType::FLOAT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::FLOAT, conditionList[i].length);
                } else {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::CHAR, conditionList[i].length);
                }
                auto searchResult = indexTree->searchKeyForPagePointer(conditionList[i]);
                if (searchResult != UNDEFINEED_PAGE_NUM && find(result.begin(), result.end(), searchResult) != result.end())
                    nextResult.push_back(searchResult);
                
                delete indexTree;
            } else {
                for (auto itr: result) {
                    auto currentAttributes = table.getTupleAtPage(itr);
                    bool ok = false;
                    if (relationList[i] == "=") {
                        if (conditionList[i] == currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == "<>") {
                        if (conditionList[i] != currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == "<") {
                        if (conditionList[i] > currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == "<=") {
                        if (conditionList[i] >= currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == ">") {
                        if (conditionList[i] < currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == ">=") {
                        if (conditionList[i] <= currentAttributes[attributeIndex])
                            ok = true;
                    }
                    if (ok)
                        nextResult.push_back(itr);
                }
            }
            result = nextResult;
        }
        
        for (auto itr: result) {
            table.printinfo(itr);
        }
        printf("Total %lu records selected", result.size());
    }
    printf("Command running time: %f second\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
    return 1;
}

bool API::deleteRecord(SQLcommand sql)
{
//    printf("---%d\n",sql.condNum);
//    printf("---%s %s %s\n",sql.condCont[2].attrName.c_str(), sql.condCont[2].op.c_str(), sql.condCont[2].attrValueStr.c_str());
    
    clock_t begin = clock();
    printf("----API::deleteRecord----\n");
    CatalogManager cm;
    if (!cm.tableExisted(sql.tableName)) {
        printf("Table %s doesn't exist! Select failed!\n", sql.tableName.c_str());
        return 0;
    }
    
    vector<Attribute> conditionList;
    conditionList.clear();
    vector<string> relationList;
    relationList.clear();
    
    for (int i = 1; i <= sql.condNum; i++) {
        bool exist = false;
        for (auto Attribute: cm.tableInformation(sql.tableName)) {
            if (Attribute.attrName == sql.condCont[i].attrName) {
                exist = true;
                if (sql.condCont[i].attrType == "INT") {
                    if (Attribute.type == AttributeType::CHAR) {
                        printf("In where clause, the argument provided for attribute %s doesn't match its type, delete failed!\n", Attribute.attrName.c_str());
                        return 0;
                    } else if (Attribute.type == AttributeType::INT) {
                        Attribute.intdata = sql.condCont[i].attrValueInt;
                    } else if (Attribute.type == AttributeType::FLOAT) {
                        Attribute.floatdata = (float)sql.condCont[i].attrValueInt;
                    }
                } else if (sql.condCont[i].attrType == "FLOAT") {
                    if (Attribute.type != AttributeType::FLOAT) {
                        printf("In where clause, the argument provided for attribute %s doesn't match its type, delete failed!\n", Attribute.attrName.c_str());
                        return 0;
                    }
                    Attribute.floatdata = sql.condCont[i].attrValueFlo;
                } else if (sql.condCont[i].attrType == "CHAR") {
                    if (Attribute.type != AttributeType::CHAR) {
                        printf("In where clause, the argument provided for attribute %s doesn't match its type, delete failed!\n", Attribute.attrName.c_str());
                        return 0;
                    }
                    if (Attribute.length < sql.condCont[i].attrValueStr.length()) {
                        printf("In where clause, the argument provided for attribute %s is too long, delete failed!\n", Attribute.attrName.c_str());
                        return 0;
                    }
                    memset(Attribute.chardata, 0, Attribute.length);
                    memcpy(Attribute.chardata, sql.condCont[i].attrValueStr.c_str(), sql.condCont[i].attrValueStr.length());
                }
                conditionList.push_back(Attribute);
                relationList.push_back(sql.condCont[i].op);
            }
        }
        if (!exist) {
            printf("Attribute named %s doesn't exist! Delete failed!\n", sql.condCont[i].attrName.c_str());
            return 0;
        }
    }
    
    Table table(sql.tableName);
    
    vector<PageIndexType> result = table.getAll();
    
    if (sql.condNum != 0) {
        for (int i = 0; i < conditionList.size(); ++i) {
            if ((relationList[i] == "=") && (cm.indexNum(sql.tableName, conditionList[i].attrName) > 0)) {
                BPTree *indexTree;
                if (conditionList[i].type == AttributeType::INT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::INT, conditionList[i].length);
                } else if (conditionList[i].type == AttributeType::FLOAT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::FLOAT, conditionList[i].length);
                } else {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::CHAR, conditionList[i].length);
                }
                auto searchResult = indexTree->searchKeyForPagePointer(conditionList[i]);
                if (searchResult != UNDEFINEED_PAGE_NUM) {
                    result.clear();
                    result.push_back(searchResult);
                    break;
                }
                delete indexTree;
            }
        }
        
        for (int i = 0; i < conditionList.size(); ++i) {
            vector<PageIndexType> nextResult;
            nextResult.clear();
            
            auto tableInfo = cm.tableInformation(sql.tableName);
            int attributeIndex;
            for (attributeIndex = 0; attributeIndex < tableInfo.size(); ++attributeIndex)
                if (tableInfo[attributeIndex].attrName == conditionList[i].attrName) break;
            
            if ((relationList[i] == "=") && (cm.indexNum(sql.tableName, conditionList[i].attrName) > 0)) {
                BPTree *indexTree;
                if (conditionList[i].type == AttributeType::INT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::INT, conditionList[i].length);
                } else if (conditionList[i].type == AttributeType::FLOAT) {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::FLOAT, conditionList[i].length);
                } else {
                    indexTree = new BPTree(sql.tableName, conditionList[i].attrName, BPTreeKeyType::CHAR, conditionList[i].length);
                }
                auto searchResult = indexTree->searchKeyForPagePointer(conditionList[i]);
                if (searchResult != UNDEFINEED_PAGE_NUM && find(result.begin(), result.end(), searchResult) != result.end())
                    nextResult.push_back(searchResult);
                
                delete indexTree;
            } else {
                for (auto itr: result) {
                    auto currentAttributes = table.getTupleAtPage(itr);
                    bool ok = false;
                    if (relationList[i] == "=") {
                        if (conditionList[i] == currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == "<>") {
                        if (conditionList[i] != currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == "<") {
                        if (conditionList[i] > currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == "<=") {
                        if (conditionList[i] >= currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == ">") {
                        if (conditionList[i] < currentAttributes[attributeIndex])
                            ok = true;
                    } else if (relationList[i] == ">=") {
                        if (conditionList[i] <= currentAttributes[attributeIndex])
                            ok = true;
                    }
                    if (ok)
                        nextResult.push_back(itr);
                }
            }
            result = nextResult;
        }
    }
    
    if (!result.empty()) {
        auto tableInfo = cm.tableInformation(sql.tableName);
        for (auto attribute: cm.tableInformation(sql.tableName)) {
            if (cm.indexNum(sql.tableName, attribute.attrName) > 0) {
                int attributeIndex;
                for (attributeIndex = 0; attributeIndex < tableInfo.size(); ++attributeIndex)
                    if (tableInfo[attributeIndex].attrName == attribute.attrName) break;
                
                BPTree *indexTree = nullptr;
                if (attribute.type == AttributeType::INT) {
                    indexTree = new BPTree(sql.tableName, attribute.attrName, BPTreeKeyType::INT, attribute.length);
                } else if (attribute.type == AttributeType::FLOAT) {
                    indexTree = new BPTree(sql.tableName, attribute.attrName, BPTreeKeyType::FLOAT, attribute.length);
                } else if (attribute.type == AttributeType::CHAR){
                    indexTree = new BPTree(sql.tableName, attribute.attrName, BPTreeKeyType::CHAR, attribute.length);
                }
                
                for (auto itr: result) {
                    indexTree->deleteKey(table.getTupleAtPage(itr)[attributeIndex]);
                }
                
                delete indexTree;
            }
        }
        for (auto itr: result) {
            table.deleteTuple(itr);
        }
    }
    
    printf("%lu record deleted\n", result.size());
    printf("Command running time: %f second\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
    
    return 1;
}
