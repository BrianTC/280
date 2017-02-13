#ifndef PFORMAT_H_
#define PFORMAT_H_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <random>
#include <cctype>
using namespace std;

class pformat
{
public:
	static string paragraphFormater(std::string &buffer, int &maxlineln);
	static vector<string> splitstring(std::string &buffer);
};

#endif  // PFORMAT_H_