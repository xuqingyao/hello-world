#ifndef TOKENSCANNER_h
#define TOKENSCANNER_h

#include <string>
#include <iostream>

class tokenScanner {
public:
	struct stringBlock {
		std::string str;
		stringBlock *next;
	};
private:
	stringBlock * head;
	//std::istream in;
	std::string buffer;
	int currentPosition = 0;

private:
	void skipWhitespace() {
		while (buffer[currentPosition] == ' ')
			currentPosition++;
	}
	void skipComment() {
		currentPosition = buffer.length();
	}
	void skipMatch() {
		currentPosition++;
	}

public:
	tokenScanner() {
		setInput("");
	}
	tokenScanner(std::string str) {
		setInput(str);
	}
	/*tokenScanner(std::istream infile) {
	setInput(infile);
	}*/
	~tokenScanner() {
		//delete in;
	}
	void setInput(std::string str) {
		buffer = str;
	}
	/*void setInput(std::istream & infile) {
	in = infile;
	}*/
	bool hasMoreToken() {
		if (buffer[currentPosition] == ' ')
			skipWhitespace();
		if (buffer[currentPosition] == '#')
			skipComment();
		if (buffer[currentPosition] == '(' || buffer[currentPosition] == ')')
			skipMatch();
		return currentPosition < buffer.length();
	}
	std::string nextToken() {
		if (buffer[currentPosition] == ' ')
			skipWhitespace();
		if (buffer[currentPosition] == '#')
			skipComment();
		if (buffer[currentPosition] == '(' || buffer[currentPosition] == ')')
			skipMatch();
		if (currentPosition >= buffer.length())
			return "";
		else if (buffer[currentPosition] == '"') {
			currentPosition++;
			std::string str = "";
			int start = currentPosition, end;
			while (buffer[currentPosition] != '"') {
				if (buffer[currentPosition] == '\\') {
					if (buffer[currentPosition + 1] == 'n')
						str += '\n';
					else
						str += '\"';
					currentPosition++;
				}
				else
					str += buffer[currentPosition];
				currentPosition++;
			}
			currentPosition++;
			return str;
		}

		else if (buffer[currentPosition] != ' ' && buffer[currentPosition] != '(' && buffer[currentPosition] != ')' && buffer[currentPosition] != '#' && buffer[currentPosition] != '"') {
			int start = currentPosition, end;
			if (buffer[currentPosition] == '\t')
				start++;
			while (currentPosition < buffer.length() && (buffer[currentPosition] != ' ' && buffer[currentPosition] != '('  && buffer[currentPosition] != ')' && buffer[currentPosition] != '#' && buffer[currentPosition] != '"'))
				currentPosition++;
			if (buffer[currentPosition - 1] == ',' || buffer[currentPosition - 1] == ')' || buffer[currentPosition - 1] == ':')
				end = currentPosition - start - 1;
			else
				end = currentPosition - start;
			return buffer.substr(start, end);
		}
		else
			return std::string(1, buffer[currentPosition++]);
	}

};


#endif
