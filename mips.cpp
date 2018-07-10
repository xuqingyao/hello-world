#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <vector>
#include "memory.h"
#include "tokenscanner.h"

std::map<std::string, int> keyword;
void initKey(){
	keyword["$0"] = 0; keyword["$1"] = 1; keyword["$2"] = 2; keyword["$3"] = 3;
	keyword["$4"] = 4; keyword["$5"] = 5; keyword["$6"] = 6; keyword["$7"] = 7;
	keyword["$8"] = 8; keyword["$9"] = 9; keyword["$10"] = 10; keyword["$11"] = 11;
	keyword["$12"] = 12; keyword["$13"] = 13; keyword["$14"] = 14; keyword["$15"] = 15;
	keyword["$16"] = 16; keyword["$17"] = 17; keyword["$18"] = 18; keyword["$19"] = 19;
	keyword["$20"] = 20; keyword["$21"] = 21; keyword["$22"] = 22; keyword["$23"] = 23;
	keyword["$24"] = 24; keyword["$25"] = 25; keyword["$26"] = 26; keyword["$27"] = 27;
	keyword["$28"] = 28; keyword["$29"] = 29; keyword["$30"] = 30; keyword["$31"] = 31;
	keyword["$zero"] = 0; keyword["$at"] = 1; keyword["$v0"] = 2; keyword["$v1"] = 3;
	keyword["$a0"] = 4; keyword["$a1"] = 5; keyword["$a2"] = 6; keyword["$a3"] = 7;
	keyword["$t0"] = 8; keyword["$t1"] = 9; keyword["$t2"] = 10; keyword["$t3"] = 11;
	keyword["$t4"] = 12; keyword["$t5"] = 13; keyword["$t6"] = 14; keyword["$t7"] = 15;
	keyword["$s0"] = 16; keyword["$s1"] = 17; keyword["$s2"] = 18; keyword["$s3"] = 19;
	keyword["$s4"] = 20; keyword["$s5"] = 21; keyword["$s6"] = 22; keyword["$s7"] = 23;
	keyword["$t8"] = 24; keyword["$t9"] = 25; keyword["$k0"] = 26; keyword["$k1"] = 27;
	keyword["$gp"] = 28; keyword["$sp"] = 29; keyword["$fp"] = 30; keyword["$ra"] = 31;
	keyword["$hi"] = 32; keyword["$lo"] = 33; keyword["$pc"] = 34;
}
std::map<std::string, int> Label;
void initLabel(){
	Label[".align"] = 1; Label[".ascii"] = 2; Label[".asciiz"] = 3; Label[".byte"] = 4;
	Label[".half"] = 5; Label[".word"] = 6; Label[".space"] = 7; Label["main"] = 8;
	Label["add"] = 9; Label["addu"] = 10; Label["addiu"] = 11; Label["sub"] = 12;
	Label["subu"] = 13; Label["mul"] = 14; Label["mulu"] = 15; Label["div"] = 16;
	Label["divu"] = 17; Label["xor"] = 18; Label["xoru"] = 19; Label["neg"] = 20;
	Label["negu"] = 21; Label["rem"] = 22; Label["remu"] = 23; Label["seq"] = 24;
	Label["sge"] = 25; Label["sgt"] = 26; Label["sle"] = 27; Label["slt"] = 28;
	Label["sne"] = 29; Label["sb"] = 30; Label["sh"] = 31; Label["sw"] = 32;
	Label["move"] = 33; Label["mfhi"] = 34; Label["mflo"] = 35; Label["nop"] = 36;
	Label["syscall"] = 37; Label["b"] = 38; Label["beq"] = 39; Label["bne"] = 40;
	Label["bge"] = 41; Label["ble"] = 42; Label["bgt"] = 43; Label["blt"] = 44;
	Label["beqz"] = 45; Label["bnez"] = 46; Label["blez"] = 47; Label["bgez"] = 48;
	Label["bgtz"] = 49; Label["bltz"] = 50; Label["j"] = 51; Label["jr"] = 52; 
	Label["jal"] = 53; Label["jalr"] = 54; Label["li"] = 55; Label["la"] = 56;
	Label["lb"] = 57; Label["lh"] = 58; Label["lw"] = 59; 
}
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
/*int transToLabel(std::string &str) {
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
}*/
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
	initKey();
	initLabel();
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
				if (str[0] == '.')
					l.label = Label[str];
				else 
					l.label = 0;
				//std::cout << l.label << std::endl;
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
				if (str[0] == '_') {
					l.label = 0;
					textnum.insert(std::map<std::string, int> ::value_type(str + ':', textLine));
				}
				else
					l.label = Label[str];
				//std::cout << l.label << std::endl;
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
				//if (str[0] == '_')
					//textnum.insert(std::map<std::string, int> ::value_type(str + ':', textLine));
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
					Register[32] = (Register[keyword[tmp1]] * p.n) / 2 ^ 32;
					Register[33] = (Register[keyword[tmp1]] * p.n) % 2 ^ 32;
				}
				else {
					tmp2 = p.s[2];
					Register[32] = (Register[keyword[tmp1]] * Register[keyword[tmp2]]) / 2 ^ 32;
					Register[33] = (Register[keyword[tmp1]] * Register[keyword[tmp2]]) % 2 ^ 32;
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
					Register[32] = (Register[keyword[tmp1]] * abs(p.n)) / 2 ^ 32;
					Register[33] = (Register[keyword[tmp1]] * abs(p.n)) % 2 ^ 32;
				}
				else {
					tmp2 = p.s[2];
					Register[32] = (Register[keyword[tmp1]] * abs(Register[keyword[tmp2]])) / 2 ^ 32;
					Register[33] = (Register[keyword[tmp1]] * abs(Register[keyword[tmp2]])) % 2 ^ 32;
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
					Register[32] = Register[keyword[tmp1]] % p.n;
					Register[33] = Register[keyword[tmp1]] / p.n;
				}
				else {
					tmp2 = p.s[2];
					Register[32] = Register[keyword[tmp1]] % Register[keyword[tmp2]];
					Register[33] = Register[keyword[tmp1]] / Register[keyword[tmp2]];
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
					Register[32] = Register[keyword[tmp1]] % abs(p.n);
					Register[33] = Register[keyword[tmp1]] / abs(p.n);
				}
				else {
					tmp2 = p.s[2];
					Register[32] = Register[keyword[tmp1]] % abs(Register[keyword[tmp2]]);
					Register[33] = Register[keyword[tmp1]] / abs(Register[keyword[tmp2]]);
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
			Register[keyword[tmp1]] = Register[32];
		}
		else if (p.label == 35) {
			tmp1 = p.s[1];
			Register[keyword[tmp1]] = Register[33];
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
