#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <vector>
#include "tokenscanner.h"
#include "memory.h"

std::map<std::string, int> keyword = {
	std::make_pair("$0", 0), std::make_pair("$1", 1), std::make_pair("$2", 2), std::make_pair("$3", 3),
	std::make_pair("$4", 4), std::make_pair("$5", 5), std::make_pair("$6", 6), std::make_pair("$7", 7),
	std::make_pair("$8", 8), std::make_pair("$9", 9), std::make_pair("$10", 10), std::make_pair("$11", 11),
	std::make_pair("$12", 12), std::make_pair("$13", 13), std::make_pair("$14", 14), std::make_pair("$15", 15),
	std::make_pair("$16", 16), std::make_pair("$17", 17), std::make_pair("$18", 18), std::make_pair("$19", 19),
	std::make_pair("$20", 20), std::make_pair("$21", 21), std::make_pair("$22", 22), std::make_pair("$23", 23),
	std::make_pair("$24", 24), std::make_pair("$25", 25), std::make_pair("$26", 26), std::make_pair("$27", 27),
	std::make_pair("$28", 28), std::make_pair("$29", 29),std::make_pair("$30", 30), std::make_pair("$31", 31),
	std::make_pair("$zero", 0), std::make_pair("$at", 1), std::make_pair("$v0", 2), std::make_pair("$v1", 3),
	std::make_pair("$a0", 4), std::make_pair("$a1", 5), std::make_pair("$a2", 6), std::make_pair("$a3", 7),
	std::make_pair("$t0", 8), std::make_pair("$t1", 9), std::make_pair("$t2", 10), std::make_pair("$t3", 11),
	std::make_pair("$t4", 12), std::make_pair("$t5", 13), std::make_pair("$t6", 14), std::make_pair("$t7", 15),
	std::make_pair("$s0", 16), std::make_pair("$s1", 17), std::make_pair("$s2", 18), std::make_pair("$s3", 19),
	std::make_pair("$s4", 20), std::make_pair("$s5", 21), std::make_pair("$s6", 22), std::make_pair("$s7", 23),
	std::make_pair("$t8", 24), std::make_pair("$t9", 25), std::make_pair("$k0", 26), std::make_pair("$k1", 27),
	std::make_pair("$gp", 28), std::make_pair("$sp", 29),std::make_pair("$fp", 30), std::make_pair("$ra", 31),
	std::make_pair("$hi", 32), std::make_pair("$lo", 33),std::make_pair("$pc", 34)
};

Memory memo;
std::map<std::string, int> memory;
std::vector<std::string> text;
std::map<std::string, int> textnum;
std::vector<std::string> data;
int Register[35] = { 0 };
bool textStore = false, dataStore = false, mainbegin = false;
int currentLine = 0, datapos = 0, dataLine = 0, textLine = 0;

void processData();
void processText();

int main(int argc, char* argv[]) {
	Register[29] = 4 * 1024 * 1024;
	std::ifstream infile(argv[1]);
	//infile.open("10.s");
	std::string str, line;
	if (infile) {
		while (getline(infile, line)) {
			tokenScanner scanner(line);
			str = scanner.nextToken();
			if (str == ".data") {
				dataStore = true;
				textStore = false;
				continue;
			}
			else if (str == ".text") {
				textStore = true;
				dataStore = false;
				continue;
			}
			if (dataStore) {
				data.push_back(line);
				dataLine++;
			}
			else if (textStore) {
				text.push_back(line);
				if (str[0] == '_')
					textnum.insert(std::map<std::string, int> ::value_type(str + ':', textLine));
				textLine++;
			}
		}
	}
	infile.close();
	//freopen("10.in", "r", stdin);
	//std::cout << dataLine << ' ' << textLine << std::endl;
	for (; currentLine < dataLine;) {
		processData();
		//std::cout << currentLine << std::endl; 
	}
	for (currentLine = 0; currentLine < textLine; ) {
		processText();
		//std::cout << currentLine << std::endl;
	}

	system("pause");
	return 0;
}

int transToInt(std::string str) {
	int i = 0, ans;
	if (str[0] == '-')
		i++;
	ans = str[i] - '0';
	while (str[i + 1] != '\0') {
		i++;
		ans = ans * 10 + str[i] - '0';
	}
	if (str[0] == '-')
		return -ans;
	return ans;
}

char transToChar(std::string str) {
	return str[0];
}

short transToShort(std::string str) {
	short i = 0, ans;
	if (str[0] == '-')
		i++;
	ans = str[i] - '0';
	while (str[i + 1] != '\0') {
		i++;
		ans = ans * 10 + str[i] - '0';
	}
	if (str[0] == '-')
		return -ans;
	return ans;
}

void processData() {
	//std::cout << data[currentLine] << std::endl;
	tokenScanner scanner(data[currentLine]);
	std::string line = scanner.nextToken();
	std::string tmp1;
	int i1;
	short s1;
	char c1;
	if (line[0] == '.') {
		if (line == ".align") {
			tmp1 = scanner.nextToken();
			i1 = transToInt(tmp1);
			memo.align(i1);
			datapos = memo.getPos();
		}
		else if (line == ".ascii") {
			tmp1 = scanner.nextToken();
			memo.ascii(tmp1);
			datapos = memo.getPos();
		}
		else if (line == ".asciiz") {
			tmp1 = scanner.nextToken();
			memo.asciiz(tmp1);
			datapos = memo.getPos();
		}
		else if (line == ".byte") {
			while (scanner.hasMoreToken()) {
				tmp1 = scanner.nextToken();
				c1 = transToChar(tmp1);
				memo.byte(c1);
			}
			datapos = memo.getPos();
		}
		else if (line == ".half") {
			while (scanner.hasMoreToken()) {
				tmp1 = scanner.nextToken();
				s1 = transToShort(tmp1);
				memo.half(s1);
			}
			datapos = memo.getPos();
		}
		else if (line == ".word") {
			while (scanner.hasMoreToken()) {
				tmp1 = scanner.nextToken();
				i1 = transToInt(tmp1);
				memo.word(i1);
			}
			datapos = memo.getPos();
		}
		else if (line == ".space") {
			tmp1 = scanner.nextToken();
			i1 = transToInt(tmp1);
			memo.space(i1);
			datapos = memo.getPos();
		}
		//std::cout << line << ' ';
		currentLine++;
	}
	else {
		currentLine++;
		//processData();
		memory.insert(std::map<std::string, int> ::value_type(line, datapos));
		//std::cout << line << ' ';
		//currentLine--;
	}
}
void processText() {
	//std::cout << text[currentLine] << std::endl;
	tokenScanner scanner(text[currentLine]);
	std::string line = scanner.nextToken();
	std::string tmp1, tmp2, tmp3;
	int i1, i2;
	char c1;
	short s1;
	if (line == "main") {
		mainbegin = true;
		//std::cout << line << ' ';
		currentLine++;
	}
	else if (line[0] == 'a' || line[0] == 's' || line[0] == 'm' || line[0] == 'd' || line[0] == 'x' || line[0] == 'n' || line[0] == 'r') {
		if (mainbegin) {
			if (line == "add") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] + i1;
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] + Register[keyword[tmp3]];
			}
			else if (line == "addu") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] + abs(i1);
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] + abs(Register[keyword[tmp3]]);
			}
			else if (line == "addiu") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				i1 = transToInt(tmp3);
				Register[keyword[tmp1]] = Register[keyword[tmp2]] + abs(i1);
			}
			else if (line == "sub") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] - i1;
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] - Register[keyword[tmp3]];
			}
			else if (line == "subu") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] - abs(i1);
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] - abs(Register[keyword[tmp3]]);
			}
			else if (line == "mul") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					tmp3 = scanner.nextToken();
					if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
						i1 = transToInt(tmp3);
						Register[keyword[tmp1]] = Register[keyword[tmp2]] * i1;
					}
					else
						Register[keyword[tmp1]] = Register[keyword[tmp2]] * Register[keyword[tmp3]];
				}
				else {
					if ((tmp2[0] == '-' && (tmp2[1] >= '0' && tmp2[1] <= '9')) || (tmp2[0] >= '0' && tmp2[0] <= '9')) {
						i1 = transToInt(tmp2);
						Register[keyword["$hi"]] = (Register[keyword[tmp1]] * i1) / 2 ^ 32;
						Register[keyword["$lo"]] = (Register[keyword[tmp1]] * i1) % 2 ^ 32;
					}
					else {
						Register[keyword["$hi"]] = (Register[keyword[tmp1]] * Register[keyword[tmp2]]) / 2 ^ 32;
						Register[keyword["$lo"]] = (Register[keyword[tmp1]] * Register[keyword[tmp2]]) % 2 ^ 32;
					}
				}
			}
			else if (line == "mulu") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					tmp3 = scanner.nextToken();
					if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
						i1 = transToInt(tmp3);
						Register[keyword[tmp1]] = Register[keyword[tmp2]] * abs(i1);
					}
					else
						Register[keyword[tmp1]] = Register[keyword[tmp2]] * abs(Register[keyword[tmp3]]);
				}
				else {
					if ((tmp2[0] == '-' && (tmp2[1] >= '0' && tmp2[1] <= '9')) || (tmp2[0] >= '0' && tmp2[0] <= '9')) {
						i1 = transToInt(tmp2);
						Register[keyword["$hi"]] = (Register[keyword[tmp1]] * abs(i1)) / 2 ^ 32;
						Register[keyword["$lo"]] = (Register[keyword[tmp1]] * abs(i1)) % 2 ^ 32;
					}
					else {
						Register[keyword["$hi"]] = (Register[keyword[tmp1]] * abs(Register[keyword[tmp2]])) / 2 ^ 32;
						Register[keyword["$lo"]] = (Register[keyword[tmp1]] * abs(Register[keyword[tmp2]])) % 2 ^ 32;
					}
				}
			}
			else if (line == "div") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					tmp3 = scanner.nextToken();
					if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
						i1 = transToInt(tmp3);
						Register[keyword[tmp1]] = Register[keyword[tmp2]] / i1;
					}
					else
						Register[keyword[tmp1]] = Register[keyword[tmp2]] / Register[keyword[tmp3]];
				}
				else {
					if ((tmp2[0] == '-' && (tmp2[1] >= '0' && tmp2[1] <= '9')) || (tmp2[0] >= '0' && tmp2[0] <= '9')) {
						i1 = transToInt(tmp2);
						Register[keyword["$hi"]] = Register[keyword[tmp1]] % i1;
						Register[keyword["$lo"]] = Register[keyword[tmp1]] / i1;
					}
					else {
						Register[keyword["$hi"]] = Register[keyword[tmp1]] % Register[keyword[tmp2]];
						Register[keyword["$lo"]] = Register[keyword[tmp1]] / Register[keyword[tmp2]];
					}
				}
			}
			else if (line == "divu") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					tmp3 = scanner.nextToken();
					if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
						i1 = transToInt(tmp3);
						Register[keyword[tmp1]] = Register[keyword[tmp2]] / abs(i1);
					}
					else
						Register[keyword[tmp1]] = Register[keyword[tmp2]] / abs(Register[keyword[tmp3]]);
				}
				else {
					if ((tmp2[0] == '-' && (tmp2[1] >= '0' && tmp2[1] <= '9')) || (tmp2[0] >= '0' && tmp2[0] <= '9')) {
						i1 = transToInt(tmp2);
						Register[keyword["$hi"]] = Register[keyword[tmp1]] % abs(i1);
						Register[keyword["$lo"]] = Register[keyword[tmp1]] / abs(i1);
					}
					else {
						Register[keyword["$hi"]] = Register[keyword[tmp1]] % abs(Register[keyword[tmp2]]);
						Register[keyword["$lo"]] = Register[keyword[tmp1]] / abs(Register[keyword[tmp2]]);
					}
				}
			}
			else if (line == "xor") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ i1;
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ Register[keyword[tmp3]];
			}
			else if (line == "xoru") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ abs(i1);
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ abs(Register[keyword[tmp3]]);
			}
			else if (line == "neg") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				Register[keyword[tmp1]] = -Register[keyword[tmp2]];
			}
			else if (line == "negu") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				Register[keyword[tmp1]] = ~abs(Register[keyword[tmp2]]);
			}
			else if (line == "rem") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] % i1;
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] % Register[keyword[tmp3]];
			}
			else if (line == "remu") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = Register[keyword[tmp2]] % abs(i1);
				}
				else
					Register[keyword[tmp1]] = Register[keyword[tmp2]] % abs(Register[keyword[tmp3]]);
			}
			else if (line == "seq") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] == i1);
				}
				else
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] == Register[keyword[tmp3]]);
			}
			else if (line == "sge") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] >= i1);
				}
				else
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] >= Register[keyword[tmp3]]);
			}
			else if (line == "sgt") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] > i1);
				}
				else
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] > Register[keyword[tmp3]]);
			}
			else if (line == "sle") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] <= i1);
				}
				else
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] <= Register[keyword[tmp3]]);
			}
			else if (line == "slt") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if ((tmp3[0] == '-' && (tmp3[1] >= '0' && tmp3[1] <= '9')) || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] < i1);
				}
				else
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] < Register[keyword[tmp3]]);
			}
			else if (line == "sne") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken();
				if (tmp3[0] == '-' || (tmp3[0] >= '0' && tmp3[0] <= '9')) {
					i1 = transToInt(tmp3);
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] != i1);
				}
				else
					Register[keyword[tmp1]] = (Register[keyword[tmp2]] != Register[keyword[tmp3]]);
			}
			else if (line == "sb") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					i1 = transToInt(tmp2);
					tmp3 = scanner.nextToken();
					i2 = Register[keyword[tmp3]] + i1;
				}
				else
					i2 = memory[tmp2];
				//memo.gotoPos(i2);
				//memo.byte((char)Register[keyword[tmp1]]);
				memo.save(i2, 1, Register[keyword[tmp1]]);
			}
			else if (line == "sh") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					i1 = transToInt(tmp2);
					tmp3 = scanner.nextToken();
					i2 = Register[keyword[tmp3]] + i1;
				}
				else
					i2 = memory[tmp2];
				//memo.gotoPos(i2);
				//memo.half((short)Register[keyword[tmp1]]);
				memo.save(i2, 2, Register[keyword[tmp1]]);
			}
			else if (line == "sw") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					i1 = transToInt(tmp2);
					tmp3 = scanner.nextToken();
					i2 = Register[keyword[tmp3]] + i1;
				}
				else
					i2 = memory[tmp2];
				//memo.gotoPos(i2);
				//memo.word(Register[keyword[tmp1]]);
				memo.save(i2, 4, Register[keyword[tmp1]]);
			}
			else if (line == "move") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				Register[keyword[tmp1]] = Register[keyword[tmp2]];
			}
			else if (line == "mfhi") {
				tmp1 = scanner.nextToken();
				Register[keyword[tmp1]] = Register[keyword["$hi"]];
			}
			else if (line == "mflo") {
				tmp1 = scanner.nextToken();
				Register[keyword[tmp1]] = Register[keyword["$lo"]];
			}
			else if (line == "nop") {
			}
			else if (line == "syscall") {
				if (Register[keyword["$v0"]] == 1)
					std::cout << Register[keyword["$a0"]];
				else if (Register[keyword["$v0"]] == 4) {
					//std::string *str = (std::string*)Register[keyword["a0"]];
					//std::cout << *str;
					memo.print(Register[keyword["$a0"]]);
				}
				else if (Register[keyword["$v0"]] == 5) {
					std::cin >> i1;
					Register[keyword["$v0"]] = i1;
				}
				else if (Register[keyword["$v0"]] == 8) {
					std::cin >> tmp1;
					/*if (tmp1.length() > Register[keyword["a1"]] - 1)
					tmp1 = tmp1.substr(0, Register[keyword["a1"]] - 1);
					memo.gotoPos(Register[keyword["a0"]]);
					memo.asciiz(tmp1);*/
					memo.saveString(tmp1, Register[keyword["$a0"]], tmp1.length());
					Register[keyword["$a1"]] = tmp1.length();
				}
				else if (Register[keyword["$v0"]] == 9) {
					Register[keyword["$v0"]] = memo.space(Register[keyword["$a0"]]);
				}
				else if (Register[keyword["$v0"]] == 10) {
					exit(0);
				}
				else if (Register[keyword["$v0"]] == 17) {
					exit(Register[keyword["$a0"]]);
				}
			}
		}
		//std::cout << line << ' ';
		currentLine++;
	}
	else if (line[0] == 'b' || line[0] == 'j') {
		if (mainbegin) {
			if (line == "b") {
				tmp1 = scanner.nextToken() + ':';
				currentLine = textnum[tmp1];
			}
			if (line == "beq") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken() + ':';
				int tmp;
				if (tmp2[0] == '$')
					tmp = Register[keyword[tmp2]];
				else
					tmp = transToInt(tmp2);
				if (Register[keyword[tmp1]] == tmp)
					currentLine = textnum[tmp3];
			}
			if (line == "bne") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken() + ':';
				int tmp;
				if (tmp2[0] == '$')
					tmp = Register[keyword[tmp2]];
				else
					tmp = transToInt(tmp2);
				if (Register[keyword[tmp1]] != tmp)
					currentLine = textnum[tmp3];
			}
			if (line == "bge") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken() + ':';
				int tmp;
				if (tmp2[0] == '$')
					tmp = Register[keyword[tmp2]];
				else
					tmp = transToInt(tmp2);
				if (Register[keyword[tmp1]] >= tmp)
					currentLine = textnum[tmp3];
			}
			if (line == "ble") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken() + ':';
				int tmp;
				if (tmp2[0] == '$')
					tmp = Register[keyword[tmp2]];
				else
					tmp = transToInt(tmp2);
				if (Register[keyword[tmp1]] <= tmp)
					currentLine = textnum[tmp3];
			}
			if (line == "bgt") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken() + ':';
				int tmp;
				if (tmp2[0] == '$')
					tmp = Register[keyword[tmp2]];
				else
					tmp = transToInt(tmp2);
				if (Register[keyword[tmp1]] > tmp)
					currentLine = textnum[tmp3];
			}
			if (line == "blt") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				tmp3 = scanner.nextToken() + ':';
				int tmp;
				if (tmp2[0] == '$')
					tmp = Register[keyword[tmp2]];
				else
					tmp = transToInt(tmp2);
				if (Register[keyword[tmp1]] < tmp)
					currentLine = textnum[tmp3];
			}
			if (line == "beqz") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken() + ':';
				if (Register[keyword[tmp1]] == 0)
					currentLine = textnum[tmp2];
			}
			if (line == "bnez") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken() + ':';
				if (Register[keyword[tmp1]] != 0)
					currentLine = textnum[tmp2];
			}
			if (line == "blez") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken() + ':';
				if (Register[keyword[tmp1]] <= 0)
					currentLine = textnum[tmp2];
			}
			if (line == "bgez") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken() + ':';
				if (Register[keyword[tmp1]] >= 0)
					currentLine = textnum[tmp2];
			}
			if (line == "bgtz") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken() + ':';
				if (Register[keyword[tmp1]] > 0)
					currentLine = textnum[tmp2];
			}
			if (line == "bltz") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken() + ':';
				if (Register[keyword[tmp1]] < 0)
					currentLine = textnum[tmp2];
			}
			if (line == "j") {
				tmp1 = scanner.nextToken() + ':';
				currentLine = textnum[tmp1];
			}
			if (line == "jr") {
				tmp1 = scanner.nextToken();
				currentLine = Register[keyword[tmp1]];
				currentLine--;
			}
			if (line == "jal") {
				tmp1 = scanner.nextToken() + ':';
				Register[31] = currentLine + 1;
				currentLine = textnum[tmp1];
			}
			if (line == "jalr") {
				tmp1 = scanner.nextToken();
				Register[31] = currentLine + 1;
				currentLine = Register[keyword[tmp1]];
				currentLine--;
			}
		}
		currentLine++;
		//std::cout << line << ' ';
	}
	else if (line[0] == 'l') {
		if (mainbegin) {
			if (line == "li") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				i1 = transToInt(tmp2);
				Register[keyword[tmp1]] = i1;
			}

			if (line == "la") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					i1 = transToInt(tmp2);
					tmp3 = scanner.nextToken();
					i2 = Register[keyword[tmp3]] + i1;
				}
				else
					i2 = memory[tmp2];
				Register[keyword[tmp1]] = i2;
			}
			if (line == "lb") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					i1 = transToInt(tmp2);
					tmp3 = scanner.nextToken();
					i2 = Register[keyword[tmp3]] + i1;
				}
				else
					i2 = memory[tmp2];
				Register[keyword[tmp1]] = memo.readByte(i2);
			}
			if (line == "lh") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					i1 = transToInt(tmp2);
					tmp3 = scanner.nextToken();
					i2 = Register[keyword[tmp3]] + i1;
				}
				else
					i2 = memory[tmp2];
				Register[keyword[tmp1]] = memo.readHalf(i2);
			}
			if (line == "lw") {
				tmp1 = scanner.nextToken();
				tmp2 = scanner.nextToken();
				if (scanner.hasMoreToken()) {
					i1 = transToInt(tmp2);
					tmp3 = scanner.nextToken();
					i2 = Register[keyword[tmp3]] + i1;
				}
				else
					i2 = memory[tmp2];
				Register[keyword[tmp1]] = memo.readWord(i2);
			}
		}
		//std::cout << line << ' ';
		currentLine++;
	}
	else {
		currentLine++;
		//std::cout << line << ' ';
	}
}
