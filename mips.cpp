#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <vector>
#include "memory.h"
#include "tokenscanner.h"

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
std::map<std::string, int> Label = {
	std::make_pair(".align", 1), std::make_pair(".ascii", 2), std::make_pair(".asciiz", 3), std::make_pair(".byte", 4),
	std::make_pair(".half", 5), std::make_pair(".word", 6), std::make_pair(".space", 7), std::make_pair(".data", 8),
	std::make_pair(".text", 9), std::make_pair(".add", 10), std::make_pair("addu", 11), std::make_pair("addiu", 12),
	std::make_pair("sub", 13), std::make_pair("subu", 14), std::make_pair("mul", 15), std::make_pair("mulu", 16),
	std::make_pair("div", 17), std::make_pair("divu", 18), std::make_pair("xor", 19), std::make_pair("xoru", 20),
	std::make_pair("neg", 21), std::make_pair("negu", 22), std::make_pair("rem", 23), std::make_pair("remu", 24),
	std::make_pair("li", 25), std::make_pair("seq", 26), std::make_pair("sge", 27), std::make_pair("sgt", 28),
	std::make_pair("sle", 29), std::make_pair("slt", 30), std::make_pair("sne", 31), std::make_pair("b", 32),
	std::make_pair("beq", 33), std::make_pair("bne", 34), std::make_pair("bge", 35), std::make_pair("ble", 36),
	std::make_pair("bgt", 37), std::make_pair("blt", 38), std::make_pair("beqz", 39), std::make_pair("bnez", 40),
	std::make_pair("blez", 41), std::make_pair("bgez", 42), std::make_pair("bgtz", 43), std::make_pair("bltz", 44),
	std::make_pair("j", 45), std::make_pair("jr", 46), std::make_pair("jal", 47), std::make_pair("jalr", 48),
	std::make_pair("la", 49), std::make_pair("lb", 50), std::make_pair("lh", 51), std::make_pair("lw", 52),
	std::make_pair("sb", 53), std::make_pair("sh", 54), std::make_pair("sw", 55), std::make_pair("move", 56),
	std::make_pair("mfhi", 57), std::make_pair("mflo", 58), std::make_pair("nop", 59), std::make_pair("syscall", 60),
	std::make_pair("main", 61)
};
bool isNumber(std::string &str) {
	return ((str[0] == '-' && (str[1] >= '0' && str[1] <= '9')) || (str[0] >= '0' && str[0] <= '9'));
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

struct textGroup {
	std::string s[4];
	int n;
	int label;
	bool type;//是否有数字
	bool flag;//mul/div类型
	textGroup() {
		for (int i = 0; i < 4; ++i) {
			s[i] = "";
		}
		n = 0;
		label = 0;
		type = 0;
		flag = 0;
	}
	~textGroup() {}
};
int transToLabel(std::string str) {
	if (str == ".align") return 1;
	else if (str == ".ascii") return 2;
	else if (str == ".asciiz") return 3;
	else if (str == ".byte") return 4;
	else if (str == ".half") return 5;
	else if (str == ".word") return 6;
	else if (str == ".space") return 7;
	else if (str == "main") return 8;
	else if (str == "add") return 9;
	else if (str == "addu") return 10;
	else if (str == "addiu") return 11;
	else if (str == "sub") return 12;
	else if (str == "subu") return 13;
	else if (str == "mul") return 14;
	else if (str == "mulu") return 15;
	else if (str == "div") return 16;
	else if (str == "divu") return 17;
	else if (str == "xor") return 18;
	else if (str == "xoru") return 19;
	else if (str == "neg") return 20;
	else if (str == "negu") return 21;
	else if (str == "rem") return 22;
	else if (str == "remu") return 23;
	else if (str == "seq") return 24;
	else if (str == "sge") return 25;
	else if (str == "sgt") return 26;
	else if (str == "sle") return 27;
	else if (str == "slt") return 28;
	else if (str == "sne") return 29;
	else if (str == "sb") return 30;
	else if (str == "sh") return 31;
	else if (str == "sw") return 32;
	else if (str == "move") return 33;
	else if (str == "mfhi") return 34;
	else if (str == "mflo") return 35;
	else if (str == "nop") return 36;
	else if (str == "syscall") return 37;
	else if (str == "b") return 38;
	else if (str == "beq") return 39;
	else if (str == "bne") return 40;
	else if (str == "bge") return 41;
	else if (str == "ble") return 42;
	else if (str == "bgt") return 43;
	else if (str == "blt") return 44;
	else if (str == "beqz") return 45;
	else if (str == "bnez") return 46;
	else if (str == "blez") return 47;
	else if (str == "bgez") return 48;
	else if (str == "bgtz") return 49;
	else if (str == "bltz") return 50;
	else if (str == "j") return 51;
	else if (str == "jr") return 52;
	else if (str == "jal") return 53;
	else if (str == "jalr") return 54;
	else if (str == "li") return 55;
	else if (str == "la") return 56;
	else if (str == "lb") return 57;
	else if (str == "lh") return 58;
	else if (str == "lw") return 59;
	else return 0;
}
struct dataGroup {
	std::string s[2];
	int *n;
	int num, Size, label;
	dataGroup() {
		s[0] = s[1] = "";
		n = new int[10];
		for (int i = 0; i < 10; ++i) {
			n[i] = 0;
		}
		label = 0;
		num = 0;
		Size = 10;
	}
	void doubleSpace() {
		int *tmp = n;
		Size *= 2;
		n = new int[Size];
		for (int i = 0; i < Size / 2; ++i)
			n[i] = tmp[i];
		delete tmp;
	}
	void input(int i) {
		if (num == Size)
			doubleSpace();
		n[num++] = i;
	}
	~dataGroup() {}
};

Memory memo;
std::map<std::string, int> memory;
//std::vector<std::string> text;
std::map<std::string, int> textnum;
//std::vector<std::string> data;
int Register[35] = { 0 };
bool textStore = false, dataStore = false, mainbegin = false;
int currentLine = 0, datapos = 0, dataLine = 0, textLine = 0;
std::vector<textGroup> text1;
std::vector<dataGroup> data1;
void processData();
void processText();

int main(int argc, char* argv[]) {
	Register[29] = 4 * 1024 * 1024;
	std::ifstream infile;
	infile.open("4.s");
	std::string str, line, tmp;
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
				struct dataGroup l;
				l.s[0] = str;
				l.label = transToLabel(str);
				if (str == ".ascii" || str == ".asciiz")
					l.s[1] = scanner.nextToken();
				else {
					while (scanner.hasMoreToken()) {
						tmp = scanner.nextToken();
						l.input(transToInt(tmp));
					}
				}
				data1.push_back(l);
				dataLine++;
			}
			else if (textStore) {
				struct textGroup l;
				l.s[0] = str;
				l.label = transToLabel(str);
				int i = 1;
				while (scanner.hasMoreToken()) {
					tmp = scanner.nextToken();
					if (isNumber(tmp)) {
						l.n = transToInt(tmp);
						l.type = 1;
					}
					else
						l.s[i++] = tmp;
				}
				if ((l.type && i == 3) || i == 4)
					l.flag = 1;
				text1.push_back(l);
				if (str[0] == '_')
					textnum.insert(std::map<std::string, int> ::value_type(str + ':', textLine));
				textLine++;
			}
		}
	}
	infile.close();
	for (; currentLine < dataLine;) {
		processData();
	}
	for (currentLine = 0; currentLine < textLine; ) {
		processText();
	}
	system("pause");
	return 0;
}


void processData() {
	dataGroup p = data1[currentLine];
	std::string line = p.s[0];
	std::string tmp1;
	int i1;
	short s1;
	char c1;
	//std::cout << p.label << std::endl;
	if (p.label == 1){
		memo.align(p.n[0]);
		datapos = memo.getPos();
	}
	else if (p.label == 2){
		memo.ascii(p.s[1]);
		datapos = memo.getPos();
	}
	else if (p.label == 3){
		memo.asciiz(p.s[1]);
		datapos = memo.getPos();
	}
	else if (p.label == 4){
		for (int i = 0; i < p.num; ++i) {
			memo.byte(p.n[i]);
		}
		datapos = memo.getPos();
	}
	else if (p.label == 5) {
		for (int i = 0; i < p.num; ++i) {
			memo.half(p.n[i]);
		}
		datapos = memo.getPos();
	}
	else if (p.label == 6){
		for (int i = 0; i < p.num; ++i) {
			memo.word(p.n[i]);
		}
		datapos = memo.getPos();
	}
	else if (p.label == 7){
		memo.space(p.n[0]);
		datapos = memo.getPos();
	}
	else {
		memory.insert(std::map<std::string, int> ::value_type(line, datapos));
	}
	currentLine++;
}
void processText() {
	textGroup p = text1[currentLine];
	std::string tmp1, tmp2, tmp3;
	int i1, i2;
	char c1;
	short s1;
	//std::cout << p.label << std::endl;
	if (p.label == 8) {
		mainbegin = true;
		currentLine++;
	}
	else if (mainbegin) {
		if (p.label == 9) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] + p.n;
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] + Register[keyword[tmp3]];
			}
		}
		else if (p.label == 10) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] + abs(p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] + abs(Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 11) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			Register[keyword[tmp1]] = Register[keyword[tmp2]] + abs(p.n);
		}
		else if (p.label == 12) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] - p.n;
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] - Register[keyword[tmp3]];
			}
		}
		else if (p.label == 13) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] - abs(p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] - abs(Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 14) {
			tmp1 = p.s[1];
			//std::cout << p.flag << ' ';
			if (p.flag) {
				tmp2 = p.s[2];
				if (p.type)
					Register[keyword[tmp1]] = Register[keyword[tmp2]] * p.n;
				else {
					tmp3 = p.s[3];
					Register[keyword[tmp1]] = Register[keyword[tmp2]] * Register[keyword[tmp3]];
				}
				//std::cout << Register[keyword[tmp1]];
			}
			else {
				if (p.type) {
					Register[keyword["$hi"]] = (Register[keyword[tmp1]] * p.n) / 2 ^ 32;
					Register[keyword["$lo"]] = (Register[keyword[tmp1]] * p.n) % 2 ^ 32;
				}
				else {
					tmp2 = p.s[2];
					Register[keyword["$hi"]] = (Register[keyword[tmp1]] * Register[keyword[tmp2]]) / 2 ^ 32;
					Register[keyword["$lo"]] = (Register[keyword[tmp1]] * Register[keyword[tmp2]]) % 2 ^ 32;
				}
			}
		}
		else if (p.label == 15) {
			tmp1 = p.s[1];
			//std::cout << p.flag << ' ';
			if (p.flag) {
				tmp2 = p.s[2];
				if (p.type)
					Register[keyword[tmp1]] = Register[keyword[tmp2]] * abs(p.n);
				else {
					tmp3 = p.s[3];
					Register[keyword[tmp1]] = Register[keyword[tmp2]] * abs(Register[keyword[tmp3]]);
				}
				//std::cout << Register[keyword[tmp1]];
			}
			else {
				if (p.type) {
					Register[keyword["$hi"]] = (Register[keyword[tmp1]] * abs(p.n)) / 2 ^ 32;
					Register[keyword["$lo"]] = (Register[keyword[tmp1]] * abs(p.n)) % 2 ^ 32;
				}
				else {
					tmp2 = p.s[2];
					Register[keyword["$hi"]] = (Register[keyword[tmp1]] * abs(Register[keyword[tmp2]])) / 2 ^ 32;
					Register[keyword["$lo"]] = (Register[keyword[tmp1]] * abs(Register[keyword[tmp2]])) % 2 ^ 32;
				}
			}
		}
		else if (p.label == 16) {
			tmp1 = p.s[1];
			if (p.flag) {
				tmp2 = p.s[2];
				if (p.type)
					Register[keyword[tmp1]] = Register[keyword[tmp2]] / p.n;
				else {
					tmp3 = p.s[3];
					Register[keyword[tmp1]] = Register[keyword[tmp2]] / Register[keyword[tmp3]];
				}
			}
			else {
				if (p.type) {
					Register[keyword["$hi"]] = Register[keyword[tmp1]] % p.n;
					Register[keyword["$lo"]] = Register[keyword[tmp1]] / p.n;
				}
				else {
					tmp2 = p.s[2];
					Register[keyword["$hi"]] = Register[keyword[tmp1]] % Register[keyword[tmp2]];
					Register[keyword["$lo"]] = Register[keyword[tmp1]] / Register[keyword[tmp2]];
				}
			}
		}
		else if (p.label == 17) {
			tmp1 = p.s[1];
			if (p.flag) {
				tmp2 = p.s[2];
				if (p.type)
					Register[keyword[tmp1]] = Register[keyword[tmp2]] / abs(p.n);
				else {
					tmp3 = p.s[3];
					Register[keyword[tmp1]] = Register[keyword[tmp2]] / abs(Register[keyword[tmp3]]);
				}
			}
			else {
				if (p.type) {
					Register[keyword["$hi"]] = Register[keyword[tmp1]] % abs(p.n);
					Register[keyword["$lo"]] = Register[keyword[tmp1]] / abs(p.n);
				}
				else {
					tmp2 = p.s[2];
					Register[keyword["$hi"]] = Register[keyword[tmp1]] % abs(Register[keyword[tmp2]]);
					Register[keyword["$lo"]] = Register[keyword[tmp1]] / abs(Register[keyword[tmp2]]);
				}
			}
		}
		else if (p.label == 18) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ p.n;
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ Register[keyword[tmp3]];
			}
		}
		else if (p.label == 19) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ abs(p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] ^ abs(Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 20) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			Register[keyword[tmp1]] = -Register[keyword[tmp2]];
		}
		else if (p.label == 21) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			Register[keyword[tmp1]] = ~abs(Register[keyword[tmp2]]);
		}
		else if (p.label == 22) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] % p.n;
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] % Register[keyword[tmp3]];
			}
		}
		else if (p.label == 23) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = Register[keyword[tmp2]] % abs(p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = Register[keyword[tmp2]] % abs(Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 24) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] == p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] == Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 25) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] >= p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] >= Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 26) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] > p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] > Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 27) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] <= p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] <= Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 28) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] < p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] < Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 29) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type)
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] != p.n);
			else {
				tmp3 = p.s[3];
				Register[keyword[tmp1]] = (Register[keyword[tmp2]] != Register[keyword[tmp3]]);
			}
		}
		else if (p.label == 30) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type) {
				i2 = Register[keyword[tmp2]] + p.n;
			}
			else
				i2 = memory[tmp2];
			memo.save(i2, 1, Register[keyword[tmp1]]);
		}
		else if (p.label == 31) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type) {
				i2 = Register[keyword[tmp2]] + p.n;
			}
			else
				i2 = memory[tmp2];
			memo.save(i2, 2, Register[keyword[tmp1]]);
		}
		else if (p.label == 32) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type) {
				i2 = Register[keyword[tmp2]] + p.n;
			}
			else
				i2 = memory[tmp2];
			memo.save(i2, 4, Register[keyword[tmp1]]);
		}
		else if (p.label == 33) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			Register[keyword[tmp1]] = Register[keyword[tmp2]];
		}
		else if (p.label == 34) {
			tmp1 = p.s[1];
			Register[keyword[tmp1]] = Register[keyword["$hi"]];
		}
		else if (p.label == 35) {
			tmp1 = p.s[1];
			Register[keyword[tmp1]] = Register[keyword["$lo"]];
		}
		else if (p.label == 36) {
		}
		else if (p.label == 37) {
			if (Register[keyword["$v0"]] == 1)
				std::cout << Register[keyword["$a0"]];
			else if (Register[keyword["$v0"]] == 4) {
				memo.print(Register[keyword["$a0"]]);
			}
			else if (Register[keyword["$v0"]] == 5) {
				std::cin >> i1;
				Register[keyword["$v0"]] = i1;
			}
			else if (Register[keyword["$v0"]] == 8) {
				std::cin >> tmp1;
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
		else if (p.label == 38) {
			tmp1 = p.s[1] + ':';
			currentLine = textnum[tmp1];
		}
		else if (p.label == 39) {
			tmp1 = p.s[1];
			int tmp;
			if (!p.type) {
				tmp2 = p.s[2];
				tmp3 = p.s[3] + ':';
				tmp = Register[keyword[tmp2]];
				if (Register[keyword[tmp1]] == tmp)
					currentLine = textnum[tmp3];
			}
			else {
				tmp2 = p.s[2] + ':';
				tmp = p.n;
				if (Register[keyword[tmp1]] == tmp)
					currentLine = textnum[tmp2];
			}
		}
		else if (p.label == 40) {
			tmp1 = p.s[1];
			int tmp;
			if (!p.type) {
				tmp2 = p.s[2];
				tmp3 = p.s[3] + ':';
				tmp = Register[keyword[tmp2]];
				if (Register[keyword[tmp1]] != tmp)
					currentLine = textnum[tmp3];
			}
			else {
				tmp2 = p.s[2] + ':';
				tmp = p.n;
				if (Register[keyword[tmp1]] != tmp)
					currentLine = textnum[tmp2];
			}
		}
		else if (p.label == 41) {
			tmp1 = p.s[1];
			int tmp;
			if (!p.type) {
				tmp2 = p.s[2];
				tmp3 = p.s[3] + ':';
				tmp = Register[keyword[tmp2]];
				if (Register[keyword[tmp1]] >= tmp)
					currentLine = textnum[tmp3];
			}
			else {
				tmp2 = p.s[2] + ':';
				tmp = p.n;
				if (Register[keyword[tmp1]] >= tmp)
					currentLine = textnum[tmp2];
			}
		}
		else if (p.label == 42) {
			tmp1 = p.s[1];
			int tmp;
			if (!p.type) {
				tmp2 = p.s[2];
				tmp3 = p.s[3] + ':';
				tmp = Register[keyword[tmp2]];
				if (Register[keyword[tmp1]] <= tmp)
					currentLine = textnum[tmp3];
			}
			else {
				tmp2 = p.s[2] + ':';
				tmp = p.n;
				if (Register[keyword[tmp1]] <= tmp)
					currentLine = textnum[tmp2];
			}
		}
		else if (p.label == 43) {
			tmp1 = p.s[1];
			int tmp;
			if (!p.type) {
				tmp2 = p.s[2];
				tmp3 = p.s[3] + ':';
				tmp = Register[keyword[tmp2]];
				if (Register[keyword[tmp1]] > tmp)
					currentLine = textnum[tmp3];
			}
			else {
				tmp2 = p.s[2] + ':';
				tmp = p.n;
				if (Register[keyword[tmp1]] > tmp)
					currentLine = textnum[tmp2];
			}
		}
		else if (p.label == 44) {
			tmp1 = p.s[1];
			int tmp;
			if (!p.type) {
				tmp2 = p.s[2];
				tmp3 = p.s[3] + ':';
				tmp = Register[keyword[tmp2]];
				if (Register[keyword[tmp1]] < tmp)
					currentLine = textnum[tmp3];
			}
			else {
				tmp2 = p.s[2] + ':';
				tmp = p.n;
				if (Register[keyword[tmp1]] < tmp)
					currentLine = textnum[tmp2];
			}
		}
		else if (p.label == 45) {
			tmp1 = p.s[1];
			tmp2 = p.s[2] + ':';
			if (Register[keyword[tmp1]] == 0)
				currentLine = textnum[tmp2];
		}
		else if (p.label == 46) {
			tmp1 = p.s[1];
			tmp2 = p.s[2] + ':';
			if (Register[keyword[tmp1]] != 0)
				currentLine = textnum[tmp2];
		}
		else if (p.label == 47) {
			tmp1 = p.s[1];
			tmp2 = p.s[2] + ':';
			if (Register[keyword[tmp1]] <= 0)
				currentLine = textnum[tmp2];
		}
		else if (p.label == 48) {
			tmp1 = p.s[1];
			tmp2 = p.s[2] + ':';
			if (Register[keyword[tmp1]] >= 0)
				currentLine = textnum[tmp2];
		}
		else if (p.label == 49) {
			tmp1 = p.s[1];
			tmp2 = p.s[2] + ':';
			if (Register[keyword[tmp1]] > 0)
				currentLine = textnum[tmp2];
		}
		else if (p.label == 50) {
			tmp1 = p.s[1];
			tmp2 = p.s[2] + ':';
			if (Register[keyword[tmp1]] < 0)
				currentLine = textnum[tmp2];
		}
		else if (p.label == 51) {
			tmp1 = p.s[1] + ':';
			currentLine = textnum[tmp1];
		}
		else if (p.label == 52) {
			tmp1 = p.s[1];
			currentLine = Register[keyword[tmp1]];
			currentLine--;
		}
		else if (p.label == 53) {
			tmp1 = p.s[1] + ':';
			Register[31] = currentLine + 1;
			currentLine = textnum[tmp1];
		}
		else if (p.label == 54) {
			tmp1 = p.s[1];
			Register[31] = currentLine + 1;
			currentLine = Register[keyword[tmp1]];
			currentLine--;
		}
		else if (p.label == 55) {
			tmp1 = p.s[1];
			Register[keyword[tmp1]] = p.n;
		}
		else if (p.label == 56) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type) {
				i2 = Register[keyword[tmp2]] + p.n;
			}
			else {
				i2 = memory[tmp2];
			}
			Register[keyword[tmp1]] = i2;
		}
		else if (p.label == 57) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type) {
				i2 = Register[keyword[tmp2]] + p.n;
			}
			else {
				i2 = memory[tmp2];
			}
			Register[keyword[tmp1]] = memo.readByte(i2);
		}
		else if (p.label == 58) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type) {
				i2 = Register[keyword[tmp2]] + p.n;
			}
			else {
				i2 = memory[tmp2];
			}
			Register[keyword[tmp1]] = memo.readHalf(i2);
		}
		else if (p.label == 59) {
			tmp1 = p.s[1];
			tmp2 = p.s[2];
			if (p.type) {
				i2 = Register[keyword[tmp2]] + p.n;
			}
			else {
				i2 = memory[tmp2];
			}
			Register[keyword[tmp1]] = memo.readWord(i2);
		}
		else if (p.label == 0){}
		currentLine++;
	}
	else {
		currentLine++;
	}
}
