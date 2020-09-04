

#include "BPTree.hpp"

void BPTree::updateParent(BPTreeNode node) {
    if (node.nodeType == BPTreeNodeType::BPTreeInternalNode) {
        for (int i = 0; i < node.entryNumber; i++) {
            BPTreeNode tempNode;
            tempNode = getNodeAtPage(node.nodeEntries[i].pagePointer);
            tempNode.parentNodePagePointer = node.nodePage.pageIndex;
            tempNode.writeNode();
        }
    }
}

BPTreeNode BPTree::splitLeaveNode(BPTreeNode &node) {
    BPTreeNode newNode(createNode());
    Page tempPage = newNode.nodePage;
    newNode = node;
    newNode.nodePage = tempPage;
    node.siblingNodePagePointer = newNode.nodePage.pageIndex;
    
    int mid = node.entryNumber / 2;
    node.entryNumber = mid;
    
    for (int i = mid; i < newNode.entryNumber; ++i) {
        newNode.nodeEntries[i - mid + 1] = newNode.nodeEntries[i];
    }
    newNode.entryNumber -= mid - 1;
    return newNode;
}

BPTreeNode BPTree::splitInternalNode(BPTreeNode &node) {
    BPTreeNode newNode = createNode();
    Page tempPage = newNode.nodePage;
    newNode = node;
    newNode.nodePage = tempPage;
    node.siblingNodePagePointer = newNode.nodePage.pageIndex;
    
    int mid = node.entryNumber / 2;
    node.entryNumber = mid;
    
    for (int i = mid; i < newNode.entryNumber; ++i) {
        newNode.nodeEntries[i - mid] = newNode.nodeEntries[i];
    }
    newNode.entryNumber -= mid;
    return newNode;
}

BPTreeNode BPTree::createNode() {
    BPTreeNode node;
    node.nodeEntries[0].pagePointer = UNDEFINEED_PAGE_NUM;
    node.entryNumber = 1;
    node.keyDataLength = keyDataLength;
    node.keyType = keyType;
    node.parentNodePagePointer = UNDEFINEED_PAGE_NUM;
    node.siblingNodePagePointer = UNDEFINEED_PAGE_NUM;
    node.nodeType = BPTreeNodeType::BPTreeUndefinedNode;
    
    node.nodePage.tableName = tableName;
    node.nodePage.attributeName = attributeName;
    node.nodePage.pageType = PageType::IndexPage;
    BufferManager bufferManager;
    bufferManager.allocatePage(node.nodePage);
    return node;
}

bool BPTree::deleteNode(BPTreeNode &node) {
    BufferManager bufferManager;
    return bufferManager.deallocatePage(node.nodePage);
}

PageIndexType BPTree::getLeadingPage() {
    return getLeadingPageAtNode(getNodeAtPage(ROOTPAGE));
}

PageIndexType BPTree::getLeadingPageAtNode(BPTreeNode node) {
    assert(node.nodeType != BPTreeNodeType::BPTreeUndefinedNode);
    assert(!node.isEmpty());
    if (node.nodeType == BPTreeNodeType::BPTreeLeafNode) {
        return node.nodePage.pageIndex;
    } else {
        return getLeadingPageAtNode(getNodeAtPage(node.nodeEntries[0].pagePointer));
    }
}

BPTreeNode BPTree::getNodeAtPage(PageIndexType pageIndex) {
    BPTreeNode node;
    node.nodePage.tableName     = tableName;
    node.nodePage.attributeName = attributeName;
    node.nodePage.pageType      = PageType::IndexPage;
    node.nodePage.pageIndex     = pageIndex;
    node.readNode();
    return node;
}

bool BPTree::insertKeyPointerPair(BPTreeKey key, PageIndexType pagePointer) {
    assert(key.type == keyType);
    assert(pagePointer != UNDEFINEED_PAGE_NUM);
    assert(key.keyLen == keyDataLength);
    BPTreeEntry entry;
    entry.key         = key;
    entry.pagePointer = pagePointer;
    return this->insertEntryIntoNode(entry, getNodeAtPage(ROOTPAGE));
}

bool BPTree::insertEntryIntoNode(BPTreeEntry entry, BPTreeNode node) {
    assert(node.nodeType != BPTreeNodeType::BPTreeUndefinedNode);
    if (node.nodeType == BPTreeNodeType::BPTreeInternalNode) {
        if (entry.key < node.nodeEntries[1].key) {
            return this->insertEntryIntoNode(entry, getNodeAtPage(node.nodeEntries[0].pagePointer));
        } else {
            for (int i = node.entryNumber - 1; i > 0; --i) {
                if (entry.key >= node.nodeEntries[i].key) {
                    return this->insertEntryIntoNode(entry, getNodeAtPage(node.nodeEntries[i].pagePointer));
                }
            }
        }
    } else if (node.nodeType == BPTreeNodeType::BPTreeLeafNode) {
        node.insertEntry(entry);
        if (!node.isOverflow()) {
            node.writeNode();
            return true;
        } else {
            if (!isRoot(node)) {
                BPTreeNode newNode = splitLeaveNode(node);
                node.writeNode();
                updateParent(newNode);
                newNode.writeNode();
                BPTreeEntry entry;
                entry.key = getMinKey(newNode);
                entry.pagePointer = newNode.nodePage.pageIndex;
                return updateEntryIntoNode(entry, getNodeAtPage(node.parentNodePagePointer));
            } else {
                BPTreeNode leftNode = createNode();
                Page tempPage = leftNode.nodePage;
                leftNode = node;
                leftNode.nodePage = tempPage;
                BPTreeNode rightNode = splitLeaveNode(leftNode);
                
                leftNode.parentNodePagePointer = node.nodePage.pageIndex;
                rightNode.parentNodePagePointer = node.nodePage.pageIndex;
                
                updateParent(leftNode);
                updateParent(rightNode);
                
                leftNode.writeNode();
                rightNode.writeNode();
                
                node.clearNode();
                node.entryNumber = 2;
                node.nodeEntries[0].pagePointer = leftNode.nodePage.pageIndex;
                node.nodeEntries[1].pagePointer = rightNode.nodePage.pageIndex;
                node.nodeEntries[1].key = getMinKey(rightNode);
                node.nodeType = BPTreeNodeType::BPTreeInternalNode;
                node.writeNode();
                return true;
            }
        }
    }
    assert(false); // Error
    return false;
}

bool BPTree::updateEntryIntoNode(BPTreeEntry entry, BPTreeNode node) {
    assert(node.nodeType != BPTreeNodeType::BPTreeUndefinedNode);
    node.insertEntry(entry);
    if (!node.isOverflow()) {
        node.writeNode();
        return true;
    } else {
        if (!isRoot(node)) {
            BPTreeNode newNode = splitInternalNode(node);
            node.writeNode();
            updateParent(newNode);
            newNode.writeNode();
            BPTreeEntry entry;
            entry.key = getMinKey(newNode);
            entry.pagePointer = newNode.nodePage.pageIndex;
            return updateEntryIntoNode(entry, getNodeAtPage(node.parentNodePagePointer));
        } else {
            BPTreeNode leftNode = createNode();
            Page tempPage = leftNode.nodePage;
            leftNode = node;
            leftNode.nodePage = tempPage;
            BPTreeNode rightNode = splitInternalNode(leftNode);
            
            leftNode.parentNodePagePointer = node.nodePage.pageIndex;
            rightNode.parentNodePagePointer = node.nodePage.pageIndex;
            
            updateParent(leftNode);
            updateParent(rightNode);
            
            leftNode.writeNode();
            rightNode.writeNode();
            
            node.clearNode();
            node.entryNumber = 2;
            node.nodeEntries[0].pagePointer = leftNode.nodePage.pageIndex;
            node.nodeEntries[1].pagePointer = rightNode.nodePage.pageIndex;
            node.nodeEntries[1].key = getMinKey(rightNode);
            node.nodeType = BPTreeNodeType::BPTreeInternalNode;
            node.writeNode();
            return true;
        }
    }
    assert(false);
    return false;
}

bool BPTree::deleteKey(BPTreeKey key) {
    return deleteKeyInNode(key, getNodeAtPage(ROOTPAGE));
}

bool BPTree::deleteKeyInNode(BPTreeKey key, BPTreeNode node) {
    assert(key.keyLen == node.keyDataLength);
    assert(key.type == node.keyType);
    if (node.nodeType == BPTreeNodeType::BPTreeInternalNode) {
        if (key < node.nodeEntries[1].key) {
            return deleteKeyInNode(key, getNodeAtPage(node.nodeEntries[0].pagePointer));
        } else {
            for (int i = node.entryNumber - 1; i > 0; --i) {
                if (key >= node.nodeEntries[i].key) {
                    return deleteKeyInNode(key, getNodeAtPage(node.nodeEntries[i].pagePointer));
                }
            }
        }
    } else if (node.nodeType == BPTreeNodeType::BPTreeLeafNode) {
        node.deleteEntry(key);
        node.writeNode();
        if (!node.isUnderflow()) {
            return true;
        } else {
            if (isRoot(node)) {
                return true; // Root node underflow do nothing
            } else {
                return handelUnderflowInChildNodeOfNodePage(getNodeAtPage(node.parentNodePagePointer), node.nodePage.pageIndex);
            }
        }
    }
    
    assert(false);
    return false;
}

bool BPTree::handelUnderflowInChildNodeOfNodePage(BPTreeNode node, PageIndexType childPage) {
    assert(!node.isEmpty());
    assert(node.entryNumber >= 2);
    PageIndexType siblingPage;
    if (childPage == node.nodeEntries[node.entryNumber - 1].pagePointer) {
        siblingPage = childPage;
        childPage = node.nodeEntries[node.entryNumber - 2].pagePointer;
    } else {
        siblingPage = getNodeAtPage(childPage).siblingNodePagePointer;
    }
    
    int childIndex = 0, siblingIndex = 0;
    for (int i = 0; i < node.entryNumber; ++i) {
        if (node.nodeEntries[i].pagePointer == childPage) childIndex = i;
        if (node.nodeEntries[i].pagePointer == siblingPage) siblingIndex = i;
    }
    
    BPTreeNode childNode = getNodeAtPage(childPage);
    BPTreeNode siblingNode = getNodeAtPage(siblingPage);
    
    if (childNode.nodeType == BPTreeNodeType::BPTreeInternalNode) {
        siblingNode.nodeEntries[0].key = getMinKey(getNodeAtPage(siblingNode.nodeEntries[0].pagePointer));
        
        int biggerLenghth = childNode.nodeEntries[0].getEntryDataLength() * int(ceil((double)(childNode.entryNumber + siblingNode.entryNumber) / 2.0)) + sizeof(BPTreeNodeHeader);
        if (biggerLenghth < (PAGESIZE / 2)) { // 此时可以merge
            childNode.siblingNodePagePointer = siblingNode.siblingNodePagePointer;
            for (int i = childNode.entryNumber; i < childNode.entryNumber + siblingNode.entryNumber; ++i) {
                childNode.nodeEntries[i] = siblingNode.nodeEntries[i - childNode.entryNumber];
            }
            childNode.entryNumber += siblingNode.entryNumber; //merge 进childnode
            
            for (int i = siblingIndex; i < node.entryNumber; ++i)
                node.nodeEntries[i] = node.nodeEntries[i + 1];
            node.entryNumber--;
            node.writeNode();
            
            updateParent(childNode);
            childNode.writeNode();
            deleteNode(siblingNode);
            
            if (node.isUnderflow()) {
                if (isRoot(node)) {
                    if (node.entryNumber == 1) {
                        BPTreeNode tempNode = getNodeAtPage(node.nodeEntries[0].pagePointer);
                        Page tempPage = node.nodePage;
                        node = tempNode;
                        node.nodePage = tempPage;
                        updateParent(node);
                        node.writeNode();
                        deleteNode(tempNode);
                        return true;
                    } else {
                        return true;
                    }
                } else {
                    return handelUnderflowInChildNodeOfNodePage(getNodeAtPage(node.parentNodePagePointer), node.nodePage.pageIndex);
                }
            } else {
                return true;
            }
        } else { // 此时需要 redistribute
            int tot = (childNode.entryNumber + siblingNode.entryNumber);
            int mid = tot / 2;
            BPTreeEntry *entries = new BPTreeEntry[tot];
            for (int i = 0; i < childNode.entryNumber; ++i)
                entries[i] = childNode.nodeEntries[i];
            for (int i = 0; i < siblingNode.entryNumber; ++i)
                entries[i + childNode.entryNumber] = siblingNode.nodeEntries[i];
            
            childNode.entryNumber = mid;
            siblingNode.entryNumber = tot - mid;
            
            for (int i = 0; i < childNode.entryNumber; ++i)
                childNode.nodeEntries[i] = entries[i];
            for (int i = 0; i < siblingNode.entryNumber; ++i)
                siblingNode.nodeEntries[i] = entries[i + childNode.entryNumber];
            
            updateParent(childNode);
            updateParent(siblingNode);
            
            childNode.writeNode();
            siblingNode.writeNode();
            
            node.nodeEntries[childIndex].key = getMinKey(childNode);
            node.nodeEntries[siblingIndex].key = getMinKey(siblingNode);
            node.writeNode();
            
            delete[] entries;
            return true;
        }
    } else {
        int biggerLenghth = childNode.nodeEntries[0].getEntryDataLength() * int(ceil((double)(childNode.entryNumber + siblingNode.entryNumber) / 2.0)) + sizeof(BPTreeNodeHeader);
        if (biggerLenghth < (PAGESIZE / 2)) { // 此时可以merge
            childNode.siblingNodePagePointer = siblingNode.siblingNodePagePointer;
            for (int i = childNode.entryNumber; i < childNode.entryNumber + siblingNode.entryNumber - 1; ++i) {
                childNode.nodeEntries[i] = siblingNode.nodeEntries[i - childNode.entryNumber + 1];
            }
            childNode.entryNumber += siblingNode.entryNumber - 1; //merge 进childnode
            
            for (int i = siblingIndex; i < node.entryNumber; ++i)
                node.nodeEntries[i] = node.nodeEntries[i + 1];
            node.entryNumber--;
            node.writeNode();
            
            updateParent(childNode);
            childNode.writeNode();
            deleteNode(siblingNode);
            
            if (node.isUnderflow()) {
                if (isRoot(node)) {
                    if (node.entryNumber == 1) {
                        BPTreeNode tempNode = getNodeAtPage(node.nodeEntries[0].pagePointer);
                        Page tempPage = node.nodePage;
                        node = tempNode;
                        node.nodePage = tempPage;
                        updateParent(node);
                        node.writeNode();
                        deleteNode(tempNode);
                        return true;
                    } else {
                        return true;
                    }
                } else {
                    return handelUnderflowInChildNodeOfNodePage(getNodeAtPage(node.parentNodePagePointer), node.nodePage.pageIndex);
                }
            } else {
                return true;
            }
        } else { // 此时需要 redistribute
            int tot = (childNode.entryNumber + siblingNode.entryNumber - 2);
            int mid = tot / 2;
            BPTreeEntry *entries = new BPTreeEntry[tot];
            for (int i = 1; i < childNode.entryNumber; ++i)
                entries[i - 1] = childNode.nodeEntries[i];
            for (int i = 1; i < siblingNode.entryNumber; ++i)
                entries[i + childNode.entryNumber - 2] = siblingNode.nodeEntries[i];
            
            childNode.entryNumber = mid;
            siblingNode.entryNumber = tot - mid;
            
            childNode.entryNumber++;
            siblingNode.entryNumber++;
            
            for (int i = 1; i < childNode.entryNumber; ++i)
                childNode.nodeEntries[i] = entries[i - 1];
            for (int i = 1; i < siblingNode.entryNumber; ++i)
                siblingNode.nodeEntries[i] = entries[i + childNode.entryNumber - 2];
            
            updateParent(childNode);
            updateParent(siblingNode);
            
            childNode.writeNode();
            siblingNode.writeNode();
            
            node.nodeEntries[childIndex].key = getMinKey(childNode);
            node.nodeEntries[siblingIndex].key = getMinKey(siblingNode);
            node.writeNode();
            
            delete[] entries;
            return true;
        }
        
    }
    assert(false);
    return false;
}

PageIndexType BPTree::searchKeyForPagePointer(BPTreeKey key) {
    assert(key.type == keyType);
    assert(key.keyLen == keyDataLength);
    return searchKeyForPagePointerInNode(key, getNodeAtPage(ROOTPAGE));
}

PageIndexType BPTree::searchKeyForPagePointerInNode(BPTreeKey key, BPTreeNode node) {
    assert(key.keyLen == node.keyDataLength);
    assert(key.type == node.keyType);
    if (node.nodeType == BPTreeNodeType::BPTreeLeafNode) {
        for (int i = 0; i < node.entryNumber; ++i) {
            if (node.nodeEntries[i].key == key)
                return node.nodeEntries[i].pagePointer;
        }
    } else if (node.nodeType == BPTreeNodeType::BPTreeInternalNode) {
        if (key < node.nodeEntries[1].key) {
            return searchKeyForPagePointerInNode(key, getNodeAtPage(node.nodeEntries[0].pagePointer));
        } else {
            for (int i = node.entryNumber - 1; i > 0; --i) {
                if (key >= node.nodeEntries[i].key)
                    return searchKeyForPagePointerInNode(key, getNodeAtPage(node.nodeEntries[i].pagePointer));
            }
        }
    }
    return UNDEFINEED_PAGE_NUM;
    assert(false); // Error
}

bool BPTree::isRoot(BPTreeNode node) {
    return node.nodePage.pageIndex == ROOTPAGE;
}

BPTreeKey BPTree::getMaxKey(BPTreeNode node) {
    assert(node.nodeType != BPTreeNodeType::BPTreeUndefinedNode);
    assert(!node.isEmpty());
    if (node.nodeType == BPTreeNodeType::BPTreeLeafNode) {
        return node.nodeEntries[node.entryNumber - 1].key;
    } else if (node.nodeType == BPTreeNodeType::BPTreeInternalNode){
        return getMaxKey(getNodeAtPage(node.nodeEntries[node.entryNumber - 1].pagePointer));
    }
    assert(false); // Error
}

BPTreeKey BPTree::getMinKey(BPTreeNode node) {
    assert(node.nodeType != BPTreeNodeType::BPTreeUndefinedNode);
    assert(!node.isEmpty());
    if (node.nodeType == BPTreeNodeType::BPTreeLeafNode) {
        return node.nodeEntries[1].key;
    } else if (node.nodeType == BPTreeNodeType::BPTreeInternalNode) {
        return getMinKey(getNodeAtPage(node.nodeEntries[0].pagePointer));
    }
    assert(false); // Error
}
