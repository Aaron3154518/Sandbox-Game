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
	bytes.clear();
	reset();
}

bool ByteArray::empty() const {
	return bytes.empty();
}

size_t ByteArray::size() const {
	size_t sum = 0;
	for (const Data& data : bytes) { sum += data.len; }
	return sum;
}
void ByteArray::readBytes(char* ch, size_t n) {
	if (pos < bytes.size() && bytes[pos].len == n) {
		memcpy(ch, bytes[pos].data, bytes[pos].len);
		++pos;
	}
}

void ByteArray::writeBytes(const char* ch, size_t n) {
	bytes.push_back(Data{ new char(n), n });
	memcpy(bytes.back().data, ch, bytes.back().len);
}

void ByteArray::read(IO& io) {
	clear();
	size_t len;
	io.read(len);
	//memcpy(&len, ch, sizeof(size_t));
	//ch += sizeof(size_t);
	bytes.resize(len);
	for (Data& data : bytes) {
		io.read(data.len);
		//memcpy(&data.len, ch, sizeof(size_t));
		//ch += sizeof(size_t);
		data.data = new char[data.len];
		io.readBytes(data.data, data.len);
		//memcpy(data.data, ch, data.len);
		//ch += data.len;
	}
}

void ByteArray::write(IO& io) const {
	size_t s = size();
	io.write(s);
	//n = s + sizeof(size_t) * bytes.size() + sizeof(size_t);
	//char* result = new char[n];
	//char* pos = result;
	//memcpy(pos, &s, sizeof(size_t));
	//pos += sizeof(size_t);
	for (const Data& data : bytes) {
		io.write(data.len);
		//memcpy(pos, &data.len, sizeof(size_t));
		//pos += sizeof(size_t);
		io.writeBytes(data.data, data.len);
		//memcpy(pos, data.data, data.len);
		//pos += data.len;
	}
}
 
void ByteArray::readFile(std::ifstream& fs, size_t n) {
	if (fs.is_open()) {
		bytes.push_back(Data{ new char(n), n });
		fs.read(bytes.back().data, bytes.back().len);
	}
}

void ByteArray::writeFile(std::ofstream& fs) const {
	if (fs.is_open()) {
		// Bigger vector or iterative write? <- what?
		for (auto it = bytes.begin(); it != bytes.end(); it++) {
			fs.write(it->data, it->len);
		}
	}
}

bool ByteArray::operator ==(const ByteArray& other) const {
	if (bytes.size() != other.bytes.size()) { return false; }
	auto it1 = bytes.begin(), it2 = other.bytes.begin();
	while (it1 != bytes.end() && it2 != other.bytes.end()) {
		if (it1->len != it2->len) { return false; }
		if (!memcmp(it1->data, it2->data, it1->len)) { return false; }
		++it1; ++it2;
	}
	return true;
}