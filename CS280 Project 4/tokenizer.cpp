//////////////////////////////////////////////////////////////////////////////////////
//////////Going to be using your tokenizer since our linenum differnces///////////////
//////////were going to be much to annoying to sort out than its worth////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <map>
#include "p2lex.h"
using namespace std;
int linenum=0;
map<TokenType, string> TokenNames = {
	{ ID, "id" },
	{ STR, "str" },
	{ INT, "int" },
	{ PLUS, "plus" },
	{ STAR, "star" },
	{ LEFTSQ, "leftsq" },
	{ RIGHTSQ, "rightsq" },
	{ PRINT, "print" },
	{ SET, "set" },
	{ SC, "sc" },
	{ LPAREN, "lparen" },
	{ RPAREN, "rparen" },
	{ DONE, "done" },
	{ ERR, "err" },
};

string getPrintName(TokenType t)
{
	return TokenNames[t];
}

map<TokenType, bool> TokenExtras = {
	{ ID, true },
	{ STR, true },
	{ INT, true },
	{ ERR, true },
};

ostream& operator<<(ostream& out, const Token& t)
{
	out << TokenNames[t.tok];
	if (TokenExtras[t.tok]) {
		out << "(" << t.lexeme << ")";
	}
	return out;
}

Token
getToken(std::istream* instream)
{
	enum LexState { BEGIN, INID, INSTR, ININT, INCOMMENT } state = BEGIN;

	int	ch;
	string	lexeme;

	while ((ch = instream->get()) != EOF) {
		switch (state) {
		case BEGIN:
			if (ch == '\n')
				++linenum;

			if (isspace(ch))
				continue;

			lexeme = ch;

			if (isalpha(ch)) {
				state = INID;
				continue;
			}

			if (isdigit(ch)) {
				state = ININT;
				continue;
			}

			if (ch == '/' && instream->peek() == '/') {
				state = INCOMMENT;
				continue;
			}

			if (ch == '"') {
				state = INSTR;
				continue;
			}

			switch (ch) {
			case '+':
				return Token(PLUS, lexeme);

			case '*':
				return Token(STAR, lexeme);

			case '[':
				return Token(LEFTSQ, lexeme);

			case ']':
				return Token(RIGHTSQ, lexeme);

			case '(':
				return Token(LPAREN, lexeme);

			case ')':
				return Token(RPAREN, lexeme);

			case ';':
				return Token(SC, lexeme);
			}

			return Token(ERR, lexeme);

			break;

		case INID:
			if (isalpha(ch)) {
				lexeme += ch;
				continue;
			}

			instream->putback(ch);
			if (lexeme == "print")
				return Token(PRINT, lexeme);
			if (lexeme == "set")
				return Token(SET, lexeme);
			return Token(ID, lexeme);

		case INSTR:
			if (ch == '\n')
				return Token(ERR, lexeme);

			lexeme += ch;
			if (ch != '"') {
				continue;
			}

			return Token(STR, lexeme);

		case ININT:
			if (isdigit(ch)) {
				lexeme += ch;
				continue;
			}

			instream->putback(ch);
			return Token(INT, lexeme);

		case INCOMMENT:
			if (ch != '\n')
				continue;

			state = BEGIN;
			break;

		default:
			cerr << "Value of state is not known!" << endl;
			return Token(); // error
		}
	}

	if (state == BEGIN)
		return Token(DONE, "");
	else if (state == INID)
		return Token(ID, lexeme);
	else if (state == ININT)
		return Token(INT, lexeme);
	else
		return Token(ERR, lexeme);
}
/////*
////Brian Chickey
////CS280-003
////*/
//#include "p2lex.h"
//#include <cctype>
//#include <fstream>
//#include <istream>
//#include <sstream>
//#include <map>
//using namespace std;
//static string tokenToStr[] = {
//	"id",		// identifier
//	"str",		// string
//	"int",		// integer constant
//	"plus",		// the + operator
//	"star",		// the * operator
//	"leftsq",		// the [
//	"rightsq",	// the ]
//	"print",		// print keyword
//	"set",		// set keyword
//	"sc",		// semicolon
//	"lparen",		// the (
//	"rparen",		// the )
//	"done",		// finished!
//	"err",		// an unrecoverable error
//};
//static map<char, TokenType> symbolTable = {
//	{ ';',TokenType::SC },
//	{ '+',TokenType::PLUS },
//	{ '*',TokenType::STAR },
//	{ '[',TokenType::LEFTSQ },
//	{ ']',TokenType::RIGHTSQ },
//	{ '(',TokenType::LPAREN },
//	{ ')',TokenType::RPAREN }
//};
//std::ostream & operator<<(std::ostream & out, const Token & t)
//{
//	// TODO: insert retur statement here
//	Token b;
//	b.getTok();
//	string trimmedLex = "";
//	string temporary = t.lexeme;
//	for (string::iterator itr = temporary.begin(); itr != temporary.end(); itr++) {
//		if (*itr != '\n') {
//			trimmedLex += *itr;
//		}
//	}
//
//	cout << "" << tokenToStr[t.tok] << (t.lexeme != "" ? "(" + trimmedLex + ")" : "");
//	return out;
//}
//int linenum = 1;
////Helpers to clean things up
//bool identStart(char c) {
//	//		_				a-z					A-Z		
//	return c == 95 || 64 < c && c < 91 || 96 < c&&c < 123;
//}
//bool identBody(char c) {
//	//		_ a-z A-Z			0-9
//	return identStart(c) || 47 < c&&c < 58;
//}
//char prevChar, currentChar;
//char getNextChar(std::istream * instream) {
//	prevChar = currentChar;
//	currentChar = (char)instream->get();
//	if (currentChar == '\n'&&prevChar != '\n')
//		linenum++;
//	return currentChar;
//}
//void putBackChar(std::istream * instream, char cc) {
//	instream->putback(cc);
//	currentChar = prevChar;
//	if (cc == '\n'&&currentChar != '\n')
//		linenum--;
//}
//Token getToken(std::istream * instream)
//{
//	//cout << (symbolTable['o'] == NULL) << endl;
//	string lex = "";
//	char cur_char = getNextChar(instream);
//	char peek = (char)instream->peek();
//	//trim white space
//	while (isspace(cur_char)) {
//		cur_char = getNextChar(instream);
//	}
//	//comments
//	if (cur_char == '/'&&peek == '/') {
//		while (cur_char != '\n' || (cur_char == EOF))
//			cur_char = getNextChar(instream);
//		linenum--;
//	}
//	//trim white space again?
//	while (isspace(cur_char)) {
//		cur_char = getNextChar(instream);
//	}
//	//Symbols
//	if (symbolTable.find(cur_char) != symbolTable.end()) {
//		lex = "";
//		lex += (cur_char);
//		return Token(symbolTable.at(cur_char), "");
//	}
//	//strings
//	if (cur_char == '\"') {
//		//cout << "Starting string..." << endl;
//		lex = "\"";
//		cur_char = getNextChar(instream);
//		//cout << cur_char << endl;
//		while (cur_char != '"') {
//			//cout << "strlopp:" << cur_char << endl;
//			lex += cur_char;
//			//error case non one or not closed before EOF
//			if (cur_char == '\n' || cur_char == EOF)
//			{
//				return Token(TokenType::ERR, lex);
//			}
//			//increment 
//			cur_char = getNextChar(instream);
//		}
//		lex += cur_char;
//		//cout << lex << endl;
//		return Token(TokenType::STR, lex);
//	}
//	//integers
//	if (isdigit(cur_char)) {
//		//cout << "Starting int..." << endl;
//		lex = "";
//		//cout << cur_char << endl;
//		while (isdigit(cur_char)) {
//			//cout << "strlopp:" << cur_char << endl;
//			lex += cur_char;
//			cur_char = getNextChar(instream);
//		}
//		if (isspace(cur_char) || (symbolTable.find(cur_char) != symbolTable.end())) {
//			putBackChar(instream, cur_char);
//			return Token(TokenType::INT, lex);
//		}
//		putBackChar(instream, cur_char);
//		return Token(TokenType::ERR, lex += cur_char);
//		//cout << lex << endl;
//
//	}
//	//ident
//	//there may or may not be indexing issues here pls check later
//	if (identStart(cur_char)) {
//		//cout << "Starting ident..." << endl;
//		lex = "";
//		//cout << cur_char << endl;
//		while (identBody(cur_char)) {
//			//cout << "strlopp:" << cur_char << endl;
//			lex += cur_char;
//			if (!identBody(instream->peek())) {
//				if (lex == "set")
//					return Token(TokenType::SET, "");
//				if (lex == "print")
//					return Token(TokenType::PRINT, "");
//			}
//			cur_char = getNextChar(instream);
//		}
//		putBackChar(instream, cur_char);
//		//cout << lex << endl;
//		return Token(TokenType::ID, lex);
//	}
//	//set
//	if (getNextChar(instream) == EOF) {
//		return Token(TokenType::DONE, "");
//	}
//	return Token(TokenType::ERR, lex);
//}