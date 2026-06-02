#include <glut.h>
#include "Scene.h"
#include "ChessBoard.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <vector>

namespace Chess3D {

    Scene::Scene()
        : width(800), height(600),
        isDragging(false),
        lastMouseX(0), lastMouseY(0),
        angleX(45.0f), angleY(0.0f), distZ(-3.0f), moveSpeed(0.1f), moveX(0.0f), moveZ(0.0f)
    {
        board = new ChessBoard(0.0f, 0.0f, 0.0f,
            2.4f, 0.05f, 2.4f,
            GraphUtils::diffGray,
            GraphUtils::ambiGray,
            GraphUtils::specGray);

        shapes.push_back(board);
    }

    Scene::~Scene() {
        for (Shape* shape : shapes) {
            delete shape;
        }
    }

    void Scene::on_paint() {
        glViewport(0, 0, width, height);
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, width / height, 1, 100);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Apply translation and rotation for camera movement
        glTranslatef(moveX, 0.0f, distZ + moveZ);
        glRotatef(angleX, 1.0f, 0.0f, 0.0f);
        glRotatef(angleY, 0.0f, 1.0f, 0.0f);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        float lightPos[] = { 5.0f, 5.0f, 5.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

        for (Shape* shape : shapes) {
            shape->draw();
        }

        for (const auto& p : pieces) {
            drawPiece(p);
        }

        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glutSwapBuffers();
    }

    void Scene::drawPiece(const Piece& p) {
        float cellSize = 2.4f / 8.0f;
        float baseX = -1.2f + p.x * cellSize + cellSize / 2;
        float baseZ = -1.2f + p.y * cellSize + cellSize / 2;

        glPushMatrix();
        glTranslatef(baseX, 0.05f, baseZ);
        glColor3f(0.8f, 0.3f, 0.2f);

        glutSolidSphere(cellSize * 0.3f, 20, 20);
        glPopMatrix();
    }

    void Scene::on_mouse(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                isDragging = true;
                lastMouseX = x;
                lastMouseY = y;
            }
            else {
                isDragging = false;
            }
        }
        else if (button == 3) {
            distZ += 0.5f;
            glutPostRedisplay();
        }
        else if (button == 4) {
            distZ -= 0.5f;
            glutPostRedisplay();
        }
    }

    void Scene::on_motion(int x, int y) {
        if (isDragging) {
            int dx = x - lastMouseX;
            int dy = y - lastMouseY;
            angleY += dx * 0.5f;
            angleX += dy * 0.5f;
            lastMouseX = x;
            lastMouseY = y;
            glutPostRedisplay();
        }
    }

    void Scene::on_size(int w, int h) {
        width = static_cast<float>(w);
        height = static_cast<float>(h == 0 ? 1 : h);
        glutPostRedisplay();
    }

    void Chess3D::Scene::saveMoveToFile(const std::string& from, const std::string& to) {
        std::ofstream file("moves.txt", std::ios::app);
        if (file.is_open()) {
            file << from << "-" << to << "\n";
        }
    }
}