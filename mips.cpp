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
	int Rdest, Rscr, Scr, address;
	int type;
	int command;
	std::string label;
	textGroup() {
		Rdest = Rscr = Scr = address  = command = -1;
		type = 0;//type == 1 —— number | type == 0 —— Register | type == 2 —— mul/div (3, n) | type == 3 —— mul/div (3, r) | 
		label = "";
	}
	~textGroup(){}
};

struct dataGroup {
	std::string s[2];
	int *n;
	int num, Size, command;
	dataGroup() {
		s[0] = s[1] = "";
		n = new int[10];
		for (int i = 0; i < 10; ++i) {
			n[i] = 0;
		}
		command = 0;
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
	std::ifstream infile(argv[1]);
	//nfile.open("4.s");
	std::string str, line, tmp;
	std::string s[3];
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
					l.command = Label[str];
				else 
					l.command = 0;
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
				s[0] = s[1] = s[2] = "";
				struct textGroup l;
				if (str[0] == '_') {
					l.command = 0;
					textnum.insert(std::map<std::string, int> ::value_type(str, textLine));
				}
				else
					l.command = Label[str];
				int i = 0;
				while (scanner.hasMoreToken()) {
					s[i++] = scanner.nextToken();
				}
				if ((l.command >= 9 && l.command <= 13) || (l.command >= 18 && l.command <= 29)) {
					l.Rdest = keyword[s[0]];
					l.Rscr = keyword[s[1]];
					if (s[2] == ""){}
					else if (isNumber(s[2])) {
						l.type = 1;
						l.Scr = transToInt(s[2]);
					}
					else {
						l.Scr = keyword[s[2]];
					}
				}
				else if (l.command >= 14 && l.command <= 17) {
					if (i == 3) {
						l.Rdest = keyword[s[0]];
						l.Rscr = keyword[s[1]];
						if (isNumber(s[2])) {
							l.type = 2;
							l.Scr = transToInt(s[2]);
						}
						else {
							l.type = 3;
							l.Scr = keyword[s[2]];
						}
					}
					else {
						l.Rdest = keyword[s[0]];
						if (isNumber(s[1])) {
							l.type = 4;
							l.Scr = transToInt(s[1]);
						}
						else {
							l.type = 5;
							l.Scr = keyword[s[1]];
						}
					}
				}
				else if (l.command >= 39 && l.command <= 44) {
					l.Rscr = keyword[s[0]];
					if (isNumber(s[1])) {
						l.type = 1;
						l.Scr = transToInt(s[1]);
					}
					else
						l.Scr = keyword[s[1]];
					l.label = s[2];
				}
				else if (l.command == 38 || l.command == 51 || l.command == 53) {
					l.label = s[0];
				}
				else if (l.command == 52 || l.command == 54) {
					l.Rscr = keyword[s[0]];
				}
				else if (l.command >= 45 && l.command <= 50) {
					l.Rscr = keyword[s[0]];
					l.label = s[1];
				}
				else if ((l.command >= 30 && l.command <= 32) || (l.command >= 56 && l.command <= 59)) {
					l.Rdest = keyword[s[0]];
					if (i == 3) {
						l.type = 6;
						l.address = transToInt(s[1]);
						l.Rscr = keyword[s[2]];
					}
					else {
						l.type = 7;
						l.label = s[1];
					}
				}
				else if (l.command == 55) {
					l.Rdest = keyword[s[0]];
					l.Scr = transToInt(s[1]);
				}
				else if (l.command >= 33 && l.command <= 35) {
					if (i == 2) {
						l.Rdest = keyword[s[0]];
						l.Rscr = keyword[s[1]];
					}
					else
						l.Rdest = keyword[s[0]];
				}
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
	if (p.command == 1){
		memo.align(p.n[0]);
		datapos = memo.getPos();
	}
	else if (p.command == 2){
		memo.ascii(p.s[1]);
		datapos = memo.getPos();
	}
	else if (p.command == 3){
		memo.asciiz(p.s[1]);
		datapos = memo.getPos();
	}
	else if (p.command == 4){
		for (int i = 0; i < p.num; ++i) {
			memo.byte(p.n[i]);
		}
		datapos = memo.getPos();
	}
	else if (p.command == 5) {
		for (int i = 0; i < p.num; ++i) {
			memo.half(p.n[i]);
		}
		datapos = memo.getPos();
	}
	else if (p.command == 6){
		for (int i = 0; i < p.num; ++i) {
			memo.word(p.n[i]);
		}
		datapos = memo.getPos();
	}
	else if (p.command == 7){
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
	if (p.command == 8) {
		mainbegin = true;
		currentLine++;
	}
	else if (mainbegin) {
		if (p.command == 9) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] + p.Scr;
			else
				Register[p.Rdest] = Register[p.Rscr] + Register[p.Scr];
		}
		else if (p.command == 10) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] + abs(p.Scr);
			else
				Register[p.Rdest] = Register[p.Rscr] + abs(Register[p.Scr]);
		}
		else if (p.command == 11) {
			Register[p.Rdest] = Register[p.Rscr] + abs(p.Scr);
		}
		else if (p.command == 12) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] - p.Scr;
			else
				Register[p.Rdest] = Register[p.Rscr] - Register[p.Scr];
		}
		else if (p.command == 13) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] - abs(p.Scr);
			else
				Register[p.Rdest] = Register[p.Rscr] - abs(Register[p.Scr]);
		}
		else if (p.command == 14) {
			if (p.type == 2)
				Register[p.Rdest] = Register[p.Rscr] * p.Scr;
			else if (p.type == 3)
				Register[p.Rdest] = Register[p.Rscr] * Register[p.Scr];
			else if (p.type == 4) {
				Register[32] = (Register[p.Rdest] * p.Scr) / 2 ^ 32;
				Register[33] = (Register[p.Rdest] * p.Scr) % 2 ^ 32;
			}
			else {
				Register[32] = (Register[p.Rdest] * Register[p.Scr]) / 2 ^ 32;
				Register[33] = (Register[p.Rdest] * Register[p.Scr]) % 2 ^ 32;
			}
		}
		else if (p.command == 15) {
			if (p.type == 2)
				Register[p.Rdest] = Register[p.Rscr] * abs(p.Scr);
			else if (p.type == 3)
				Register[p.Rdest] = Register[p.Rscr] * abs(Register[p.Scr]);
			else if (p.type == 4) {
				Register[32] = (Register[p.Rdest] * abs(p.Scr)) / 2 ^ 32;
				Register[33] = (Register[p.Rdest] * abs(p.Scr)) % 2 ^ 32;
			}
			else {
				Register[32] = (Register[p.Rdest] * abs(Register[p.Scr])) / 2 ^ 32;
				Register[33] = (Register[p.Rdest] * abs(Register[p.Scr])) % 2 ^ 32;
			}
		}
		else if (p.command == 16) {
			if (p.type == 2)
				Register[p.Rdest] = Register[p.Rscr] / p.Scr;
			else if (p.type == 3)
				Register[p.Rdest] = Register[p.Rscr] / Register[p.Scr];
			else if (p.type == 4){
				Register[32] = Register[p.Rdest] % p.Scr;
				Register[33] = Register[p.Rdest] / p.Scr;
			}
			else {
				Register[32] = Register[p.Rdest] % Register[p.Scr];
				Register[33] = Register[p.Rdest] / Register[p.Scr];
			}
		}
		else if (p.command == 17) {
			if (p.type == 2)
				Register[p.Rdest] = Register[p.Rscr] / abs(p.Scr);
			else if (p.type == 3)
				Register[p.Rdest] = Register[p.Rscr] / abs(Register[p.Scr]);
			else if (p.type == 4) {
				Register[32] = Register[p.Rdest] % abs(p.Scr);
				Register[33] = Register[p.Rdest] / abs(p.Scr);
			}
			else {
				Register[32] = Register[p.Rdest] % abs(Register[p.Scr]);
				Register[33] = Register[p.Rdest] / abs(Register[p.Scr]);
			}
		}
		else if (p.command == 18) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] ^ p.Scr;
			else
				Register[p.Rdest] = Register[p.Rscr] ^ Register[p.Scr];
		}
		else if (p.command == 19) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] ^ abs(p.Scr);
			else
				Register[p.Rdest] = Register[p.Rscr] ^ abs(Register[p.Scr]);
		}
		else if (p.command == 20) {
			Register[p.Rdest] = -Register[p.Rscr];
		}
		else if (p.command == 21) {
			Register[p.Rdest] = ~abs(Register[p.Rscr]);
		}
		else if (p.command == 22) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] % p.Scr;
			else
				Register[p.Rdest] = Register[p.Rscr] % Register[p.Scr];
		}
		else if (p.command == 23) {
			if (p.type)
				Register[p.Rdest] = Register[p.Rscr] % abs(p.Scr);
			else
				Register[p.Rdest] = Register[p.Rscr] % abs(Register[p.Scr]);
		}
		else if (p.command == 24) {
			if (p.type)
				Register[p.Rdest] = (Register[p.Rscr] == p.Scr);
			else
				Register[p.Rdest] = (Register[p.Rscr] == Register[p.Scr]);
		}
		else if (p.command == 25) {
			if (p.type)
				Register[p.Rdest] = (Register[p.Rscr] >= p.Scr);
			else
				Register[p.Rdest] = (Register[p.Rscr] >= Register[p.Scr]);
		}
		else if (p.command == 26) {
			if (p.type)
				Register[p.Rdest] = (Register[p.Rscr] > p.Scr);
			else
				Register[p.Rdest] = (Register[p.Rscr] > Register[p.Scr]);
		}
		else if (p.command == 27) {
			if (p.type)
				Register[p.Rdest] = (Register[p.Rscr] <= p.Scr);
			else
				Register[p.Rdest] = (Register[p.Rscr] <= Register[p.Scr]);
		}
		else if (p.command == 28) {
			if (p.type)
				Register[p.Rdest] = (Register[p.Rscr] < p.Scr);
			else
				Register[p.Rdest] = (Register[p.Rscr] < Register[p.Scr]);
		}
		else if (p.command == 29) {
			if (p.type)
				Register[p.Rdest] = (Register[p.Rscr] != p.Scr);
			else
				Register[p.Rdest] = (Register[p.Rscr] != Register[p.Scr]);
		}
		else if (p.command == 30) {
			if (p.type == 6) {
				i2 = Register[p.Rscr] + p.address;
			}
			else
				i2 = memory[p.label];
			memo.save(i2, 1, Register[p.Rdest]);
		}
		else if (p.command == 31) {
			if (p.type == 6) {
				i2 = Register[p.Rscr] + p.address;
			}
			else
				i2 = memory[p.label];
			memo.save(i2, 2, Register[p.Rdest]);
		}
		else if (p.command == 32) {
			if (p.type == 6) {
				i2 = Register[p.Rscr] + p.address;
			}
			else
				i2 = memory[p.label];
			memo.save(i2, 4, Register[p.Rdest]);
		}
		else if (p.command == 33) {
			Register[p.Rdest] = Register[p.Rscr];
		}
		else if (p.command == 34) {
			Register[p.Rdest] = Register[32];
		}
		else if (p.command == 35) {
			Register[p.Rdest] = Register[33];
		}
		else if (p.command == 36) {
		}
		else if (p.command == 37) {
			if (Register[2] == 1)
				std::cout << Register[4];
			else if (Register[2] == 4) {
				memo.print(Register[4]);
			}
			else if (Register[2] == 5) {
				std::cin >> i1;
				Register[2] = i1;
			}
			else if (Register[2] == 8) {
				std::cin >> tmp1;
				memo.saveString(tmp1, Register[4], tmp1.length());
				Register[5] = tmp1.length();
			}
			else if (Register[2] == 9) {
				Register[2] = memo.space(Register[4]);
			}
			else if (Register[2] == 10) {
				exit(0);
			}
			else if (Register[2] == 17) {
				exit(Register[4]);
			}
		}
		else if (p.command == 38) {
			currentLine = textnum[p.label];
		}
		else if (p.command == 39) {
			if (p.type) {
				i1 = p.Scr;
			}
			else
				i1 = Register[p.Scr];
			if (Register[p.Rscr] == i1)
				currentLine = textnum[p.label];
		}
		else if (p.command == 40) {
			if (p.type) {
				i1 = p.Scr;
			}
			else
				i1 = Register[p.Scr];
			if (Register[p.Rscr] != i1)
				currentLine = textnum[p.label];
		}
		else if (p.command == 41) {
			if (p.type) {
				i1 = p.Scr;
			}
			else
				i1 = Register[p.Scr];
			if (Register[p.Rscr] >= i1)
				currentLine = textnum[p.label];
		}
		else if (p.command == 42) {
			if (p.type) {
				i1 = p.Scr;
			}
			else
				i1 = Register[p.Scr];
			if (Register[p.Rscr] <= i1)
				currentLine = textnum[p.label];
		}
		else if (p.command == 43) {
			if (p.type) {
				i1 = p.Scr;
			}
			else
				i1 = Register[p.Scr];
			if (Register[p.Rscr] > i1)
				currentLine = textnum[p.label];
		}
		else if (p.command == 44) {
			if (p.type) {
				i1 = p.Scr;
			}
			else
				i1 = Register[p.Scr];
			if (Register[p.Rscr] < i1)
				currentLine = textnum[p.label];
		}
		else if (p.command == 45) {
			if (Register[p.Rscr] == 0)
				currentLine = textnum[p.label];
		}
		else if (p.command == 46) {
			if (Register[p.Rscr] != 0)
				currentLine = textnum[p.label];
		}
		else if (p.command == 47) {
			if (Register[p.Rscr] <= 0)
				currentLine = textnum[p.label];
		}
		else if (p.command == 48) {
			if (Register[p.Rscr] >= 0)
				currentLine = textnum[p.label];
		}
		else if (p.command == 49) {
			if (Register[p.Rscr] > 0)
				currentLine = textnum[p.label];
		}
		else if (p.command == 50) {
			if (Register[p.Rscr] < 0)
				currentLine = textnum[p.label];
		}
		else if (p.command == 51) {
			currentLine = textnum[p.label];
		}
		else if (p.command == 52) {
			currentLine = Register[p.Rscr];
			currentLine--;
		}
		else if (p.command == 53) {
			Register[31] = currentLine + 1;
			currentLine = textnum[p.label];
		}
		else if (p.command == 54) {
			Register[31] = currentLine + 1;
			currentLine = Register[p.Rscr];
			currentLine--;
		}
		else if (p.command == 55) {
			Register[p.Rdest] = p.Scr;
		}
		else if (p.command == 56) {
			if (p.type == 6) {
				i2 = Register[p.Rscr] + p.address;
			}
			else {
				i2 = memory[p.label];
			}
			Register[p.Rdest] = i2;
		}
		else if (p.command == 57) {
			if (p.type == 6) {
				i2 = Register[p.Rscr] + p.address;
			}
			else {
				i2 = memory[p.label];
			}
			Register[p.Rdest] = memo.readByte(i2);
		}
		else if (p.command == 58) {
			if (p.type == 6) {
				i2 = Register[p.Rscr] + p.address;
			}
			else {
				i2 = memory[p.label];
			}
			Register[p.Rdest] = memo.readHalf(i2);
		}
		else if (p.command == 59) {
			if (p.type == 6) {
				i2 = Register[p.Rscr] + p.address;
			}
			else {
				i2 = memory[p.label];
			}
			Register[p.Rdest] = memo.readWord(i2);
		}
		else if (p.command == 0){}
		currentLine++;
	}
	else {
		currentLine++;
	}
}
