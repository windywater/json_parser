#include "Reader.h"
#include <sstream>

namespace json
{

Reader::Reader()
{
}

Reader::~Reader()
{
}

bool Reader::parse(const std::string& stream, Value& root)
{
	m_streamBegin = stream.c_str();
	m_streamCurrent = m_streamBegin;
	m_streamEnd = m_streamBegin + stream.size();
	return readValue(root);
}

bool Reader::readToken(Token& token)
{
	skipSpaces();
	bool ok = true;

	token.begin = m_streamCurrent;
	char c = *m_streamCurrent;
	m_streamCurrent++;

	switch (c)
	{
	case '{':
		token.type = TokenObjectBegin;
		break;
	case '}':
		token.type = TokenObjectEnd;
		break;
	case '[':
		token.type = TokenArrayBegin;
		break;
	case ']':
		token.type = TokenArrayEnd;
		break;
	case '"':
		token.type = TokenString;
		ok = readStringToken();
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
	case '.':
		token.type = TokenNumber;
		readNumberToken();
		break;
	case 't':
		token.type = TokenTrue;
		ok = match("rue", 3);
		break;
	case 'f':
		token.type = TokenFalse;
		ok = match("alse", 4);
		break;
	case 'n':
		token.type = TokenNull;
		ok = match("ull", 3);
		break;
	case ',':
		token.type = TokenElementSep;
		break;
	case ':':
		token.type = TokenKeyValueSep;
		break;
	case '\0':
		token.type = TokenEndOfStream;
		break;
	default:
		ok = false;
		break;
	}

	if (!ok)
		token.type = TokenError;

	token.end = m_streamCurrent-1;
	return true;
}

void Reader::skipSpaces()
{
	while (m_streamCurrent != m_streamEnd)
	{
		char c = *m_streamCurrent;
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			m_streamCurrent++;
		else
			break;
	}
}

bool Reader::readStringToken()
{
	bool stringClose = false;
	char c;
	while (m_streamCurrent != m_streamEnd)
	{
		c = *m_streamCurrent++;
		if (c == '"')
		{
			stringClose = true;
			break;
		}
		else if (c == '\\')
		{
			m_streamCurrent++;
		}
	}

	return stringClose;
}

void Reader::readNumberToken()
{
	char c;
	while (m_streamCurrent != m_streamEnd)
	{
		c = *m_streamCurrent;
		if ((c >= '0' && c <= '9') || c == '.')
			m_streamCurrent++;
		else
			break;
	}
}

bool Reader::match(const char* pattern, int patternLength)
{
	bool matched = false;

	for (int i = 0; i < patternLength; i++)
	{
		const char* p = m_streamCurrent++;
		if (p == m_streamEnd)
			return false;

		if (*p != pattern[i])
			return false;
	}

	return true;
}

bool Reader::readValue(Value& value)
{
	skipSpaces();

	Token token;
	readToken(token);
	bool ok = true;

	switch (token.type)
	{
	case TokenObjectBegin:
		ok = readObject(value);
		break;
	case TokenArrayBegin:
		ok = readArray(value);
		break;
	case TokenNumber:
		ok = readNumber(token, value);
		break;
	case TokenString:
		ok = readString(token, value);
		break;
	case TokenTrue:
	case TokenFalse:
		ok = readBoolean(token, value);
		break;
	case TokenNull:
		ok = readNull(value);
		break;
	default:
		ok = false;
	}

	return ok;
}

bool Reader::readNumber(Token token, Value& value)
{
	std::string numStr(token.begin, token.end - token.begin + 1);
	if (numStr.find('.') == std::string::npos)	// 整数和小数分别处理
		value = Value(stoi(numStr));
	else
		value = Value(stod(numStr));

	return true;
}

bool Reader::readString(Token token, Value& value)
{
	if (token.end - token.begin == 1)
	{
		value = Value("");
		return true;
	}

	std::string str;
	const char* begin = token.begin + 1;
	const char* end = token.end;
	const char* cur = begin;
	while (cur < end)
	{
		if (*cur == '\\')	// 转义
		{
			cur++;
			switch (*cur)
			{
			case '"':
				str += '"';
				break;
			case '\\':
				str += '\\';
				break;
			case 'b':
				str += '\b';
				break;
			case 'r':
				str += '\r';
				break;
			case 'n':
				str += '\n';
				break;
			case 't':
				str += '\t';
				break;
			case 'u':
			{
				std::string utf8Chars;
				if (!parseUnicodeToUtf8(cur, utf8Chars))
					return false;
				str += utf8Chars;
				break;
			}
			default:
				return false;
			}
		}
		else
		{
			str += *cur;
		}

		cur++;
	}

	value = Value(str);
	return true;
}

bool Reader::parseUnicodeToUtf8(const char*& cur, std::string& str)
{
	cur++;	// 跳过u

	// \u后面至少要有4个字符
	if (m_streamEnd - cur < 4)
		return false;

	int unicode = 0;
	std::stringstream ss;
	ss << std::hex << std::string(cur, 4);
	ss >> unicode;
	str = unicodeToUtf8(unicode);
	cur += 3;	// readString中处理完后还有一次cur++，因此这里只用后移3字符

	return true;
}

std::string Reader::unicodeToUtf8(int cp)
{
	std::string result;

	// based on description from http://en.wikipedia.org/wiki/UTF-8

	if (cp <= 0x7f) {
		result.resize(1);
		result[0] = static_cast<char>(cp);
	}
	else if (cp <= 0x7FF) {
		result.resize(2);
		result[1] = static_cast<char>(0x80 | (0x3f & cp));
		result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
	}
	else if (cp <= 0xFFFF) {
		result.resize(3);
		result[2] = static_cast<char>(0x80 | (0x3f & cp));
		result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
		result[0] = static_cast<char>(0xE0 | (0xf & (cp >> 12)));
	}
	else if (cp <= 0x10FFFF) {
		result.resize(4);
		result[3] = static_cast<char>(0x80 | (0x3f & cp));
		result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
		result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
		result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
	}

	return result;
}

bool Reader::readBoolean(Token token, Value& value)
{
	if (*token.begin == 't')
		value = Value(true);
	else if (*token.begin == 'f')
		value = Value(false);

	return true;
}

bool Reader::readNull(Value& value)
{
	value = Value(NullType);
	return true;
}

bool Reader::readArray(Value& value)
{
	bool ok = true;
	value = Value(ArrayType);

	Token token;
	for (;;)
	{
		Value subValue;
		ok = readValue(subValue);
		if (ok)
			value.append(subValue);
		else
			return false;

		ok = readToken(token);
		if (token.type == TokenElementSep)
			continue;
		else if (token.type == TokenArrayEnd)
			break;
		else
			return false;
	}

	return true;
}

bool Reader::readObject(Value& value)
{
	bool ok = true;
	value = Value(ObjectType);

	Token token;
	for (;;)
	{
		ok = readToken(token);
		Value key(StringType);
		ok = readString(token, key);

		ok = readToken(token);
		if (token.type != TokenKeyValueSep)
			return false;

		Value val;
		ok = readValue(val);
		if (!ok)
			return false;

		std::string keyStr;
		key.toString(keyStr);
		value[keyStr] = val;

		ok = readToken(token);
		if (token.type == TokenElementSep)
			continue;
		else if (token.type == TokenObjectEnd)
			break;
		else
			return false;
	}

	return true;
}

}	// namespace json
