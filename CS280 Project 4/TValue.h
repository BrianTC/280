#pragma once
#include <string>
class TValue {
public:
	enum VType
	{
		T_STRING,
		T_INT
	};
private:
	std::string s;
	std::string lex;
	int i;
	VType type;
public:
	TValue() {
		s = "";
		lex = "";
		i = 0;
		type = T_STRING;
	}
	TValue(int itger, std::string lexime = "") : i(itger), type(T_INT), lex(lexime) {}
	TValue(std::string str, std::string lexime = "") : s(str), type(T_STRING), lex(lexime) {}
	int getInt() { return i; }
	VType getType() { return type; }
	std::string getString() { return s; }

};