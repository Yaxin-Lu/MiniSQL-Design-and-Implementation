

#ifndef BPTreeKey_hpp
#define BPTreeKey_hpp

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include "Attribute.hpp"

using namespace std;

static const int MAXCHARLENGTH = 256;

enum class BPTreeKeyType {
    UNDEFINED,
    CHAR,
    FLOAT,
    INT
};

class BPTreeKey {
public:
    BPTreeKey();
    BPTreeKey(const BPTreeKey &key);
    BPTreeKey(const Attribute &attri);
    ~BPTreeKey();
    
    int compare(const BPTreeKey &key);
    int getKeyDataLength();
    void convertToRawData();
    void parseFromRawData();
    
    bool operator<  (const BPTreeKey &key);
    bool operator== (const BPTreeKey &key);
    bool operator>  (const BPTreeKey &key);
    bool operator>= (const BPTreeKey &key);
    
    BPTreeKeyType   type;
    char            charData[256];
    char            rawData[256];
    int             intData;
    float           floatData;
    int             keyLen;
};

#endif /* BPTreeKey_hpp */
