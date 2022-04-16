#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <numeric>
#include <math.h>

using namespace std;

class Parser {
public:
	vector<string> tokens;
	vector<string> output;
	
	Parser()
	{
		
	}
	
	Parser(vector<string> Tokens)
	{
		tokens = Tokens;
	}
	
	void VectorToString(string &s)
	{
		s = std::accumulate(tokens.begin(), tokens.end(), std::string{});
	}
	
	int prec(vector<string> tokens,int i) {
		if (tokens[i] == "^")
			return 3;
		else if (tokens[i] == "/" || tokens[i] == "*")
			return 2;
		else if (tokens[i] == "+" || tokens[i] == "-")
			return 1;
		else if (tokens[i] == "sin" || tokens[i] == "cos"||tokens[i]=="sqrt")
			return 4;
		else
			return -1;
	}
	
	int getTopValue(stack<string> st)
	{
		if (st.top() == "^")
			return 3;
		else if (st.top() == "/" || st.top() == "*")
			return 2;
		else if (st.top() == "+" || st.top() == "-")
			return 1;
		else if (st.top() == "sin" || st.top() == "cos" || st.top() == "sqrt")
			return 4;
	}
	
	bool is_number(const std::string& s)
	{
		return !s.empty() && std::find_if(s.begin(),
			s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
	}
	
	void infixToPostfix(vector<string> tokens) 
	{
		stack<string> exp;
		vector<string> nos;
		string result;

		for (int i = 0; i < tokens.size(); i++) 
		{	
			if(tokens[i]=="[")
			tokens[i] = "(";
			if (tokens[i] == "]")
				tokens[i] = ")";
			
			// If the scanned character is
			// an operand, add it to output string.
			if (is_number(tokens[i])==true)
			{
				nos.push_back(tokens[i]);
			}

			// If the scanned character is an
			// ‘(‘, push it to the stack.
			else if (tokens[i] == "(")
				exp.push("(");

			// If the scanned character is an ‘)’,
			// pop and to output string from the stack
			// until an ‘(‘ is encountered.
			else if (tokens[i] == ")") 
			{
				while (exp.top() != "(" && !exp.empty())
				{
					nos.push_back(exp.top());
					exp.pop();
				}
				exp.pop();
			}
			
			//If an operator is scanned that has same or higher precedence than the operator 
			//at the top of the stack, push it to the stack.
			else {
				while (!exp.empty() && prec(tokens,i) <= getTopValue(exp) && exp.top() != "("){
					nos.push_back(exp.top());
					exp.pop();
				}
			if(tokens[i]!="sin"&&tokens[i]!="cos"&&tokens[i]!="sqrt")
				exp.push(tokens[i]);
			}
			if (tokens[i] == "sin")
			{
				exp.push("sin");
			}
			if (tokens[i] == "cos")
			{
				exp.push("cos");
			}
			if (tokens[i] == "sqrt")
			{
				exp.push("sqrt");
			}
		}
		
		// Pop all the remaining elements from the stack
		while (!exp.empty()) {
			nos.push_back(exp.top());
			exp.pop();
		}
		this->output = nos;
	}
	
	//function that evaluates postfix expression
	float evaluate(vector<string> postfix) {
		stack<float> stack;
		float x;
		string h;
		for (int i = 0; i < postfix.size(); i++) {
			if (postfix[i] == "+" || postfix[i] == "-" || postfix[i] == "*" || postfix[i] == "/" || postfix[i]== "^"
				|| postfix[i]=="sin"||postfix[i]=="cos" || postfix[i]=="sqrt")
			{	
				float a = stack.top();
				stack.pop();
				if (postfix[i] == "sin") {
					stack.push(sin(a));
				}
				else if (postfix[i] == "cos") {
					stack.push(cos(a));
				}
				else if (postfix[i] == "sqrt") {
					stack.push(sqrt(a));
				}
				else
				{
				float b = stack.top();
				stack.pop();
				if (postfix[i] == "+") {
					stack.push(a + b);
				}
				else if (postfix[i] == "-") {
					stack.push(b - a);
				}
				else if (postfix[i] == "*") {
					stack.push(a * b);
				}
				else if (postfix[i] == "/") {
					stack.push(b / a);
				}
				else if (postfix[i] == "^") {
					stack.push(pow(b, a));
				}
				}
			}
			else {
				h = postfix[i];
				x = stof(h);
				stack.push(x);
			}
		}
		return stack.top();
	}
	
	//function to convert from string to int without modifying the string value
	int convertToInt(string s) 
	{
		int x = 0;
		for (int i = 0; i < s.length(); i++) {
			x = x * 10 + (s[i] - '0');
		}
		return x;
	}

	//>> operator
	friend istream& operator>>(istream& in, Parser& p);
	
	void getTokens()
	{
		for(auto i: tokens)
		{
			cout << i << " ";
		}
	}
	void getOutput()
	{
		for(auto i: output)
		{
			cout << i << " ";
		}
	}
};

class InputArgsParser : Parser {
public:
	char** argv;
	int argc;

	InputArgsParser()
	{

	}

	InputArgsParser(int Argc, char** Argv)
	{
		argc = Argc;
		argv = new char* [argc];
		for (int i = 0; i < argc; i++)
		{
			argv[i] = new char[strlen(Argv[i])];
			for (int j = 0; j < strlen(Argv[i]); j++)
				argv[i] = Argv[i];
		}
		cout << "Parsing the file " << argv[2] << endl;

	}
	~InputArgsParser()
	{
		delete[] argv;
	}
};

//>> operator for Parser
istream& operator>>(istream& in, Parser& p)
{	 
	string s;
	in >> s;
	p.tokens.push_back(s);
	return in;
}

int main(int argc, char** argv)
{
	string s;
	InputArgsParser inputParser(argc, argv);
	ifstream inputFile(inputParser.argv[2]);
	if (inputFile.is_open())
	{
		vector<string> infix;
		Parser parser=Parser();
		int k = 0;
		while (inputFile >> parser) 
		{
			k++;
		}
		cout << "Reading " << k << " different operands/numbers: ";
		parser.getTokens();
		cout << endl;
	
		//remove last element of parser.tokens because of empty space
		parser.tokens.pop_back();
		parser.infixToPostfix(parser.tokens);
		cout << endl;
		cout << "The postfix output is: ";
		parser.getOutput();
		cout << endl;
		cout << "The result is: ";
		cout <<parser.evaluate(parser.output);
		cout << endl;
		inputFile.close();
	}
	else
	{
		cout << "Unable to open file";
	}
}

