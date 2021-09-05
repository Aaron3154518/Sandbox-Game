#include "Point.h"

const std::array<Dim, 2>& getDimList() {
	const static std::array<Dim, 2> dim{ Dim::x, Dim::y };
	return dim;
}

SDL_Point toSDLPoint(const Point<int>& p) { return { p.x, p.y }; }
SDL_FPoint toSDLPoint(const Point<float>& p) { return { p.x, p.y }; }
Point<int> toPoint(const SDL_Point& p) { return { p.x, p.y }; }
Point<float> toPoint(const SDL_FPoint& p) { return { p.x, p.y }; }