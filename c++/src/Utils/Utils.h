#ifndef UTILS_H
#define UTILS_H

#define FILE_READ_FUNC(type) \
readObject(std::fstream& fs, type& t)
#define FILE_WRITE_FUNC(type) \
writeObject(std::fstream& fs, const type& t)
#define READ_V(var) \
readValue(fs, var)
#define WRITE_V(var) \
writeValue(fs, var)
#define READ_O(obj) \
readObject(fs, obj)
#define WRITE_O(obj) \
writeObject(fs, obj)
#define CHECK_FILE_IS_OPEN(file, msg, code) \
if (!file.is_open()) { \
    std::cerr << msg << std::endl; \
    code \
}
#define CHECK_FILE_OPEN(file, code) \
if (!file) { \
    std::cerr << "File Open Unsuccessful" << std::endl; \
    code \
}
#define CHECK_FILE_IO(file, code) \
if (!file.good()) { \
    std::cerr << "File I/O Operation Unsuccessful" << std::endl; \
    code \
}

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>

#include <SDL.h>

#include "Rect.h"

// TODO: fix
struct Animation;
struct Recipe;

// String functions
std::string lowerCase(std::string str);

bool strToDouble(const std::string& str, double& result);

std::string replaceInString(const std::string& source,
    const std::string& search, const std::string& replace);

// File functions
bool isFile(const std::string& fileName);
bool isDir(const std::string& dirName);

bool validSaveFile(const std::string& fileName);

std::string createFile(const std::string& folder,
    const std::string& file, const std::string& ext);

std::string toFileName(const std::string& displayName);
std::string toDisplayName(const std::string& fileName);

// File I/O functions
// Is value type (primitive or enum)
template<typename T>
constexpr bool isValueType() {
    return std::is_fundamental<T>::value || std::is_enum<T>::value;
}

// For values
template<typename T>
void readValue(std::fstream& fs, T& t) {
    static_assert(isValueType<T>(),
        "Read type must be primitive or enum");
    if (fs.is_open()) {
        char* a = new char[sizeof(T)];
        fs.read(a, sizeof(T));
        std::memcpy(&t, a, sizeof(T));
        delete a;
    }
}

template<typename T>
void writeValue(std::fstream& fs, T& t) {
    static_assert(isValueType<T>(),
        "Write type must be primitive or enum");
    if (fs.is_open()) {
        char* a = new char[sizeof(T)];
        std::memcpy(a, &t, sizeof(T));
        fs.write(a, sizeof(T));
        delete a;
    }
}

// For objects
void FILE_READ_FUNC(std::string);
void FILE_WRITE_FUNC(std::string);

void FILE_READ_FUNC(SDL_Point);
void FILE_WRITE_FUNC(SDL_Point);

template<typename T>
typename std::enable_if_t<isValueType<T>(), void>
FILE_READ_FUNC(std::vector<T>) {
    size_t l;
    READ_V(l);
    t.resize(l);
    for (T& elem : t) { READ_V(elem); }
}
template<typename T>
typename std::enable_if_t<isValueType<T>(), void>
FILE_WRITE_FUNC(std::vector<T>) {
    size_t l = t.size();
    WRITE_V(l);
    for (const T& elem : t) { WRITE_V(elem); }
}
template<typename T>
typename std::enable_if_t<!isValueType<T>(), void>
FILE_READ_FUNC(std::vector<T>) {
    size_t l;
    READ_V(l);
    t.resize(l);
    for (T& elem : t) { READ_O(elem); }
}
template<typename T>
typename std::enable_if_t<!isValueType<T>(), void>
FILE_WRITE_FUNC(std::vector<T>) {
    size_t l = t.size();
    WRITE_V(l);
    for (const T& elem : t) { WRITE_O(elem); }
}

// Point/vector functions
double magnitude(SDL_Point p);
double distance(SDL_Point p1, SDL_Point p2);

// Helper classes
class Timestep {
public:
    Timestep(uint32_t time = 0)
        : mTime(time) {}

    operator uint32_t() const { return mTime; }

    float seconds() const { return mTime / 1000.0f; }
    uint32_t milliseconds() const { return mTime; }

    void add(Timestep t) { mTime += t.mTime; }
private:
    uint32_t mTime;
};

struct EventButton {
    bool pressed = false, clicked = false;
    SDL_Point clickPos = { 0,0 };
    uint32_t duration = 0;
};

struct Event {
    bool handled = false;
    Timestep dt;

    bool quit = false;

    bool resize = false;
    int newW = 0, newH = 0;

    SDL_Point mouse;
    double mouseDx = 0., mouseDy = 0.;
    // < 0 = down, > 0 = up
    Sint32 scroll = 0;

    // Keys currently down
    std::map<SDL_Keycode, Timestep> down;
    // Set of just pressed/released keys
    std::set<SDL_Keycode> pressed, released;
    // Inputted text
    std::string inputText;

    EventButton left, middle, right;

    void updateMouse(int x, int y);
    void update(Timestep ts);
    void update(SDL_Event& e);

    bool keyDown(SDL_Keycode key, Timestep& ts) const;
    bool keyPressed(SDL_Keycode key) const;
    bool keyReleased(SDL_Keycode key) const;

    bool clicked(const EventButton& button) const;
private:
    static constexpr int MAX_CLICK_DIFF = 10;

    EventButton& getButton(int sdlButtonType) {
        switch (sdlButtonType) {
        case SDL_BUTTON_LEFT:
            return left;
            break;
        case SDL_BUTTON_RIGHT:
            return right;
            break;
        case SDL_BUTTON_MIDDLE:
            return middle;
            break;
        default:
            return left;
            break;
        }
    }
};

#endif
