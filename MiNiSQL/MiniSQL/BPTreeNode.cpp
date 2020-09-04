

#include "BPTreeNode.hpp"

void BPTreeNode::readNodeRawData() {
    BufferManager bufferManager;
    bufferManager.readPage(nodePage);
}

void BPTreeNode::writeNodeRawData() {
    BufferManager bufferManager;
    bufferManager.writePage(nodePage);
}

void BPTreeNode::convertToRawData() {
    char *cursor = nodePage.pageData;
    BPTreeNodeHeader &nodeHeader = (BPTreeNodeHeader&)(*cursor);
    
    nodeHeader.entryNumber = entryNumber;
    nodeHeader.keyDataLength = keyDataLength;
    nodeHeader.keyType = keyType;
    nodeHeader.parentNodePagePointer = parentNodePagePointer;
    nodeHeader.siblingNodePagePointer = siblingNodePagePointer;
    nodeHeader.nodeType = nodeType;
    
    cursor += sizeof(BPTreeNodeHeader);

    for (int i = 0; i < entryNumber; ++i) {
        nodeEntries[i].key.convertToRawData();
        memcpy(cursor, nodeEntries[i].key.rawData, nodeEntries[i].key.getKeyDataLength());
        cursor += nodeEntries[i].key.getKeyDataLength();
        memcpy(cursor, &nodeEntries[i].pagePointer, sizeof(PageIndexType));
        cursor += sizeof(PageIndexType);
    }
}

void BPTreeNode::parseFromRawData() {
    char *cursor = nodePage.pageData;
    BPTreeNodeHeader &nodeHeader = (BPTreeNodeHeader&)(*nodePage.pageData);
    
    entryNumber = nodeHeader.entryNumber;
    keyDataLength = nodeHeader.keyDataLength;
    keyType = nodeHeader.keyType;
    parentNodePagePointer = nodeHeader.parentNodePagePointer;
    siblingNodePagePointer = nodeHeader.siblingNodePagePointer;
    nodeType = nodeHeader.nodeType;
    
    cursor += sizeof(BPTreeNodeHeader);
    
    for (int i = 0; i < entryNumber; ++i) {
        nodeEntries[i].key.type = keyType;
        nodeEntries[i].key.keyLen = keyDataLength;
        memcpy(nodeEntries[i].key.rawData, cursor, nodeEntries[i].getEntryDataLength());
        nodeEntries[i].key.parseFromRawData();
        cursor += nodeEntries[i].key.getKeyDataLength();
        memcpy(&nodeEntries[i].pagePointer, cursor, sizeof(PageIndexType));
        cursor += sizeof(PageIndexType);
    }
}

void BPTreeNode::clearNode() {
    entryNumber = 1;
    parentNodePagePointer = UNDEFINEED_PAGE_NUM;
    siblingNodePagePointer = UNDEFINEED_PAGE_NUM;
}


bool BPTreeNode::isOverflow() {
    return getNodeRawDataLength() > PAGESIZE;
}

bool BPTreeNode::isUnderflow() {
    return getNodeRawDataLength() < (PAGESIZE / 2);
}

bool BPTreeNode::isLeaf() {
    return nodeType == BPTreeNodeType::BPTreeLeafNode;
}

bool BPTreeNode::isEmpty() {
    assert(entryNumber >= 0);
    return (entryNumber <= 1);
}

bool BPTreeNode::insertEntry(BPTreeEntry entry) {
    assert(entry.key.type != BPTreeKeyType::UNDEFINED);
    int insertPoint;
    for (insertPoint = 0; insertPoint < entryNumber; ++insertPoint)
        if (nodeEntries[insertPoint].key > entry.key) break;
    return insertEntryAtIndex(entry, insertPoint);
}

bool BPTreeNode::deleteEntry(BPTreeKey key) {
    assert(key.type == keyType);
    int deletePoint;
    for (deletePoint = 1; deletePoint < entryNumber; ++deletePoint) {
        if (nodeEntries[deletePoint].key == key) break;
    }
    if (deletePoint == entryNumber) return false;
    return deleteEntryAtIndex(deletePoint);
}

bool BPTreeNode::insertEntryAtIndex(BPTreeEntry entry, int index) {
    assert(index >= 0);
    assert(index <= entryNumber);
    for (int i = entryNumber - 1; i >= index; --i) {
        nodeEntries[i + 1]  = nodeEntries[i];
    }
    nodeEntries[index] = entry;
    entryNumber++;
    return true;
}

bool BPTreeNode::deleteEntryAtIndex(int index) {
    assert(index >= 0);
    assert(index < entryNumber);
    for (int i = index; i < entryNumber; ++i) {
        nodeEntries[i] = nodeEntries[i + 1];
    }
    entryNumber--;
    return true;
}

int BPTreeNode::getNodeRawDataLength() {
    int accumulator = 0;
    
    accumulator += sizeof(BPTreeNodeHeader);

    for (int i = 0; i < entryNumber; ++i) {
        accumulator += nodeEntries[i].getEntryDataLength();
    }

    return accumulator;
}

PageIndexType BPTreeNode::getPagePointerForKey(BPTreeKey key) {
    assert(key.type != BPTreeKeyType::UNDEFINED);
    if (key < nodeEntries[1].key) return nodeEntries[0].pagePointer;
    else {
        for (int i = 1; i < entryNumber; i++)
            if (key >= nodeEntries[i].key)
                return nodeEntries[i].pagePointer;
    }
    assert(false); // pointer not found
}
