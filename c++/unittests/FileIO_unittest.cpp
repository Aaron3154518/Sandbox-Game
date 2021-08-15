#include <iostream>
#include <vector>
#include <string>
#include <sys/stat.h>

#include "../src/Utils/FileIO.h"

struct A {
	int i = 0, j = 9;

	void print() {
		std::cerr << i << "readValue " << j << std::endl;
	}
};


bool isFile(const std::string& fileName) {
	struct stat info;
	return stat(fileName.c_str(), &info) == 0 &&
		info.st_mode & S_IFREG;
}

void fileTest() {
	auto name = "test.txt";
	if (!isFile(name)) {
		std::cerr << "Write" << std::endl;
		std::vector<std::string> vec = { "Hi", "How", "Are", "You" };
		int i = 9;
		std::string str = "Yo Mama";
		FileWrite file;
		file.open(name);
		file.write(i);
		file.write(str);
		file.write(vec);
		file.commit();
		std::cerr << i << std::endl;
		std::cerr << str << std::endl;
		for (std::string s : vec) { std::cerr << s << ", "; }
		std::cerr << std::endl;
	}

	std::cerr << "Read" << std::endl;
	std::vector<std::string> vec;
	int i;
	std::string str;
	FileRead file;
	file.open(name);
	file.read(i);
	file.read(str);
	file.read(vec);
	file.close();
	std::cerr << i << std::endl;
	std::cerr << str << std::endl;
	for (std::string s : vec) { std::cerr << s << ", "; }
	std::cerr << std::endl;
}

void byteArrayTest() {
	auto name = "test.txt";

	std::cerr << "Write" << std::endl;
	std::vector<std::string> vec = { "Hi", "How", "Are", "You" };
	int i = 9;
	std::string str = "Yo Mama";
	ByteArray file;
	file.write(i);
	file.write(str);
	file.write(vec);
	std::cerr << i << std::endl;
	std::cerr << str << std::endl;
	for (std::string s : vec) { std::cerr << s << ", "; }
	std::cerr << std::endl;

	std::cerr << "Read" << std::endl;
	std::vector<std::string> vec2;
	int i2;
	std::string str2;
	file.read(i2);
	file.read(str2);
	file.read(vec2);
	std::cerr << i2 << std::endl;
	std::cerr << str2 << std::endl;
	for (std::string s : vec2) { std::cerr << s << ", "; }
	std::cerr << std::endl;
}

int main() {
	std::cerr << "\n\u001b[36mFile Test:\u001b[0m\n" << std::endl;
	fileTest();

	std::cerr << "\n\u001b[36mByteArray Test:\u001b[0m\n" << std::endl;
	byteArrayTest();

	return 0;
}
