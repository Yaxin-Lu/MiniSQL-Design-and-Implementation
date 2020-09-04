
#include "BPTreeKey.hpp"

BPTreeKey::BPTreeKey() {
    keyLen = 0;
    intData = 0;
    floatData = 0.0f;
    charData[0] = '\0';
    type = BPTreeKeyType::UNDEFINED;
}

BPTreeKey::BPTreeKey(const BPTreeKey &key) {
    keyLen = key.keyLen;
    intData = key.intData;
    floatData = key.floatData;
    memcpy(charData, key.charData, keyLen);
    memcpy(rawData, key.rawData, 256);
    type = key.type;
}

BPTreeKey::BPTreeKey(const Attribute &attri) {
    keyLen = attri.length;
    intData = attri.intdata;
    floatData = attri.floatdata;
    memcpy(charData, attri.chardata, keyLen);
    memcpy(rawData, attri.rawdata, 256);
    switch (attri.type) {
        case AttributeType::INT :
            type = BPTreeKeyType::INT;
            break;
        
        case AttributeType::FLOAT :
            type = BPTreeKeyType::FLOAT;
            break;
            
        case AttributeType::CHAR :
            type = BPTreeKeyType::CHAR;
            break;
            
        default:
            type = BPTreeKeyType::UNDEFINED;
            break;
    }
}

BPTreeKey::~BPTreeKey() {
}

int BPTreeKey::getKeyDataLength() {
    switch (type) {
        case BPTreeKeyType::CHAR:
            return keyLen;
            break;
        
        case BPTreeKeyType::FLOAT:
            return sizeof(float);
            break;
            
        case BPTreeKeyType::INT:
            return sizeof(int);
            break;
            
        case BPTreeKeyType::UNDEFINED:
            return 0;
            break;
            
        default:
            break;
    }
}

int BPTreeKey::compare(const BPTreeKey &key) {
    assert(type == key.type);
    assert(type != BPTreeKeyType::UNDEFINED);
    switch (type) {
        case BPTreeKeyType::INT: {
            if (intData > key.intData)
                return 1;
            else if (intData == key.intData)
                return 0;
            else return -1;
            break;
        }
        
        case BPTreeKeyType::FLOAT: {
            if (floatData > key.floatData)
                return 1;
            else if ( fabs(floatData - key.floatData) < 0.000001)
                return 0;
            else return -1;
            break;
        }
            
        case BPTreeKeyType::CHAR: {
            return strncmp(charData, key.charData, keyLen);
            break;
        }
            
        default:
            return 0;
            break;
    }
}

void BPTreeKey::convertToRawData() {
//    assert(type != BPTreeKeyType::UNDEFINED); For leaf
    
    switch (type) {
        case BPTreeKeyType::INT: {
            keyLen = sizeof(int);
            memcpy(rawData, &intData, keyLen);
            break;
        }
            
        case BPTreeKeyType::FLOAT: {
            keyLen = sizeof(float);
            memcpy(rawData, &floatData, keyLen);
            break;
        }
            
        case BPTreeKeyType::CHAR: {
            assert(keyLen >= 0);
            assert(keyLen <= 255);
            memcpy(rawData, charData, keyLen);
            break;
        }
            
        default:
            break;
    }
}

void BPTreeKey::parseFromRawData() {
    assert(type != BPTreeKeyType::UNDEFINED);
    keyLen = getKeyDataLength();
    switch (type) {
        case BPTreeKeyType::INT: {
            assert(keyLen == sizeof(int));
            memcpy(&intData, rawData, keyLen);
            break;
        }
            
        case BPTreeKeyType::FLOAT: {
            assert(keyLen == sizeof(float));
            memcpy(&floatData, rawData, keyLen);
            break;
        }
            
        case BPTreeKeyType::CHAR: {
            assert(keyLen >= 0);
            assert(keyLen <= 255);
            memcpy(charData, rawData, keyLen);
            break;
        }
            
        default:
            break;
    }
}

bool BPTreeKey::operator< (const BPTreeKey &key) {
    return compare(key) < 0;
}

bool BPTreeKey::operator==(const BPTreeKey &key) {
    return compare(key) == 0;
}

bool BPTreeKey::operator> (const BPTreeKey &key) {
    return compare(key) > 0;
}

bool BPTreeKey::operator>=(const BPTreeKey &key) {
    return (*this > key) || (*this == key);
}
