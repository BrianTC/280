#include "p2lex.h"
//^includes string and iostream
#include <fstream>
#include <istream>
#include <sstream>
#include <map>//I was getting tired of commenting/uncommenting pause at the end of the thing so I could read it on windows
//and I dont need to pause a unix console because it saves output and doesnt close uppon completion 
//preprocessor directives are p. cool
#ifdef _WIN32
	const bool enable_pause = true;
#else
	const bool enable_pause = false;
#endif
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

int main(int argc, char *argv[]) {
	bool mode_verbose=false;
	string fName;
	if (argc>3) {
		cout << "Improper number of arguments" << endl;
		return -1;
	}
	else if (argc == 3) {
		//cout << argv[1] << endl;
		if (argv[1] == string("-v"))
			mode_verbose = true;
		else {
			cout << "only recognized argument is -v";
			return -2;
		}
	}
	
	ifstream infile;
	istream * issp;

	fName = argv[argc - 1];
	infile.open(fName);
	if (!infile.is_open()) {
		cout << "Could not open file: " << fName << endl;
		return -3;
	}
	issp = &infile;

	map<TokenType, int> genStats;
	map<TokenType, map<string, int> > uniStats;
	uniStats[TokenType::ID][""];
	uniStats[TokenType::STR][""];
	uniStats[TokenType::INT][""];

	Token tt = Token();
	bool noerror = true;
	while (tt.getTok() != TokenType::DONE) {
		if (argc == 1)
			tt = getToken(&cin);
		else
			tt = getToken(issp);

		genStats[tt.getTok()]++;
		//its ugly but I see no other way to limit what qualifies as "unique"
		if(tt.getTok()==TokenType::STR|| tt.getTok() == TokenType::ID|| tt.getTok() == TokenType::INT)
			uniStats[tt.getTok()][tt.getLexeme()]++;

		if (mode_verbose) {
			if(tt.getTok()!=TokenType::DONE)
				cout << tt << endl;
		}
		if (tt.getTok() == TokenType::ERR) {
			cout << tt << endl;
			noerror = false;
			break;
		}
	}
	if(noerror){
		genStats.erase(TokenType::DONE);

		for(map<TokenType,int>::iterator itr = genStats.begin();itr!=genStats.end();itr++){
		//for (const auto itr : genStats) { this doesnt work on afs?
			cout << tokenToStr[itr->first] << ": " << itr->second << endl;
		}
		for (map<TokenType, map<string, int>>::iterator itr = uniStats.begin(); itr != uniStats.end(); itr++) {
			cout << "Number of unique lexemes for " << tokenToStr[itr->first] << ": " << (itr->second.size()-1) << endl;
		}
	}
	//experemental???
	if(enable_pause)
		system("pause");
	if (argc == 1) {
		infile.close();
	}
	return 153;
}