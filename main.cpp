#include <glut.h>
#include "Scene.h"
#include "ChessLogic.h"
#include <iostream>
#include <mutex>

using Chess3D::Scene;

extern void movePieceConsole();
extern Chess3D::Scene* scene;

Scene* scene;

void on_paint() {
    scene->on_paint();
}

void on_size(int w, int h) {
    scene->on_size(w, h);
}

void on_timer(int v) {
    scene->on_paint();
    glutTimerFunc(25, on_timer, 0);
}

void on_mouse(int button, int state, int x, int y) {
    scene->on_mouse(button, state, x, y);
    glutPostRedisplay();
}

void on_motion(int x, int y) {
    scene->on_motion(x, y);
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    scene = new Scene();

    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("3D Chess Board");

    glutDisplayFunc(on_paint);
    glutReshapeFunc(on_size);
    glutMouseFunc(on_mouse);
    glutMotionFunc(on_motion);
    glutTimerFunc(25, on_timer, 0);

    std::thread consoleThread(movePieceConsole);
    consoleThread.detach();

    std::ofstream clearFile("moves.txt", std::ios::trunc);
    clearFile.close();

    glutMainLoop();

    delete scene;
    return 0;
}
