//#include "pch.h"
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <utility>
#include <list>
#include <iterator>
#include <cstring>
#include <unordered_map>
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
					int code = getCode("IF");
					addTokenPair(code, '^');
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
				if (str[ptr + step] == ' ' || str[ptr + step] == ':')
				{
					int code = getCode("ELSE");
					addTokenPair(code, '^');
					ans = true;
				}
				flag = true;
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
					int code = getCode("ELIF");
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
		string keyWords[] = { "INT", "CHAR", "COMMENT", "ID", "NUM", "LIT", "STR", "IF", "ELSE", "ELIF", "WHILE", "INPUT", "PRINT", "PRINT", "PRINTLN", "RO", "<=", ">=", "==", "!=", "->", "++" };
		for (int i = 0; i < 22; i++)
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
	//Modify symbol table for char array
public:
	Lexer* lexer;
	string currToken;
	string currLexeme;
	string peekToken;
	string peekLexeme;
	string symbolTableFilename;
	string tacFileName;
	unordered_map<string, pair<string, string>> symbolTable;
	unordered_map<string, int> opCodeTable;
	//list<pair<string, pair<string, string>>> symbolTable;
	ofstream fout;
	ofstream fout2;
	int currentAddress;
	int noOfTabs;
	int prevNoOfTabs;
	int tokenCount;
	int lineNumber;
	string lastSymbolNameInSymbolTable;
	Parser() {

	}
	Parser(Lexer* _lexer, string _symbolTableFilename, string _tacFileName) {
		lexer = _lexer;
		tokenCount = 0;
		currentAddress = 0;
		noOfTabs = 0;
		prevNoOfTabs = 0;
		lineNumber = 0;
		symbolTableFilename = _symbolTableFilename;
		tacFileName = _tacFileName;
		fout.open(symbolTableFilename);
		fout2.open(tacFileName);

		initializeOpCodeTable();
	}
	void initializeOpCodeTable() {
		opCodeTable["=="] = 0;
		opCodeTable["<="] = 1;
		opCodeTable[">="] = 2;
		opCodeTable["!="] = 3;
		opCodeTable[">"] = 4;
		opCodeTable["<"] = 5;
		opCodeTable["+"] = 6;
		opCodeTable["-"] = 7;
		opCodeTable["/"] = 8;
		opCodeTable["*"] = 9;
		opCodeTable["goto"] = 10;
		opCodeTable["INI"] = 11;
		opCodeTable["INC"] = 12;
		opCodeTable["OUTI"] = 13;
		opCodeTable["OUTC"] = 14;
		opCodeTable["OUTS"] = 15;
		opCodeTable["="] = 16;
	}
	string getNewToken() {
		char buffer[10];
		_itoa_s(tokenCount, buffer, 10);
		tokenCount++;
		string a = "t";
		string b = buffer;
		return a + b;
	}
	bool checkToken(string tok) {
		return tok == currToken;
	}
	void addToSymbolTable(string type, string symbol, int _size = 0)
	{
		/*pair<string, pair<string, string>> temp;
		temp.first = type;
		temp.second.first = symbol;
		temp.second.second = to_string(currentAddress);
		symbolTable.push_back(temp);*/

		pair<string, string> temp;
		temp.first = type;
		temp.second = to_string(currentAddress);
		symbolTable[symbol] = temp;

		lastSymbolNameInSymbolTable = symbol;
		int size = 0;
		if (_size != 0)
			size = _size;
		else if (type._Equal("INT"))
			size = 4;
		else
			size = 1;

		fout << "('" << type << "', '" << symbol << ", " << currentAddress << "')" << endl;
		currentAddress += size;
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
			string tokenName = getNewToken();
			addToSymbolTable("INT", tokenName);
			fout2 << tokenName;
			match("NUM");
		}
		else if (checkToken("ID")) {
			fout2 << currLexeme;
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
			addToSymbolTable("INT", currLexeme);
			match("ID");
			prevNoOfTabs = noOfTabs;
			i2();
		}
		else if (checkToken("=")) {
			match("=");
			if (!checkPeek("+") && !checkPeek("-") && !checkPeek("/") && !checkPeek("*")) {
				//fout2 << symbolTable.back().second.first << " = ";
				fout2 << lastSymbolNameInSymbolTable << " = ";
				num();
				fout2 << endl;
				lineNumber++;
			}
			else {
				prevNoOfTabs = noOfTabs;
				//string myName = symbolTable.back().second.first;
				string myName = lastSymbolNameInSymbolTable;
				fout2 << myName << " = " << E() << endl;
				lineNumber++;
				prevNoOfTabs = noOfTabs;
				i2();
				return;
			}
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
			//else if (checkToken("+") || checkToken("-") || checkToken("/") || checkToken("*")) {
			//	prevNoOfTabs = noOfTabs;
			//	E3();
			//	prevNoOfTabs = noOfTabs;
			//	i2();
			//}
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
			fout2 << ", ";
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
			fout2 << "{0}";
		}
		else if (checkToken("=")) {
			match("=");
			match("{");
			fout2 << "{";
			prevNoOfTabs = noOfTabs;
			num();
			prevNoOfTabs = noOfTabs;
			j();
			match("}");
			fout2 << "}";
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
		if (checkToken("[")) {
			match("[");
			prevNoOfTabs = noOfTabs;
			//fout2 << symbolTable.back().second.first << "[";
			fout2 << lastSymbolNameInSymbolTable << "[";
			num();
			match("]");
			fout2 << "] = ";
			prevNoOfTabs = noOfTabs;
			i3();
			fout2 << endl;
			lineNumber++;
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
			fout2 << ", ";
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
			string tokenName = getNewToken();
			addToSymbolTable("CHAR", tokenName);
			fout2 << tokenName;
			match("LIT");
		}
		else if (checkToken("ID")) {
			fout2 << currLexeme;
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
			fout2 << " = {";
			alphaNumber();
			prevNoOfTabs = noOfTabs;
			k();
			match("}");
			fout2 << "}";
			match(";");
		}
		else if (checkToken("STR")) {
			int size = currLexeme.size();
			match("STR");
			string tokenName = getNewToken();
			addToSymbolTable("CHAR[]", tokenName, size);
			fout2 << " = " << tokenName << endl;
			lineNumber++;
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
		//symbolTable.back().first = "CHAR[]";
		symbolTable[lastSymbolNameInSymbolTable].first = "CHAR[]";
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
			addToSymbolTable("CHAR", currLexeme);
			match("ID");
			prevNoOfTabs = noOfTabs;
			c2();
		}
		else if (checkToken("=")) {
			match("=");
			//fout2 << symbolTable.back().second.first << " = ";
			fout2 << lastSymbolNameInSymbolTable << " = ";
			prevNoOfTabs = noOfTabs;
			alphaNumber();
			fout2 << endl;
			lineNumber++;
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
		if (checkToken("[")) {
			match("[");
			//fout2 << symbolTable.back().second.first << "[";
			fout2 << lastSymbolNameInSymbolTable << "[";
			prevNoOfTabs = noOfTabs;
			num();
			match("]");
			fout2 << "]";
			prevNoOfTabs = noOfTabs;
			c3();
			fout2 << endl;
			lineNumber++;
		}
		else {
			prevNoOfTabs = noOfTabs;
			c2();
		}
		noOfTabs--;
	}
	void declaration() {
		cout << "Declaration" << endl;
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
		cout << endl;
	}
	void R() {
		cout << endl;
		noOfTabs++;
		tabs("R");
		if (checkToken("LIT")) {
			string tokenName = getNewToken();
			addToSymbolTable("CHAR", tokenName);
			fout2 << tokenName;
			match("LIT");
		}
		else if (checkToken("NUM")) {
			string tokenName = getNewToken();
			addToSymbolTable("INT", tokenName);
			fout2 << tokenName;
			match("NUM");
		}
		else if (checkToken("ID")) {
			fout2 << currLexeme;
			match("ID");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
	}
	string R2() {
		string ans = "";
		cout << endl;
		noOfTabs++;
		tabs("R2");
		if (checkToken("LIT")) {
			string tokenName = getNewToken();
			addToSymbolTable("CHAR", tokenName);
			ans = tokenName;
			match("LIT");
		}
		else if (checkToken("NUM")) {
			string tokenName = getNewToken();
			addToSymbolTable("INT", tokenName);
			ans = tokenName;
			match("NUM");
		}
		else if (checkToken("ID")) {
			ans = currLexeme;
			match("ID");
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
		return ans;
	}
	//void E3() {
	//	cout << endl;
	//	noOfTabs++;
	//	tabs("E3");
	//	if (checkToken("+")) {
	//		match("+");
	//		prevNoOfTabs = noOfTabs;
	//		R2();
	//		prevNoOfTabs = noOfTabs;
	//		E3();
	//	}
	//	else if (checkToken("-")) {
	//		match("-");
	//		prevNoOfTabs = noOfTabs;
	//		R2();
	//		prevNoOfTabs = noOfTabs;
	//		E3();
	//	}
	//	else if (checkToken("/")) {
	//		match("/");
	//		prevNoOfTabs = noOfTabs;
	//		R2();
	//		prevNoOfTabs = noOfTabs;
	//		E3();
	//	}
	//	else if (checkToken("*")) {
	//		match("*");
	//		prevNoOfTabs = noOfTabs;
	//		R2();
	//		prevNoOfTabs = noOfTabs;
	//		E3();
	//	}
	//	//else {
	//	//	abort("Bad token: " + currToken);
	//	//}
	//	noOfTabs--;
	//}
	string E() {
		cout << endl;
		noOfTabs++;
		tabs("E");
		string t1 = M();
		string t2 = X();
		string ans = getNewToken();
		addToSymbolTable("INT", ans);
		fout2 << ans << " = " << t1 << t2 << endl;
		lineNumber++;
		noOfTabs--;
		return ans;
	}
	string M() {
		cout << endl;
		noOfTabs++;
		tabs("M");
		string t1 = D();
		string t2 = Y();
		string ans = "";
		if (t2.size() > 0) {
			ans = getNewToken();
			addToSymbolTable("INT", ans);
			fout2 << ans << " = " << t1 << t2 << endl;
			lineNumber++;
		}
		else
			ans = t1;
		noOfTabs--;
		return ans;
	}
	string D() {
		cout << endl;
		noOfTabs++;
		tabs("M");
		string t1 = R2();
		string t2 = Z();
		string ans = "";
		if (t2.size() > 0) {
			ans = getNewToken();
			addToSymbolTable("INT", ans);
			fout2 << ans << " = " << t1 << t2 << endl;
			lineNumber++;
		}
		else
			ans = t1;
		noOfTabs--;
		return ans;
	}
	string Z() {
		cout << endl;
		noOfTabs++;
		tabs("Z");
		string ans = "";
		if (checkToken("/")) {
			match("/");
			prevNoOfTabs = noOfTabs;
			string t1 = R2();
			prevNoOfTabs = noOfTabs;
			string t2 = Z();
			string newToken = getNewToken();
			addToSymbolTable("INT", newToken);
			fout2 << newToken << " = " << t1 << t2 << endl;
			lineNumber++;
			ans = " / " + newToken;
		}
		return ans;
	}
	string Y() {
		cout << endl;
		noOfTabs++;
		tabs("Y");
		string ans = "";
		if (checkToken("*")) {
			match("*");
			prevNoOfTabs = noOfTabs;
			string t1 = D();
			prevNoOfTabs = noOfTabs;
			string t2 = Y();
			string newToken = getNewToken();
			addToSymbolTable("INT", newToken);
			fout2 << newToken << " = " << t1 << t2 << endl;
			lineNumber++;
			ans = " * " + newToken;
		}
		return ans;
	}
	string X() {
		cout << endl;
		noOfTabs++;
		tabs("X");
		string ans = "";
		if (checkToken("+")) {
			match("+");
			prevNoOfTabs = noOfTabs;
			string t1 = M();
			prevNoOfTabs = noOfTabs;
			string t2 = X();
			string newToken = getNewToken();
			addToSymbolTable("INT", newToken);
			fout2 << newToken << " = " << t1 << t2 << endl;
			lineNumber++;
			ans = " + " + newToken;
		}
		else if (checkToken("-")) {
			match("-");
			prevNoOfTabs = noOfTabs;
			string t1 = M();
			prevNoOfTabs = noOfTabs;
			string t2 = X();
			string newToken = getNewToken();
			addToSymbolTable("INT", newToken);
			fout2 << newToken << " = " << t1 << t2 << endl;
			lineNumber++;
			ans = " - " + newToken;
		}
		return ans;
	}
	string B() {
		cout << endl;
		noOfTabs++;
		tabs("B");
		string ans;
		if (checkToken("ID") && checkPeek("=")) {
			string myName = currLexeme;
			match("ID");
			match("=");
			prevNoOfTabs = noOfTabs;
			string val = B();
			fout2 << myName << " = " << val << endl;
			lineNumber++;
			ans = myName;
		}
		else {
			prevNoOfTabs = noOfTabs;
			ans = E();
			match(";");
		}
		noOfTabs--;
		return ans;
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
		cout << "Assignment" << endl;
		if (checkToken("ID")) {
			string myName = currLexeme;
			match("ID");
			match("=");
			prevNoOfTabs = noOfTabs;
			string val = B();
			fout2 << myName << " = " << val << endl;
			lineNumber++;
		}
		else {
			abort("Bad token: " + currToken);
		}
		cout << endl;
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
	pair<int, int> conditionalStatement() {
		pair <int, int> ret;
		cout << endl;
		noOfTabs++;
		tabs("conditionalStatement");
		if (checkToken("LIT") || checkToken("NUM") || checkToken("ID")) {
			prevNoOfTabs = noOfTabs;
			ret.first = lineNumber + 1;
			fout2 << "if ";
			R();
			fout2 << " " + currLexeme + " ";
			match("RO");
			prevNoOfTabs = noOfTabs;
			R();
			fout2 << " goto " << lineNumber + 3 << endl;
			lineNumber++;
			fout2 << "goto " << endl;
			lineNumber++;
			ret.second = lineNumber;
		}
		else {
			abort("Bad token: " + currToken);
		}
		noOfTabs--;
		return ret;
	}
	void whileLoop() {
		cout << "WhileLoop" << endl;
		if (checkToken("WHILE")) {
			match("WHILE");
			pair<int, int> temp = conditionalStatement();
			match(":");
			match("{");
			prevNoOfTabs = noOfTabs;
			statements();
			match("}");
			fout2 << "goto " << temp.first << endl;
			lineNumber++;
			backPatch(temp.second, lineNumber + 1);
		}
		else {
			abort("Bad token: " + currToken);
		}
		cout << endl;

	}
	void backPatch(int a, int n) {
		fout2.close();
		string tempData;
		ifstream fin;
		fin.open(tacFileName);

		int i = 0;
		while (!fin.eof()) {
			string temp;
			getline(fin, temp);
			if (a == i + 1) {
				char buffer[10];
				_itoa_s(n, buffer, 10);
				string b = buffer;
				temp += b;
			}
			string b = "\n";
			temp += b;
			tempData += temp;
			i++;
		}
		fin.close();

		fout2.open(tacFileName);
		string temp = "\n";
		tempData.pop_back();
		fout2 << tempData;
	}
	void A() {
		cout << endl;
		noOfTabs++;
		tabs("A");
		if (checkToken("ELIF")) {
			match("ELIF");
			prevNoOfTabs = noOfTabs;
			pair<int, int> temp = conditionalStatement();
			match(":");
			match("{");
			prevNoOfTabs = noOfTabs;
			statements();
			match("}");
			int myLine = lineNumber + 1;
			fout2 << "goto " << endl;
			lineNumber++;
			backPatch(temp.second, lineNumber + 1);
			prevNoOfTabs = noOfTabs;
			A();
			backPatch(myLine, lineNumber + 1);
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
		cout << "If" << endl;
		if (checkToken("IF")) {
			match("IF");
			prevNoOfTabs = noOfTabs;
			pair<int, int> temp = conditionalStatement();
			match(":");
			match("{");
			prevNoOfTabs = noOfTabs;
			statements();
			match("}");
			int myLine = lineNumber + 1;
			fout2 << "goto " << endl;
			lineNumber++;
			backPatch(temp.second, lineNumber + 1);
			prevNoOfTabs = noOfTabs;
			A();
			backPatch(myLine, lineNumber + 1);
		}
		else {
			abort("Bad token: " + currToken);
		}
		cout << endl;
	}
	void s() {
		cout << endl;
		noOfTabs++;
		tabs("s");
		if (checkToken("STR")) {
			int size = currLexeme.size();
			string tokenName = getNewToken();
			addToSymbolTable("CHAR[]", tokenName, size);
			fout2 << tokenName;
			//Initialise this value in memory later
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
		cout << "Print" << endl;
		if (checkToken("PRINT")) {
			match("PRINT");
			prevNoOfTabs = noOfTabs;
			fout2 << "OUT ";
			p();
			fout2 << endl;
			lineNumber++;
		}
		else if (checkToken("PRINTLN")) {
			match("PRINTLN");
			prevNoOfTabs = noOfTabs;
			fout2 << "OUT ";
			p();
			fout2 << "\\n" << endl;
			lineNumber++;
		}
		else {
			abort("Bad token: " + currToken);
		}
		cout << endl;

	}
	void input() {
		cout << "Input" << endl;
		if (checkToken("INPUT")) {
			match("INPUT");
			match("->");
			fout2 << "IN " + currLexeme << endl;
			lineNumber++;
			match("ID");
			match(";");
		}
		else {
			abort("Bad token: " + currToken);
		}
		cout << endl;
	}
	void increment() {
		cout << "Increment" << endl;
		if (checkToken("ID")) {
			fout2 << currLexeme;
			match("ID");
			match("++");
			match(";");
			fout2 << " ++" << endl;
			lineNumber++;
		}
		else {
			abort("Bad token: " + currToken);
		}
		cout << endl;
	}
	void comment() {
		cout << "Comment" << endl;
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
		cout << endl;
	}
	void u() {
		//cout << "u" << endl;
		if (checkToken("INT") || checkToken("CHAR") || (checkToken("ID") && checkPeek("++")) || checkToken("ID")
			|| checkToken("WHILE") || checkToken("IF") || checkToken("PRINT") || checkToken("PRINTLN")
			|| checkToken("INPUT") || checkToken("COMMENT") || checkToken("/")) {
			prevNoOfTabs = noOfTabs;
			statements();
		}
		//cout << endl;
	}
	void statements() {
		cout << "Statements" << endl;
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
	}
	void tabs(string str) {
		for (int i = 0; i < prevNoOfTabs; i++)
			cout << "\t";
		cout << "|";

		for (int i = 0; i < (noOfTabs - prevNoOfTabs + 2); i++)
			cout << "__";
		cout << str;
	}
	void printSymbolTable() {
		cout << "symbolTable: " << opCodeTable["="] << "\n";

	/*	cout << "symbolTable!!\n";
		for (auto const& p : symbolTable) {
			cout << "{" << p.first << ": " << p.second.second << "}\n";
		}*/
	}
};

class MachineCodeGenerator {
public:
	Parser* parser;
	string TACFilename;
	string machineCodeFilename;
	ifstream fin;
	ofstream fout;
	int**mem;
	int totalLines;
	int lineNo;
	MachineCodeGenerator(Parser* _parser, string _TACFilename, string _machineCodeFilename) {
		parser = _parser;
		TACFilename = _TACFilename;
machineCodeFilename = _machineCodeFilename;
lineNo = 1;

//get no of lines in TAC
fin.open(TACFilename);
int lines = 0; string line;
while (getline(fin, line))
lines++;
totalLines = lines;
mem = new int* [lines + 1];
fin.close();

fin.open(TACFilename);
fout.open(machineCodeFilename);
	}
	void abort(string message = "") {
		cout << "Error: " + message << endl;
		exit(-1);
	}
	vector<string> split(const char* str, char c = ' ')
	{
		vector<string> result;
		do
		{
			const char* begin = str;

			while (*str != c && *str)
				str++;

			result.push_back(string(begin, str));
		} while (0 != *str++);

		return result;
	}
	char* getSymbolAdress(string symbol) {
		if (parser->symbolTable.find(symbol) == parser->symbolTable.end()) {
			abort("Variable: " + symbol + " is not declared!");
		}
		return (char*)parser->symbolTable[symbol].second.c_str();
	}
	void addToMemArrayAndFile(vector<int>data) {
		mem[lineNo] = new int[4];
		for (int i = 0; i < data.size(); i++) {
			mem[lineNo][i] = int(data[i]);
			fout << data[i];
			if (i != data.size() - 1) {
				fout << " ";
			}
			else {
				fout << endl;
			}
		}
		if (data.size() < 4) {
			for (int i = data.size(); i < 4; i++) {
				mem[lineNo][i] = int();
			}
		}
		lineNo++;
	}
	bool isAssignment(vector<string>tokens) {
		if (tokens.size() == 3 && tokens[1] == "=") {
			vector<int>data;
			data.push_back(parser->opCodeTable["="]);
			data.push_back(atoi(getSymbolAdress(tokens[2])));
			data.push_back(atoi(getSymbolAdress(tokens[0])));
			addToMemArrayAndFile(data);
		}
		return false;
	}
	bool hasEnding(std::string const& fullString, std::string const& ending) {
		if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		else {
			return false;
		}
	}
	bool isOutput(vector<string>tokens) {
		if (tokens.size() == 2 && tokens[0] == "OUT") {

			if (hasEnding(tokens[1], "n")) {
				vector<int>data;
				if (parser->symbolTable[tokens[1].substr(0, tokens[1].length() - 2)].first == "INT") {
					data.push_back(parser->opCodeTable["OUTI"]);
				}
				else if (parser->symbolTable[tokens[1].substr(0, tokens[1].length() - 2)].first == "CHAR") {
					data.push_back(parser->opCodeTable["OUTC"]);
				}
				else if (parser->symbolTable[tokens[1].substr(0, tokens[1].length() - 2)].first == "CHAR[]") {
					data.push_back(parser->opCodeTable["OUTS"]);
				}
				data.push_back(atoi(getSymbolAdress(tokens[1].substr(0, tokens[1].length() - 2))));
				data.push_back(1);
				addToMemArrayAndFile(data);
			}
			else {
				vector<int>data;
				if (parser->symbolTable[tokens[1].substr(0, tokens[1].length() - 2)].first == "INT") {
					data.push_back(parser->opCodeTable["OUTI"]);
				}
				else if (parser->symbolTable[tokens[1].substr(0, tokens[1].length() - 2)].first == "CHAR") {
					data.push_back(parser->opCodeTable["OUTC"]);
				}
				else if (parser->symbolTable[tokens[1].substr(0, tokens[1].length() - 2)].first == "CHAR[]") {
					data.push_back(parser->opCodeTable["OUTS"]);
				}
				data.push_back(atoi(getSymbolAdress(tokens[1])));
				data.push_back(0);
				addToMemArrayAndFile(data);
			}
		}
		return false;
	}
	bool isInput(vector<string>tokens) {
		if (tokens.size() == 2 && tokens[0] == "IN") {
			vector<int>data;
			if (parser->symbolTable[tokens[1]].first == "INT"){
				data.push_back(parser->opCodeTable["INI"]);
			}
			else if (parser->symbolTable[tokens[1]].first == "CHAR") {
				data.push_back(parser->opCodeTable["INC"]);
			}
			data.push_back(atoi(getSymbolAdress(tokens[1])));
			addToMemArrayAndFile(data);
		}
		return false;
	}
	bool isIf(vector<string>tokens) {
		if (tokens.size() == 6 && tokens[0] == "if") {
			vector<int>data;
			data.push_back(parser->opCodeTable[tokens[2]]);
			data.push_back(atoi(getSymbolAdress(tokens[1])));
			data.push_back(atoi(getSymbolAdress(tokens[3])));
			data.push_back(atoi((char*)tokens[5].c_str()));
			addToMemArrayAndFile(data);
		}
		return false;
	}
	bool isGoto(vector<string>tokens) {
		if (tokens.size() == 2 && tokens[0] == "goto") {
			vector<int>data;
			data.push_back(parser->opCodeTable["goto"]);
			data.push_back(atoi((char*)tokens[1].c_str()));
			addToMemArrayAndFile(data);
		}
		return false;
	}
	bool isIncrement(vector<string>tokens) {
		if (tokens.size() == 2 && tokens[1] == "++") {
			vector<int>data;
			data.push_back(parser->opCodeTable["++"]);
			data.push_back(atoi(getSymbolAdress(tokens[0])));
			addToMemArrayAndFile(data);
		}
		return false;
	}
	bool isArithmetic(vector<string>tokens) {
		if (tokens.size() == 5 && tokens[1] == "=") {
			vector<int>data;
			data.push_back(parser->opCodeTable[tokens[3]]);
			data.push_back(atoi(getSymbolAdress(tokens[2])));
			data.push_back(atoi(getSymbolAdress(tokens[4])));
			data.push_back(atoi(getSymbolAdress(tokens[0])));
			addToMemArrayAndFile(data);
		}
		return false;
	}
	void parse(char* TAC) {
		vector<string> tokens = split(TAC);
		if (!isAssignment(tokens))
			if (!isOutput(tokens))
				if (!isInput(tokens))
					if (!isIf(tokens))
						if (!isGoto(tokens))
							if (!isIncrement(tokens))
								if (!isArithmetic(tokens))
									return;
	}
	void TACToMachineCode(string TAC) {
		parse((char*)TAC.c_str());
	}
	void convertTACtoMachineCode() {
		string TAC;
		while (!fin.eof()) {
			getline(fin, TAC);
			TACToMachineCode(TAC);
		}
		fin.close();
		fout.close();
	}
};
class VM {
public:
	MachineCodeGenerator* machineCodeGnerator;
	unsigned char* bytes;
	VM(MachineCodeGenerator* _machineCodeGnerator) {
		machineCodeGnerator = _machineCodeGnerator;
		bytes = new unsigned char[machineCodeGnerator->parser->currentAddress+4];
	}
	char readChar(int address) {
		char ans = (char)bytes[address];
		return ans;
	}
	void saveChar(int address, char value) {
		bytes[address] = value;
	}
	//size: 4 bytes
	int readInt(int address) {
		int ans = 0;
		for (int i = address; i < address + 4; i++) {
			ans = ans << 8;
			ans += bytes[i];
		}
		return ans;
	}
	void saveInt(int address, int value) {
		int mask = 255;
		int ans = value;
		for (int i = address + 3; i >= address; i--) {
			int temp = ans & mask;
			bytes[i] = (unsigned char)temp;
			ans = ans >> 8;
		}
	}
	//size: varies
	char* readArr(int address) {
		string temp = "";
		for (int i = 0; bytes[i] != '\n'; i++)
			temp += bytes[i];

		char* ptr = new char[temp.size() + 1];
		strcpy(ptr, temp.c_str());

		return ptr;
	}

	void run() {
		int** mem = machineCodeGnerator->mem;
		int lineNo = 1;
		bool flag = true;
		while (flag) {
			switch (mem[lineNo][0])
			{
			case 0://==
				if (readInt(mem[lineNo][1]) == readInt(mem[lineNo][2])) {
					lineNo = mem[lineNo][3];
				}
				break;
			case 1://<=
				if (readInt(mem[lineNo][1]) <= readInt(mem[lineNo][2])) {
					lineNo = mem[lineNo][3];
				}
				break;
			case 2://>=
				if (readInt(mem[lineNo][1]) >= readInt(mem[lineNo][2])) {
					lineNo = mem[lineNo][3];
				}
				break;
			case 3://!=
				if (readInt(mem[lineNo][1]) != readInt(mem[lineNo][2])) {
					lineNo = mem[lineNo][3];
				}
				break;
			case 4://>
				if (readInt(mem[lineNo][1]) > readInt(mem[lineNo][2])) {
					lineNo = mem[lineNo][3];
				}
				break;
			case 5://<
				if (readInt(mem[lineNo][1]) < readInt(mem[lineNo][2])) {
					lineNo = mem[lineNo][3];
				}
				break;
			case 6://+
				saveInt(mem[lineNo][3],
					readInt(mem[lineNo][1]) + readInt(mem[lineNo][2]));
				break;
			case 7://-
				saveInt(mem[lineNo][3],
					readInt(mem[lineNo][1]) - readInt(mem[lineNo][2]));
				break;
			case 8://'/
				saveInt(mem[lineNo][3],
					readInt(mem[lineNo][1]) / readInt(mem[lineNo][2]));
				break;
			case 9://*
				saveInt(mem[lineNo][3],
					readInt(mem[lineNo][1]) * readInt(mem[lineNo][2]));
				break;
			case 10://goto
				lineNo = mem[lineNo][1];
				break;
			case 11://INI
				int inpi;
				cin >> inpi;
				saveInt(mem[lineNo][1], inpi);
				break;
			case 12://INC
				char inpc;
				cin >> inpc;
				saveChar(mem[lineNo][1], inpc);
				break;
			case 13://OUTI
				cout<<readInt(mem[lineNo][1]);
				if (readInt(mem[lineNo][2])) {
					cout << endl;
				}
				break;
			case 14://OUTC
				cout<<readChar(mem[lineNo][1]);
				if (readInt(mem[lineNo][2])) {
					cout << endl;
				}
				break;
			case 15://OUTS
				cout << readArr(mem[lineNo][1]);
				if (readInt(mem[lineNo][2])) {
					cout << endl;
				}
				break;
			case 16://=
				saveInt(mem[lineNo][2],
					readInt(mem[lineNo][1]));
				break;
			default:
				flag = false;
				break;
			}
		}
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
		Parser parser(&lexer, "symbolTable.txt", "TAC.txt");
		parser.initialize();
		parser.program();
		//parser.printSymbolT able();

		MachineCodeGenerator machineCodeGnerator(&parser, "TAC.txt", "MCG.txt");
		machineCodeGnerator.convertTACtoMachineCode();

		VM  vm(&machineCodeGnerator);
		vm.run();
	}
}








/*cout << "CODETRSINGS!!\n";
list<pair<int, string>>::iterator it;
for (it = codeString.begin(); it != codeString.end(); it++)
{
cout << "('" <<it->first << "', '" << it->second << "')" << endl;
}*/