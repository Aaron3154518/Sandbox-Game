#include "Point.h"

const std::array<Dim, 2>& getDimList() {
	const static std::array<Dim, 2> dim{ Dim::x, Dim::y };
	return dim;
}

SDL_Point toSDLPoint(const Point<int>& p) { return { p.x, p.y }; }
SDL_FPoint toSDLPoint(const Point<float>& p) { return { p.x, p.y }; }