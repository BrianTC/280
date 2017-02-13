#include "pformat.h"
int zero = 0;
string addspacesback(vector<string> &linewords,int &spacecount) {
	//cout << "addspacesback\n\n\n" << endl;
	//-/ <<"spacecount: " << spacecount << endl;
	////-/ << "add space start" << endl;
	//-/ << endl;//for pformattesting
	int eadd = 0;
	int randomadd = 0;
	//output string
	string os;
	//possible source of division by zero
	//-/ << "Number of words to add spaces between: " << linewords.size() << endl;
	//-/ << "spacecount passed in: " << spacecount << endl;
	if (linewords.size() > 1) {
		eadd = spacecount / (linewords.size()-1);
		randomadd = spacecount % (linewords.size()-1);
	}

	//-/ << "evenly add(1+n): " << eadd << endl;
	//-/ << "randomly add in: " << randomadd << endl;

	int linewordl = linewords.size() - 1;
	linewordl = linewordl < 0 ? 0 : linewordl;
	//-/ << "linewordl: " << linewordl << endl;
	int *spacepos = new int[linewordl];
	//this adds uniform spacing including those which should be inbetween each word anyhow
	//at minimum adds words-1 spaces
	for (int j = 0; j <linewordl; j++) {
		spacepos[j] = (1 + eadd);
		//-/ << "appending even space to arr " << spacepos[j] << endl;
	}
	//TODO Fix it actually randomly adding in shit!
	//this is procedural as it is
	//this is just random space adding to take the balance
	//not more than one space added to each space spot
	for (int j = 0; j <randomadd; j++){
		spacepos[j]++;
		//-/ << "appending nonevensapce arr " <<  spacepos[j] << endl;
	}
	//-/<< "words on line: " << linewords.size() << endl;
	//-/<< "spaces to add: " << spacecount << endl;
	for (int j = 0; j < linewordl; j++) {
		os.append(linewords[j]);
		////-/
		//-/ << "j: " << j << ", sp: " << spacepos[j] << endl;
		for (int k = 0; k < spacepos[j]; k++)
		{
			////-/ << "appending spaces back to OS " << spacepos[j] << endl;
			os.append(" ");
		}
	}
	os.append(linewords[linewordl] + "\n");
	////-/ << "add space end" << endl;
	//-/ << "endspace" << endl;
	return os;
}
vector<string> pformat::splitstring(string &buffer) {
	vector<string> words;
	buffer.append(" ");
	int strLen = static_cast<int>(buffer.length());
	//load words into vector
	string word = "";
	for (int i = 0; i < strLen; i++) {
		//cout << "loped" << endl;
		char cchar = buffer.at(i);
		//cout << "Char at idx: " << i << " is: " << cchar << endl;
		//-/ << "math " << strLen - i - 1 << endl;*/
		if (isspace(cchar) || i == strLen) {
			//cout << "pushedword" << endl;
			if(word!="")
				words.push_back(word);
			word = "";
			//cout << "continue" << endl;
			continue;
		}
		word += cchar;
	}
	//for (vector<string>::iterator itr = words.begin(); itr != words.end(); itr++)
		//-/ << *itr <<"_"<< endl;
	return words;
}
string pformat::paragraphFormater(std::string &buffer, int &maxlineln) {
	//cout << "call made to pformat" << endl;

	vector<string> words;
	words = pformat::splitstring(buffer);
	//cout << "words split" << endl;
	while (words.size() != 0 && ((words[0].length() != 0 && isspace(words[0].at(0))) || words[0] == ""))
		words.erase(words.begin());
	//format vector into output
	string outstring="";
	vector<string> linewords;
	int count = 0;
	int spacecount = 0;
	//I dont like how this is <= to but
	//cout << "startloop" << endl;
	for (int i = 0; i < words.size(); i++) {
		//load words into a vector for the entire line, spaces to be added afterwards
		if (count <= maxlineln) {
			count += words[i].length() + 1;
			//cout << "word at position " << i << ": " << words[i] << " count @ word " << count << endl;
			//this line may cause some issues check here if it isn't working
			linewords.push_back(words[i]);
			if (count > maxlineln) {
				int tcount = count;
				////-/ << "Tcount: " << tcount << endl;
				count -= words[i].length() + 2;
				spacecount = maxlineln - count;
				//SMOL CASE
				/*cout <<"maxallowed spaces on this line: "<< ((int)linewords.size() - 2) * 3 << endl;
				cout <<"spacecount: " << spacecount << endl;
				cout << "words[i] " << words[i] << endl;*/
				if (spacecount>=((int)linewords.size()-2)*3){
					/*cout << "SMOL CASE" << endl; 
					cout << "INT MAX - thing " << _CRT_SIZE_MAX - 6 << endl;
					cout << "( (tcount - maxlineln) - words[i].length() ) " << ((tcount - maxlineln) - (int) words[i].length()) << endl;
					cout << " (tcount - maxlineln) " << (tcount - maxlineln) << endl;
					cout << "words[i].length() " << words[i].length() << endl;*/
					if (tcount - maxlineln == 1) {
						//cout << "SMOL two WORDS" << endl;
						outstring.append(addspacesback(linewords, zero));
					}
					else if ( ( (tcount - maxlineln) - (int) words[i].length() ) < 0) {
						/*cout << " SMOL hyphenation" << endl;
						cout << "word to hyphenate: " << words[i] << " ";*/
						//-/ << tcount - maxlineln << "characters from the end" << endl;*/
						//-/ << words[i] << "," << words[i].length() << "," << (tcount - maxlineln) << endl;
						//-/ << (	words[i].length() - (tcount - maxlineln) )<< endl;
						string preHyphen = words[i].substr(0, words[i].length() - (tcount - maxlineln)) + "-";
						string aftHyphen = words[i].substr(words[i].length() - (tcount - maxlineln));
						/*//-/ << "sub hyphen " << aftHyphen << endl;
						//-/ << "pre hyphen " << preHyphen << endl;*/
						linewords[linewords.size() - 1] = preHyphen;
						words[i] = aftHyphen;
						i--;
						outstring.append(addspacesback(linewords, zero));
					}
					else{
						outstring.append(" " + linewords[0]+"\n");
						i--;
					}
					count = spacecount = 0;
					linewords.clear();
					continue;
					//hyphenationcasse
				}

				//big words that help the 
				//supercalifragilisticexpialidocious
				//1 word and its larger than maxlineln
				else if (linewords.size()==1) {
					//one word on the line that would fit if we discount the space I added
					//cout << "one word is bigger than the line!" << endl;
					//-/ << "Math: " << linewords[0].size() - maxlineln << endl;*/
					if (linewords[0].size() - maxlineln == 0) {
						outstring.append(addspacesback(linewords, zero));
					}
					else {
						string preHyphen = words[i].substr(0, maxlineln - 1) + "-";
						////-/ << "pre hyphen " << preHyphen << endl;
						string aftHyphen = words[i].substr(maxlineln-1);
						////-/ << "sub hyphen " << aftHyphen << endl;
						linewords[0] = preHyphen;
						words[i] = aftHyphen;
						i--;
						outstring.append(addspacesback(linewords, zero));
					}
					count = spacecount = 0;
					linewords.clear();
					////-/ << "boop" << endl;
					continue;
				}
				linewords.pop_back();
				count += words[i].length()+2;
				i-=2;

			}
		}
		else {
			outstring.append(addspacesback(linewords, spacecount)); 
			count = spacecount = 0;
			linewords.clear();
		}
	}
	//-/ << "endloop" << endl;
	////-/ << "endloop" << endl;
	if (linewords.size() > 0) {
		outstring.append(addspacesback(linewords,zero));
	}
	//-/ << "trime tailing space" << endl;
	//trim tailing space
	if(outstring.length()>1&&isspace(outstring.at(outstring.length()-2)))
		outstring = outstring.substr(0, outstring.length() - 2)+="\n";
	return outstring;
}
