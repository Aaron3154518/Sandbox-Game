#include "FileIO.h"

const std::string TMP_EXT = ".tmp";

// IO
void IO::DATA_READ(std::string) {
	uint16_t l;
	read(l);
	char arr[l];
	readBytes(arr, l);
	t.assign(arr, l);
}
void IO::DATA_WRITE(std::string) {
	uint16_t l = t.length();
	write(l);
	writeBytes(t.data(), l);
}
void IO::DATA_READ(SDL_Point) {
	read(t.x);
	read(t.y);
}
void IO::DATA_WRITE(SDL_Point) {
	write(t.x);
	write(t.y);
}

// FileRead
bool FileRead::open(std::string file) {
	close();
	fs.open(file, std::ios_base::binary);
	if (!fs) {
		std::cerr << "Read File Open Unsuccessful" << std::endl;
		return false;
	}
	return true;
}
bool FileRead::close() {
	if (fs.is_open()) {
		fs.close();
		if (fs.fail()) {
			std::cerr << "File Read Unsuccessful" << std::endl;
			return false;
		}
	}
	return true;
}

void FileRead::readBytes(char* ch, size_t n) {
	if (fs.is_open()) {
		fs.read(ch, n);
		if (fs.fail()) {
			std::cerr << "Read Bytes Operation Failed" << std::endl;
		}
	}
}

ByteArray FileRead::readBytes(size_t n) {
	ByteArray b;
	b.readFile(fs, n);
	return b;
}

char* FileRead::getBytes(size_t n) {
	char* arr = NULL;
	if (fs.is_open()) {
		char* arr = new char(n);
		fs.read(arr, n);
	}
	return arr;
}

// FileWrite
bool FileWrite::open(std::string file) {
	discard();
	fileName = file;
	tmpName = file + TMP_EXT;
	fs.open(tmpName, std::ios_base::binary);
	if (!fs) {
		std::cerr << "Write File Open Unsuccessful" << std::endl;
		return false;
	}
	return true;
}
bool FileWrite::commit() {
	bool good = true;
	if (fs.is_open()) {
		fs.close();
		if (fs.fail()) {
			std::cerr << "Unable to write to temp file" << std::endl;
			good = false;
		} else {
			good = copyTemp();
		}
		std::remove(tmpName.c_str());
	}
	return good;
}
void FileWrite::discard() {
	if (fs.is_open()) {
		fs.close();
		std::remove(tmpName.c_str());
	}
}

bool FileWrite::copyTemp() {
	if (fileName != tmpName && !fs.is_open()) {
		std::ifstream srcFs(tmpName, std::ios_base::binary);
		if (!srcFs) {
			std::cerr << "Unable to open source file for reading" << std::endl;
			return false;
		}
		std::ofstream destFs(fileName, std::ios_base::binary);
		if (!destFs) {
			std::cerr << "Unable to open destination file for writing" << std::endl;
			return false;
		}
		// Only write if not empty
		if (srcFs.peek() != std::ifstream::traits_type::eof()) {
			destFs << srcFs.rdbuf();
		}
		srcFs.close();
		if (srcFs.fail()) {
			std::cerr << "Unable to read from source file" << std::endl;
			return false;
		}
		destFs.close();
		if (destFs.fail()) {
			std::cerr << "Unable to write to destination file" << std::endl;
			return false;
		}
	}
	return true;
}

void FileWrite::writeBytes(const char* ch, size_t n) {
	if (fs.is_open()) {
		fs.write(ch, n);
		if (fs.fail()) {
			std::cerr << "Write Bytes Operation Failed" << std::endl;
		}
	}
}

FileWrite& FileWrite::operator <<(ByteArray& b) {
	b.writeFile(fs);
	return *this;
}

// ByteArray
ByteArray::~ByteArray() { clear(); }

void ByteArray::clear() {
	for (char* arr : bytes) { delete[] arr; }
	bytes.clear();
	lens.clear();
	pos = 0;
}

bool ByteArray::empty() const {
	return bytes.empty() || lens.empty();
}

void ByteArray::readBytes(char* ch, size_t n) {
	if (pos < bytes.size() && pos < lens.size() &&
		lens[pos] == n) {
		memcpy(ch, bytes[pos], lens[pos]);
		++pos;
	}
}

void ByteArray::writeBytes(const char* ch, size_t n) {
	lens.push_back(n);
	bytes.push_back(new char(lens.back()));
	memcpy(bytes.back(), ch, lens.back());
}

void ByteArray::readFile(std::ifstream& fs, uint16_t n) {
	if (fs.is_open()) {
		lens.push_back(n);
		bytes.push_back(new char(lens.back()));
		fs.read(bytes.back(), lens.back());
	}
}

void ByteArray::writeFile(std::ofstream& fs) const {
	if (fs.is_open()) {
		// Bigger vector or iterative write?
		auto it1 = lens.begin();
		auto it2 = bytes.begin();
		while (it1 != lens.end() && it2 != bytes.end()) {
			fs.write(*it2, *it1);
			++it1; ++it2;
		}
	}
}