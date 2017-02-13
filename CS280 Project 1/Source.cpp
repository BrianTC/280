#include "pformat.h"
bool isCommandLine(string line) {
	if (line.substr(0, 1) == ".")
		return true;
	return false;
}
//
int charL = 60;//defualt value of 60
//gets the value of a line length set command .ll (POS INT < 120)
int charLength(string line) {
	vector<string> parts = pformat::splitstring(line);
	int OCL = charL;
	if (!(parts.size() == 2&&parts[0] == ".ll"))
		return charL;
	string numS = parts[1];
	//cout << endl << "numS: "<< numS << endl;
	stringstream convert(numS); // stringstream used for the conversion initialized with the contents of Text
	if (!(convert >> charL)) {
		cout << "couldnt convert string to number?\n"; //give the value to Result using the characters in the string
		//return -1;
	}
	if (charL > 120 || charL < 10)
		return OCL;
	//cout << "\nCharl: " << charL << endl;

	return charL;
}
string trimWhiteSpace(string &in) {
	string out;
	int i;
	for (i = 0; i < in.length(); i++) {
		if (!isspace(in.at(i)))
			break;
	}
	out = in.substr(i, in.length());
	for(i = out.length();i>0;i--){
		if (!isspace(in.at(i)))
			break;
	}
	out = out.substr(0, i);
	return out;
}
//format input string of continuous line of text into a paragraph to be written to the outbuffer

int main(int argc, char *argv[]) {
	//cout << "entered main" << endl;
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
		//cout << "file opened" << endl;
		string line = " ";
		//cout << "b4trimewspace" << endl;
		line = trimWhiteSpace(line);
		//cout << "aftertrim" << endl;
		//needs to be fixed as well!
		bool aline = false;
		while (getline(myfile, line)) {
			//cout << "||" << line << endl;
			//cout << line << endl;
			string addNewline = aline ? "\n" : "";
			if (isCommandLine(line))
			{
				//cout << ".commandline" << endl;
				if (line.substr(0, 4) == ".ll " || line.substr(0, 4) == ".hf") {
					//cout << ".ll commandline " << endl;
					obuffer += pformat::paragraphFormater(pbuffer, charL);
					obuffer += addNewline;
					pbuffer = "";
					charL = charLength(line);
				}
				aline = false;
				
				continue;
			}
			else if (line == "") {
				if (pbuffer.length() > 0) {
					obuffer += pformat::paragraphFormater(pbuffer, charL);
					obuffer += addNewline;
				}
				pbuffer = "";
				aline = false;
				continue;
			}

			pbuffer.append(line + " ");
			aline = true;

		}

		//CHECK LOOP TO SEE IF I REALLY HAVE TO DO THIS!
		obuffer += pformat::paragraphFormater(pbuffer, charL);

		////////////////////////////////////////////////
		myfile.close();
		cout << obuffer;
		//cout << "Pbuffer:\n" << pbuffer << endl;
		//cout << "Obuffer:\n" << obuffer << endl;
	}
	else
		cout << "could not open file!" << endl;
	//return 0;
	outfile.open("out.txt");
	if(outfile.is_open())
		outfile << obuffer<<"";
	outfile.close();
	//if (true) {
	//	string temp;
	//	cin >> temp;
	//}
	system("pause");
	return 0;
}