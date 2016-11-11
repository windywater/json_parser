#include "Writer.h"
#include <stdio.h>

namespace json
{

Writer::Writer()
{
}

Writer::~Writer()
{
}

std::string Writer::writeValue(const Value& value)
{
	switch (value.type())
	{
	case NullType:
		return "null";
	case IntType:
	{
		char buf[16];
		int intVal;
		value.toInt(intVal);
		int len = _snprintf_s(buf, 16, "%d", intVal);
		return std::string(buf, len);
	}
	case RealType:
	{
		char buf[32];
		double realVal;
		value.toDouble(realVal);
		int len = _snprintf_s(buf, 32, "%.16g", realVal);
		return std::string(buf, len);
	}
	case BooleanType:
	{
		bool boolVal;
		value.toBoolean(boolVal);
		return boolVal ? "true" : "false";
	}
	case StringType:
	{
		std::string strVal, format;
		value.toString(strVal);
		for (std::string::size_type i = 0; i < strVal.size(); i++)
		{
			const char c = strVal.at(i);
			switch (c)
			{
			case '\"':
				format += "\\\"";
				break;
			case '\\':
				format += "\\\\";
				break;
			case '\b':
				format += "\\b";
				break;
			case '\r':
				format += "\\r";
				break;
			case '\n':
				format += "\\n";
				break;
			case '\t':
				format += "\\t";
				break;
			default:
				format += c;
			}
		}

		format = "\"" + format + "\"";
		return format;
	}
	case ArrayType:
	{
		std::string str("[");
		std::size_t size = value.size();
		for (std::size_t i = 0; i < size-1; i++)
		{
			str += writeValue(value[i]);
			str += ", ";
		}
		str += writeValue(value[size - 1]);
		str += "]";
		return str;
	}
	case ObjectType:
	{
		std::string str("{");
		std::vector<std::string> keys = value.keys();
		for (std::size_t i = 0; i < keys.size(); i++)
		{
			const std::string key = keys.at(i);
			std::string valStr = writeValue(value[key]);
			str += "\"" + key + "\"";
			str += ": ";
			str += valStr;

			if (i != (keys.size() - 1))
				str += ", ";
		}
		str += "}";
		return str;
	}
	default:
		return "";
	}
}

}