#ifndef __LIGHT_H__
#define __LIGHT_H__
#include <cmath>

namespace MLight {
    struct color { 
        unsigned char r, g, b;
        color(int r_=0, int g_=0, int b_=0) noexcept
        : r(r_ < 255 ? r_ : 255), g(g_ < 255 ? g_ : 255), b(b_ < 255 ? b_ : 255) {}
    };

    template<typename T> struct point {
        T x, y;
        point() noexcept {}
        template<typename U>
        point(U x_, U y_) noexcept
        : x((T)x_), y((T)y_) {}
    };
    using pointf = point<float>;
    using pointi = point<int>;
    
    class Light {
    protected:
        color light_color;
    public:
        Light() noexcept: light_color(color(255,255,255)) {}
        Light(color c) noexcept: light_color(c) {}
        virtual float getSDF(pointf p) const = 0;
        color getColor() const { return light_color; }
    };

    class CircleLight: public Light {
        pointf center;
        float r;
    public:
        CircleLight() noexcept { }
        CircleLight(pointf c_, float r_) noexcept
        : center(c_), r(r_) {}
        CircleLight(pointf c_, float r_, color c) noexcept
        : Light(c), center(c_), r(r_) {}

        virtual float getSDF(pointf p) const override {
            float ux = p.x - center.x, uy = p.y - center.y;
            return sqrt(ux * ux + uy * uy) - r;
        }
    };
}

#endif