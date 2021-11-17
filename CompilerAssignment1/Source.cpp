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


class Lexer {
public:
	list<pair<int, int>>::iterator tokenLexemeIt;
	list<pair<int, int>> tokenLexeme;
	list<pair<int, string>> codeString;
	int newLines = 0;
	string inputFilename, outputFilename;
	string lexString;
	Lexer(string _inputFilename, string _outputFilename) {
		inputFilename = _inputFilename;
		outputFilename = _outputFilename;
	}
	//struct Token{
	//	Token(string t, string k) {
	//		text = t;
	//		kind = k;
	//	}
	//	string text;
	//	string kind;
	//};
	//enum TokenType {
	//	a=0
	//};
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
				//if (str[ptr + step] >= 'a' && str[ptr + step] <= 'z' || str[ptr + step] >= 'A' && str[ptr + step] <= 'Z' || str[ptr + step] <= ' ')
				if (str[ptr + step] <= 255)
				{
					id += str[ptr + step];
					step++;
					state = 2;
				}
				else
					flag = true;
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

	bool lex()
	{
		char str[10240];
		strcpy_s(str, lexString.c_str());
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

	void readFile()
	{
		string myText;
		string temp;
		ifstream fileReader(inputFilename);

		while (getline(fileReader, temp))
			myText += (temp + "\n");

		fileReader.close();

		lexString = myText;
	}
	void displayTokens()
	{
		ofstream fout(outputFilename);
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
	pair<string, string> getToken() {
		pair<string, string> token("", "");
		if (tokenLexemeIt != tokenLexeme.end()) {
			pair<int, int> temp;
			temp.first = (*tokenLexemeIt).first;
			temp.second = (*tokenLexemeIt).second;

			string s1, s2;
			if (temp.first >= 256)
				s1 = getString(temp.first);
			else
				s1 += (char)temp.first;

			if (temp.second >= 256)
				s2 = getString(temp.second);
			else
				s2 += (char)temp.second;

			token.first = s1;
			token.second = s2;

			tokenLexemeIt++;
		}

		return token;
	}
	void GetReadyForParser() {
		addTokenPair('\0', '^');
		tokenLexemeIt = tokenLexeme.begin();
	}
};
//void parserTest()
//{
//	tokenLexemeIt = tokenLexeme.begin();
//	string token = "";
//	while ((token = getToken().first) != "") {
//		cout << token<<endl;
//	}
//}
class Parser {
public:
	Lexer* lexer;
	string currToken;
	string currLexeme;
	string peekToken;
	string peekLexeme;
	string symbolTableFilename;
	list<pair<string, string>> symbolTable;
	ofstream fout;
	int noOfTabs;
	int prevNoOfTabs;

	Parser(Lexer* _lexer, string _symbolTableFilename) {
		lexer = _lexer;
		noOfTabs = 0;
		prevNoOfTabs = 0;
		symbolTableFilename = _symbolTableFilename;
		fout.open(symbolTableFilename);
	}
	bool checkToken(string tok) {
		return tok == currToken;
	}
	void addToSymbolTable(string type, string symbol)
	{
		pair<string, string> temp;
		temp.first = type;
		temp.second = symbol;
		symbolTable.push_back(temp);
		fout << "('" << type << "', '" << symbol << "')" << endl;
	}
	void initialize() {
		//Call this twice to initialize current and peek
		nextToken();
		nextToken();
	}
	bool checkPeek(string tok) {
		return tok == peekToken;
	}
	void match(string tok) {
		if (!checkToken(tok))
			abort("Expected " + tok + ", got " + currToken);
		nextToken();
	}
	void nextToken() {
		currToken = peekToken;
		currLexeme = peekLexeme;
		pair<string, string> tokenLexemePair = lexer->getToken();
		peekToken = tokenLexemePair.first;
		peekLexeme = tokenLexemePair.second;
	}
	void abort(string message) {
		cout << "Error: " + message << endl;
		exit(-1);
	}
	void program() {
		//Parse all the statements in the program.
		//while (!checkToken("\0"))
		statements();
	}
	void num() {
		cout << endl;
		noOfTabs++;
		tabs("num");
		if (checkToken("NUM")) {
			match("NUM");
		}
		else if (checkToken("ID")) {
			match("ID");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void i2() {
		cout << endl;
		noOfTabs++;
		tabs("i2");
		if (checkToken(";")) {
			match(";");
		}
		else if (checkToken(",")) {
			match(",");
			addToSymbolTable("INT",currLexeme);
			match("ID");
			prevNoOfTabs = noOfTabs;
			i2();
		}
		else if (checkToken("=")) {
			match("=");
			num();
			if (checkToken(",")) {
				match(",");
				addToSymbolTable("INT", currLexeme);
				match("ID");
				prevNoOfTabs = noOfTabs;
				i2();
			}
			else if (checkToken(";")) {
				match(";");
			}
			//double check it!
			else if (checkToken("+") || checkToken("-") || checkToken("/") || checkToken("*")) {
				prevNoOfTabs = noOfTabs;
				E3();
				prevNoOfTabs = noOfTabs;
				i2();
			}
			else {
				abort("Bad token: " + currToken);
			}
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void j() {
		cout << endl;
		noOfTabs++;
		tabs("j");
		if (checkToken(",")) {
			match(",");
			prevNoOfTabs = noOfTabs;
			num();
			prevNoOfTabs = noOfTabs;
			j();
		}
		noOfTabs--;
	}
	void i3() {
		cout << endl;
		noOfTabs++;
		tabs("i3");
		if (checkToken(";")) {
			match(";");
		}
		else if (checkToken("=")) {
			match("=");
			match("{");
			prevNoOfTabs = noOfTabs;
			num();
			prevNoOfTabs = noOfTabs;
			j();
			match("}");
			match(";");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void i1() {
		cout << endl;
		noOfTabs++;
		tabs("i1");
		if (checkToken("{")) {
			match("{");
			prevNoOfTabs = noOfTabs;
			num();
			match("}");
			prevNoOfTabs = noOfTabs;
			i3();
		}
		else {
			prevNoOfTabs = noOfTabs;
			i2();
		}
		noOfTabs--;
	}
	void k() {
		cout << endl;
		noOfTabs++;
		tabs("k");
		if (checkToken(",")) {
			match(",");
			prevNoOfTabs = noOfTabs;
			alphaNumber();
			prevNoOfTabs = noOfTabs;
			k();
		}
		noOfTabs--;
	}
	void alphaNumber() {
		cout << endl;
		noOfTabs++;
		tabs("alphaNumber");
		if (checkToken("LIT")) {
			match("LIT");
		}
		else if (checkToken("ID")) {
			match("ID");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void c4() {
		cout << endl;
		noOfTabs++;
		tabs("c4");
		if (checkToken("{")) {
			match("{");
			prevNoOfTabs = noOfTabs;
			alphaNumber();
			prevNoOfTabs = noOfTabs;
			k();
			match("}");
			match(";");
		}
		else if (checkToken("STR")) {
			match("STR");
			match(";");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void c3() {
		cout << endl;
		noOfTabs++;
		tabs("c3");
		if (checkToken(";")) {
			match(";");
		}
		else if (checkToken("=")) {
			match("=");
			prevNoOfTabs = noOfTabs;
			c4();
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void c2() {
		cout << endl;
		noOfTabs++;
		tabs("c2");
		if (checkToken(";")) {
			match(";");
		}
		else if (checkToken(",")) {
			match(",");
			match("ID");
			prevNoOfTabs = noOfTabs;
			c2();
		}
		else if (checkToken("=")) {
			match("=");
			prevNoOfTabs = noOfTabs;
			alphaNumber();
			if (checkToken(",")) {
				match(",");
				match("ID");
				prevNoOfTabs = noOfTabs;
				c2();
			}
			else if (checkToken(";")) {
				match(";");
			}
			else {
				abort("Bad token: " + currToken);
			}
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void c1() {
		cout << endl;
		noOfTabs++;
		tabs("c1");
		if (checkToken("{")) {
			match("{");
			prevNoOfTabs = noOfTabs;
			num();
			match("}");
			prevNoOfTabs = noOfTabs;
			c3();
		}
		else {
			prevNoOfTabs = noOfTabs;
			c2();
		}
		noOfTabs--;
	}
	void declaration() {
		cout << endl;
		noOfTabs++;
		tabs("Declaration");
		//cout << "Declaration";
		if (checkToken("INT")) {
			match("INT");
			match(":");
			addToSymbolTable("INT", currLexeme);
			match("ID");
			prevNoOfTabs = noOfTabs;
			i1();
		}
		else if (checkToken("CHAR")) {
			match("CHAR");
			match(":");
			addToSymbolTable("CHAR", currLexeme);
			match("ID");
			prevNoOfTabs = noOfTabs;
			c1();
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;
	}
	void R() {
		cout << endl;
		noOfTabs++;
		tabs("R");
		if (checkToken("LIT")) {
			match("LIT");
		}
		else if (checkToken("NUM")) {
			match("NUM");
		}
		else if (checkToken("ID")) {
			match("ID");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void E3() {
		cout << endl;
		noOfTabs++;
		tabs("E3");
		if (checkToken("+")) {
			match("+");
			prevNoOfTabs = noOfTabs;
			R();
			prevNoOfTabs = noOfTabs;
			E3();
		}
		else if (checkToken("-")) {
			match("-");
			prevNoOfTabs = noOfTabs;
			R();
			prevNoOfTabs = noOfTabs;
			E3();
		}
		else if (checkToken("/")) {
			match("/");
			prevNoOfTabs = noOfTabs;
			R();
			prevNoOfTabs = noOfTabs;
			E3();
		}
		else if (checkToken("*")) {
			match("*");
			prevNoOfTabs = noOfTabs;
			R();
			prevNoOfTabs = noOfTabs;
			E3();
		}
		//else {
		//	abort("Bad token: " + currToken);
		//}
		noOfTabs--;
	}
	void E() {
		cout << endl;
		noOfTabs++;
		tabs("E");
		if (checkToken("LIT")) {
			match("LIT");
			prevNoOfTabs = noOfTabs;
			E3();
		}
		else if (checkToken("NUM")) {
			match("NUM");
			prevNoOfTabs = noOfTabs;
			E3();
		}
		else if (checkToken("ID")) {
			match("ID");
			prevNoOfTabs = noOfTabs;
			B();
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void B() {
		cout << endl;
		noOfTabs++;
		tabs("B");
		if (checkToken("=")) {
			match("=");
			prevNoOfTabs = noOfTabs;
			E();
		}
		else if (checkToken("+") || checkToken("-") || checkToken("*") || checkToken("/")) {
			prevNoOfTabs = noOfTabs;
			E3();
		}
		noOfTabs--;
	}
	/*void assignment() {
		cout << "Assignment" << endl;
		if (checkToken("ID")) {
			match("ID");
			if (checkToken("++")) {
				increment();
			}
			else
				if (checkToken("=")) {
					match("=");
					E();
					match(";");
				}
				else {
					abort("Bad token: " + currToken);
				}
		}
		else {
			abort("Bad token: " + currToken);
		}
		cout << endl;
	}*/
	void assignment() {
		cout << endl;
		noOfTabs++;
		tabs("Assignment");
		//cout << "Assignment";
		if (checkToken("ID")){ 
			match("ID");
			match("=");
			prevNoOfTabs = noOfTabs;
			E();
			match(";");
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;
	}
	//OLD
	/*void R() {
		if (checkToken("CHAR")) {
			match("CHAR");

		}
		else if (checkToken("ID")) {
			match("ID");
		}
		else if (checkToken("NUM")) {
			match("NUM");
		}
		else {
			abort("Bad token: " + currToken);
		}
	}*/
	void conditionalStatement() {
		cout << endl;
		noOfTabs++;
		tabs("conditionalStatement");
		if (checkToken("LIT") || checkToken("NUM") || checkToken("ID")) {
			prevNoOfTabs = noOfTabs;
			R();
			match("RO");
			prevNoOfTabs = noOfTabs;
			R();
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void whileLoop() {
		cout << endl;
		noOfTabs++;
		tabs("WhileLoop");
		//cout << "WhileLoop";
		if (checkToken("WHILE")) {
			match("WHILE");
			conditionalStatement();
			match(":");
			match("{");
			prevNoOfTabs = noOfTabs;
			statements();
			match("}");
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;

	}
	void A() {
		cout << endl;
		noOfTabs++;
		tabs("A");
		if (checkToken("ELIF")) {
			match("ELIF");
			prevNoOfTabs = noOfTabs;
			conditionalStatement();
			match(":");
			match("{");
			prevNoOfTabs = noOfTabs;
			statements();
			match("}");
			prevNoOfTabs = noOfTabs;
			A();
		}
		else if (checkToken("ELSE")) {
			match("ELSE");
			match(":");
			match("{");
			prevNoOfTabs = noOfTabs;
			statements();
			match("}");
		}
		noOfTabs--;
	}
	void IfStatements() {
		cout << endl;
		noOfTabs++;
		tabs("If");
		//cout << "If";
		if (checkToken("IF")) {
			match("IF");
			prevNoOfTabs = noOfTabs;
			conditionalStatement();
			match(":");
			match("{");
			prevNoOfTabs = noOfTabs;
			statements();
			match("}");
			prevNoOfTabs = noOfTabs;
			A();
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;
	}
	void s() {
		cout << endl;
		noOfTabs++;
		tabs("s");
		if (checkToken("STR")) {
			match("STR");
		}
		else {
			prevNoOfTabs = noOfTabs;
			R();
		}
		noOfTabs--;
	}
	void p() {
		cout << endl;
		noOfTabs++;
		tabs("p");
		if (checkToken("(")) {
			match("(");
			prevNoOfTabs = noOfTabs;
			s();
			match(")");
			match(";");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	void prints() {
		cout << endl;
		noOfTabs++;
		tabs("Print");
		//cout << "Print";
		if (checkToken("PRINT")) {
			match("PRINT");
			prevNoOfTabs = noOfTabs;
			p();
		}
		else if (checkToken("PRINTLN")) {
			match("PRINTLN");
			prevNoOfTabs = noOfTabs;
			p();
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;
	}
	void input() {
		cout << endl;
		noOfTabs++;
		tabs("Input");
		//cout << "Input";
		if (checkToken("INPUT")) {
			match("INPUT");
			match("->");
			match("ID");
			match(";");
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;
	}
	void increment() {
		cout << endl;
		noOfTabs++;
		tabs("increment");
		//cout << "Increment";
		if (checkToken("ID")) {
			match("ID");
			match("++");
			match(";");
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;
	}
	void comment(){
		cout << endl;
		noOfTabs++;
		tabs("comment");
		//cout << "Comment";
		if (checkToken("COMMENT")) {
			match("COMMENT");
		}
		else if (checkToken("/")) {
			match("/");
			match("/");
		}
		else {
			abort("Bad token: " + currToken);
		}
		//cout << endl;
		noOfTabs--;
	}
	void u() {
		cout << endl;
		noOfTabs++;
		tabs("u");
		//cout << "u";
		if (checkToken("INT") || checkToken("CHAR") || (checkToken("ID") && checkPeek("++")) || checkToken("ID")
			|| checkToken("WHILE") || checkToken("IF") || checkToken("PRINT") || checkToken("PRINTLN")
			|| checkToken("INPUT") || checkToken("COMMENT") || checkToken("/")) {
			prevNoOfTabs = noOfTabs;
			statements();
		}
		noOfTabs--;
	}
	void statements() {
		cout << endl;
		noOfTabs++;
		tabs("statements");
		//cout << "statements";
		if (checkToken("INT") || checkToken("CHAR")) {
			prevNoOfTabs = noOfTabs;
			declaration();
			prevNoOfTabs = noOfTabs;
			u();
			//nextToken();
		}
		else if (checkToken("ID") && checkPeek("++")) {
			prevNoOfTabs = noOfTabs;
			increment();
			prevNoOfTabs = noOfTabs;
			u();
			//nextToken();
		}
		else if (checkToken("ID")) {
			prevNoOfTabs = noOfTabs;
			assignment();
			prevNoOfTabs = noOfTabs;
			u();
			//nextToken();
		}
		else if (checkToken("WHILE")) {
			prevNoOfTabs = noOfTabs;
			whileLoop();
			prevNoOfTabs = noOfTabs;
			u();
			//nextToken();
		}
		else if (checkToken("IF")) {
			prevNoOfTabs = noOfTabs;
			IfStatements();
			prevNoOfTabs = noOfTabs;
			u();
			//nextToken();
		}
		else if (checkToken("PRINT") || checkToken("PRINTLN")) {
			prevNoOfTabs = noOfTabs; 
			prints();
			prevNoOfTabs = noOfTabs; 
			u();
			//nextToken();
		}
		else if (checkToken("INPUT")) {
			prevNoOfTabs = noOfTabs; 
			input();
			prevNoOfTabs = noOfTabs;
			u();
			//nextToken();
		}
		else if (checkToken("COMMENT") || checkToken("/")) {
			prevNoOfTabs = noOfTabs; 
			comment();
			prevNoOfTabs = noOfTabs; 
			u();
			//nextToken();
		}
		else {
			abort("Invalid statement at " + currToken);
		}
		noOfTabs--;
	}
	void tabs(string str) {
		for (int i = 0; i < prevNoOfTabs; i++)
			cout << "\t";
		cout << "|";

		for(int i =0;i< (noOfTabs - prevNoOfTabs + 2);i++)
			cout << "__";
		cout << str;
	}
};

int main()
{
	Lexer lexer("input.cc", "output.txt");
	lexer.initialise();
	lexer.readFile();
	if (lexer.lex())
	{
		//lexer.displayTokens();
		lexer.GetReadyForParser();
		Parser parser(&lexer, "symbolTable.txt");
		parser.initialize();
		parser.program();
	}
}








/*cout << "CODETRSINGS!!\n";
list<pair<int, string>>::iterator it;
for (it = codeString.begin(); it != codeString.end(); it++)
{
cout << "('" <<it->first << "', '" << it->second << "')" << endl;
}*/