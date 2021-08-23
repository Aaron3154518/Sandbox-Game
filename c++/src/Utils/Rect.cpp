#include "Rect.h"

//namespace utils {
    // Rect
Rect::Rect() {
    x = 0; y = 0;
    w = 0; h = 0;
}
Rect::Rect(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}
Rect::Rect(const SDL_Rect& other) : Rect(other.x, other.y, other.w, other.h) {}
Rect::~Rect() {}

Rect& Rect::operator=(const SDL_Rect& rhs) {
    x = rhs.x;
    y = rhs.y;
    w = rhs.w;
    h = rhs.h;
#ifdef DEBUG
    std::cerr << "Copy Rect: " << rhs << std::endl;
    std::cerr << "    New Rect: " << *this << std::endl;
#endif
    return *this;
}

void Rect::shift(int dX, int dY) {
#ifdef DEBUG
    std::cerr << "\033[92mMoving Rectangle\033[0m" << std::endl;
    std::cerr << "Before: " << *this << std::endl;
#endif
    x += dX;
    y += dY;
#ifdef DEBUG
    std::cerr << "After: " << *this << std::endl;
#endif
}

void Rect::setPos(int nX, int nY) {
#ifdef DEBUG
    std::cerr << "\033[92mMoving Rectangle\033[0m" << std::endl;
    std::cerr << "Before: " << *this << std::endl;
#endif
    x = nX;
    y = nY;
#ifdef DEBUG
    std::cerr << "After: " << *this << std::endl;
#endif
}

void Rect::setCenter(double nCX, double nCY) {
    x = (int)(nCX - w / 2);
    y = (int)(nCY - h / 2);
}

void Rect::setCenter(const SDL_Point& pos) {
    setCenter(pos.x, pos.y);
}

void Rect::resize(int nW, int nH, bool center) {
#ifdef DEBUG
    std::cerr << "\033[92mResizing Rectangle\033[0m" << std::endl;
    std::cerr << "Before: " << *this << std::endl;
#endif
    double oldCX = cX(), oldCY = cY();
    w = abs(nW);
    h = abs(nH);
    if (center) {
        x = (int)(oldCX - w / 2);
        y = (int)(oldCY - h / 2);
    }
#ifdef DEBUG
    std::cerr << "After: " << *this << std::endl;
#endif
}

void Rect::resizeFactor(double factor, bool center) {
    resize((int)(w * factor + .5), (int)(h * factor + .5), center);
}

Rect Rect::getMinRect(SDL_Texture* tex, int maxW, int maxH) {
    int imgW, imgH;
    if (SDL_QueryTexture(tex, NULL, NULL, &imgW, &imgH) != 0) {
        SDL_Log("SDL_QueryTexture failed: %s", SDL_GetError());
    }
    Rect r = getMinRect(imgW, imgH, maxW, maxH);
#ifdef DEBUG
    std::cerr << "Got " << r <<
        " from image with size " << imgW << " x " << imgH << std::endl;
#endif
    return r;
}

Rect Rect::getMinRect(int w, int h, int maxW, int maxH) {
    double factor;
    if (maxW <= 0 && maxH <= 0) {
        factor = 1;
    } else if (maxW <= 0) {
        factor = (double)maxH / h;
    } else if (maxH <= 0) {
        factor = (double)maxW / w;
    } else {
        factor = std::min((double)maxW / w, (double)maxH / h);
    }
    return Rect(0, 0, (int)(w * factor), (int)(h * factor));
}

std::ostream& operator <<(std::ostream& os, const Rect& rhs) {
    os << "(" << rhs.x << "," << rhs.y << ") -> (" << rhs.x2() << "," << rhs.y2()
        << "), size = " << rhs.w << " x " << rhs.h << ", center = (" << rhs.cX()
        << "," << rhs.cY() << ")";
    return os;
}

Rect& operator +=(Rect& lhs, const SDL_Point& rhs) {
    lhs.x += rhs.x; lhs.y += rhs.y;
    return lhs;
}

Rect operator +(Rect lhs, const SDL_Point& rhs) {
    return lhs += rhs;
}

Rect& operator -=(Rect& lhs, const SDL_Point& rhs) {
    lhs.x -= rhs.x; lhs.y -= rhs.y;
    return lhs;
}

Rect operator -(Rect lhs, const SDL_Point& rhs) {
    return lhs -= rhs;
}
//}


bool operator ==(const SDL_Point& lhs, const SDL_Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
// Point with point
SDL_Point& operator +=(SDL_Point& lhs, const SDL_Point& rhs) {
    lhs.x += rhs.x; lhs.y += rhs.y;
    return lhs;
}
SDL_Point operator +(SDL_Point lhs, const SDL_Point& rhs) {
    return lhs += rhs;
}
SDL_Point& operator -=(SDL_Point& lhs, const SDL_Point& rhs) {
    lhs.x -= rhs.x; lhs.y -= rhs.y;
    return lhs;
}
SDL_Point operator -(SDL_Point lhs, const SDL_Point& rhs) {
    return lhs -= rhs;
}
SDL_Point& operator *=(SDL_Point& lhs, const SDL_Point& rhs) {
    lhs.x *= rhs.x; lhs.y *= rhs.y;
    return lhs;
}
SDL_Point operator *(SDL_Point lhs, const SDL_Point& rhs) {
    return lhs *= rhs;
}
SDL_Point& operator /=(SDL_Point& lhs, const SDL_Point& rhs) {
    lhs.x /= rhs.x; lhs.y /= rhs.y;
    return lhs;
}
SDL_Point operator /(SDL_Point lhs, const SDL_Point& rhs) {
    return lhs /= rhs;
}
// Point with double
SDL_Point& operator +=(SDL_Point& lhs, const double& rhs) {
    lhs.x += rhs; lhs.y += rhs;
    return lhs;
}
SDL_Point operator +(SDL_Point lhs, const double& rhs) {
    return lhs += rhs;
}
SDL_Point& operator -=(SDL_Point& lhs, const double& rhs) {
    lhs.x -= rhs; lhs.y -= rhs;
    return lhs;
}
SDL_Point operator -(SDL_Point lhs, const double& rhs) {
    return lhs -= rhs;
}
SDL_Point& operator *=(SDL_Point& lhs, const double& rhs) {
    lhs.x *= rhs; lhs.y *= rhs;
    return lhs;
}
SDL_Point operator *(SDL_Point lhs, const double& rhs) {
    return lhs *= rhs;
}
SDL_Point& operator /=(SDL_Point& lhs, const double& rhs) {
    lhs.x /= rhs; lhs.y /= rhs;
    return lhs;
}
SDL_Point operator /(SDL_Point lhs, const double& rhs) {
    return lhs /= rhs;
}
std::ostream& operator <<(std::ostream& os, const SDL_Point& rhs) {
    os << "(" << rhs.x << "," << rhs.y << ")";
    return os;
}