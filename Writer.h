#pragma once
#include "Value.h"

namespace json
{

class Writer
{
public:
	Writer();
	~Writer();
	std::string writeValue(const Value& value);
private:

};

}