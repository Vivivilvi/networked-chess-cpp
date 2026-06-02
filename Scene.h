#ifndef Scene_h
#define Scene_h

#include "Shape.h"
#include <vector>
#include "ChessBoard.h"
#include <utility>
#include <fstream>
#include <string>

namespace Chess3D {

    enum class PieceType {
        PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
    };

    struct Piece {
        PieceType type;
        int x, y;
    };

    class Scene {
        std::vector<Shape*> shapes;
        float width, height;
        float angleX, angleY, distZ;
        float moveSpeed, moveX, moveZ;
        int lastMouseX, lastMouseY;
        bool isDragging;
        ChessBoard* board;

        std::vector<Piece> pieces;

    public:
        Scene();
        ~Scene();
        void on_paint();
        void on_size(int width, int height);
        void on_mouse(int button, int state, int x, int y);
        void on_motion(int x, int y);
        void drawPiece(const Piece& p);

        ChessBoard* getBoard() const { return board; }

        // Обновляем метод для перерисовки
        void updateBoardDisplay() {
            glutPostRedisplay();  // Просто запрашиваем перерисовку
        }

        void saveMoveToFile(const std::string& from, const std::string& to);
    };
}

#endif