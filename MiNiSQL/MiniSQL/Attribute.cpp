

#include "Attribute.hpp"

Attribute::Attribute() {
	length = 0;
	intdata = 0;
	floatdata = 0.0f;
	chardata[0] = '\0';
	type = AttributeType::UNDEFINED;
}

Attribute::Attribute(const Attribute &attr) {
	length = attr.length;
    intdata = attr.intdata;
    floatdata = attr.floatdata;
    memcpy(chardata, attr.chardata, length);
    memcpy(rawdata, attr.rawdata, 256);
    type = attr.type;
    attrName = attr.attrName;

}

Attribute::~Attribute() {

}

int Attribute::compare (const Attribute &attr) {
	assert(type == attr.type);
	assert(type != AttributeType::UNDEFINED);

	switch (type) {
		case AttributeType::INT: {
			if (intdata > attr.intdata)
				return 1;
			else if (intdata == attr.intdata)
				return 0;
			else
				return -1;
			break;
		}
		case AttributeType::FLOAT: {
			if (floatdata > attr.floatdata)
				return 1;
			else if ( fabs(floatdata - attr.floatdata) < 0.000001)
				return 0;
			else
				return -1;
			break;
		}
		case AttributeType::CHAR: {
			return strncmp(chardata, attr.chardata, length);
			break;
		}
		default:
			return 0;
			break;
	}
}

int Attribute::getKeyDataLength() {
	switch (type) {
		case AttributeType::CHAR:
			return length;
			break;
		case AttributeType::INT:
			return sizeof(int);
			break;
		case AttributeType::FLOAT:
			return sizeof(float);
			break;
		case AttributeType::UNDEFINED:
			return 0;
			break;
		default:
			break;
	}
}

void Attribute::convertToRawData() {
	switch (type) {
		case AttributeType::INT: {
			length = sizeof(int);
			memcpy(rawdata, &intdata, length);
			break;
		}
		case AttributeType::FLOAT: {
			length = sizeof(float);
			memcpy(rawdata, &floatdata, length);
			break;
		}
		case AttributeType::CHAR: {
			assert(length >= 0);
			assert(length <= 255);
			memcpy(rawdata, chardata, length);
			break;
		}

		default:
			break;
	}
}

void Attribute::parseFromRawData() {
	assert(type != AttributeType::UNDEFINED);
	length = getKeyDataLength();

	switch (type) {
		case AttributeType::INT: {
			assert(length == sizeof(int));
			memcpy(&intdata, rawdata, length);
			break;
		}
		case AttributeType::FLOAT: {
			assert(length == sizeof(float));
			memcpy(&floatdata, rawdata,length);
			break;
		}
		case AttributeType::CHAR: {
			assert(length >= 0);
			assert(length <= 255);
			memcpy(chardata, rawdata, length);
			break;
		}
		default:
			break;
	}
}

bool Attribute::operator <  (const Attribute &attr) {
	return compare(attr) < 0;
}

bool Attribute::operator == (const Attribute &attr){
	return compare(attr) == 0;
}

bool Attribute::operator != (const Attribute &attr){
	return compare(attr) != 0;
}

bool Attribute::operator >  (const Attribute &attr){
	return compare(attr) > 0;
}

bool Attribute::operator <= (const Attribute &attr){
	return (*this < attr) || (*this == attr);
}

bool Attribute::operator >= (const Attribute &attr){
	return (*this > attr) || (*this == attr);
}
