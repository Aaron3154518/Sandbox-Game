#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <sys/stat.h>
#include <type_traits>
#include <vector>
#include <unistd.h>

#include <dirent.h>
#include <SDL.h>

#include "Rect.h"
#include "../Definitions.h"

// TODO: fix
struct Animation;
struct Recipe;

// String functions
std::string lowerCase(std::string str);

bool strToDouble(const std::string& str, double& result);

std::string replaceInString(const std::string& source,
    const std::string& search, const std::string& replace);

std::string randomString(size_t len);

// File functions
bool isFile(const std::string& fileName);
bool isDir(std::string dirName);

void mkDir(const char* name);
int rmDir(const char* name);

std::vector<std::string> getDirContents(const std::string& dirName);

bool validSaveFile(const std::string& fileName);

std::string toFileName(const std::string& displayName);
std::string toDisplayName(const std::string& fileName);

// Point/vector functions
double magnitude(const SDL_Point& p);
double distance(const SDL_Point& p1, const SDL_Point& p2);

bool eq(float f1, float f2, float err = gameVals::FLOAT_ERR());
bool eq(double f1, double f2, double err = gameVals::DOUBLE_ERR());
int toInt(double d, double err = gameVals::DOUBLE_ERR());

Rect toBlockRect(Rect r);

bool rectsOverlap(Rect r1, Rect r2);

SDL_Point mousePos();

// Bitwise functions
bool any8(const uint8_t& src, const uint8_t& comp);

bool all8(const uint8_t& src, const uint8_t& comp);

#endif
