#include "Reader.h"

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
	if (numStr.find('.') == std::string::npos)
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
		if (*cur == '\\')
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
