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

bool Rect::operator==(const Rect& rhs) {
	return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
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

void Rect::setPos(const Rect& r, Rect::PosType mode) {
	setPos(r, mode, mode);
}

void Rect::setPos(const Rect& r, Rect::PosType xMode, Rect::PosType yMode) {
	switch (xMode) {
		case PosType::pCenter: setCX(r.cX()); break;
		case PosType::pBotRight: setX2(r.x2()); break;
		default: x = r.x; break;
	}
	switch (yMode) {
		case PosType::pCenter: setCY(r.cY()); break;
		case PosType::pBotRight: setY2(r.y2()); break;
		default: y = r.y; break;
	}
}

void Rect::setCenter(double nCX, double nCY) {
	x = (int)(nCX - w / 2);
	y = (int)(nCY - h / 2);
}

void Rect::setCenter(const SDL_Point& pos) {
	setCenter(pos.x, pos.y);
}

void Rect::setTopLeft(const SDL_Point& pos) {
	x = pos.x; y = pos.y;
}

void Rect::setBottomRight(const SDL_Point& pos) {
	setX2(pos.x); setY2(pos.y);
}

void Rect::resize(SDL_Point nDim, bool center) {
	resize(nDim.x, nDim.y, center);
}

void Rect::resize(int nW, int nH, bool center) {
#ifdef DEBUG
	std::cerr << "\033[92mResizing Rectangle\033[0m" << std::endl;
	std::cerr << "Before: " << *this << std::endl;
#endif
	double oldCX = cX(), oldCY = cY();
	w = nW;
	h = nH;
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

void Rect::normalize() {
	// Swap x and x2
	if (w < 0) { x += w; w *= -1; }
	// Swap y and y2
	if (h < 0) { y += h; h *= -1; }
}

Rect Rect::getMinRect(SDL_Texture* tex, int maxW, int maxH) {
	int imgW, imgH;
	if (SDL_QueryTexture(tex, NULL, NULL, &imgW, &imgH) != 0) {
		SDL_Log("SDL_QueryTexture failed: %s", SDL_GetError());
		return Rect(0, 0, maxW, maxH);
	}
	Rect r = getMinRect(imgW, imgH, maxW, maxH);
#ifdef DEBUG
	std::cerr << "Got " << r <<
		" from image with size " << imgW << " x " << imgH << std::endl;
#endif
	return r;
}

Rect Rect::getMinRect(int w, int h, int maxW, int maxH) {
	// Save signs and make w/h positive
	int wSign = 1, hSign = 1;
	if (w < 0) { w *= -1; wSign = -1; }
	if (h < 0) { h *= -1; hSign = -1; }

	// maxW/maxH <= 0 or w/h == 0 means ignore that dimensions
	bool noW = maxW * w <= 0, noH = maxH * h <= 0;
	double factor = 1.;
	if (!noW && !noH) {
		factor = std::min((double)maxW / w, (double)maxH / h);
	} else if (noW) {
		factor = (double)maxH / h;
	} else if (noH) {
		factor = (double)maxW / w;
	}
	return Rect(0, 0, (int)(wSign * w * factor), (int)(hSign * h * factor));
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
bool operator !=(const SDL_Point& lhs, const SDL_Point& rhs) {
	return !(lhs == rhs);
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
	lhs.x = std::floor(lhs.x + rhs); lhs.y = std::floor(lhs.y + rhs);
	return lhs;
}
SDL_Point operator +(SDL_Point lhs, const double& rhs) {
	return lhs += rhs;
}
SDL_Point& operator -=(SDL_Point& lhs, const double& rhs) {
	lhs.x = std::floor(lhs.x - rhs); lhs.y = std::floor(lhs.y - rhs);
	return lhs;
}
SDL_Point operator -(SDL_Point lhs, const double& rhs) {
	return lhs -= rhs;
}
SDL_Point& operator *=(SDL_Point& lhs, const double& rhs) {
	lhs.x = std::floor(lhs.x * rhs); lhs.y = std::floor(lhs.y * rhs);
	return lhs;
}
SDL_Point operator *(SDL_Point lhs, const double& rhs) {
	return lhs *= rhs;
}
SDL_Point& operator /=(SDL_Point& lhs, const double& rhs) {
	lhs.x = std::floor(lhs.x / rhs); lhs.y = std::floor(lhs.y / rhs);
	return lhs;
}
SDL_Point operator /(SDL_Point lhs, const double& rhs) {
	return lhs /= rhs;
}
std::ostream& operator <<(std::ostream& os, const SDL_Point& rhs) {
	os << "(" << rhs.x << "," << rhs.y << ")";
	return os;
}