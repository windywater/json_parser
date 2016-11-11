#include "Value.h"
#include <assert.h>

namespace json
{

const Value& Value::nullValue(NullType);

Value::Value()
{
	m_type = NullType;
}

Value::Value(ValueType type)
{
	m_type = type;
	switch (type)
	{
	case NullType:
		break;
	case IntType:
		m_value.intValue = 0;
		break;
	case RealType:
		m_value.doubleValue = 0;
		break;
	case BooleanType:
		m_value.boolValue = false;
		break;
	case StringType:
		m_value.stringValue = new StringValueType;
		break;
	case ArrayType:
		m_value.arrayValue = new ArrayValueType;
		break;
	case ObjectType:
		m_value.objectValue = new ObjectValueType;
		break;
	default:
		break;
	}
}

ValueType Value::type()
{
	return m_type;
}

void Value::destroy()
{
	switch (m_type)
	{
	case StringType:
		delete m_value.stringValue;
		break;
	case ArrayType:
		delete m_value.arrayValue;
		break;
	case ObjectType:
		delete m_value.objectValue;
	}

	m_type = NullType;
}

void Value::assign(const Value& other)
{
	destroy();

	m_type = other.m_type;
	switch (m_type)
	{
	case NullType:
	case IntType:
	case RealType:
	case BooleanType:
		m_value = other.m_value;
		break;
	case StringType:
		m_value.stringValue = new StringValueType(*other.m_value.stringValue);
		break;
	case ArrayType:
		m_value.arrayValue = new ArrayValueType(*other.m_value.arrayValue);
		break;
	case ObjectType:
		m_value.objectValue = new ObjectValueType(*other.m_value.objectValue);
		break;
	default:
		break;
	}
}

Value::Value(const Value& other)
{
	assign(other);
}

Value::Value(int intValue)
{
	m_type = IntType;
	m_value.intValue = intValue;
}

Value::Value(double doubleValue)
{
	m_type = RealType;
	m_value.doubleValue = doubleValue;
}

Value::Value(const char* strValue)
{
	m_type = StringType;
	m_value.stringValue = new StringValueType(strValue);
}

Value::Value(const char* strValue, std::size_t length)
{
	m_type = StringType;
	m_value.stringValue = new StringValueType(strValue, length);
}

Value::Value(const std::string& strValue)
{
	m_type = StringType;
	m_value.stringValue = new StringValueType(strValue);
}

Value::Value(bool boolValue)
{
	m_type = BooleanType;
	m_value.boolValue = boolValue;
}

Value::~Value()
{
	destroy();
}

bool Value::isNull() const
{
	return m_type == NullType;
}

bool Value::isNumber() const
{
	return (m_type == IntType || m_type == RealType);
}

bool Value::isBoolean() const
{
	return m_type == BooleanType;
}

bool Value::isString() const
{
	return m_type == StringType;
}

bool Value::isArray() const
{
	return m_type == ArrayType;
}

bool Value::isObject() const
{
	return m_type == ObjectType;
}

bool Value::toInt(int& intValue)
{
	bool ok = true;

	switch (m_type)
	{
	case NullType:
		intValue = 0;
		break;
	case IntType:
		intValue = m_value.intValue;
		break;
	case RealType:
		intValue = static_cast<int>(m_value.doubleValue);
		break;
	case BooleanType:
		intValue = 0;
		break;
	default:
		ok = false;
		break;
	}

	return ok;
}

bool Value::toDouble(double& doubleValue)
{
	bool ok = true;

	switch (m_type)
	{
	case NullType:
		doubleValue = 0.0;
		break;
	case IntType:
		doubleValue = static_cast<double>(m_value.intValue);
		break;
	case RealType:
		doubleValue = m_value.doubleValue;
		break;
	case BooleanType:
		doubleValue = 0.0;
		break;
	default:
		ok = false;
		break;
	}

	return ok;
}

bool Value::toFloat(float& floatValue)
{
	bool ok = true;

	switch (m_type)
	{
	case NullType:
		floatValue = 0.0f;
		break;
	case IntType:
		floatValue = static_cast<float>(m_value.intValue);
		break;
	case RealType:
		floatValue = static_cast<float>(m_value.doubleValue);
		break;
	case BooleanType:
		floatValue = 0.0f;
		break;
	default:
		ok = false;
		break;
	}

	return ok;
}

bool Value::toString(std::string& strValue)
{
	bool ok = true;

	switch (m_type)
	{
	case NullType:
		strValue = "";
		break;
	case IntType:
		ok = false;	// TODO
		break;
	case RealType:
		ok = false;	// TODO
		break;
	case BooleanType:
		strValue = m_value.boolValue ? "true" : "false";
		break;
	case StringType:
		strValue = *m_value.stringValue;
		break;
	default:
		ok = false;
		break;
	}

	return ok;
}

bool Value::toBoolean(bool& boolValue)
{
	bool ok = true;

	switch (m_type)
	{
	case NullType:
		boolValue = false;
		break;
	case IntType:
		boolValue = (m_value.intValue != 0);
		break;
	case BooleanType:
		boolValue = m_value.boolValue;
		break;
	default:
		ok = false;
		break;
	}

	return ok;
}

bool Value::append(const Value& value)
{
	if (m_type != ArrayType)
		return false;

	m_value.arrayValue->push_back(value);
	return true;
}

bool Value::insert(std::size_t pos, const Value& value)
{
	if (m_type != ArrayType)
		return false;

	if (pos < 0 || pos >= m_value.arrayValue->size())
		return false;

	ArrayValueType::iterator it = m_value.arrayValue->begin() + pos;
	m_value.arrayValue->insert(it, value);
	return true;
}

bool Value::removeIndex(std::size_t index)
{
	if (m_type != ArrayType)
		return false;

	if (index < 0 || index >= m_value.arrayValue->size())
		return false;

	ArrayValueType::iterator it = m_value.arrayValue->begin() + index;
	m_value.arrayValue->erase(it);
	return true;
}

bool Value::removeMember(const std::string& key)
{
	if (m_type != ObjectType)
		return false;

	ObjectValueType::iterator it = m_value.objectValue->find(key);
	if (it == m_value.objectValue->end())
		return false;

	m_value.objectValue->erase(it);
	return true;
}

int Value::size() const
{
	switch (m_type)
	{
	case ArrayType:
		return (int)m_value.arrayValue->size();
	case ObjectType:
		return (int)m_value.objectValue->size();
	default:
		return -1;
	}

	return -1;
}

Value& Value::operator=(const Value& other)
{
	assign(other);
	return *this;
}

bool Value::operator==(const Value& other) const
{
	if (m_type != other.m_type)
		return false;

	switch (m_type)
	{
	case NullType:
		return true;
	case IntType:
		return m_value.intValue == other.m_value.intValue;
	case RealType:
		return m_value.doubleValue == other.m_value.doubleValue;
	case BooleanType:
		return m_value.boolValue == other.m_value.boolValue;
	case StringType:
		return *(m_value.stringValue) == *(other.m_value.stringValue);
	case ArrayType:
		return (m_value.arrayValue->size() == other.m_value.arrayValue->size() && 
			*(m_value.arrayValue) == *(other.m_value.arrayValue));
	case ObjectType:
		return (m_value.objectValue->size() == other.m_value.objectValue->size() &&
			*(m_value.objectValue) == *(other.m_value.objectValue));
	default:
		return false;
	}

	return false;
}

const Value& Value::operator[](std::size_t index) const
{
	assert(isArray());
	assert(index >= 0 && index < m_value.arrayValue->size());
	return m_value.arrayValue->at(index);
}

Value& Value::operator[](std::size_t index)
{
	assert(isArray());
	assert(index >= 0 && index < m_value.arrayValue->size());
	return m_value.arrayValue->at(index);
}

const Value& Value::operator[](const std::string& key) const
{
	assert(isObject());
	ObjectValueType::iterator it = m_value.objectValue->find(key);
	if (it == m_value.objectValue->end())
		return nullValue;

	return it->second;
}

Value& Value::operator[](const std::string& key)
{
	assert(isObject());
	ObjectValueType::iterator it = m_value.objectValue->find(key);
	if (it == m_value.objectValue->end())
	{
		Value empty(NullType);
		it = m_value.objectValue->insert(m_value.objectValue->begin(), make_pair(key, empty));
	}

	return it->second;
}

}