#ifndef utils_h
#define utils_h

#include <glut.h>

namespace GraphUtils {
	extern float diffGray[];
	extern float ambiGray[];
	extern float specGray[];

	const float shininess = 64;

	void parallelepiped(float length, float width, float height);

	extern GLfloat diffWhite[4];
	extern GLfloat diffBlack[4];
	extern GLfloat diffRed[4];

}
#endif