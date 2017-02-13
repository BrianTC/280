/*
Brian Chickey
CS280-003
*/
#include "p2lex.h"
#include "TValue.h"
#include "parsetree.h"
#include <fstream>
#include <map>

//disable system:pause on linux since it wont work
#ifdef _WIN32
const bool enable_pause = true;
#else
const bool enable_pause = false;
#endif
using namespace std;
//Token Wraper
Token saved;
bool isSaved = false;
Token GetAToken(std::istream *in) {
	if (isSaved) {
		isSaved = false;
		return saved;
	}
	return getToken(in);
}
void PushbackToken(Token& t) {
	if (isSaved) {
		std::cerr << "Can't push back more than one token!!!";
		exit(0);
	}
	saved = t;
	isSaved = true;
}
//Error thigns
int globalErrorCount = 0;
/// error handler
void error(string msg) {
	cout << linenum << ": " << msg << endl;
	++globalErrorCount;
}
//Parse counters
int starCount = 0;
int plusCount = 0;
int subCount = 0;
string parse_errors = "";

//String, provides a method for seeing if its an empty string.
//ironically it returns an empty string too sometimes.
class _String : public ParseTree {
private:
	Token	iTok;
public:
	_String(const Token& iTok, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), iTok(iTok) {}
	string	getString() { return (iTok.getLexeme()); }
	string isStringEmpty() {
		if (iTok.getLexeme() == "\"\"")//This is due to the implementation of the string token. "" is the conents of the 'empty' string
			return string("Empty string not permitted on line ") + to_string((int)onWhichLine()) + string("\n");
		return "";
	}
	TValue * Evaluate() {
		//*//cout << "String: " << this->getString();
		string ts = this->getString();
		////*//cout<<endl << "|" << ts << "|" << endl;
		//THis must be done :/
		//ts = ts.substr(1, ts.length()-2);
		////*//cout << "|" << ts <<"|"<<endl;
		return new TValue(ts);
	}
};
//Id, provides a method for checking if it was initialized yet
//also a list of known identifiers 
static map<string, ParseTree*> setIDs;
static map<string, TValue*> typevalue;
class _ID : public ParseTree {
private:
	Token	iTok;
public:
	_ID(const Token& iTok) : ParseTree(), iTok(iTok) {}
	string	getName() { return (iTok.getLexeme()); }
	string checkInitYet() {
		bool check = setIDs.find(iTok.getLexeme()) == setIDs.end() ? false : true;
		if (!check) {
			return  string("Symbol ") += iTok.getLexeme() += string(" used without being set at line ") += to_string((int)onWhichLine()) += string("\n");
		}
		return "";
	}
	TValue * Evaluate() {
		//*//cout << "ID: " << this->getName();
		///IF Shits fucked chec this
		return typevalue[this->getName()];
	}
};

// a + b
class _PlusOp : public ParseTree {
private:
	Token	iTok;
public:
	_PlusOp(const Token& iTok, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), iTok(iTok) {}
	int countPlusOp() { return 1; }
	TValue * Evaluate() {
		TValue *left=nullptr, *right=nullptr,*rval=nullptr;
		if (this->leftChild)left = this->leftChild->Evaluate();
		else return 0;
		//*//cout << " Plus ";
		if(this->rightChild)right=this->rightChild->Evaluate();
		else return 0;
		//evaluation
		if (left->getType() == TValue::T_INT && right->getType() == TValue::T_INT) {
				return new TValue(right->getInt() + left->getInt());
		}
		if (left->getType() == TValue::T_STRING && right->getType() == TValue::T_STRING) {
			return new TValue(left->getString() + right->getString());
		}
		error("Undefined Addition Operation");
		return 0;
	}
};

// a * b
class _StarOp : public ParseTree {
private:
	Token	iTok;
public:
	_StarOp(const Token& iTok, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), iTok(iTok) {}
	int countStarOp() { return 1; }
	//WHEE
	TValue * Evaluate() {
		TValue *left = nullptr, *right = nullptr, *rval = nullptr;
		if (this->leftChild)left = this->leftChild->Evaluate();
		else return 0;
		//*//cout << " Star ";
		if (this->rightChild)right = this->rightChild->Evaluate();
		else return 0;
		//evaluation
		if (left->getType() == TValue::T_INT && right->getType() == TValue::T_INT) {
			return new TValue(right->getInt() * left->getInt());
		}
		//left in right string
		if (left->getType() == TValue::T_INT && right->getType() == TValue::T_STRING){
			string newstring="";
			for (int i = 0; i < left->getInt(); i++)
				newstring += right->getString();
			return new TValue(newstring);
		}
		//left string right int
		if (left->getType() == TValue::T_STRING && right->getType() == TValue::T_INT) {
			string newstring = "";
			for (int i = 0; i < right->getInt(); i++)
				newstring += left->getString();
			return new TValue(newstring);
		}
		error("Undefined Star Operation");
		return 0;
	}
};

//String[a;b]
class _SubOp : public ParseTree {
private:
	Token	iTok;
public:
	_SubOp(const Token& iTok, ParseTree *left = 0, ParseTree *right = 0) : iTok(iTok), ParseTree(left, right) {}
	int countSubOp() { return 1; }
	TValue * Evaluate() {
		//*//cout << iTok.getLexeme() << " Sub ";
		TValue *left = nullptr, *right = nullptr;
		size_t slen = iTok.getLexeme().length();
		if (this->leftChild)left = this->leftChild->Evaluate();
		//*//cout << " : ";
		if (this->rightChild)right = this->rightChild->Evaluate();
		//only one int, then two
		if (left->getType()==TValue::T_INT) {
			int lval = left->getInt();
			if (right && right->getType() == TValue::T_INT) {
				int rval = right->getInt();
				if (rval<lval || rval>slen) {
					error("Invalid substring aruments");
					return 0;
				}
				return new TValue(iTok.getLexeme().substr(lval,rval ));
			}
			else if (right && right->getType() != TValue::T_INT) {
				error("Incompatable types for operation: Substring");
				return 0;
			}
			if (lval > slen) {
				error("Substring operand out of range");
				return 0;
			}
			return new TValue(iTok.getLexeme().substr(left->getInt()));
		}
		else error("Incompatable types for operation: Substring");
		return 0;
	}
};

class _SetOp : public ParseTree {
private:
	Token	iTok;
	ParseTree *left;
public:
	_SetOp(const Token& iTok, ParseTree *left = 0) : ParseTree(left), iTok(iTok), left(left) { }
	void initalize() {
		////*//cout << "initalizing id: " << iTok.getLexeme() << endl;
		setIDs[iTok.getLexeme()] = left;
	}
	//typevalue[iTok.getLexeme()] = Value(iTok.getLexeme(), );
	TValue * Evaluate() {
		
		//*//cout << "Set ID: " << iTok.getLexeme() << " = ";
		TValue * rval=nullptr;
		if (this->leftChild) rval=this->leftChild->Evaluate();
		else return 0;
		//*//cout << endl;
		if (rval->getType()==TValue::T_INT) {
			typevalue[iTok.getLexeme()] = new TValue(rval->getInt(), iTok.getLexeme());
		}
		else if (rval->getType() == TValue::T_STRING) {
			typevalue[iTok.getLexeme()] = new TValue(rval->getString(), iTok.getLexeme());
		}
		return 0;
	}
};

class _PrintOp : public ParseTree {
private:
	ParseTree *left;
public:
	_PrintOp( ParseTree *left = 0) : ParseTree(left),left(left) { }
	TValue * Evaluate() {
		//cout << "Print: ";
		TValue*end_val = nullptr;
		if(this->leftChild)end_val=this->leftChild->Evaluate();
		if (end_val);
		else return 0;

		if (end_val->getType() == TValue::T_STRING)
			cout << end_val->getString();
		else
			cout << end_val->getInt();
		cout << endl;
		return 0;
	}
};


//int linenum = 0;

/// function prototypes
ParseTree *Program(istream *in);
ParseTree *StmtList(istream *in);
ParseTree *Stmt(istream *in);
ParseTree *Expr(istream *in);
ParseTree *Term(istream *in);
ParseTree *Primary(istream *in);
ParseTree *String(istream *in);


ParseTree *Program(istream *in)
{
	ParseTree *result = StmtList(in);
	// make sure there are no more tokens...
	if (GetAToken(in).getTok() != DONE)
		return 0;
	return result;
}

ParseTree *StmtList(istream *in)
{
	ParseTree *statement = Stmt(in);
	//or followed by a statement list
	if (globalErrorCount > 0)
		return 0;
	Token t = GetAToken(in);
	ParseTree *stmtlst = new ParseTree();
	if (t.getTok() != DONE) {
		PushbackToken(t);
		stmtlst = StmtList(in);
	}
	return new ParseTree(statement, stmtlst);
}

ParseTree *Stmt(istream *in)
{
	Token t;
	t = GetAToken(in);
	//PRINT Expr SC | Set ID Expr SC
	if (t.getTok() == PRINT) {
		// process PRINT
		ParseTree *exp = Expr(in);
		t = GetAToken(in);
		if (t.getTok() == SC) {
			return new _PrintOp(exp);
		}
		else {
			PushbackToken(t);
			error("Missing semicolon");
			return 0;
		}
	}
	else if (t.getTok() == SET) {
		t = GetAToken(in);
		if (t.getTok() != ID) {
			error("Identifier expected after SET");
			return 0;//expect an ID token after SET
		}
		Token tID = t;
		ParseTree *exp = Expr(in);
		t = GetAToken(in);
		if (t.getTok() == SC) {
			return new _SetOp(tID, exp);
		}
		else {
			PushbackToken(t);
			error("Missing semicolon");
			return 0;
		}
	}
	else {
		error("Syntax Error, invalid statement");
		//clear remaining tokens till end of line to avoid getting multiple errors for on statement
		while (t.getTok() != SC)
			t = GetAToken(in);
	}
	return 0;
}

ParseTree *Expr(istream *in)
{
	//Term (+ Expr)
	ParseTree *term = Term(in);
	Token t = GetAToken(in);
	if (t.getTok() == PLUS)
		return new _PlusOp(t, term, Expr(in));
	else
		PushbackToken(t);
	return term;
}

ParseTree *Term(istream *in)
{
	//Primary (* Term)
	ParseTree *prim = Primary(in);
	Token t = GetAToken(in);
	if (t.getTok() == STAR)
		return new _StarOp(t, prim, Term(in));
	else
		PushbackToken(t);
	return prim;
}

ParseTree *Primary(istream *in)
{
	Token t = GetAToken(in);

	if (t.getTok() == ID) {
		return new _ID(t);
	}
	else if (t.getTok() == INT) {
		return new _Integer(t);
	}
	else if (t.getTok() == STR) {
		PushbackToken(t);
		return String(in);
	}
	else if (t.getTok() == LPAREN) {
		ParseTree *ex = Expr(in);
		if (ex == 0)
			return 0;
		t = GetAToken(in);
		if (t.getTok() != RPAREN) {
			error("expected right parens");
			return 0;
		}
		return ex;
	}

	return 0;
}

ParseTree *String(istream *in)
{
	//String ([ EXP (; EXP) ] )
	Token strTok = GetAToken(in);
	//its silly that this is needed but it is so here be it.
	strTok = Token(TokenType::STR, strTok.getLexeme().substr(1, strTok.getLexeme().length() - 2));
	Token t = GetAToken(in);
	if (t.getTok() == LEFTSQ) {
		ParseTree *subLeft = Expr(in);
		ParseTree * subRight = 0;
		t = GetAToken(in);
		if (t.getTok() == SC) {
			subRight = Expr(in);

		}
		else {
			PushbackToken(t);
		}
		//CHECK for close
		t = GetAToken(in);
		if (t.getTok() == RIGHTSQ) {
			return new _SubOp(strTok, subLeft, subRight);
		}
		else {
			PushbackToken(t);
			return 0;
		}
	}
	else {
		PushbackToken(t);
		return new _String(strTok);//Normal string no [] 
	}

	return 0;
}

int main(int argc, char * argv[])
{
	istream * iss = &cin;
	ifstream ifs;
	if (argc > 2) {
		cout << "Invalid number of arguments!" << endl;
		return 0;
	}
	if (argc == 2) {
		ifs.open(argv[1]);
		iss = &ifs;
		if (!ifs.is_open()) {
			cout << "Unable to open file: " << argv[1] << endl;
			return 0;
		}

	}
	ParseTree *prog = Program(iss);
	if (prog == 0 || globalErrorCount != 0) {
		if (enable_pause)
			system("pause");
		ifs.close();
		return 0;
	}
	ParseTree::Postorder(prog);
	/*cout << "Count of + operators: " << plusCount << endl;
	cout << "Count of * operators: " << starCount << endl;
	cout << "Count of [] operators: " << subCount << endl;*/
	cout << parse_errors;
	ifs.close();
	//Evaluate the program
	if(prog) prog->Evaluate();
	if (enable_pause)
		system("pause");
	return 0;
}