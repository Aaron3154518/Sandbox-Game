#include "Utils.h"

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
        result << source.substr(loc, pos) << replace;
        loc = pos + search.length();
        pos = source.find(search, loc);
    }
    return result.str();
}

bool isFile(const std::string &fileName) {
    struct stat info;
    return stat(fileName.c_str(), &info) == 0 &&
        info.st_mode & S_IFREG;
}

bool isDir(const std::string& dirName) {
    struct stat info;
    return stat(dirName.c_str(), &info) == 0 &&
        info.st_mode & S_IFDIR;
}

// Functions to put files together
std::string createFile(const std::string& folder,
    const std::string& file, const std::string& ext) {
    return folder + file + ext;
}

// Event
void Event::update(Timestep ts) {
    dt = ts;
    for (EventButton* eb : { &left, &middle, &right }) {
        if (eb->pressed) { eb->duration += ts; }
        eb->clicked = false;
    }
    // Update buttons and mouse
    int dx, dy;
    SDL_GetMouseState(&dx, &dy);
    mouse = { dx, dy };
    mouseDx = 0; mouseDy = 0;
    // Reset event flags
    quit = resize = false;
    // Parse events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            std::cerr << "Quit" << std::endl;
            quit = true;
            break;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                resize = true;
                newW = e.window.data1;
                newH = e.window.data2;
            }
            break;
        default:
            update(e);
            break;
        }
    }
    handled = false;
}

void Event::update(SDL_Event& e) {
    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
    {
        EventButton& eb = getButton(e.button.button);
        eb.pressed = true;
        eb.duration = 0;
        eb.clickPos = mouse;
    }
    break;
    case SDL_MOUSEBUTTONUP:
    {
        EventButton& eb = getButton(e.button.button);
        eb.pressed = false;
        eb.clicked = true;
    }
    break;
    case SDL_MOUSEMOTION:
    {
        mouseDx = e.motion.xrel;
        mouseDy = e.motion.yrel;
        break;
    }
    // TODO: keys
    }
}