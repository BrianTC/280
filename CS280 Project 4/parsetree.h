#pragma once
#include "TValue.h"
//Parse counters
extern int starCount;
extern int plusCount;
extern int subCount;
extern std::string parse_errors;
class ParseTree {
	//private:
	//	ParseTree *leftChild;
	//	ParseTree *rightChild;
	//	int	whichLine;
public:
	ParseTree *leftChild;
	ParseTree *rightChild;
	int	whichLine;
	ParseTree(ParseTree *left = 0, ParseTree *right = 0) : leftChild(left), rightChild(right) { whichLine = linenum; }
	int onWhichLine() { return whichLine; }
	void echoErrorStream() { return; }
	virtual int getInteger() { return 0; }
	virtual std::string getString() { return ""; }
	virtual int countStarOp() { return 0; }
	virtual int countPlusOp() { return 0; }
	virtual int countSubOp() { return 0; }
	virtual void initalize() { return; }
	virtual std::string checkInitYet() { return ""; }
	virtual std::string isStringEmpty() { return ""; }
	static void Postorder(ParseTree* Root) {
		if (Root != NULL) {
			plusCount += Root->countPlusOp();
			starCount += Root->countStarOp();
			subCount += Root->countSubOp();
			Root->initalize();
			parse_errors += Root->checkInitYet();
			parse_errors += Root->isStringEmpty();
			/*cout << "Left  Address: " << Root->leftChild << endl;
			cout << "Right Address: " << Root->rightChild << endl;
			cout << endl;*/
			Postorder(Root->leftChild);
			Postorder(Root->rightChild);

		}
	}

	virtual TValue * Evaluate() {
		/*cout << "Left  Address: " << Root->leftChild << endl;
		cout << "Right Address: " << Root->rightChild << endl;
		cout << endl;*/
		if (this->leftChild)this->leftChild->Evaluate();
		if (this->rightChild)this->rightChild->Evaluate();
		return 0;
	}
};

//// subclasses of parse, all start with an _underscore
class _Integer : public ParseTree {
private:
	Token	iTok;

public:
	_Integer(const Token& iTok) : ParseTree(), iTok(iTok) {}
	int	getInteger() { return stoi(iTok.getLexeme()); }
	TValue * Evaluate() {
		//*//cout << "Integer: " << iTok.getLexeme();
		return new TValue(this->getInteger());
	}
};