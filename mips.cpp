#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <queue>
#include <vector>
#include "memory.h"
#include "tokenscanner.h"

std::map<std::string, int> keyword;
void initKey() {
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
void initLabel() {
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
		Rdest = Rscr = Scr = address = command = -1;
		type = 0;
		label = "";
	}
	~textGroup() {}
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
std::map<std::string, int> textnum;
std::vector<textGroup> text1;
std::vector<dataGroup> data1;
int Register[35] = { 0 };
bool textStore = false, dataStore = false, mainbegin = false;
int currentLine = 0, datapos = 0, dataLine = 0, textLine = 0;
bool memLock = 0, dataLock, conLock;
char regLock[35] = { 0 };
void processData();
void processText();

std::string debug_map(int x)
{
	for (auto i : Label)
		if (i.second == x)
			return i.first;
	return "";
}

int main(int argc, char **argv) {
	//freopen("test.log", "w", stdout);
	//	freopen("4.in", "r", stdin);
	Register[29] = 4 * 1024 * 1024;
	std::ifstream infile;
	infile.open(argv[1]);
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
					if (str[0] == '_' || str == "") {
						l.command = 0;
						textnum.insert(std::map<std::string, int> ::value_type(str, textLine));
					}
					else {
						l.command = Label[str];
						int i = 0;
						while (scanner.hasMoreToken()) {
							s[i++] = scanner.nextToken();
						}
						if ((l.command >= 9 && l.command <= 13) || (l.command >= 18 && l.command <= 29)) {
							l.Rdest = keyword[s[0]];
							l.Rscr = keyword[s[1]];
							if (s[2] == "") {
								l.type = 1;
							}
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
								l.label = s[2];
							}
							else {
								l.Scr = keyword[s[1]];
								l.label = s[2];
							}
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
								l.type = 1;
								l.Rdest = keyword[s[0]];
								l.Rscr = keyword[s[1]];
							}
							else
								l.Rdest = keyword[s[0]];
						}
						text1.push_back(l);
						textLine++;
					}
				}
				
		}
	}
	infile.close();
	for (; currentLine < dataLine;) {
		processData();
	}
	while (true)
		processText();
	return 0;
}

textGroup p;
std::string Str = "";
struct Num {
	int type = 0;
	int num = 0, num1 = 0, num2 = 0;
	int Command = 0;
	int Rdest = 0;
}N;
struct Mem {
	int ans = 0, ans1 = 0, ans2 = 0, Rdest = 0, pos = 0, type = 0;
	int Command = 0;
}M;
struct Reg {
	int Command = 0;
	int ans = 0, ans1 = 0, ans2 = 0, pos = 0;
	int type = 0;
}R;
void clearN() {
	N.Command = N.num = N.num1 = N.num2 = N.Rdest = N.type = 0;
}
void clearM() {
	M.ans = M.ans1 = M.ans2 = M.Rdest = M.pos = M.type = M.Command = 0;
}
void clearR() {
	R.Command = R.ans = R.ans1 = R.ans2 = R.pos = R.type = 0;
}
void IF() {
	if (dataLock)
	{
		memLock = 0;
		return;
	}
	if (memLock || conLock) {
		p.command = 0;
		return;
	}
	p = text1[Register[34]];
	Register[34] += 1;
}

void ID() {
	N.Command = p.command;
	N.Rdest = p.Rdest;
	N.type = p.type;
	if (N.Command == 0)
		return;
	if (mainbegin) {
		switch (N.Command) {
		case 9: case 12: case 18: case 22: case 24: case 25: case 26: case 27: case 28: case 29:
			if (regLock[p.Rscr] || (!p.type && regLock[p.Scr])) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num1 = Register[p.Rscr];
			if (p.type)
				N.num2 = p.Scr;
			else
				N.num2 = Register[p.Scr];
			p.command = 0;
			break;
		case 10: case 13: case 19: case 23://addu subu
			if (regLock[p.Rscr] || (!p.type && regLock[p.Scr])) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num1 = Register[p.Rscr];
			if (p.type)
				N.num2 = abs(p.Scr);
			else
				N.num2 = abs(Register[p.Scr]);
			p.command = 0;
			break;
		case 11://addiu
			if (regLock[p.Rscr]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num1 = Register[p.Rscr];
			N.num2 = abs(p.Scr);
			p.command = 0;
			break;
		case 14: case 16://mul
			if ((p.type == 2 && regLock[p.Rscr]) || (p.type == 3 && (regLock[p.Rscr] || regLock[p.Scr])) || (p.type == 4 && regLock[p.Rdest]) || (p.type == 5 && (regLock[p.Rdest] || regLock[p.Scr]))) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			if (p.type == 2) {
				regLock[p.Rdest] ++;
				N.num1 = Register[p.Rscr];
				N.num2 = p.Scr;
			}
			else if (p.type == 3) {
				regLock[p.Rdest] ++;
				N.num1 = Register[p.Rscr];
				N.num2 = Register[p.Scr];
			}
			else if (p.type == 4) {
				regLock[32] ++;
				regLock[33] ++;
				N.num1 = Register[p.Rdest];
				N.num2 = p.Scr;
			}
			else {
				regLock[32] ++;
				regLock[33] ++;
				N.num1 = Register[p.Rdest];
				N.num2 = Register[p.Scr];
			}
			p.command = 0;
			break;
		case 15: case 17://mulu
			if ((p.type == 2 && regLock[p.Rscr]) || (p.type == 3 && (regLock[p.Rscr] || regLock[p.Scr])) || (p.type == 4 && regLock[p.Rdest]) || (p.type == 5 && (regLock[p.Rdest] || regLock[p.Scr]))) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			if (p.type == 2) {
				regLock[p.Rdest] ++;
				N.num1 = Register[p.Rscr];
				N.num2 = abs(p.Scr);
			}
			else if (p.type == 3) {
				regLock[p.Rdest] ++;
				N.num1 = Register[p.Rscr];
				N.num2 = abs(Register[p.Scr]);
			}
			else if (p.type == 4) {
				regLock[32] ++;
				regLock[33] ++;
				N.num1 = Register[p.Rdest];
				N.num2 = abs(p.Scr);
			}
			else {
				regLock[32] ++;
				regLock[33] ++;
				N.num1 = Register[p.Rdest];
				N.num2 = abs(Register[p.Scr]);
			}
			p.command = 0;
			break;
		case 20://neg
			if (regLock[p.Rscr]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num = Register[p.Rscr];
			p.command = 0;
			break;
		case 21://negu
			if (regLock[p.Rscr]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num = abs(Register[p.Rscr]);
			p.command = 0;
			break;
		case 30: case 31: case 32://sb
			if (regLock[p.Rdest]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			if (p.type == 6) {
				if (regLock[p.Rscr])
				{
					dataLock = 1;
					N.Command = 0;
					return;
				}
				N.num1 = Register[p.Rscr];
				N.num2 = p.address;
			}
			else
				N.num1 = memory[p.label];
			N.num = Register[p.Rdest];
			p.command = 0;
			break;
		case 33:
			if (regLock[p.Rscr]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num = Register[p.Rscr];
			p.command = 0;
			break;
		case 34:
			if (regLock[32]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num = Register[32];
			p.command = 0;
			break;
		case 35:
			if (regLock[33]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			N.num = Register[33];
			p.command = 0;
			break;
		case 37:
			if (regLock[2] || regLock[4] || (Register[2] == 8 && regLock[5])) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			if (Register[2] == 5 || Register[2] == 9)
				regLock[2]++;
			N.num1 = Register[4];
			N.num2 = Register[2];
			p.command = 0;
			break;
		case 38:
			N.num = textnum[p.label];
			p.command = 0;
			break;
		case 39: case 40: case 41: case 42: case 43: case 44:
			if (regLock[p.Rscr] || (!p.type && regLock[p.Scr])) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			conLock = 1;
			if (p.type) {
				N.num2 = p.Scr;
			}
			else
				N.num2 = Register[p.Scr];
			N.num1 = Register[p.Rscr];
			N.num = textnum[p.label];
			p.command = 0;
			break;
		case 45: case 46: case 47: case 48: case 49: case 50:
			if (regLock[p.Rscr]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			conLock = 1;
			N.num1 = Register[p.Rscr];
			N.num = textnum[p.label];
			p.command = 0;
			break;
		case 51:
			conLock = 1;
			N.num = textnum[p.label];
			p.command = 0;
			break;
		case 52:
			if (regLock[p.Rscr]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			conLock = 1;
			N.num = Register[p.Rscr];
			p.command = 0;
			break;
		case 53:
			conLock = 1;
			regLock[31]++;
			N.num1 = Register[34];
			N.num2 = textnum[p.label];
			p.command = 0;
			break;
		case 54:
			if (regLock[p.Rscr]) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			conLock = 1;
			regLock[31]++;
			N.num1 = Register[34];
			N.num2 = Register[p.Rscr];
			p.command = 0;
			break;
		case 55:
			regLock[p.Rdest] ++;
			N.num = p.Scr;
			p.command = 0;
			break;
		case 56: case 57: case 58: case 59:
			if (regLock[p.Rscr] && p.type == 6) {
				dataLock = 1;
				N.Command = 0;
				return;
			}
			regLock[p.Rdest] ++;
			if (p.type == 6) {
				N.num1 = Register[p.Rscr];
				N.num2 = p.address;
			}
			else {
				N.num = memory[p.label];
			}
			p.command = 0;
			break;
		default:
			break;
		}
	}
}

void EX() {
	M.Command = N.Command;
	M.Rdest = N.Rdest;
	M.type = N.type;
	if (M.Command == 0)
		return;
	else if (M.Command == 8)
		mainbegin = true;
	else if (mainbegin) {
		switch (M.Command) {
		case 9: case 10: case 11:
			M.ans = N.num1 + N.num2;
			break;
		case 12: case 13://sub
			M.ans = N.num1 - N.num2;
			break;
		case 14: case 15://mul
			if (N.type == 2 || N.type == 3)
				M.ans = N.num1 * N.num2;
			else {
				M.ans1 = (N.num1 * N.num2) / 2 ^ 32;
				M.ans2 = (N.num1 * N.num2) % 2 ^ 32;
			}
			break;
		case 16: case 17://div
			if (N.type == 2 || N.type == 3)
				M.ans = N.num1 / N.num2;
			else {
				M.ans1 = (N.num1 % N.num2);
				M.ans2 = (N.num1 / N.num2);
			}
			break;
		case 18: case 19://xor
			M.ans = N.num1 ^ N.num2;
			break;
		case 20://neg
			M.ans = -N.num;
			break;
		case 21://negu
			M.ans = ~N.num;
			break;
		case 22: case 23://rem
			M.ans = N.num1 % N.num2;
			break;
		case 24://seq
			M.ans = N.num1 == N.num2;
			break;
		case 25://sge
			M.ans = N.num1 >= N.num2;
			break;
		case 26://sgt
			M.ans = N.num1 > N.num2;
			break;
		case 27://sle
			M.ans = N.num1 <= N.num2;
			break;
		case 28://slt
			M.ans = N.num1 < N.num2;
			break;
		case 29://sne
			M.ans = N.num1 != N.num2;
			break;
		case 30: case 31: case 32://sb
			if (N.type == 6) {
				M.ans = N.num1 + N.num2;
			}
			else
				M.ans = N.num1;
			M.pos = N.num;
			break;
		case 33: case 34: case 35:
			M.ans = N.num;
			break;
		case 37:
			switch (N.num2) {
			case 1:
				std::cout << N.num1;
				break;
			case 4:
				//memo.print(Register[4]);
				M.pos = N.num1;
				break;
			case 5:
				std::cin >> M.ans;
				//Register[2] = M.ans;
				break;
			case 8:
				std::cin >> Str;
				M.ans = Str.length();
				M.pos = N.num1;
				//memo.saveString(Str, Register[4], tmp1.length());
				//Register[5] = Str.length();
				break;
			case 9:
				M.pos = N.num1;
				//Register[2] = memo.space(Register[4]);
				break;
			case 10:
				exit(0);
				break;
			case 17:
				//system("pause");
				exit(N.num1);
				break;
			}
			break;
		case 38:
			Register[34] = N.num;
			conLock = 0;
			//conLock = 1;
			break;
		case 39:
			if (N.num1 == N.num2) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 40:
			if (N.num1 != N.num2) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 41:
			if (N.num1 >= N.num2)
				Register[34] = N.num;
			conLock = 0;
			break;
		case 42:
			if (N.num1 <= N.num2) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 43:
			if (N.num1 > N.num2) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 44:
			if (N.num1 < N.num2) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 45:
			if (N.num1 == 0) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 46:
			if (N.num1 != 0) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 47:
			if (N.num1 <= 0) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 48:
			if (N.num1 >= 0) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 49:
			if (N.num1 > 0) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 50:
			if (N.num1 < 0) {
				Register[34] = N.num;
				//conLock = 1;
			}
			conLock = 0;
			break;
		case 51: case 52:
			Register[34] = N.num;
			conLock = 0;
			//conLock = 1;
			break;
		case 53: case 54:
			//Register[31] = currentLine + 1;
			M.ans = N.num1;
			Register[34] = N.num2;
			conLock = 0;
			//conLock = 1;
			break;
		case 55:
			//Register[p.Rdest] = p.Scr;
			M.ans = N.num;
			break;
		case 56: case 57: case 58: case 59:
			if (N.type == 6) {
				M.ans = N.num1 + N.num2;
			}
			else {
				M.ans = N.num;
			}
			//Register[p.Rdest] = M.ans;
			break;
		default:
			break;
		}
	}
	clearN();
}
void MA() {
	R.Command = M.Command;
	R.ans = M.ans;
	R.ans1 = M.ans1;
	R.ans2 = M.ans2;
	R.type = M.type;
	R.pos = M.Rdest;
	if (M.Command == 0)
		return;
	if (mainbegin) {
		switch (M.Command) {
		case 30:
			memo.save(M.ans, 1, M.pos);
			memLock = 1;
			break;
		case 31:
			memo.save(M.ans, 2, M.pos);
			memLock = 1;
			break;
		case 32:
			memo.save(M.ans, 4, M.pos);
			memLock = 1;
			break;
		case 37:
			if (Register[2] == 4)
				memo.print(M.pos);
			else if (Register[2] == 8)
				memo.saveString(Str, M.pos, M.ans);
			else if (Register[2] == 9)
				R.ans = memo.space(M.pos);
			memLock = 1;
			break;
		case 57:
			R.ans = memo.readByte(M.ans);
			memLock = 1;
			break;
		case 58:
			R.ans = memo.readHalf(M.ans);
			memLock = 1;
			break;
		case 59:
			R.ans = memo.readWord(M.ans);
			memLock = 1;
			break;
		default:
			break;
		}
	}
	clearM();
}

void WB() {
	if (mainbegin) {
		if (R.Command == 0)
			return;
		if ((R.Command >= 9 && R.Command <= 13) || (R.Command >= 18 && R.Command <= 29) || (R.Command >= 55 && R.Command <= 59) || (R.Command >= 33 && R.Command <= 35)) {
			Register[R.pos] = R.ans;
			regLock[R.pos]--;
		}
		else if (R.Command >= 14 && R.Command <= 17) {
			if (R.type == 4 || R.type == 5) {
				Register[32] = R.ans1;
				Register[33] = R.ans2;
				regLock[32]--;
				regLock[33]--;
			}
			else {
				Register[R.pos] = R.ans;
				regLock[R.pos]--;
			}
		}
		else if (R.Command == 53 || R.Command == 54) {
			Register[31] = R.ans;
			regLock[31]--;
		}
		else if (R.Command == 37) {
			if (Register[2] == 5 || Register[2] == 9) {
				Register[2] = R.ans;
				regLock[2]--;
			}
		}
		memLock = 0;
		dataLock = 0;
		clearR();
	}
	//    std::cout << "cycle" << Register[34] << std::endl;
	//    for (int i = 0; i < 35; ++i)
	//        std::cout << Register[i] << ' ';
	//    std::cout << std::endl;
}

void processData() {
	dataGroup p = data1[currentLine];
	std::string line = p.s[0];
	std::string tmp1;
	int i1;
	short s1;
	char c1;
	switch (p.command) {
	case 1:
		memo.align(p.n[0]);
		datapos = memo.getPos();
		break;
	case 2:
		memo.ascii(p.s[1]);
		datapos = memo.getPos();
		break;
	case 3:
		memo.asciiz(p.s[1]);
		datapos = memo.getPos();
		break;
	case 4:
		for (int i = 0; i < p.num; ++i) {
			memo.byte(p.n[i]);
		}
		datapos = memo.getPos();
		break;
	case 5:
		for (int i = 0; i < p.num; ++i) {
			memo.half(p.n[i]);
		}
		datapos = memo.getPos();
		break;
	case 6:
		for (int i = 0; i < p.num; ++i) {
			memo.word(p.n[i]);
		}
		datapos = memo.getPos();
		break;
	case 7:
		memo.space(p.n[0]);
		datapos = memo.getPos();
		break;
	default:
		memory.insert(std::map<std::string, int> ::value_type(line, datapos));
		break;
	}
	currentLine++;
}

void processText() {
	WB();
	MA();
	EX();
	ID();
	IF();
}
