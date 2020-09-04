

#include <iostream>
#include <string>
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include "IndexCatalogPage.hpp"
#include "TableInfo.hpp"
#include "BPTree.hpp"
#include "Table.hpp"
#include "y.tab.h"
extern int yyparse();


using namespace std;

//void printAll(BPTreeNode node) {
//    for (int i = 1; i < node.entryNumber; ++i) {
//        cout << node.nodeEntries[i].key.intData << endl;
//    }
//}
//
//void printTree(BPTree &tree, BPTreeNode node, int deepth) {
//    srand(time(0));
//    if (node.nodeType == BPTreeNodeType::BPTreeLeafNode) {
//        for (int j = 0; j < deepth; j++)
//            cout << "-";
//        cout << "deepth " << deepth << " ";
//        cout << "Leaf node is ";//<< node.nodeEntries[1].key.intData << endl;
//        for (int i = 1; i < node.entryNumber; ++i) {
//            cout << node.nodeEntries[i].key.intData;
////                for (int j = 0; j < 20; j++)
////                    cout << node.nodeEntries[i].key.charData[j];
//            cout << " ";
//        }
//        cout << " page " << node.nodePage.pageIndex;
//        cout << " parentpage " << node.parentNodePagePointer;
//        cout << endl;
////        for (int i = 1; i < node.entryNumber; ++i) {
////            for (int j = 0; j < deepth; j++)
////                cout << "-";
////            cout << node.nodeEntries[i].key.intData << endl;
////        }
////        cout << endl << endl << endl;
//    } else {
//        for (int i = 0; i < node.entryNumber; ++i) {
//            for (int j = 0; j < deepth; j++)
//                cout << "-";
//            cout << "internal node pointer ";
//            cout << node.nodeEntries[i].key.intData << endl;
////            for (int k = 0; k < 20; k++)
////                cout << node.nodeEntries[i].key.charData[k];
////            cout << endl;
//
//
//            printTree(tree, tree.getNodeAtPage(node.nodeEntries[i].pagePointer), deepth + 1);
//        }
//    }
//}
//int saver[1000024];
//
//void testDelete() {
//    srand(time(0));
//    BPTree tree("test2", "test2", BPTreeKeyType::INT, 4);
//    BPTreeKey key;
//    key.keyLen = 4;
//    key.type = BPTreeKeyType::INT;
//    map<int, bool> used;
//    for (int i = 0; i < 1000000; i++) {
//        while (used[key.intData])
//            key.intData = rand() % 100000000;
//        used[key.intData] = true;
//        saver[i] = key.intData;
//    key.type = BPTreeKeyType::INT;
//        tree.insertKeyPointerPair(key, i);
//    }
////    printTree(tree, tree.getNodeAtPage(ROOTPAGE), 1);
//    cout<<endl<<endl<<endl;
//    for (int i = 0; i < 1000000; i++) {
//        key.intData = saver[i];
//    key.type = BPTreeKeyType::INT;
//        tree.deleteKey(key);
////        cout << "deleting " << saver[i] << endl;
////    printTree(tree, tree.getNodeAtPage(ROOTPAGE), 1);
////        cout << endl << endl << endl;
//    }
//    printTree(tree, tree.getNodeAtPage(ROOTPAGE), 1);
//}
//
//void testBPTree() {
//    BPTree tree("test", "test", BPTreeKeyType::CHAR, 20);
//    BPTreeKey key;
//    key.keyLen = 20;
//    key.type = BPTreeKeyType::CHAR;
//    BPTreeNode node = tree.getNodeAtPage(ROOTPAGE);
////    for (int i = 1; i < node.entryNumber; i++) {
////        cout << node.nodeEntries[i].key.floatData << endl;
////        cout << node.nodeEntries[i].pagePointer << endl;
////        cout << tree.searchKeyForPagePointer(node.nodeEntries[i].key) << endl;
////    }
//    printTree(tree, tree.getNodeAtPage(ROOTPAGE),1);
////    cout << tree.getLeadingPage() << endl;
//    PageIndexType leading = tree.getLeadingPage();
//
//    for (;leading != UNDEFINEED_PAGE_NUM; leading = tree.getNodeAtPage(leading).siblingNodePagePointer) {
//        cout << leading << endl;
//    }
//
//    for (int i = 1; i <= 10000; i++) {
//        for (int j = 0; j < 20; j++)
//            key.charData[j] = rand() % 26 + 'a';
//        key.charData[20] = '\0';
////        cout << i << endl;
////        cout << string(key.charData) << endl;
//
////        key.floatData = rand() % 100000000;
////        key.intData = 2001;
////        cout << "insert " << key.intData << endl;
////        tree.insertKeyPointerPair(key, i);
////    printTree(tree, tree.getNodeAtPage(ROOTPAGE),1);
////        cout << endl << endl << endl << endl;
////        cout << i << endl;
////        cout << key.floatData << endl;
////        cout << tree.searchKeyForPagePointer(key) << endl;
////        cout << "orz" << endl << endl << endl;
//    }
////    node = tree.getNodeAtPage(leading);
////    for (;;) {
////        for (int i = 1; i < node.entryNumber; ++i)
////            cout << node.nodeEntries[i].key.intData << endl;
////        if (node.siblingNodePagePointer == UNDEFINEED_PAGE_NUM) break;
////        node = tree.getNodeAtPage(node.siblingNodePagePointer);
////    }
////    printTree(tree, tree.getNodeAtPage(ROOTPAGE),1);
//}
//
//void testBpNode() {
//    BPTreeNode node;
//    node.nodeType = BPTreeNodeType::BPTreeLeafNode;
//}
//
//void testKey() {
//    BPTreeKey key1;
//    key1.type = BPTreeKeyType::CHAR;
//    key1.keyLen = 20;
//    BPTreeKey key2;
//    key2 = key1;
//    memcpy(key1.charData, "aaaaaaaaaaaaaaaaaaaa", 20);
//    memcpy(key2.charData, "ffffffffffffffffffff", 20);
//    cout << key1.compare(key2) << endl;
//    cout << key2.compare(key1) << endl;
//}
//
//void play3488() {
//    BPTree tree("test2", "test2", BPTreeKeyType::INT, 4);
//    BPTreeKey key;
//    key.keyLen = 4;
//    key.type = BPTreeKeyType::INT;
//    key.intData = 34888;
//    cout << tree.searchKeyForPagePointer(key) << endl;
//    tree.deleteKey(key);
//    cout << tree.searchKeyForPagePointer(key) << endl;
//}

int main(int argc, const char * argv[]) {
    /*
//    freopen("output", "w", stdout);
//    play3488();
//    testKey();
//    testBPTree();
    //testDelete();




    BufferManager buffer;
    buffer.deleteTableFile("student");





//    Page page;
//    page.tableName = "test";
//    page.attributeName = "test";
//    page.pageType = PageType::IndexPage;
//    page.pageIndex = 2;
//
//    buffer.deallocatePage(page);
//
    CatalogManager catalog;
//
    catalog.dropTable("student");
//
    TableInfo table;
    table.tableName = "student";
    table.attrNum = 2;
    table.attrType[0] = "int";
    table.attrType[1] = "char(10)";
    table.attrName[0] = "studentid";
    table.attrName[1] = "studentname";
    table.primaryKeyLoc = 0;
    catalog.insertTable(table);


    Table table1("student");
    Attribute attr1;
    attr1.type = AttributeType::INT;
    attr1.length = 4;
    Attribute attr2;
    attr2.type = AttributeType::CHAR;
    attr2.length = 10;
    vector<Attribute> v;
    for(int i = 1; i <= 1000; i++)
    {
        v.clear();
        attr1.intdata = i;
        memcpy(attr2.chardata ,"Mary", 5);
        v.push_back(attr1);
        v.push_back(attr2);
        table1.insertTuple(v);
    }

    for(int i = 200; i <= 500; i++)
    {
        table1.deleteTuple(i);
    }


    vector<PageIndexType> v1;
    v1 = table1.scanLessEqual(0,attr1);

    for(unsigned int i = 0; i < v1.size(); i++)
        table1.printinfo(v1[i]);

//    catalog.insertIndex("student", "studentid", "studentNumber");
//    catalog.deleteIndex("studentstudentid");
//    catalog.insertIndex("student", "studentid", "anotherIndex");
//    catalog.insertIndex("student", "studentid", "Yes,Again");
//    string s;
//    printf("primarykey = %s\n",catalog.primaryKey("student").c_str());
//    printf("First index is %s\n",indexPage.readIndexName(1).c_str());
//    printf("Second index is %s\n",indexPage.readIndexName(2).c_str());
//    printf("Third index is %s\n",indexPage.readIndexName(3).c_str());
*/


    BufferManager bm;

    if (!bm.indexCatalogFileIsExist("",""))
    {
        IndexCatalogPage indexPage;
        indexPage.writeInitialPage();   //这行只在第一次运行的时候要用，用于初始化存放index的文件
    }

    cout << ">>> ";
    while (1) {
        yyparse();
    }


    return 0;
}
