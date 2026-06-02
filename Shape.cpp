#include "Shape.h"

namespace Chess3D {

    Shape::Shape(float x, float y, float z,
        float sx, float sy, float sz,
        float* diffColor, float* ambiColor, float* specColor)
        : x(x), y(y), z(z), sx(sx), sy(sy), sz(sz),
        diffColor(diffColor), ambiColor(ambiColor), specColor(specColor) {
    }

}