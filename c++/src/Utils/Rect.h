#ifndef RECT_H
#define RECT_H

#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

//namespace utils {
class Rect : public SDL_Rect {
public:
    Rect();
    Rect(int x, int y, int w, int h);
    Rect(const Rect& other);
    Rect(const SDL_Rect& other);
    ~Rect();

    Rect& operator =(const SDL_Rect& rhs);

    int x2() const { return x + w; }
    int y2() const { return y + h; }
    double cX() const { return x + (double)w / 2; }
    double cY() const { return y + (double)h / 2; }
    SDL_Point topLeft() const { return SDL_Point{ x, y }; }
    SDL_Point topRight() const { return SDL_Point{ x + w, y }; }
    SDL_Point bottomLeft() const { return SDL_Point{ x, y + h }; }
    SDL_Point bottomRight() const { return SDL_Point{ x + w, y + h }; }
    SDL_Point center() const { return SDL_Point{ x + w / 2, y + h / 2 }; }

    void shift(int dX, int dY);
    void setPos(int nX, int nY);
    void setX2(double nX2) { x = (int)(nX2 - w); }
    void setY2(double nY2) { y = (int)(nY2 - h); }
    void setTopLeft(SDL_Point tl) { setPos(tl.x, tl.y); }
    void setBottomRight(SDL_Point br) { setX2(br.x); setY2(br.y); }
    void setCX(double nCX) { x = (int)(nCX - w / 2); }
    void setCY(double nCY) { y = (int)(nCY - h / 2); }
    void setCenter(double nCX, double nCY);
    void setCenter(const SDL_Point& pos);
    void resize(int nW, int nH, bool center);
    void resizeFactor(double factor, bool center);

    static Rect getMinRect(SDL_Texture* tex, int maxW, int maxH);
    static Rect getMinRect(int w, int h, int maxW, int maxH);

    operator bool() { return w != 0 && h != 0; }
    friend std::ostream& operator <<(std::ostream& os, const Rect& rhs);
    friend Rect& operator +=(Rect& lhs, const SDL_Point& rhs);
    friend Rect operator +(Rect lhs, const SDL_Point& rhs);
    friend Rect& operator -=(Rect& lhs, const SDL_Point& rhs);
    friend Rect operator -(Rect lhs, const SDL_Point& rhs);
};
//}

// TODO: Move these!
// For comparing SDL_Points
bool operator ==(const SDL_Point& lhs, const SDL_Point& rhs);
SDL_Point& operator +=(SDL_Point& lhs, const SDL_Point& rhs);
SDL_Point operator +(SDL_Point lhs, const SDL_Point& rhs);
SDL_Point& operator -=(SDL_Point& lhs, const SDL_Point& rhs);
SDL_Point operator -(SDL_Point lhs, const SDL_Point& rhs);
SDL_Point& operator *=(SDL_Point& lhs, const double& rhs);
SDL_Point operator *(SDL_Point lhs, const double& rhs);
SDL_Point& operator /=(SDL_Point& lhs, const double& rhs);
SDL_Point operator /(SDL_Point lhs, const double& rhs);
std::ostream& operator <<(std::ostream& os, const SDL_Point& rhs);

#endif /* Rect */
