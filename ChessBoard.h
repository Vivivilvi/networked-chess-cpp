#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "Shape.h"
#include "utils.h"
#include <cctype>
#include "Piece.h"
#include <iostream>

namespace Chess3D {
    class ChessBoard : public Shape {
    private:
        char board[8][8];
        bool pieceSelected = false;
        int selectedRow = 1;
        int selectedCol = 1;
        bool whiteTurn = true;

    public:
        ChessBoard(float x, float y, float z,
            float sx, float sy, float sz,
            float* diffColor, float* ambiColor, float* specColor);

        void initBoard();
        void drawBoard();
        void drawNumBoard();
        void drawSelection();
        void draw() override;
        void drawPieces();

        bool screenToBoardCell(int x, int y, int* outRow, int* outCol);
        bool isWhite(char piece) { return isupper(piece); }

        char getPiece(int row, int col) const {
            return board[row][col];
        }

        void setPiece(int row, int col, char piece) {
            board[row][col] = piece;
        }

        // Добавляем метод для обновления отображения
        void refreshDisplay() {
            // Просто помечаем, что нужно перерисовать
            // Ничего не делаем, так как draw() перерисовывает всё заново
        }

        // Метод для получения всей доски (для отладки)
        void printBoard() {
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    std::cout << board[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }
    };
}

#endif