//#include "pch.h"
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <utility>
#include <list>
#include <iterator>
#include <cstring>
using namespace std;

list<pair<int, int>> tokenLexeme;
list<pair<int, string>> codeString;
int newLines = 0;

int getCode(string str)
{
	list<pair<int, string>>::iterator it;
	int returnCode = 0;
	for (it = codeString.begin(); it != codeString.end() && returnCode == 0; it++)
		if (str.compare((*it).second) == 0)
			returnCode = (*it).first;

	if (returnCode == 0)
	{
		it--;
		returnCode = (*it).first + 1;
	}

	return returnCode;
}

string getString(int code)
{
	list<pair<int, string>>::iterator it;
	string returnCode = "";
	for (it = codeString.begin(); it != codeString.end() && returnCode == ""; it++)
		if ((*it).first == code)
			returnCode = (*it).second;

	return returnCode;
}

void addString(int code, string str)
{
	list<pair<int, string>>::iterator it;
	it = codeString.end();
	it--;

	if ((*it).first < code)
	{
		pair<int, string> temp;
		temp.first = code;
		temp.second = str;
		codeString.push_back(temp);
	}
}

void addTokenPair(int token, int lexeme)
{
	pair<int, int> temp;
	temp.first = token;
	temp.second = lexeme;
	tokenLexeme.push_back(temp);
}

void displayTokens()
{
	ofstream fout("output.txt");
	list<pair<int, int>>::iterator it;
	for (it = tokenLexeme.begin(); it != tokenLexeme.end(); it++)
	{
		pair<int, int> temp;
		temp.first = (*it).first;
		temp.second = (*it).second;

		string s1, s2;
		if (temp.first >= 256)
			s1 = getString(temp.first);
		else
			s1 += (char)temp.first;

		if (temp.second >= 256)
			s2 = getString(temp.second);
		else
			s2 += (char)temp.second;

		cout << "('" << s1 << "', '" << s2 << "')" << endl;
		fout << "('" << s1 << "', '" << s2 << "')" << endl;
	}
	fout.close();
}

bool dataTypes(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] == 'i' || str[ptr] == 'I')
				state = 1;
			else if (str[ptr] == 'c' || str[ptr] == 'C')
				state = 4;
			else
				flag = true;
			step++;
			break;
		case 1:
			if (str[ptr + step] == 'n' || str[ptr + step] == 'N')
				state = 2;
			else
				flag = true;
			step++;
			break;
		case 2:
			if (str[ptr + step] == 't' || str[ptr + step] == 'T')
				state = 3;
			else
				flag = true;
			step++;
			break;
		case 3:
			if (str[ptr + step] == ':' || str[ptr + step] == ' ')
			{
				int code = getCode("INT");
				addTokenPair(code, '^');
				ans = true;
			}
			flag = true;
			break;
		case 4:
			if (str[ptr + step] == 'h' || str[ptr + step] == 'H')
				state = 5;
			else
				flag = true;
			step++;
			break;
		case 5:
			if (str[ptr + step] == 'a' || str[ptr + step] == 'A')
				state = 6;
			else
				flag = true;
			step++;
			break;
		case 6:
			if (str[ptr + step] == 'r' || str[ptr + step] == 'R')
				state = 7;
			else
				flag = true;
			step++;
			break;
		case 7:
			if (str[ptr + step] == ':' || str[ptr + step] == ' ')
			{
				int code = getCode("CHAR");
				addTokenPair(code, '^');
				ans = true;
			}
			flag = true;
			break;
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool arithematicOperator(char str[], long& ptr)
{
	bool ans = false;

	if (str[ptr] == '+')
	{
		if (str[ptr + 1] == '+') {
			int code = getCode("++");
			addTokenPair(code, '^');
			ans = true;
			ptr += 2;
		}
		else {
			addTokenPair('+', '^');
			ans = true;
			ptr++;
		}
	}
	else if (str[ptr] == '-')
	{
		addTokenPair('-', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == '*')
	{
		addTokenPair('*', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == '/')
	{
		addTokenPair('/', '^');
		ans = true;
		ptr++;
	}

	return ans;
}

bool comments(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] == '/')
				state = 1;
			else if (str[ptr] == '\\')
				state = 4;
			else
				flag = true;
			step++;
			break;
		case 1:
			if (str[ptr + step] == '*')
				state = 2;
			else
				flag = true;
			step++;
			break;
		case 2:
			if (str[ptr + step] == '*')
				state = 3;
			else if (str[ptr + step] == '\n')
				newLines++;
			step++;
			break;
		case 3:
			if (str[ptr + step] == '/')
			{
				int code = getCode("COMMENT");
				addTokenPair(code, '^');
				ans = true;
				flag = true;
			}
			flag = true;
			step++;
			break;
		case 4:
			if (str[ptr] == '\\')
				state = 5;
			else
				flag = true;
			step++;
			break;
		case 5:
			if (str[ptr + step] == '\n' || str[ptr + step] == '\0')
			{
				int code = getCode("COMMENT");
				addTokenPair(code, '^');
				ans = true;
				flag = true;
				newLines++;
			}
			step++;
			break;
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool identifier(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;
	string id = "";

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] >= 'a' && str[ptr] <= 'z' || str[ptr] >= 'A' && str[ptr] <= 'Z')
				state = 1;
			else
				flag = true;
			id += str[ptr];
			step++;
			break;
		case 1:
			if (str[ptr + step] >= 'a' && str[ptr + step] <= 'z' || str[ptr + step] >= 'A' && str[ptr + step] <= 'Z' || str[ptr + step] >= '0' && str[ptr + step] <= '9' || str[ptr + step] == '_')
			{
				state = 1;
				id += str[ptr + step];
				step++;
			}
			else
				state = 2;
			break;
		case 2:
		{
			int code = getCode("ID");
			int code2 = getCode(id);
			addString(code2, id);
			addTokenPair(code, code2);
			ans = true;
			flag = true;
			break;
		}
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool numericConstant(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;
	string id = "";

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr + step] >= '0' && str[ptr + step] <= '9')
			{
				id += str[ptr + step];
				step++;
			}
			else if (step == 0)
				flag = true;
			else
				state = 1;
			break;
		case 1:
		{
			int code = getCode("NUM");
			int code2 = getCode(id);
			addString(code2, id);
			addTokenPair(code, code2);
			ans = true;
			flag = true;
			break;
		}
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool literalConstant(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;
	string id = "";

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr + step] == '\'')
			{
				state = 1;
				step++;
			}
			else
				flag = true;
			break;
		case 1:
			if (str[ptr + step] >= 'a' && str[ptr + step] <= 'z' || str[ptr + step] >= 'A' && str[ptr + step] <= 'Z' || str[ptr + step] <= ' ')
			{
				id += str[ptr + step];
				step++;
			}
			else
				state = 2;
			break;
		case 2:
			if (str[ptr + step] == '\'')
			{
				state = 3;
				step++;
			}
			else
				flag = true;
			break;
		case 3:
		{
			int code = getCode("LIT");
			int code2 = getCode(id);
			addString(code2, id);
			addTokenPair(code, code2);
			ans = true;
			flag = true;
			break;
		}
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool strings(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;
	string id = "";

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr + step] == '"')
			{
				state = 1;
				step++;
			}
			else
				flag = true;
			break;
		case 1:
			if (str[ptr + step] == '\n')
				flag = true;
			else if (str[ptr + step] != '"')
				id += str[ptr + step];
			else
			{
				int code = getCode("STR");
				int code2 = getCode(id);
				addString(code2, id);
				addTokenPair(code, code2);
				ans = true;
				flag = true;
			}
			step++;
			break;
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool separators(char str[], long& ptr)
{
	bool ans = false;

	if (str[ptr] == '(')
	{
		addTokenPair('(', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == ')')
	{
		addTokenPair(')', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == '[')
	{
		addTokenPair('[', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == ']')
	{
		addTokenPair(']', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == '{')
	{
		addTokenPair('{', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == '}')
	{
		addTokenPair('}', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == ':')
	{
		addTokenPair(':', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == ';')
	{
		addTokenPair(';', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == ',')
	{
		addTokenPair(',', '^');
		ans = true;
		ptr++;
	}
	return ans;
}

bool conditionals(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;
	string condition = "";

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] == 'i' || str[ptr] == 'I')
				state = 1;
			else if (str[ptr] == 'e' || str[ptr] == 'E')
				state = 3;
			else
				flag = true;
			step++;
			break;
		case 1:
			if (str[ptr + step] == 'f' || str[ptr + step] == 'F')
				state = 2;
			else
				flag = true;
			step++;
			break;
		case 2:
			if (str[ptr + step] == ' ' || str[ptr + step] == '(')
			{
				int code = getCode("CS");
				int code2 = getCode("IF");
				addTokenPair(code, code2);
				ans = true;
			}
			flag = true;
			break;
		case 3:
			if (str[ptr + step] == 'l' || str[ptr + step] == 'L')
				state = 4;
			else
				flag = true;
			step++;
			break;
		case 4:
			if (str[ptr + step] == 's' || str[ptr + step] == 'S')
				state = 5;
			else if (str[ptr + step] == 'i' || str[ptr + step] == 'I')
				state = 7;
			else
				flag = true;
			step++;
			break;
		case 5:
			if (str[ptr + step] == 'e' || str[ptr + step] == 'E')
				state = 6;
			else
				flag = true;
			step++;
			break;
		case 6:
			if (str[ptr + step] == ' ')
			{
				int code = getCode("CS");
				int code2 = getCode("ELSE");
				addTokenPair(code, code2);
				ans = true;
			}
			flag = true;
			step++;
			break;
		case 7:
			if (str[ptr + step] == 'f' || str[ptr + step] == 'F')
				state = 8;
			else
				flag = true;
			step++;
			break;
		case 8:
			if (str[ptr + step] == ' ' || str[ptr + step] == '(')
			{
				int code = getCode("CS");
				int code2 = getCode("ELIF");
				addTokenPair(code, code2);
				ans = true;
			}
			flag = true;
			break;
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool loop(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] == 'w' || str[ptr] == 'W')
				state = 1;
			else
				flag = true;
			step++;
			break;
		case 1:
			if (str[ptr + step] == 'h' || str[ptr + step] == 'H')
				state = 2;
			else
				flag = true;
			step++;
			break;
		case 2:
			if (str[ptr + step] == 'i' || str[ptr + step] == 'I')
				state = 3;
			else
				flag = true;
			step++;
			break;
		case 3:
			if (str[ptr + step] == 'l' || str[ptr + step] == 'L')
				state = 4;
			else
				flag = true;
			step++;
			break;
		case 4:
			if (str[ptr + step] == 'e' || str[ptr + step] == 'E')
				state = 5;
			else
				flag = true;
			step++;
			break;
		case 5:
			if (str[ptr + step] == ' ' || str[ptr + step] == '(')
			{
				int code = getCode("WHILE");
				addTokenPair(code, '^');
				ans = true;
			}
			flag = true;
			break;
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool input(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] == 'i' || str[ptr] == 'I')
				state = 1;
			else
				flag = true;
			step++;
			break;
		case 1:
			if (str[ptr + step] == 'n' || str[ptr + step] == 'N')
				state = 2;
			else
				flag = true;
			step++;
			break;
		case 2:
			if (str[ptr + step] == 'p' || str[ptr + step] == 'P')
				state = 3;
			else
				flag = true;
			step++;
			break;
		case 3:
			if (str[ptr + step] == 'u' || str[ptr + step] == 'U')
				state = 4;
			else
				flag = true;
			step++;
			break;
		case 4:
			if (str[ptr + step] == 't' || str[ptr + step] == 'T')
				state = 5;
			else
				flag = true;
			step++;
			break;
		case 5:
			if (str[ptr + step] == ' ' || str[ptr + step] == '-')
				state = 7;
			else
				flag = true;
			break;
		case 7:
		{
			int code = getCode("INPUT");
			addTokenPair(code, '^');
			ans = true;
			flag = true;
			break;
		}
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool print(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] == 'p' || str[ptr] == 'P')
				state = 1;
			else
				flag = true;
			step++;
			break;
		case 1:
			if (str[ptr + step] == 'r' || str[ptr + step] == 'R')
				state = 2;
			else
				flag = true;
			step++;
			break;
		case 2:
			if (str[ptr + step] == 'i' || str[ptr + step] == 'I')
				state = 3;
			else
				flag = true;
			step++;
			break;
		case 3:
			if (str[ptr + step] == 'n' || str[ptr + step] == 'N')
				state = 4;
			else
				flag = true;
			step++;
			break;
		case 4:
			if (str[ptr + step] == 't' || str[ptr + step] == 'T')
				state = 5;
			else
				flag = true;
			step++;
			break;
		case 5:
			if (str[ptr + step] == '(')
			{
				int code = getCode("PRINT");
				addTokenPair(code, '^');
				ans = true;
				flag = true;
			}
			else if (str[ptr + step] == 'l' || str[ptr + step] == 'L')
			{
				state = 6;
				step++;
			}
			else
				flag = true;
			break;
		case 6:
			if (str[ptr + step] == 'n' || str[ptr + step] == 'N')
				state = 7;
			else
				flag = true;
			step++;
			break;
		case 7:
			if (str[ptr + step] == '(')
			{
				int code = getCode("PRINTLN");
				addTokenPair(code, '^');
				ans = true;
			}
			flag = true;
			break;
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool relationalOperators(char str[], long& ptr)
{
	int state = 0, step = 0;
	bool flag = false;
	bool ans = false;

	while (flag == false)
	{
		switch (state)
		{
		case 0:
			if (str[ptr] == '<')
				state = 1;
			else if (str[ptr] == '>')
				state = 2;
			else if (str[ptr] == '=')
				state = 3;
			else if (str[ptr] == '!')
				state = 4;
			else
				flag = true;
			step++;
			break;
		case 1:
			if (str[ptr + step] == '=')
			{
				int code = getCode("RO");
				int code2 = getCode("<=");
				addTokenPair(code, code2);
				ans = true;
			}
			else
			{
				int code = getCode("RO");
				addTokenPair(code, '<');
				ans = true;
			}
			flag = true;
			step++;
			break;
		case 2:
			if (str[ptr + step] == '=')
			{
				int code = getCode("RO");
				int code2 = getCode(">=");
				addTokenPair(code, code2);
				ans = true;
			}
			else
			{
				int code = getCode("RO");
				addTokenPair(code, '>');
				ans = true;
			}
			flag = true;
			step++;
			break;
		case 3:
			if (str[ptr + step] == '=')
			{
				int code = getCode("RO");
				int code2 = getCode("==");
				addTokenPair(code, code2);
				ans = true;
			}
			flag = true;
			step++;
			break;
		case 4:
			if (str[ptr + step] == '=')
			{
				int code = getCode("RO");
				int code2 = getCode("!=");
				addTokenPair(code, code2);
				ans = true;
			}
			flag = true;
			step++;
			break;
		default:
			break;
		}
	}

	if (ans)
		ptr += step;

	return ans;
}

bool assignmentOperator(char str[], long& ptr)
{
	bool ans = false;

	if (str[ptr] == '=' && str[ptr + 1] != '=')
	{
		addTokenPair('=', '^');
		ans = true;
		ptr++;
	}
	else if (str[ptr] == '-' && str[ptr + 1] == '>') {
		int code = getCode("->");
		addTokenPair(code, '^');
		ans = true;
		ptr += 2;
	}
	return ans;
}

int charsTillNewLine(char str[]) {
	int i = 0, passed = 0;

	while (passed < newLines) {
		if (str[i] == '\n')
			passed++;
		i++;
	}
	return i;
}

bool lex(char str[])
{
	long i = 0;
	bool flag = true;
	long len = strlen(str);


	while (flag && i < len)
	{
		while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') {
			if (str[i] == '\n')
				newLines++;
			i++;
		}
		if (!comments(str, i))
			if (!dataTypes(str, i))
				if (!assignmentOperator(str, i))
					if (!arithematicOperator(str, i))
						if (!numericConstant(str, i))
							if (!literalConstant(str, i))
								if (!strings(str, i))
									if (!separators(str, i))
										if (!conditionals(str, i))
											if (!loop(str, i))
												if (!input(str, i))
													if (!print(str, i))
														if (!relationalOperators(str, i))
															if (!identifier(str, i))
																if (str[i] != '\0')
																	flag = false;
		if (str[i] == '\0')
			i += 10;
	}

	if (!flag) {
		int col = i - charsTillNewLine(str);
		cout << "Error at Token Number " << i << ", Located at Line " << newLines + 1 << ", Column " << col + 1 << endl;
	}
	return flag;
}

void initialise()
{
	string keyWords[] = { "INT", "CHAR", "COMMENT", "ID", "NUM", "LIT", "STR", "CS", "IF", "ELSE", "ELIF", "WHILE", "INPUT", "PRINT", "PRINT", "PRINTLN", "RO", "<=", ">=", "==", "!=", "->", "++" };
	for (int i = 0; i < 23; i++)
	{
		pair<int, string> temp;
		temp.first = 256 + i;
		temp.second = keyWords[i];
		codeString.push_back(temp);
	}
}

string readFile()
{
	string myText;
	string temp;
	ifstream fileReader("input.cc");

	while (getline(fileReader, temp))
		myText += (temp + "\n");

	fileReader.close();

	return myText;
}

int main()
{
	initialise();

	string temp = readFile();
	char data[10240];
	strcpy_s(data, temp.c_str());
	if (lex(data))
		displayTokens();

}

