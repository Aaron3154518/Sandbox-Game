#ifndef POINT_H
#define POINT_H

#include <array>

#include <SDL.h>

enum Dim : uint8_t { x = 0, y };

template<typename T>
struct Point {
	T x, y;
	T& operator [](Dim d) {
		switch (d) {
			case Dim::x: return x;
			case Dim::y: return y;
		}
		return x;
	}
};

const std::array<Dim, 2>& getDimList();

SDL_Point toSDLPoint(const Point<int>& p);
SDL_FPoint toSDLPoint(const Point<float>& p);

#endif