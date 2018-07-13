#ifndef MEMORY_h
#define MEMORY_h

#include <iostream>
#include <cmath>
#include <cstring>
#include <string>

class Memory {
private:
	unsigned char *memory;
	int pos;
public:
	Memory() {
		memory = new unsigned char[4 * 1024 * 1024];
		memset(memory, 0, 4 * 1024 * 1024);
		pos = 0;
	}
	~Memory() {
		delete memory;
	}
	void align(int n) {
		int tmp, Pos = pow(2, n);
		tmp = Pos;
		while (tmp < pos)
			tmp += Pos;
		pos = tmp;
	}
	void ascii(std::string str) {
		for (int i = 0; i < str.length(); ++i) {
			memory[pos++] = str[i];
		}
	}
	void asciiz(std::string str) {
		for (int i = 0; i < str.length(); ++i) {
			memory[pos++] = str[i];
		}
		memory[pos++] = '\0';
	}
	inline void byte(int a) {
		memory[pos++] = a;
	}
	inline void half(short a) {
		memory[pos++] = a >> 8;
		memory[pos++] = a;
	}
	inline void word(int a) {
		memory[pos++] = a >> 24;
		memory[pos++] = a >> 16;
		memory[pos++] = a >> 8;
		memory[pos++] = a;
	}
	inline int space(int n) {
		int tmp = pos;
		pos += n;
		return tmp;
	}
	inline int getPos() {
		return pos;
	}
	inline void save(int p, int len, int a) {
		for (int i = 0; i < len; ++i)
			memory[p + i] = a >> (8 * (len - i - 1));
	}
	inline void gotoPos(int p) {
		pos = p;
	}
	inline int readWord(int p) {
		int ans;
		ans = memory[p + 3] + (memory[p + 2] << 8) + (memory[p + 1] << 16) + (memory[p] << 24);
		return ans;
	}
	inline short readHalf(int p) {
		short ans;
		ans = memory[p + 1] + (memory[p] << 8);
		return ans;
	}
	inline int readByte(int p) {
		return memory[p];
	}
	inline int getTop() {
		return pos - 1;
	}
	inline void print(int p) {
		for (int i = p; memory[i] != '\0'; ++i)
			std::cout << memory[i];
	}
	inline void saveString(std::string &str, int p, int len) {
		for (int i = 0; i < len; ++i)
			memory[i + p] = str[i];
	}
};

#endif
