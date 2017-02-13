#include "p2lex.h"
//^includes string and iostream
#include <fstream>
#include <istream>
#include <sstream>
#include <map>
using namespace std;
static string tokenToStr[] = {
	"id",		// identifier
	"str",		// string
	"int",		// integer constant
	"plus",		// the + operator
	"star",		// the * operator
	"leftsq",		// the [
	"rightsq",	// the ]
	"print",		// print keyword
	"set",		// set keyword
	"sc",		// semicolon
	"lparen",		// the (
	"rparen",		// the )
	"done",		// finished!
	"err",		// an unrecoverable error
};
static map<char, TokenType> symbolTable = {
	{ ';',TokenType::SC },
	{ '+',TokenType::PLUS },
	{ '*',TokenType::STAR },
	{ '[',TokenType::LEFTSQ },
	{ ']',TokenType::RIGHTSQ },
	{ '(',TokenType::LPAREN },
	{ ')',TokenType::RPAREN }
};
std::ostream & operator<<(std::ostream & out, const Token & t)
{
	// TODO: insert retur statement here
	Token b;
	b.getTok();
	string trimmedLex="";
	string temporary = t.lexeme;
	for (string::iterator itr = temporary.begin(); itr != temporary.end(); itr++) {
		if (*itr != '\n') {
			trimmedLex+=*itr;
		}
	}

	cout << "" << tokenToStr[t.tok] << ( t.lexeme != "" ? "("+trimmedLex+")" : "" );
	return out;
}
int linenum = 0;
//Helpers to clean things up
bool identStart(char c) {
	//		_				a-z					A-Z		
	return c == 95 || 64 < c && c < 91 || 96 < c&&c < 123;
}
bool identBody(char c) {
	//		_ a-z A-Z			0-9
	return identStart(c) || 47 < c&&c < 58;
}
char getNextChar(std::istream * instream) {
	char temp= (char)instream->get();
	if (temp == '\n')
		linenum++;
	return temp;
}
void putBackChar(std::istream * instream,char cc) {
	instream->putback(cc);
	if (cc == '\n')
		linenum--;
}
Token getToken(std::istream * instream)
{
	//cout << (symbolTable['o'] == NULL) << endl;
	string lex = "";
	char cur_char = getNextChar(instream);
	char peek = (char)instream->peek();
	//trim white space
	while (isspace(cur_char)) {
		cur_char = getNextChar(instream);
	}
	//comments
	if (cur_char == '/'&&peek == '/') {
		while (cur_char != '\n' || (cur_char == EOF))
			cur_char = getNextChar(instream);
		/*if (cur_char == '\n')
			instream->get();*/
		//cout << cur_char << endl;
	}
	//trim white space again?
	while (isspace(cur_char)) {
		cur_char = getNextChar(instream);
	}
	//Symbols
	if (symbolTable.find(cur_char) != symbolTable.end()) {
		lex="";
		lex+=(cur_char);
		return Token(symbolTable.at(cur_char),"");
	}
	//strings
	if (cur_char == '\"') {
		//cout << "Starting string..." << endl;
		lex="\"";
		cur_char = getNextChar(instream);
		//cout << cur_char << endl;
		while (cur_char != '"') {
			//cout << "strlopp:" << cur_char << endl;
			lex+=cur_char;
			//error case non one line or not closed before EOF
			if (cur_char == '\n' || cur_char == EOF)
			{
				return Token(TokenType::ERR, lex);
			}
			//increment 
			cur_char = getNextChar(instream);
		}
		lex += cur_char;
		//cout << lex << endl;
		return Token(TokenType::STR, lex);
	}
	//integers
	if (isdigit(cur_char)) {
		//cout << "Starting int..." << endl;
		lex = "";
		//cout << cur_char << endl;
		while (isdigit(cur_char)) {
			//cout << "strlopp:" << cur_char << endl;
			lex += cur_char;
			cur_char = getNextChar(instream);
		}
		if (isspace(cur_char) || (symbolTable.find(cur_char) != symbolTable.end())){
			putBackChar(instream, cur_char);
			return Token(TokenType::INT, lex);
		}
		putBackChar(instream, cur_char);
		return Token(TokenType::ERR, lex+=cur_char);
		//cout << lex << endl;
		
	}
	//ident
	//there may or may not be indexing issues here pls check later
	if (identStart(cur_char) ){
		//cout << "Starting ident..." << endl;
		lex = "";
		//cout << cur_char << endl;
		while (identBody(cur_char)) {
			//cout << "strlopp:" << cur_char << endl;
			lex += cur_char;
			if (!identBody(instream->peek())) {
				if (lex == "set")
					return Token(TokenType::SET, "");
				if (lex == "print")
					return Token(TokenType::PRINT, "");
			}
			cur_char = getNextChar(instream);
		}
		putBackChar(instream, cur_char);
		//cout << lex << endl;
		return Token(TokenType::ID, lex);
	}
	//set
	if (getNextChar(instream) == EOF) {
		return Token(TokenType::DONE, "");
	}
	return Token(TokenType::ERR,lex);
}
