#include <glut.h>
#include <cctype>
#include "Piece.h"
 
namespace Chess3D {

    void drawPawn(float sphereRadius = 0.25f) {
        glPushMatrix();
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.5f, 0.9f, 10, 10);              // конус — тело
        glTranslatef(0.0f, 0.0f, 0.9f);                  // подняться наверх
        glutSolidSphere(sphereRadius, 20, 20);           // сфера — голова
        glPopMatrix();
    }

    void drawRook(float height = 0.7f) {

        glPushMatrix();
        glRotatef(90.0f, -1.0f, 0.0f, 0.0f); 
        drawSolidCylinder(0.5f, height, 20, 1);           // цилиндр — башня
        glPopMatrix();
    }

    void drawKnight() {
        glPushMatrix();

        // вертикальный параллелепипед
        glPushMatrix();
        glTranslatef(0.0f, 0.1f, 0.0f);
        glScalef(0.5f, 0.9f, 0.5f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // горизонтальный параллелепипед (как "Г")
        glPushMatrix();
        glTranslatef(0.05f, 0.5f, -0.15f);
        glScalef(0.5f, 0.5f, 0.9f);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPopMatrix();
    }

    void drawBishop() {
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glutSolidCone(0.4f, 1.4f, 20, 20);              // конус — тело
        glPopMatrix();
    }

    void drawKing(float scale = 0.8f) {
        glPushMatrix();
        glRotatef(0.0f, 1.0f, 0.0f, 0.0f);

        // половинка пирамиды — строим вручную
        glBegin(GL_TRIANGLES);

        // Нижняя часть пирамиды (основной квадрат)
        glVertex3f(0, 1.5, 0);  // Верхушка
        glVertex3f(-scale, 0, scale);  // Левый передний угол
        glVertex3f(scale, 0, scale);  // Правый передний угол

        glVertex3f(0, 1.5, 0);  // Верхушка
        glVertex3f(scale, 0, scale);  // Правый передний угол
        glVertex3f(scale, 0, -scale);  // Правый задний угол

        glVertex3f(0, 1.5, 0);  // Верхушка
        glVertex3f(scale, 0, -scale);  // Правый задний угол
        glVertex3f(-scale, 0, -scale);  // Левый задний угол

        glVertex3f(0, 1.5, 0);  // Верхушка
        glVertex3f(-scale, 0, -scale);  // Левый задний угол
        glVertex3f(-scale, 0, scale);  // Левый передний угол

        // Верхняя часть пирамиды (чтобы замкнуть фигуру)
        glVertex3f(-scale, 0, scale);  // Левый передний угол
        glVertex3f(scale, 0, scale);  // Правый передний угол
        glVertex3f(scale, 0, -scale);  // Правый задний угол

        glVertex3f(-scale, 0, scale);  // Левый передний угол
        glVertex3f(scale, 0, -scale);  // Правый задний угол
        glVertex3f(-scale, 0, -scale);  // Левый задний угол

        glEnd();

        glPopMatrix();
    }


    void drawQueen() {
        glPushMatrix();
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        // Основание — круглый диск (плоский)
        drawSolidCylinder(0.5f, 0.5f, 20, 1);
        glTranslatef(0.0f, 0.0f, 0.0f);

        // Параллелепипеды "+" на вершине
        // Вертикальный
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.4f);
        glScalef(0.9f, 0.9f, 0.5f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Горизонтальный
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.4f);
        glScalef(0.5f, 0.9f, 0.9f);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPopMatrix();
    }

    void drawSolidCylinder(GLfloat radius, GLfloat height, GLint slices, GLint stacks)
    {
        GLUquadricObj* quadric = gluNewQuadric();  // Oбъект квадрики для цилиндра

        gluQuadricDrawStyle(quadric, GLU_FILL);  // Настроить стиль рисования
        gluQuadricNormals(quadric, GLU_SMOOTH);  // Настроить нормали для сглаживания
        gluCylinder(quadric, radius, radius, height, slices, stacks);  // Отрисовываем сам цилиндр

        // Если нужно, можно также добавить крышку цилиндра сверху и снизу (если нужно)
        gluDisk(quadric, 0.0, radius, slices, 1);  // Нижняя крышка цилиндра
        glTranslatef(0.0f, 0.0f, height);  // Перемещаемся наверх
        gluDisk(quadric, 0.0, radius, slices, 1);  // Верхняя крышка цилиндра

        gluDeleteQuadric(quadric);  // Освобождаем ресурсы
    }


    void drawPiece(char piece) {
        glDisable(GL_LIGHTING);
        if (isupper(piece))
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.2f, 0.2f, 0.2f);

        switch (tolower(piece)) {
        case 'p': drawPawn(); break;
        case 'r': drawRook(); break;
        case 'n': drawKnight(); break;
        case 'b': drawBishop(); break;
        case 'q': drawQueen(); break;
        case 'k': drawKing(); break;
        default: glutSolidSphere(0.3f, 10, 10); break;
        }

        glEnable(GL_LIGHTING);
    }

}
