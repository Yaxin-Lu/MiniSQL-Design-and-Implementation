

#ifndef BPTreeEntry_hpp
#define BPTreeEntry_hpp

#include "BPTreeKey.hpp"
#include <iostream>
#include "Global.h"

using namespace std;

class BPTreeEntry {
public:
    
    BPTreeEntry() {}
    ~BPTreeEntry() {}
    
    BPTreeEntry(const BPTreeEntry &entry) {
        key = entry.key;
        pagePointer = entry.pagePointer;
    }
    
    int getEntryDataLength();
    
    BPTreeKey       key;
    PageIndexType   pagePointer;
};

#endif /* BPTreeEntry_hpp */
