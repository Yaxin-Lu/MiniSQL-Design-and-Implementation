

#include "BPTreeEntry.hpp"

int BPTreeEntry::getEntryDataLength() {
    return sizeof(pagePointer) + key.getKeyDataLength();
}
