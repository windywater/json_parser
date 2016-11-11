#pragma once
#include <string>
#include <map>
#include <vector>

namespace json
{

enum ValueType
{
	NullType = 0,
	IntType,
	RealType,
	BooleanType,
	StringType,
	ArrayType,
	ObjectType
};

class Value
{
public:
	Value();
	Value(ValueType type);
	Value(const Value& other);
	ValueType type();

	Value(int intValue);
	Value(double doubleValue);
	Value(const char* strValue);
	Value(const char* strValue, std::size_t length);
	Value(const std::string& strValue);
	Value(bool boolValue);
	~Value();

	bool isNull() const;
	bool isNumber() const;
	bool isBoolean() const;
	bool isString() const;
	bool isArray() const;
	bool isObject() const;

	bool toInt(int& intValue);
	bool toDouble(double& doubleValue);
	bool toFloat(float& floatValue);
	bool toString(std::string& strValue);
	bool toBoolean(bool& boolValue);

	bool append(const Value& value);
	bool insert(std::size_t pos, const Value& value);
	bool removeIndex(std::size_t index);
	bool removeMember(const std::string& key);
	int size() const;

	Value& operator=(const Value& other);
	bool operator==(const Value& other) const;	
	const Value& operator[](std::size_t index) const;
	Value& operator[](std::size_t index);

	const Value& operator[](const std::string& key) const;
	Value& operator[](const std::string& key);

private:
	void destroy();
	void assign(const Value& otherValue);

	static const Value& nullValue;
private:
	typedef std::string StringValueType;
	typedef std::vector<Value> ArrayValueType;
	typedef std::map<std::string, Value> ObjectValueType;

	union 
	{
		int intValue;
		double doubleValue;
		bool boolValue;
		StringValueType* stringValue;
		ArrayValueType* arrayValue;
		ObjectValueType* objectValue;
	} m_value;

	ValueType m_type;
};

}