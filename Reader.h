#pragma once
#include <string>
#include "Value.h"

namespace json
{

class Reader
{
public:
	Reader();
	~Reader();

	bool parse(const std::string& stream, Value& root);
private:
	enum TokenType
	{
		TokenEndOfStream = 0,
		TokenObjectBegin,
		TokenObjectEnd,
		TokenArrayBegin,
		TokenArrayEnd,
		TokenString,
		TokenNumber,
		TokenTrue,
		TokenFalse,
		TokenNull,
		TokenKeyValueSep,	// :
		TokenElementSep,	// ,
		TokenError
	};

	struct Token
	{
		TokenType type;
		const char* begin;
		const char* end;
	};

	bool readToken(Token& token);
	void skipSpaces();
	bool readStringToken();
	void readNumberToken();
	bool parseUnicodeToUtf8(const char*& cur, std::string& str);
	std::string unicodeToUtf8(int cp);

	bool readValue(Value& value);
	bool readObject(Value& value);
	bool readString(Token token, Value& value);
	bool readArray(Value& value);
	bool readNumber(Token token, Value& value);
	bool readNull(Value& value);
	bool readBoolean(Token token, Value& value);
	bool match(const char* pattern, int patternLength);
private:
	const char* m_streamBegin;
	const char* m_streamEnd;
	const char* m_streamCurrent;
};

}
