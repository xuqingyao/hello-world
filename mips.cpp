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
		type = 0;//type == 1 —— number | type == 0 —— Register | type == 2 —— mul/div (3, n) | type == 3 —— mul/div (3, r) | 
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
void processData();
void processText();

int main(int argc, char* argv[]) {
	Register[29] = 4 * 1024 * 1024;
	std::ifstream infile;
	infile.open("2.s");
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
	infile.close();
	for (; currentLine < dataLine;) {
		processData();
	}
	for (currentLine = 0; currentLine < textLine; ) {
		processText();
	}
	getchar();
	system("pause");
	return 0;
}

textGroup p;
//int Command = 0, ans = 0, ans1 = 0, ans2 = 0, result = 0;
std::string Str = "";
struct Mem {
	textGroup t;
	int ans = 0, ans1 = 0, ans2 = 0;
	int Command = 0;
}M, R;
void IF() {
	p = text1[currentLine];
}

void ID() {
	M.Command = p.command;
}

/*struct REG {
	textGroup T;
	int ans, ans1, ans2;
	int Command;
}R;*/
void EX() {
	M.t = p;
	if (M.Command == 8)
		mainbegin = true;
	else if (mainbegin) {
		switch (M.Command) {
		case 9://add
			if (p.type)
				//Register[p.Rdest] = M.ans;
				M.ans = Register[p.Rscr] + p.Scr;
			else
				M.ans = Register[p.Rscr] + Register[p.Scr];
			break;
		case 10://addu
			if (p.type)
				M.ans = Register[p.Rscr] + abs(p.Scr);
			else
				M.ans = Register[p.Rscr] + abs(Register[p.Scr]);
			break;
		case 11://addiu
			M.ans = Register[p.Rscr] + abs(p.Scr);
			break;
		case 12://sub
			if (p.type)
				M.ans = Register[p.Rscr] - p.Scr;
			else
				M.ans = Register[p.Rscr] - Register[p.Scr];
			break;
		case 13://subu
			if (p.type)
				M.ans = Register[p.Rscr] - abs(p.Scr);
			else
				M.ans = Register[p.Rscr] - abs(Register[p.Scr]);
			break;
		case 14://mul
			if (p.type == 2)
				M.ans = Register[p.Rscr] * p.Scr;
			else if (p.type == 3)
				M.ans = Register[p.Rscr] * Register[p.Scr];
			else if (p.type == 4) {
				M.ans1 = (Register[p.Rdest] * p.Scr) / 2 ^ 32;
				M.ans2 = (Register[p.Rdest] * p.Scr) % 2 ^ 32;
			}
			else {
				M.ans1 = (Register[p.Rdest] * Register[p.Scr]) / 2 ^ 32;
				M.ans2 = (Register[p.Rdest] * Register[p.Scr]) % 2 ^ 32;
			}
			break;
		case 15://mulu
			if (p.type == 2)
				M.ans = Register[p.Rscr] * abs(p.Scr);
			else if (p.type == 3)
				M.ans = Register[p.Rscr] * abs(Register[p.Scr]);
			else if (p.type == 4) {
				M.ans1 = (Register[p.Rdest] * abs(p.Scr)) / 2 ^ 32;
				M.ans2 = (Register[p.Rdest] * abs(p.Scr)) % 2 ^ 32;
			}
			else {
				M.ans1 = (Register[p.Rdest] * abs(Register[p.Scr])) / 2 ^ 32;
				M.ans2 = (Register[p.Rdest] * abs(Register[p.Scr])) % 2 ^ 32;
			}
			break;
		case 16://div
			if (p.type == 2)
				M.ans = Register[p.Rscr] / p.Scr;
			else if (p.type == 3)
				M.ans = Register[p.Rscr] / Register[p.Scr];
			else if (p.type == 4) {
				M.ans1 = Register[p.Rdest] % p.Scr;
				M.ans2 = Register[p.Rdest] / p.Scr;
			}
			else {
				M.ans1 = Register[p.Rdest] % Register[p.Scr];
				M.ans2 = Register[p.Rdest] / Register[p.Scr];
			}
			break;
		case 17://divu
			if (p.type == 2)
				M.ans = Register[p.Rscr] / abs(p.Scr);
			else if (p.type == 3)
				M.ans = Register[p.Rscr] / abs(Register[p.Scr]);
			else if (p.type == 4) {
				M.ans1 = Register[p.Rdest] % abs(p.Scr);
				M.ans2 = Register[p.Rdest] / abs(p.Scr);
			}
			else {
				M.ans1 = Register[p.Rdest] % abs(Register[p.Scr]);
				M.ans2 = Register[p.Rdest] / abs(Register[p.Scr]);
			}
			break;
		case 18://xor
			if (p.type)
				M.ans = Register[p.Rscr] ^ p.Scr;
			else
				M.ans = Register[p.Rscr] ^ Register[p.Scr];
			break;
		case 19://xoru
			if (p.type)
				M.ans = Register[p.Rscr] ^ abs(p.Scr);
			else
				M.ans = Register[p.Rscr] ^ abs(Register[p.Scr]);
			break;
		case 20://neg
			M.ans = -Register[p.Rscr];
			break;
		case 21://negu
			M.ans = ~abs(Register[p.Rscr]);
			break;
		case 22://rem
			if (p.type)
				M.ans = Register[p.Rscr] % p.Scr;
			else
				M.ans = Register[p.Rscr] % Register[p.Scr];
			break;
		case 23://remu
			if (p.type)
				M.ans = Register[p.Rscr] % abs(p.Scr);
			else
				M.ans = Register[p.Rscr] % abs(Register[p.Scr]);
			break;
		case 24://seq
			if (p.type)
				M.ans = (Register[p.Rscr] == p.Scr);
			else
				M.ans = (Register[p.Rscr] == Register[p.Scr]);
			break;
		case 25://sge
			if (p.type)
				M.ans = (Register[p.Rscr] >= p.Scr);
			else
				M.ans = (Register[p.Rscr] >= Register[p.Scr]);
			break;
		case 26://sgt
			if (p.type)
				M.ans = (Register[p.Rscr] > p.Scr);
			else
				M.ans = (Register[p.Rscr] > Register[p.Scr]);
			break;
		case 27://sle
			if (p.type)
				M.ans = (Register[p.Rscr] <= p.Scr);
			else
				M.ans = (Register[p.Rscr] <= Register[p.Scr]);
			break;
		case 28://slt
			if (p.type)
				M.ans = (Register[p.Rscr] < p.Scr);
			else
				M.ans = (Register[p.Rscr] < Register[p.Scr]);
			break;
		case 29://sne
			if (p.type)
				M.ans = (Register[p.Rscr] != p.Scr);
			else
				M.ans = (Register[p.Rscr] != Register[p.Scr]);
			break;
		case 30://sb
			if (p.type == 6) {
				M.ans = Register[p.Rscr] + p.address;
			}
			else
				M.ans = memory[p.label];
			//memo.save(M.ans, 1, Register[p.Rdest]);
			break;
		case 31://sh
			if (p.type == 6) {
				M.ans = Register[p.Rscr] + p.address;
			}
			else
				M.ans = memory[p.label];
			//memo.save(M.ans, 2, Register[p.Rdest]);
			break;
		case 32://sw
			if (p.type == 6) {
				M.ans = Register[p.Rscr] + p.address;
			}
			else
				M.ans = memory[p.label];
			//memo.save(M.ans, 4, Register[p.Rdest]);
			break;
		case 33:
			M.ans = Register[p.Rscr];
			break;
		case 34:
			M.ans = Register[32];
			break;
		case 35:
			M.ans = Register[33];
			break;
		case 37:
			switch (Register[2]) {
			case 1:
				std::cout << Register[4];
				break;
			case 4:
				//memo.print(Register[4]);
				break;
			case 5:
				std::cin >> M.ans;
				//Register[2] = M.ans;
				break;
			case 8:
				std::cin >> Str;
				M.ans = Str.length();
				//memo.saveString(Str, Register[4], tmp1.length());
				//Register[5] = Str.length();
				break;
			case 9:
				//Register[2] = memo.space(Register[4]);
				break;
			case 10:
				exit(0);
				break;
			case 17:
				//system("pause");
				exit(Register[4]);
				break;
			}
			break;
		case 38:
			currentLine = textnum[p.label];
			break;
		case 39:
			if (p.type) {
				M.ans = p.Scr;
			}
			else
				M.ans = Register[p.Scr];
			if (Register[p.Rscr] == M.ans)
				currentLine = textnum[p.label];
			break;
		case 40:
			if (p.type) {
				M.ans = p.Scr;
			}
			else
				M.ans = Register[p.Scr];
			if (Register[p.Rscr] != M.ans)
				currentLine = textnum[p.label];
			break;
		case 41:
			if (p.type) {
				M.ans = p.Scr;
			}
			else
				M.ans = Register[p.Scr];
			if (Register[p.Rscr] >= M.ans)
				currentLine = textnum[p.label];
			break;
		case 42:
			if (p.type) {
				M.ans = p.Scr;
			}
			else
				M.ans = Register[p.Scr];
			if (Register[p.Rscr] <= M.ans)
				currentLine = textnum[p.label];
			break;
		case 43:
			if (p.type) {
				M.ans = p.Scr;
			}
			else
				M.ans = Register[p.Scr];
			if (Register[p.Rscr] > M.ans)
				currentLine = textnum[p.label];
			break;
		case 44:
			if (p.type) {
				M.ans = p.Scr;
			}
			else
				M.ans = Register[p.Scr];
			if (Register[p.Rscr] < M.ans)
				currentLine = textnum[p.label];
			break;
		case 45:
			if (Register[p.Rscr] == 0)
				currentLine = textnum[p.label];
			break;
		case 46:
			if (Register[p.Rscr] != 0)
				currentLine = textnum[p.label];
			break;
		case 47:
			if (Register[p.Rscr] <= 0)
				currentLine = textnum[p.label];
			break;
		case 48:
			if (Register[p.Rscr] >= 0)
				currentLine = textnum[p.label];
			break;
		case 49:
			if (Register[p.Rscr] > 0)
				currentLine = textnum[p.label];
			break;
		case 50:
			if (Register[p.Rscr] < 0)
				currentLine = textnum[p.label];
			break;
		case 51:
			currentLine = textnum[p.label];
			break;
		case 52:
			currentLine = Register[p.Rscr];
			currentLine--;
			break;
		case 53:
			//Register[31] = currentLine + 1;
			M.ans = currentLine + 1;
			currentLine = textnum[p.label];
			break;
		case 54:
			//Register[31] = currentLine + 1;
			M.ans = currentLine + 1;
			currentLine = Register[p.Rscr];
			currentLine--;
			break;
		case 55:
			//Register[p.Rdest] = p.Scr;
			M.ans = p.Scr;
			break;
		case 56:
			if (p.type == 6) {
				M.ans = Register[p.Rscr] + p.address;
			}
			else {
				M.ans = memory[p.label];
			}
			//Register[p.Rdest] = M.ans;
			break;
		case 57:
			if (p.type == 6) {
				M.ans = Register[p.Rscr] + p.address;
			}
			else {
				M.ans = memory[p.label];
			}
			//Register[p.Rdest] = memo.readByte(M.ans);
			break;
		case 58:
			if (p.type == 6) {
				M.ans = Register[p.Rscr] + p.address;
			}
			else {
				M.ans = memory[p.label];
			}
			//Register[p.Rdest] = memo.readHalf(M.ans);
			break;
		case 59:
			if (p.type == 6) {
				M.ans = Register[p.Rscr] + p.address;
			}
			else {
				M.ans = memory[p.label];
			}
			//Register[p.Rdest] = memo.readWord(M.ans);
			break;
		default:
			break;
		}
	}
}
void MA() {
	R = M;
	if (mainbegin) {
		switch (M.Command) {
		case 30:
			memo.save(M.ans, 1, Register[M.t.Rdest]);
			break;
		case 31:
			memo.save(M.ans, 2, Register[M.t.Rdest]);
			break;
		case 32:
			memo.save(M.ans, 4, Register[M.t.Rdest]);
			break;
		case 37:
			if (Register[2] == 4)
				memo.print(Register[4]);
			else if (Register[2] == 8)
				memo.saveString(Str, Register[4], M.ans);
			else if (Register[2] == 9)
				R.ans = memo.space(Register[4]);
			break;
		case 57:
			R.ans = memo.readByte(M.ans);
			break;
		case 58:
			R.ans = memo.readHalf(M.ans);
			break;
		case 59:
			R.ans = memo.readWord(M.ans);
			break;
		default:
			break;
		}
	}
}

void WB() {
	if (mainbegin) {
		if ((R.Command >= 9 && R.Command <= 13) || (R.Command >= 18 && R.Command <= 29) || (R.Command >= 55 && R.Command <= 59) || (R.Command >= 33 && R.Command <= 35))
			Register[R.t.Rdest] = R.ans;
		else if (R.Command >= 14 && R.Command <= 17) {
			if (R.t.type == 4 || R.t.type == 5) {
				Register[32] = R.ans1;
				Register[33] = R.ans2;
			}
			else
				Register[R.t.Rdest] = R.ans;
		}
		else if (R.Command == 53 || R.Command == 54)
			Register[31] = R.ans;
		/*else if (R.Command >= 57 && R.Command <= 59)
			Register[R.t.Rdest] = R.ans;*/
		else if (R.Command == 37) {
			if (Register[2] == 5)
				Register[2] = R.ans;
			else if (Register[2] == 8)
				//memo.saveString(Str, Register[4], Str.length());
				Register[5] = R.ans;
			else if (Register[2] == 9)
				Register[2] = R.ans;
		}
	}
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
	//M.Command = ans = ans1 = ans2 = result = 0;
	M.Command = M.ans = M.ans1 = M.ans2 = 0;
	Str = "";
	IF();
	ID();
	EX();
	MA();
	WB();
	currentLine++;
}
