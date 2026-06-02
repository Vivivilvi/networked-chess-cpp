#include "ChessLogic.h"
#include <iostream>
#include <cmath>
#include <mutex>
#include <iomanip>

namespace Chess3D {
    std::mutex boardMutex;

    ChessLogic::ChessLogic(ChessBoard* board)
        : board(board), whiteTurn(true), gameOver(false) {
    }

    // НОВЫЙ МЕТОД: ценность фигуры
    int ChessLogic::getPieceValue(char piece) {
        switch (tolower(piece)) {
        case 'p': return 1;
        case 'n': return 3;
        case 'b': return 3;
        case 'r': return 5;
        case 'q': return 9;
        case 'k': return 0;
        default: return 0;
        }
    }

    // НОВЫЙ МЕТОД: подсчет очков
    //int ChessLogic::getScore(bool isWhite) {
    //    int score = 0;
    //    for (int row = 0; row < 8; ++row) {
    //        for (int col = 0; col < 8; ++col) {
    //            char piece = board->getPiece(row, col);
    //            if (piece != ' ') {
    //                bool pieceIsWhite = isupper(piece);
    //                if (pieceIsWhite == isWhite) {
    //                    score += getPieceValue(piece);
    //                }
    //            }
    //        }
    //    }
    //    return score;
    //}

    //// НОВЫЙ МЕТОД: вывод счета
    //void ChessLogic::printScore() {
    //    int whiteScore = getScore(true);
    //    int blackScore = getScore(false);

    //    std::cout << "\n+-----------------------------------+\n";
    //    std::cout << "|            SCORE BOARD            |\n";
    //    std::cout << "+-----------------------------------+\n";
    //    std::cout << "|  White: " << std::setw(28) << whiteScore << " |\n";
    //    std::cout << "|  Black: " << std::setw(28) << blackScore << " |\n";

    //    int diff = whiteScore - blackScore;
    //    if (diff > 0) {
    //        std::cout << "|  White leads by " << std::setw(20) << diff << " |\n";
    //    }
    //    else if (diff < 0) {
    //        std::cout << "|  Black leads by " << std::setw(20) << -diff << " |\n";
    //    }
    //    else {
    //        std::cout << "|  Equal score!                   |\n";
    //    }
    //    std::cout << "+-----------------------------------+\n";
    //}

    // НОВЫЙ МЕТОД: проверка пата
    bool ChessLogic::isStalemate(bool isWhite) {
        if (isInCheck(isWhite)) return false;

        for (int fromRow = 0; fromRow < 8; ++fromRow) {
            for (int fromCol = 0; fromCol < 8; ++fromCol) {
                char piece = board->getPiece(fromRow, fromCol);
                if (piece != ' ' && (isupper(piece) == isWhite)) {
                    for (int toRow = 0; toRow < 8; ++toRow) {
                        for (int toCol = 0; toCol < 8; ++toCol) {
                            if (isValidMove(piece, fromRow, fromCol, toRow, toCol)) {
                                char target = board->getPiece(toRow, toCol);
                                board->setPiece(toRow, toCol, piece);
                                board->setPiece(fromRow, fromCol, ' ');

                                bool stillSafe = !isInCheck(isWhite);

                                board->setPiece(fromRow, fromCol, piece);
                                board->setPiece(toRow, toCol, target);

                                if (stillSafe) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    // НОВЫЙ МЕТОД: получение победителя
    std::string ChessLogic::getWinner() const {
        if (!gameOver) return "";
        if (resultMessage.find("White wins") != std::string::npos) return "White";
        if (resultMessage.find("Black wins") != std::string::npos) return "Black";
        if (resultMessage.find("Checkmate to White") != std::string::npos) return "Black";
        if (resultMessage.find("Checkmate to Black") != std::string::npos) return "White";
        return "Draw";
    }

    // НОВЫЙ МЕТОД: получение проигравшего
    std::string ChessLogic::getLoser() const {
        if (!gameOver) return "";
        if (resultMessage.find("White wins") != std::string::npos) return "Black";
        if (resultMessage.find("Black wins") != std::string::npos) return "White";
        if (resultMessage.find("Checkmate to White") != std::string::npos) return "White";
        if (resultMessage.find("Checkmate to Black") != std::string::npos) return "Black";
        return "";
    }

    // НОВЫЙ МЕТОД: статус игры
    std::string ChessLogic::getGameStatus() {
        if (gameOver) return resultMessage;

        if (isCheckmate(true)) return "CHECKMATE! Black wins!";
        if (isCheckmate(false)) return "CHECKMATE! White wins!";
        if (isStalemate(true) || isStalemate(false)) return "STALEMATE! Game is a draw!";
        if (isInCheck(whiteTurn)) return "[!] CHECK! [!]";

        return "Game in progress";
    }

    // ОСНОВНОЙ МЕТОД movePiece (обновленный)
    bool ChessLogic::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
        std::lock_guard<std::mutex> lock(boardMutex);

        if (gameOver) {
            std::cout << "Game over: " << resultMessage << "\n";
            return false;
        }

        char piece = board->getPiece(fromRow, fromCol);
        if (piece == ' ') {
            std::cout << "No piece at the selected position!\n";
            return false;
        }

        bool isWhite = isupper(piece);
        if (isWhite != whiteTurn) {
            std::cout << "It's the other player's turn!\n";
            return false;
        }

        if (!isValidMove(piece, fromRow, fromCol, toRow, toCol)) {
            std::cout << "Invalid move for piece " << piece << "\n";
            return false;
        }

        char targetPiece = board->getPiece(toRow, toCol);
        board->setPiece(toRow, toCol, piece);
        board->setPiece(fromRow, fromCol, ' ');

        if (isInCheck(isWhite)) {
            board->setPiece(fromRow, fromCol, piece);
            board->setPiece(toRow, toCol, targetPiece);
            std::cout << "This move leaves your king in check!\n";
            return false;
        }

        // Кто сделал ход (тот, кто сейчас походил)
        bool currentPlayer = isWhite;
        // Кто будет ходить следующим
        bool nextPlayer = !currentPlayer;

        // Проверяем - под матом ли СЛЕДУЮЩИЙ игрок?
        if (isCheckmate(nextPlayer)) {
            gameOver = true;
            resultMessage = currentPlayer ? "Checkmate! White wins!" : "Checkmate! Black wins!";
            std::cout << "\n========================================\n";
            std::cout << "            CHECKMATE!                 \n";
            std::cout << "    " << resultMessage << "\n";
            std::cout << "========================================\n";
        }
        // Проверка на пат для следующего игрока
        else if (isStalemate(nextPlayer)) {
            gameOver = true;
            resultMessage = "Stalemate! Game is a draw!";
            std::cout << "\n[DRAW] " << resultMessage << "\n";
        }
        // Проверка на шах для следующего игрока
        else if (isInCheck(nextPlayer)) {
            std::cout << "\n[!] CHECK! " << (nextPlayer ? "White" : "Black") << " king is in check! [!]\n";
        }

        // Выводим текущий статус
        if (!gameOver) {
            std::cout << "\n[STATUS] " << getGameStatus() << std::endl;
        }

        // Меняем очередь хода
        whiteTurn = nextPlayer;

        return true;
    }

    // Остальные методы остаются без изменений...
    bool ChessLogic::isValidMove(char piece, int fromRow, int fromCol, int toRow, int toCol) {
        if (toRow < 0 || toRow >= 8 || toCol < 0 || toCol >= 8) return false;
        if (fromRow == toRow && fromCol == toCol) return false;

        char targetPiece = board->getPiece(toRow, toCol);
        if (targetPiece != ' ' &&
            ((isupper(piece) && isupper(targetPiece)) ||
                (islower(piece) && islower(targetPiece)))) {
            return false;
        }

        int rowDiff = toRow - fromRow;
        int colDiff = toCol - fromCol;
        int absRowDiff = abs(rowDiff);
        int absColDiff = abs(colDiff);

        switch (tolower(piece)) {
        case 'p': {
            int direction = isupper(piece) ? -1 : 1;
            int startRow = isupper(piece) ? 6 : 1;

            if (fromCol == toCol && targetPiece == ' ') {
                if (toRow == fromRow + direction) {
                    return true;
                }
                if (fromRow == startRow && toRow == fromRow + 2 * direction) {
                    if (board->getPiece(fromRow + direction, fromCol) == ' ') {
                        return true;
                    }
                }
                return false;
            }
            if (absColDiff == 1 && toRow == fromRow + direction && targetPiece != ' ') {
                return true;
            }
            return false;
        }

        case 'r':
            if ((fromRow == toRow || fromCol == toCol)) {
                return isPathClear(fromRow, fromCol, toRow, toCol);
            }
            return false;

        case 'n':
            return (absRowDiff == 2 && absColDiff == 1) || (absRowDiff == 1 && absColDiff == 2);

        case 'b':
            if (absRowDiff == absColDiff) {
                return isPathClear(fromRow, fromCol, toRow, toCol);
            }
            return false;

        case 'q':
            if ((fromRow == toRow || fromCol == toCol) || (absRowDiff == absColDiff)) {
                return isPathClear(fromRow, fromCol, toRow, toCol);
            }
            return false;

        case 'k': {
            if (absRowDiff <= 1 && absColDiff <= 1) return true;

            if (absColDiff == 2 && rowDiff == 0) {
                int row = fromRow;
                bool isWhite = isupper(piece);
                int rookCol = (toCol == 6) ? 7 : 0;
                char rook = isWhite ? 'R' : 'r';

                if (board->getPiece(row, rookCol) != rook) return false;

                int start = std::min(fromCol, rookCol) + 1;
                int end = std::max(fromCol, rookCol) - 1;
                for (int col = start; col <= end; ++col) {
                    if (board->getPiece(row, col) != ' ') return false;
                }
                if (tolower(piece) == 'k' && abs(toCol - fromCol) == 2) {
                    int rookFrom = (toCol == 6) ? 7 : 0;
                    int rookTo = (toCol == 6) ? 5 : 3;
                    char rook = isupper(piece) ? 'R' : 'r';
                    board->setPiece(toRow, rookTo, rook);
                    board->setPiece(toRow, rookFrom, ' ');
                }
                return true;
            }
            return false;
        }
        default:
            return false;
        }
    }

    bool ChessLogic::isPathClear(int fromRow, int fromCol, int toRow, int toCol) {
        int rowStep = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;
        int colStep = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;

        int currentRow = fromRow + rowStep;
        int currentCol = fromCol + colStep;

        while (currentRow != toRow || currentCol != toCol) {
            if (board->getPiece(currentRow, currentCol) != ' ') {
                return false;
            }
            currentRow += rowStep;
            currentCol += colStep;
        }
        return true;
    }

    bool ChessLogic::isInCheck(bool isWhite) {
        int kingRow = -1, kingCol = -1;
        char king = isWhite ? 'K' : 'k';

        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (board->getPiece(row, col) == king) {
                    kingRow = row;
                    kingCol = col;
                    break;
                }
            }
            if (kingRow != -1) break;
        }

        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                char piece = board->getPiece(row, col);
                if (piece != ' ' && (bool(isupper(piece)) != isWhite)) {
                    if (isValidMove(piece, row, col, kingRow, kingCol)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool ChessLogic::isCheckmate(bool isWhite) {
        if (!isInCheck(isWhite)) return false;

        for (int fromRow = 0; fromRow < 8; ++fromRow) {
            for (int fromCol = 0; fromCol < 8; ++fromCol) {
                char piece = board->getPiece(fromRow, fromCol);
                if (piece != ' ' && (isupper(piece) == static_cast<int>(isWhite))) {
                    for (int toRow = 0; toRow < 8; ++toRow) {
                        for (int toCol = 0; toCol < 8; ++toCol) {
                            if (isValidMove(piece, fromRow, fromCol, toRow, toCol)) {
                                char target = board->getPiece(toRow, toCol);
                                board->setPiece(toRow, toCol, piece);
                                board->setPiece(fromRow, fromCol, ' ');

                                bool stillInCheck = isInCheck(isWhite);

                                board->setPiece(fromRow, fromCol, piece);
                                board->setPiece(toRow, toCol, target);

                                if (!stillInCheck) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    bool ChessLogic::isGameOver() const {
        return gameOver;
    }

    std::string ChessLogic::getResultMessage() const {
        return resultMessage;
    }
}