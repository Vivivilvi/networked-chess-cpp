#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#include "ChessBoard.h"
#include <mutex>
#include <string>

namespace Chess3D {
    extern std::mutex boardMutex;

    class ChessLogic {
    private:
        ChessBoard* board;
        bool whiteTurn;
        bool gameOver;
        std::string resultMessage;

    public:
        explicit ChessLogic(ChessBoard* board);

        bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
        bool isValidMove(char piece, int fromRow, int fromCol, int toRow, int toCol);
        bool isInCheck(bool isWhite);
        bool isCheckmate(bool isWhite);
        bool isStalemate(bool isWhite);
        bool isPathClear(int fromRow, int fromCol, int toRow, int toCol);

        bool isGameOver() const;
        std::string getResultMessage() const;
        bool isWhiteTurn() const { return whiteTurn; }

        // Дополнительные методы
        std::string getWinner() const;
        std::string getLoser() const;
        std::string getGameStatus();
        int getPieceValue(char piece);
    };
}
#endif