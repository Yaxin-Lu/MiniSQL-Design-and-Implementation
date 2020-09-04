
#ifndef BPTreeNode_hpp
#define BPTreeNode_hpp

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include "Global.h"
#include "BPTreeKey.hpp"
#include "BPTreeEntry.hpp"
#include "Page.hpp"
#include "BufferManager.hpp"

using namespace std;

enum class BPTreeNodeType {
    BPTreeUndefinedNode = 0,
    BPTreeInternalNode,
    BPTreeLeafNode
};

struct BPTreeNodeHeader {
    int             entryNumber;
    int             keyDataLength;
    BPTreeKeyType   keyType;
    PageIndexType   parentNodePagePointer;
    PageIndexType   siblingNodePagePointer;
    BPTreeNodeType  nodeType;
};

class BPTreeNode {
public:
    explicit BPTreeNode() {
        nodeEntries            = new BPTreeEntry[1024];
        entryNumber            = 1; //第一个entry只会用指针部分
        keyDataLength          = 0;
        keyType                = BPTreeKeyType::UNDEFINED;
        parentNodePagePointer  = UNDEFINEED_PAGE_NUM;
        siblingNodePagePointer = UNDEFINEED_PAGE_NUM;
        nodeType               = BPTreeNodeType::BPTreeUndefinedNode;
    }

    BPTreeNode(const BPTreeNode &node) {
        nodeEntries            = new BPTreeEntry[1024];
        entryNumber            = node.entryNumber;
        keyDataLength          = node.keyDataLength;
        keyType                = node.keyType;
        parentNodePagePointer  = node.parentNodePagePointer;
        siblingNodePagePointer = node.siblingNodePagePointer;
        nodePage               = node.nodePage;
        nodeType               = node.nodeType;
        for (int i = 0; i < entryNumber; ++i)
            nodeEntries[i] = node.nodeEntries[i];
    }
    ~BPTreeNode() {
        if (nodeEntries != NULL) delete [] nodeEntries;
        nodeEntries = NULL;
    }
    
    BPTreeNode& operator=(const BPTreeNode &node) {
        entryNumber            = node.entryNumber;
        keyDataLength          = node.keyDataLength;
        keyType                = node.keyType;
        parentNodePagePointer  = node.parentNodePagePointer;
        siblingNodePagePointer = node.siblingNodePagePointer;
        nodePage               = node.nodePage;
        nodeType               = node.nodeType;
        for (int i = 0; i < entryNumber; ++i)
            nodeEntries[i] = node.nodeEntries[i];
        return *this;
    }
    

    void            readNodeRawData();
    void            writeNodeRawData();

    void            convertToRawData();
    void            parseFromRawData();
    
    void            readNode() {
        readNodeRawData();
        parseFromRawData();
    }
    void            writeNode() {
        convertToRawData();
        writeNodeRawData();
    }
    
    void            clearNode();

    bool            isOverflow();
    bool            isUnderflow();
    bool            isLeaf();
    bool            isEmpty();

    bool            insertEntry(BPTreeEntry entry);
    bool            deleteEntry(BPTreeKey key);

    bool            insertEntryAtIndex(BPTreeEntry entry, int index);
    bool            deleteEntryAtIndex(int index);

    int             getNodeRawDataLength();
    
    PageIndexType   getPagePointerForKey(BPTreeKey key);

    BPTreeEntry     *nodeEntries;
    int             entryNumber;
    int             keyDataLength;
    BPTreeKeyType   keyType;
    PageIndexType   parentNodePagePointer;
    PageIndexType   siblingNodePagePointer;
    Page            nodePage;
    BPTreeNodeType  nodeType;
};

#endif /* BPTreeNode_hpp */
