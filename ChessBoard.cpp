#include <glut.h>
#include "ChessBoard.h"
#include "Piece.h"
#include <cmath>

namespace Chess3D {
    ChessBoard::ChessBoard(float x, float y, float z,
        float sx, float sy, float sz,
        float* diffColor, float* ambiColor, float* specColor)
        : Shape(x, y, z, sx, sy, sz, diffColor, ambiColor, specColor)
    {
        initBoard();
    }


    void ChessBoard::initBoard() {
        char temp[8][8] = {
            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
        };
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                board[i][j] = temp[i][j];
    }

    void ChessBoard::drawBoard() {
        float tileSize = sx / 8.0f;
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                glPushMatrix();
                float tx = (col - 3.5f) * tileSize;
                float tz = (row - 3.5f) * tileSize;
                glTranslatef(x + tx, y, z + tz);

                if ((row + col) % 2 == 0)
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, GraphUtils::diffWhite);
                else
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, GraphUtils::diffGray);

                GraphUtils::parallelepiped(tileSize, 0.05f, tileSize);
                glPopMatrix();
            }
        }
    }

    void ChessBoard::drawSelection() {
        if (!pieceSelected) return;

        float tileSize = sx / 8.0f;
        glPushMatrix();
        float tx = (selectedCol - 3.5f) * tileSize;
        float tz = (selectedRow - 3.5f) * tileSize;
        glTranslatef(x + tx, y + 0.1f, z + tz);

        glDisable(GL_LIGHTING);
        glColor3f(0, 1, 0);
        glutWireCube(tileSize * 0.9f);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }

    void Chess3D::ChessBoard::drawPieces() {
        float tileSize = sx / 8.0f;

        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                char piece = board[row][col];
                if (piece == ' ') continue;

                float tx = (col - 3.5f) * tileSize;
                float tz = (row - 3.5f) * tileSize;

                glPushMatrix();
                glTranslatef(x + tx, y + 0.05f, z + tz);
                glScalef(tileSize * 0.6f, tileSize * 0.6f, tileSize * 0.6f);

                drawPiece(piece);
                glPopMatrix();
            }
        }
    }


    bool ChessBoard::screenToBoardCell(int x, int y, int* outRow, int* outCol) {
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        float cellSizePixels = windowWidth / 8.0f;

        *outCol = x / cellSizePixels;
        *outRow = 7 - (y / cellSizePixels);

        return (*outCol >= 0 && *outCol < 8 && *outRow >= 0 && *outRow < 8);
    }

    void ChessBoard::drawNumBoard() {
        float cellSize = sx / 8.0f;

        for (int i = 0; i <= 8; ++i) {
            // Вертикальные линии
            glBegin(GL_LINES);
            glVertex3f((i - 4) * cellSize, 0, -4 * cellSize);
            glVertex3f((i - 4) * cellSize, 0, 4 * cellSize);
            glEnd();

            // Горизонтальные линии
            glBegin(GL_LINES);
            glVertex3f(-4 * cellSize, 0, (i - 4) * cellSize);
            glVertex3f(4 * cellSize, 0, (i - 4) * cellSize);
            glEnd();
        }

        // Наносим разметку
        for (int i = 0; i < 8; ++i) {
            // Буквы снизу (a-h)
            glRasterPos3f((i - 3.5f) * cellSize - 0.1f * cellSize, 0, -4.5f * cellSize);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'a' + i);

            // Цифры слева (1-8)
            glRasterPos3f(-4.5f * cellSize, 0, (i - 3.5f) * cellSize - 0.1f * cellSize);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '8' - i);
        }
    }
    void ChessBoard::draw() {
        glPushMatrix();
        glTranslatef(x, y, z);

        drawBoard();
        drawNumBoard();
        drawSelection();
        drawPieces();

        glPopMatrix();
    }

}
