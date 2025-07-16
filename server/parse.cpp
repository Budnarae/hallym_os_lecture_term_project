#include "parse.hpp"
#include <iostream>

vector<string>  parse(string s)
{
	vector<string>  splited;
	stringstream    ss(s);
	string head, tail;

	getline(ss, head, ';');
	getline(ss, tail);
	ss = stringstream(head);
	
	string buf;
	while (getline(ss, buf, ' '))
	{
		splited.push_back(buf);
		buf.clear();
	}
	if (!tail.empty())
		splited.push_back(tail);

	return splited;
}