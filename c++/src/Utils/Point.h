#ifndef POINT_H
#define POINT_H

#include <array>
#include <iostream>

#include <SDL.h>

enum Dim : uint8_t { x = 0, y };
const std::array<Dim, 2>& getDimList();

template<typename T>
struct Point {
	T x, y;
	T& operator[](Dim d) {
		switch (d) {
			case Dim::x: return x;
			case Dim::y: return y;
		}
		return x;
	}
	const T& operator[](Dim d) const {
		switch (d) {
			case Dim::x: return x;
			case Dim::y: return y;
		}
		return x;
	}

	friend std::ostream& operator<<(std::ostream& os, const Point& p) {
		os << "(";
		bool first = true;
		for (auto dim : getDimList()) {
			if (!first) { os << ", "; }
			os << p[dim];
			first = false;
		}
		os << ")";
		return os;
	}
};

SDL_Point toSDLPoint(const Point<int>& p);
SDL_FPoint toSDLPoint(const Point<float>& p);
Point<int> toPoint(const SDL_Point& p);
Point<float> toPoint(const SDL_FPoint& p);

#endif