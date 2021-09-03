#include "Utils.h"

// String functions
std::string lowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return str;
}

bool strToDouble(const std::string& str, double& result) {
    std::istringstream ss(str);
    ss >> result;
    return !ss.fail() && ss.eof() ? true : false;
}

std::string replaceInString(const std::string& source,
    const std::string& search, const std::string& replace) {
    std::ostringstream result;
    size_t pos = source.find(search), loc = 0;
    while (pos != std::string::npos) {
        result << source.substr(loc, pos - loc) << replace;
        loc = pos + search.length();
        pos = source.find(search, loc);
    }
    result << source.substr(loc);
    return result.str();
}


// File functions
bool isFile(const std::string &fileName) {
    struct stat info;
    return stat(fileName.c_str(), &info) == 0 &&
        info.st_mode & S_IFREG;
}

bool isDir(std::string dirName) {
    // Remove any trailing '/' as they interfere with stat()
    size_t pos = dirName.size() - 1;
    while (dirName[pos] == '/' && pos-- > 0) {}
    dirName = dirName.substr(0, pos + 1);

    struct stat info;
    return stat(dirName.c_str(), &info) == 0 &&
        info.st_mode & S_IFDIR;
}

void mkDir(const char* name) {
#ifdef __linux__
    mkdir(name, S_IRWXU);
#else
    _mkdir(name);
#endif
}

int rmDir(const char* name) {
#ifdef __linux__
    return rmdir(name);
#else
    return _rmdir(name);
#endif
}

std::vector<std::string> getDirContents(const std::string& dirName) {
    std::vector<std::string> results;
    DIR* dir = opendir(dirName.c_str());
    struct dirent* en;
    if (dir) {
        while ((en = readdir(dir)) != NULL) {
            std::string name = en->d_name;
            if (name != "." && name != "..") {
                results.push_back(name);
            }
        }
        closedir(dir);
    }
    return results;
}

bool validSaveFile(const std::string& fileName) {
    return std::find_if_not(fileName.begin(), fileName.end(),
        [](const char& ch) { return isalnum(ch) || ch == ' '; }) == fileName.end();
}

std::string toFileName(const std::string& displayName) {
    return replaceInString(displayName, " ", "_");
}
std::string toDisplayName(const std::string& fileName) {
    return replaceInString(fileName, "_", " ");
}

// Point/vector functions
double magnitude(const SDL_Point& p) {
    return sqrt(p.x * p.x + p.y * p.y);
}

double distance(const SDL_Point& p1, const SDL_Point& p2) {
    return magnitude(p1 - p2);
}

bool eq(float f1, float f2, float err) {
    return abs(f1 - f2) <= err;
}
bool eq(double f1, double f2, double err) {
    return abs(f1 - f2) <= err;
}
int toInt(double d, double err) { return (int)(d + err); }

Rect toBlockRect(Rect r) {
    int lbX = r.x / gameVals::BLOCK_W();
    int lbY = r.y / gameVals::BLOCK_W();
    int ubX = (int)std::ceil((double)r.x2() / gameVals::BLOCK_W());
    int ubY = (int)std::ceil((double)r.y2() / gameVals::BLOCK_W());
    // x range = [lbX, ubX), y range = [lbY, ubY)
    return Rect(lbX, lbY, ubX - lbX, ubY - lbY);
}

bool rectsOverlap(Rect r1, Rect r2) {
    Rect res;
    return SDL_IntersectRect(&r1, &r2, &res) == SDL_TRUE;
}

SDL_Point mousePos() {
    SDL_Point result;
    SDL_GetMouseState(&result.x, &result.y);
    return result;
}