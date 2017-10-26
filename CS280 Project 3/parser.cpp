/*
Brian Chickey
CS280-003
*/
//#include "p2lex.h"
#include "p2lex.h"
#include <fstream>
#include <map>
//disable pause on linux since it wont work
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
//Parse counters
static int starCount = 0;
static int plusCount = 0;
static int subCount = 0;
string parse_errors = "";

class ParseTree {
private:
	ParseTree *leftChild;
	ParseTree *rightChild;
	int	whichLine;

protected:
	;

public:
	ParseTree(ParseTree *left = 0, ParseTree *right = 0) : leftChild(left), rightChild(right) {
		whichLine = linenum;
	}
	int onWhichLine() {
		return whichLine;
	}
	void echoErrorStream() {
		return;
	}
	virtual int getInteger() { return 0; }
	virtual string getString() { return ""; }
	virtual int countStarOp() { return 0; }
	virtual int countPlusOp() { return 0; }
	virtual int countSubOp() { return 0; }
	virtual void initalize() { return; }
	virtual string checkInitYet() { return ""; }
	virtual string isStringEmpty() { return ""; }
	static void Postorder(ParseTree* Root) {
		if (Root != NULL) {
			plusCount += Root->countPlusOp();
			starCount += Root->countStarOp();
			subCount += Root->countSubOp();
			Root->initalize();
			parse_errors += Root->checkInitYet();
			parse_errors += Root->isStringEmpty();
			Postorder(Root->leftChild);
			Postorder(Root->rightChild);

		}
	}
};

//// subclasses of parse, all start with an _underscore
class _Integer : public ParseTree {
private:
	Token	iTok;

public:
	_Integer(const Token& iTok) : ParseTree(), iTok(iTok) {}

	int	getInteger() { return stoi(iTok.getLexeme()); }
};

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
			return string("Empty string not permitted on line ") + to_string((long long int)onWhichLine()) + string("\n");
		return "";
	}
};
//Id, provides a method for checking if it was initialized yet
static map<string, ParseTree*> setIDs;
//also a list of known identifiers 
class _ID : public ParseTree {
private:
	Token	iTok;
public:
	_ID(const Token& iTok) : ParseTree(), iTok(iTok) {}
	string	getString() { return (iTok.getLexeme()); }
	string checkInitYet() {
		bool check = setIDs.find(iTok.getLexeme()) == setIDs.end() ? false : true;
		if (!check) {
			return  string("Symbol ") += iTok.getLexeme() += string(" used without being set at line ") += to_string((long long int)onWhichLine()) += string("\n");
		}
		return "";
	}
};
// a + b
class _PlusOp : public ParseTree {
private:
	Token	iTok;
public:
	_PlusOp(const Token& iTok, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), iTok(iTok) {}
	int countPlusOp() { return 1; }
};
// a * b
class _StarOp : public ParseTree {
private:
	Token	iTok;
public:
	_StarOp(const Token& iTok, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), iTok(iTok) {}
	int countStarOp() { return 1; }
};
//String[a;b]
class _SubOp : public _String {
private:
	Token	iTok;
public:
	_SubOp(const Token& iTok, ParseTree *left = 0, ParseTree *right = 0) : iTok(iTok), _String(iTok, left, right) {}
	int countSubOp() { return 1; }
};


//map of initialized identifiers 

class _SetOp : public ParseTree {
private:
	Token	iTok;
	ParseTree *left;
public:
	_SetOp(const Token& iTok, ParseTree *left = 0) : ParseTree(left), iTok(iTok), left(left) { }
	void initalize() {
		//cout << "initalizing id: " << iTok.getLexeme() << endl;
		setIDs[iTok.getLexeme()] = left;
	}
};
//int linenum = 0;
int globalErrorCount = 0;
/// error handler
void error(string msg) {
	cout << linenum << ": " << msg << endl;
	++globalErrorCount;
}

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
			return new ParseTree(exp);
		}
		else
			PushbackToken(t);
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
		else
			PushbackToken(t);
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
	cout << "Count of + operators: " << plusCount << endl;
	cout << "Count of * operators: " << starCount << endl;
	cout << "Count of [] operators: " << subCount << endl;
	cout << parse_errors;
	if (enable_pause)
		system("pause");

	ifs.close();
	return 0;
}