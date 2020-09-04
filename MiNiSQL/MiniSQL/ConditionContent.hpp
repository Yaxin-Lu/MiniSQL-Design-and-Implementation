

#ifndef ConditionContent_hpp
#define ConditionContent_hpp

#include <cstdio>
#include <string>
using namespace std;

//这个class，不仅用于where后的条件，也用于insert的values（懒）
class ConditionContent
{
public:
    string attrName;
    string attrValueStr;
    int attrValueInt;
    float attrValueFlo;
    string attrType; //"INT"、"FLOAT"、"CHAR"
    string op; // "="、"<>"、"<"、">"、"<="、">="
};

#endif /* ConditionContent_hpp */
