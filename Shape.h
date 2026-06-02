#ifndef Shape_h
#define Shape_h

#include <glut.h>

namespace Chess3D {

    class Shape {
    protected:
        float x, y, z;
        float sx, sy, sz;
        float* diffColor;
        float* ambiColor;
        float* specColor;

    public:
        Shape(float x, float y, float z,
            float sx, float sy, float sz,
            float* diffColor, float* ambiColor, float* specColor);
        virtual ~Shape() {}

        float getX() const { return x; }
        float getY() const { return y; }
        float getZ() const { return z; }
        float getSX() const { return sx; }
        float getSY() const { return sy; }
        float getSZ() const { return sz; }
        float* getDiffColor() const { return diffColor; }
        float* getAmbiColor() const { return ambiColor; }
        float* getSpecColor() const { return specColor; }

        virtual void draw() = 0;
    };

}

#endif