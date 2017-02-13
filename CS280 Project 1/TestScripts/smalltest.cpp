#include <iostream>
#include <ifstream>
#include <string>

using namesapce std;
int main(int argc, char *argv[]) {
	if (argc != 2) {
		cout << "invalid number of arguments, terminating." << endl;
		return -1;
	}
	bool holdFile = true;
	ifstream myfile;
	ofstream outfile;
	string pbuffer = "";
	string obuffer = "";
	myfile.open(argv[1]);
	if (myfile.is_open()) {
		string line;
		while(getline(myfile,line)){
			cout<<line<<endl;
		}
	}
}