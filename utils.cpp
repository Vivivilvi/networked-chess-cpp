#include "utils.h"

namespace GraphUtils {

    float diffWhite[] = { 1.0f, 1.0f, 1.0f };
    float ambiWhite[] = { 0.3f, 0.3f, 0.3f };
    float specWhite[] = { 0.7f, 0.7f, 0.7f };

    float diffGray[] = { 0.2f, 0.2f, 0.2f };
    float ambiGray[] = { 0.1f, 0.1f, 0.1f };
    float specGray[] = { 0.3f, 0.3f, 0.3f };

    void parallelepiped(float length, float width, float height) {
        glBegin(GL_QUADS);
        // Top
        glNormal3f(0, 1, 0);
        glVertex3f(-length / 2, width / 2, -height / 2);
        glVertex3f(length / 2, width / 2, -height / 2);
        glVertex3f(length / 2, width / 2, height / 2);
        glVertex3f(-length / 2, width / 2, height / 2);
        // Bottom
        glNormal3f(0, -1, 0);
        glVertex3f(-length / 2, -width / 2, -height / 2);
        glVertex3f(length / 2, -width / 2, -height / 2);
        glVertex3f(length / 2, -width / 2, height / 2);
        glVertex3f(-length / 2, -width / 2, height / 2);
        // Front
        glNormal3f(0, 0, 1);
        glVertex3f(-length / 2, -width / 2, height / 2);
        glVertex3f(length / 2, -width / 2, height / 2);
        glVertex3f(length / 2, width / 2, height / 2);
        glVertex3f(-length / 2, width / 2, height / 2);
        // Back
        glNormal3f(0, 0, -1);
        glVertex3f(-length / 2, -width / 2, -height / 2);
        glVertex3f(length / 2, -width / 2, -height / 2);
        glVertex3f(length / 2, width / 2, -height / 2);
        glVertex3f(-length / 2, width / 2, -height / 2);
        // Left
        glNormal3f(-1, 0, 0);
        glVertex3f(-length / 2, -width / 2, -height / 2);
        glVertex3f(-length / 2, width / 2, -height / 2);
        glVertex3f(-length / 2, width / 2, height / 2);
        glVertex3f(-length / 2, -width / 2, height / 2);
        // Right
        glNormal3f(1, 0, 0);
        glVertex3f(length / 2, -width / 2, -height / 2);
        glVertex3f(length / 2, width / 2, -height / 2);
        glVertex3f(length / 2, width / 2, height / 2);
        glVertex3f(length / 2, -width / 2, height / 2);
        glEnd();
    }

} // namespace GraphUtils