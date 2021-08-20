#ifndef DATA_IO_H
#define DATA_IO_H

#include <iostream>
#include <set>
#include <vector>
#include <list>
#include <cstring>
#include <fstream>
#include <sstream>

#include <SDL.h>

#include "../Definitions.h"

#define ENABLE_IF_VAL(type) \
typename std::enable_if_t<isValueType<type>(), void>
#define ENABLE_IF_OBJ(type) \
typename std::enable_if_t<!isValueType<type>(), void>
#define DATA_READ(type) \
read(type& t)
#define DATA_WRITE(type) \
write(const type& t)

// Is value type (primitive or enum)
template<typename T>
constexpr bool isValueType() {
    return std::is_fundamental<T>::value || std::is_enum<T>::value;
}

class IO {
public:
    IO() = default;
    ~IO() = default;

    // For value types
    template<typename T>
    ENABLE_IF_VAL(T) DATA_READ(T);

    template<typename T>
    ENABLE_IF_VAL(T) DATA_WRITE(T);

    // For object types
    void DATA_READ(std::string);
    void DATA_WRITE(std::string);
    void DATA_READ(SDL_Point);
    void DATA_WRITE(SDL_Point);
    template<typename T>
    void DATA_READ(std::vector<T>);
    template<typename T>
    void DATA_WRITE(std::vector<T>);

protected:
    virtual void readBytes(char* ch, size_t n) {}
    virtual void writeBytes(const char* ch, size_t n) {}
};

template<typename T>
inline ENABLE_IF_VAL(T) IO::DATA_READ(T) {
    readBytes((char*)&t, sizeof(T));
}

template<typename T>
inline ENABLE_IF_VAL(T) IO::DATA_WRITE(T) {
    writeBytes((char*)&t, sizeof(T));
}

template<typename T>
inline void IO::DATA_READ(std::vector<T>) {
    size_t l;
    read(l);
    t.resize(l);
    for (T& tVar : t) { read(tVar); }
}
template<typename T>
inline void IO::DATA_WRITE(std::vector<T>) {
    size_t l = t.size();
    write(l);
    for (const T& tVar : t) { write(tVar); }
}

class FileRead;
class FileWrite;
class ByteArray;

class FileRead : public IO {
public:
    FileRead() = default;
    ~FileRead() = default;
    // Copying returns a new object
    FileRead(const FileRead& other) : FileRead() {}
    FileRead operator =(const FileRead& other) { return FileRead(); }

    bool open(std::string file);
    bool close();
    bool isOpen() const { return fs.is_open(); }

    void readBytes(char* ch, size_t n);

    // File -> ByteArray
    ByteArray readBytes(size_t n);
    // File -> char*
    char* getBytes(size_t n);

private:
    std::ifstream fs;
};

class FileWrite : public IO {
public:
    FileWrite() = default;
    ~FileWrite() { discard(); }
    // Copying returns a new object
    FileWrite(const FileWrite& other) : FileWrite() {}
    FileWrite operator =(const FileWrite& other) { return FileWrite(); }

    bool open(std::string file);
    bool commit();
    void discard();
    bool isOpen() const { return fs.is_open(); }

    void writeBytes(const char* ch, size_t n);

    // ByteArray -> File
    FileWrite& operator <<(ByteArray& b);
private:
    bool copyTemp();

    std::ofstream fs;
    std::string fileName, tmpName;
};

class ByteArray : public IO {
public:
    ByteArray() = default;
    ~ByteArray();

    void clear();
    void reset() { pos = 0; }
    bool empty() const;

    void readBytes(char* ch, size_t n);
    void writeBytes(const char* ch, size_t n);

    // File -> ByteArray
    void readFile(std::ifstream& fs, size_t n);
    // ByteArray -> File
    void writeFile(std::ofstream& fs) const;

private:
    struct Data {
    public:
        ~Data() { delete[] data; }
        char* data;
        size_t len;
    };
    std::vector<Data> bytes;
    size_t pos = 0;
};

#endif