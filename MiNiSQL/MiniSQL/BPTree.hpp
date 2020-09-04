

#ifndef BPTree_hpp
#define BPTree_hpp

#include "BPTreeKey.hpp"
#include "BPTreeEntry.hpp"
#include "BPTreeNode.hpp"
#include "BufferManager.hpp"
#include "Global.h"
#include "Page.hpp"
#include <string>

using namespace std;

class BPTree {
public:
    BPTree(string tableName, string attributeName, BPTreeKeyType keyType, int keyDataLength) {
        assert(tableName != "");
        assert(attributeName != "");
        this->tableName = tableName;
        this->attributeName = attributeName;
        this->keyType = keyType;
        this->keyDataLength = keyDataLength;
        BPTreeNode node(getNodeAtPage(ROOTPAGE));
        if (node.nodeType == BPTreeNodeType::BPTreeUndefinedNode) {
            node.nodeEntries[0].pagePointer = UNDEFINEED_PAGE_NUM;
            node.entryNumber = 1;
            node.keyDataLength = keyDataLength;
            node.keyType = keyType;
            node.parentNodePagePointer = UNDEFINEED_PAGE_NUM;
            node.siblingNodePagePointer = UNDEFINEED_PAGE_NUM;
            node.nodeType = BPTreeNodeType::BPTreeLeafNode;
            node.writeNode();
            // 当索引文件第一次被访问，初始化Root节点
        }
    }
    
    ~BPTree() {}
    
    BPTreeNode          splitLeaveNode(BPTreeNode &node);
    BPTreeNode          splitInternalNode(BPTreeNode &node);
    BPTreeNode          createNode();
    bool                deleteNode(BPTreeNode &node);
    
    PageIndexType       getLeadingPage();
    PageIndexType       getLeadingPageAtNode(BPTreeNode node);
    
    BPTreeNode          getNodeAtPage(PageIndexType pageIndex);
    
    void                updateParent(BPTreeNode node);
    
    bool                insertKeyPointerPair(BPTreeKey key, PageIndexType pagePointer);
    bool                insertEntryIntoNode(BPTreeEntry entry, BPTreeNode node);
    bool                updateEntryIntoNode(BPTreeEntry entry, BPTreeNode node);
    
    bool                deleteKey(BPTreeKey key);
    bool                deleteKeyInNode(BPTreeKey key, BPTreeNode node);
    bool                handelUnderflowInChildNodeOfNodePage(BPTreeNode node, PageIndexType childPage);
    
    
    PageIndexType       searchKeyForPagePointer(BPTreeKey key);
    PageIndexType       searchKeyForPagePointerInNode(BPTreeKey key, BPTreeNode node);
    
    bool                isRoot(BPTreeNode node);
    
    BPTreeKey           getMaxKey(BPTreeNode node);
    BPTreeKey           getMinKey(BPTreeNode node);
    
    string              tableName;
    string              attributeName;
    BPTreeKeyType       keyType;
    int                 keyDataLength;
};

#endif /* BPTree_hpp */
