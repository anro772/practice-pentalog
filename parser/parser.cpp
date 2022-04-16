#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <numeric>
#include <math.h>
#include <exception>

using namespace std;

class InputArgsParser {
public:
	char** argv;
	int argc;
	bool isCsv = 0;
	bool canRead = 1;

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
		for (int i = 0; i < strlen(argv[2]); i++)
		{
			//check if after . there is csv
			if (argv[2][i] == '.')
			{
				if (argv[2][i + 1] == 'c' && argv[2][i + 2] == 's' && argv[2][i + 3] == 'v')
				{
					isCsv = 1;
					break;
				}
			}
		}
		cout << "Parsing the file " << argv[2] << endl;

	}
	~InputArgsParser()
	{
		delete[] argv;
	}
};

class Parser : public InputArgsParser {
public:
	vector<string> tokens;
	vector<string> output;
	int numOfTokens = 0;

	Parser()
	{

	}

	Parser(vector<string> Tokens)
	{
		tokens = Tokens;
	}

	void separate(string delimiter)		//function that separates tokens based on input delimiter
	{
		vector<string> newTokens = tokens;
		tokens.clear();
		size_t pos = 0;
		string token;
		cout << endl;
		while ((pos = newTokens[0].find(delimiter)) != std::string::npos)
		{
			token = newTokens[0].substr(0, pos);
			tokens.push_back(token);
			newTokens[0].erase(0, pos + delimiter.length());
			numOfTokens++;
		}

		numOfTokens++;
		string h = newTokens[0];
		tokens.push_back(h);
	}

	void checkType(InputArgsParser& inputParser)
	{
		if (inputParser.isCsv == 1)
			Csv(inputParser);
		else Txt(inputParser);
	}
	
	void Csv(InputArgsParser& inputParser)	//function that parses tokens on a .CSV file
	{
		numOfTokens = 0;
		if (inputParser.isCsv == 1)
		{
			separate(",");	//use the separate function to separate tokens on a .csv file (they are written with commas)
			cout << "Reading " << numOfTokens << " different operands/operators.";
		}
	}

	void Txt(InputArgsParser& inputParser)	//function that parses tokens on a .TXT file
	{
		if (inputParser.isCsv == 0)		//checks again if the file is not a CSV
		{
			int isBad = 0;		//variable that gets back to the first element after rewriting the string
			vector<string>newTokens;
			vector<string>newNewTokens;
			newNewTokens.clear();
			for (int j = 0; j < numOfTokens; j++)
			{		
				newTokens.clear();
				for (int i = 0; i < tokens[j].length(); i++)
				{
					if (isBad == 1)
					{
						i = 0;
						isBad = 0;
					}
					if (tokens[j][i] == '*' || tokens[j][i] == '/' || tokens[j][i] == '+' || tokens[j][i] == '-' 
						|| tokens[j][i] == '(' || tokens[j][i] == ')' || tokens[j][i] == '^' || tokens[j][i] == 'h')
					{
						string h = tokens[j].substr(0, i);
						if(!h.empty())
						{
							if (h.front() == '*' || h.front() == '/' || h.front() == '+' || h.front() == '-' || h.front() == '^' || h.front() == '(')
							{
								string first = h.substr(0, 1);
								newTokens.push_back(first);
								h.erase(0, 1);
							}
						}
						newTokens.push_back(h);
						newTokens.push_back(tokens[j].substr(i, 1));	//pushes the elements in a new vector
						tokens[j].erase(0, i+1);						//erases the elements in the original vector
						i = 0;
						isBad = 1;
					}
				}
				newTokens.push_back(tokens[j]);
				for (int x = 0; x < newTokens.size(); x++)
					if (!newTokens[x].empty())
						newNewTokens.push_back(newTokens[x]);
			}
			tokens.clear();		//after the entire string is parsed, the original vector is cleared
			tokens = newNewTokens;	//and after the new vector is filled, it is copied to the original vector
			numOfTokens = tokens.size();	//the number of tokens is updated
		}
		if (inputParser.isCsv == 0)
		{
			ifstream file(inputParser.argv[2]);
		}
		cout << "Reading " << numOfTokens << " different operands/operators.";
	}

	void VectorToString(string& s)	//function to convert a vector to a string
	{
		s = std::accumulate(tokens.begin(), tokens.end(), std::string{});
	}

	int prec(vector<string> tokens, int i)		//function to check the precedence of the operators
	{	
		if (tokens[i] == "^")
			return 3;
		else if (tokens[i] == "/" || tokens[i] == "*")
			return 2;
		else if (tokens[i] == "+" || tokens[i] == "-")
			return 1;
		else if (tokens[i] == "sin" || tokens[i] == "cos" || tokens[i] == "sqrt")
			return 4;
		else
			return -1;
	}

	int getTopValue(stack<string> st)	//function that gets the precedence value of the top element of the stack
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

	bool is_number(const std::string& s)	//function that checks if a string is a number
	{
		return !s.empty() && std::find_if(s.begin(),
			s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
	}

	void infixToPostfix(vector<string> tokens)	//function that transforms an infix operation to postfix
	{
		stack<string> exp;	//exp contains the operators
		vector<string> nos; //nos contains the numbers
		string result;

		for (int i = 0; i < tokens.size(); i++)
		{
			if (tokens[i] == "[")	//transforms [ ] to ( )
				tokens[i] = "(";
			if (tokens[i] == "]")
				tokens[i] = ")";

			// If the scanned character is
			// an operand, add it to output string.
			if (is_number(tokens[i]) == true)
			{
				nos.push_back(tokens[i]);
			}

			// If the scanned character is an
			// "(", push it to the stack.
			else if (tokens[i] == "(")
				exp.push("(");

			// If the scanned character is an ")",
			// pop and to output string from the stack
			// until an "(" is encountered.
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
				while (!exp.empty() && prec(tokens, i) <= getTopValue(exp) && exp.top() != "(") {
					nos.push_back(exp.top());
					exp.pop();
				}
				if (tokens[i] != "sin" && tokens[i] != "cos" && tokens[i] != "sqrt")
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
	
	float evaluate(vector<string> postfix) //function that evaluates postfix expression
	{
		stack<float> stack;
		float x;
		string h;
		for (int i = 0; i < postfix.size(); i++) {
			if (postfix[i] == "+" || postfix[i] == "-" || postfix[i] == "*" || postfix[i] == "/" || postfix[i] == "^"
				|| postfix[i] == "sin" || postfix[i] == "cos" || postfix[i] == "sqrt")
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

	int convertToInt(string s)	//function to convert from string to int without modifying the string value
	{
		int x = 0;
		for (int i = 0; i < s.length(); i++) {
			x = x * 10 + (s[i] - '0');
		}
		return x;
	}

	void calculate()	//function that prints on the screen information about the expression
	{				// and the result of the expression
		cout << endl << "The expression is: ";
		getTokens();
		infixToPostfix(tokens);
		cout << endl;
		cout << "The postfix output is: ";
		getOutput();
		cout << endl;
		cout << "The result is: ";
		cout << evaluate(output);
		cout << endl;
	}

	void readFromFile(char* filename)	//function to read from file
	{
		ifstream inputFile;
		inputFile.open(filename);
		if (inputFile.is_open())
		{
			string s;
			while (inputFile >> s)
			{
				tokens.push_back(s);
				if (s.find(",") != string::npos)
				{
					isCsv = 1;
				}
				numOfTokens++;
			}
			inputFile.close();
		}
		else
		{
			cout << "Unable to open file, check the parameters of the program or the file name";
			canRead = 0;
		}
	}

	//>> operator
	friend istream& operator>>(istream& in, Parser& p);

	void getTokens()	//getter for the tokens
	{
		for (auto i : tokens)
		{
			cout << i << " ";
		}
	}
	void getOutput()	//getter for the output
	{
		for (auto i : output)
		{
			cout << i << " ";
		}
	}
};

istream& operator>>(istream& in, Parser& p)		//>> operator for Parser
{
	string s;
	in >> s;
	p.tokens.push_back(s);
	p.numOfTokens++;
	return in;
}

int main(int argc, char** argv)
{
	string s;
	InputArgsParser inputParser(argc, argv);	//input arguments parser

	Parser parser = Parser();	//create a parser object

	parser.readFromFile(inputParser.argv[2]);	//read from file
	if (parser.canRead == 1)
	{	
		parser.checkType(inputParser);	//checks the type of the file and calls the appropriate function
		parser.calculate();		//calculates the expression
	}

}