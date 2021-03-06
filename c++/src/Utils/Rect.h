#ifndef RECT_H
#define RECT_H

#include <cmath>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

//namespace utils {
class Rect : public SDL_Rect {
public:
    enum PosType : uint8_t {
        pTopLeft = 0, pCenter, pBotRight
    };

    Rect();
    Rect(int x, int y, int w, int h);
    Rect(const SDL_Rect& other);
    ~Rect();

    Rect& operator =(const SDL_Rect& rhs);
    bool operator ==(const Rect& rhs);
    bool operator !=(const Rect& rhs) { return !(*this == rhs); }

    int x2() const { return x + w; }
    int y2() const { return y + h; }
    double cX() const { return x + (double)w / 2; }
    double cY() const { return y + (double)h / 2; }
    SDL_Point topLeft() const { return SDL_Point{ x, y }; }
    SDL_Point topRight() const { return SDL_Point{ x + w, y }; }
    SDL_Point bottomLeft() const { return SDL_Point{ x, y + h }; }
    SDL_Point bottomRight() const { return SDL_Point{ x + w, y + h }; }
    SDL_Point center() const { return SDL_Point{ x + w / 2, y + h / 2 }; }
    SDL_Point dim() const { return SDL_Point{ w,h }; }
    bool empty() const { return w == 0 || h == 0; }
    bool invalid() const { return w < 0 || h < 0; }

    void shift(int dX, int dY);
    void setPos(const Rect& r, PosType mode);
    void setPos(const Rect& r, PosType xMode, PosType yMode);
    void setX2(double nX2) { x = (int)(nX2 - w); }
    void setY2(double nY2) { y = (int)(nY2 - h); }
    void setCX(double nCX) { x = (int)(nCX - w / 2); }
    void setCY(double nCY) { y = (int)(nCY - h / 2); }
    void setCenter(double nCX, double nCY);
    void setCenter(const SDL_Point& pos);
    void setTopLeft(const SDL_Point& pos);
    void setBottomRight(const SDL_Point& pos);
    void resize(SDL_Point nDim, bool center);
    void resize(int nW, int nH, bool center);
    void resizeFactor(double factor, bool center);
    void normalize();

    static Rect getMinRect(SDL_Texture* tex, int maxW, int maxH);
    static Rect getMinRect(int w, int h, int maxW, int maxH);

    operator bool() { return !empty() && !invalid(); }
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
bool operator !=(const SDL_Point& lhs, const SDL_Point& rhs);
// Point with point
SDL_Point& operator +=(SDL_Point& lhs, const SDL_Point& rhs);
SDL_Point operator +(SDL_Point lhs, const SDL_Point& rhs);
SDL_Point& operator -=(SDL_Point& lhs, const SDL_Point& rhs);
SDL_Point operator -(SDL_Point lhs, const SDL_Point& rhs);
SDL_Point& operator *=(SDL_Point& lhs, const SDL_Point& rhs);
SDL_Point operator *(SDL_Point lhs, const SDL_Point& rhs);
SDL_Point& operator /=(SDL_Point& lhs, const SDL_Point& rhs);
SDL_Point operator /(SDL_Point lhs, const SDL_Point& rhs);
// Point with double
SDL_Point& operator +=(SDL_Point& lhs, const double& rhs);
SDL_Point operator +(SDL_Point lhs, const double& rhs);
SDL_Point& operator -=(SDL_Point& lhs, const double& rhs);
SDL_Point operator -(SDL_Point lhs, const double& rhs);
SDL_Point& operator *=(SDL_Point& lhs, const double& rhs);
SDL_Point operator *(SDL_Point lhs, const double& rhs);
SDL_Point& operator /=(SDL_Point& lhs, const double& rhs);
SDL_Point operator /(SDL_Point lhs, const double& rhs);
std::ostream& operator <<(std::ostream& os, const SDL_Point& rhs);

#endif /* Rect */
