#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <iomanip>
#include <fstream>
#include "Scene.h"
#include "ChessBoard.h"
#include "ChessLogic.h"
#include "NetworkGame.h"

extern Chess3D::Scene* scene;
std::vector<std::string> moveHistory;
Chess3D::NetworkGame* networkGame = nullptr;

bool parseMove(const std::string& input, int& fromRow, int& fromCol, int& toRow, int& toCol) {
    if (input.length() != 4) return false;
    if (!isalpha(input[0]) || !isdigit(input[1]) || !isalpha(input[2]) || !isdigit(input[3]))
        return false;

    fromCol = input[0] - 'a';
    fromRow = 8 - (input[1] - '0');
    toCol = input[2] - 'a';
    toRow = 8 - (input[3] - '0');

    return fromRow >= 0 && fromRow < 8 && fromCol >= 0 && fromCol < 8 &&
        toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8;
}

void saveLocalGameResult(const std::string& result) {
    std::ofstream out("moves.txt");
    out << std::left << std::setw(8) << "#"
        << std::setw(20) << "White"
        << std::setw(20) << "Black" << "\n";
    out << "---------------------------------------------\n";

    for (size_t i = 0; i < moveHistory.size(); i += 2) {
        size_t moveNum = i / 2 + 1;
        std::string white = moveHistory[i];
        std::string black = (i + 1 < moveHistory.size()) ? moveHistory[i + 1] : "";
        out << std::setw(8) << moveNum
            << std::setw(20) << white
            << std::setw(20) << black << "\n";
    }

    out << "\nResult: " << result << "\n";
    out.close();

    system("start notepad moves.txt");
}

void localGameLoop() {
    Chess3D::ChessBoard* board = scene->getBoard();
    Chess3D::ChessLogic logic(board);
    moveHistory.clear();

    std::cout << "\n=== Local Game Mode ===" << std::endl;
    std::cout << "White moves first. Enter moves in format: e2e4" << std::endl;
    std::cout << "Type /quit to exit" << std::endl;

    while (true) {
        if (logic.isGameOver()) {
            std::cout << "\nGame Over: " << logic.getResultMessage() << "\n";
            saveLocalGameResult(logic.getResultMessage());
            break;
        }

        std::string input;
        std::cout << (logic.isWhiteTurn() ? "[White] " : "[Black] ")
            << "Enter move: ";
        std::cin >> input;

        if (input == "/quit") {
            std::cout << "Game terminated.\n";
            break;
        }

        int fr, fc, tr, tc;
        if (!parseMove(input, fr, fc, tr, tc)) {
            std::cout << "Invalid format! Use e2e4\n";
            continue;
        }

        if (logic.movePiece(fr, fc, tr, tc)) {
            std::string move = input.substr(0, 2) + " -> " + input.substr(2, 2);
            moveHistory.push_back(move);
            std::cout << "Move completed: " << move << "\n";
            scene->updateBoardDisplay();
        }
    }
}

void showMenu() {
    std::cout << "\n======================================" << std::endl;
    std::cout << "           CHESS GAME MENU            " << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "1. Local Game (2 players on 1 PC)" << std::endl;
    std::cout << "2. Online Game (via server)" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Choice: ";
}

void movePieceConsole() {
    int choice;
    showMenu();
    std::cin >> choice;
    std::cin.ignore(); // Очищаем буфер после ввода числа

    if (choice == 1) {
        localGameLoop();
        movePieceConsole();
    }
    else if (choice == 2) {
        std::string serverIP, playerName;
        std::cout << "\n=== Online Game Setup ===" << std::endl;
        std::cout << "Server IP (127.0.0.1): ";
        std::getline(std::cin, serverIP);
        if (serverIP.empty()) serverIP = "127.0.0.1";

        std::cout << "Your name: ";
        std::getline(std::cin, playerName);

        networkGame = new Chess3D::NetworkGame(scene);

        if (networkGame->ConnectAndStart(serverIP, 8888, playerName)) {
            std::cout << "Connected to server as '" << playerName << "'" << std::endl;
            networkGame->RunGameLoop();
        }
        else {
            std::cout << "Failed to connect to server!" << std::endl;
        }

        delete networkGame;
        networkGame = nullptr;

        movePieceConsole();
    }
    else {
        std::cout << "Goodbye!" << std::endl;
        exit(0);
    }
}

//#include <iostream>
//#include <thread>
//#include <string>
//#include <cctype>
//#include <mutex>
//#include "Scene.h"
//#include "ChessBoard.h"
//#include "ChessLogic.h"
//#include <cstdlib>
//#include <vector>
//#include <iomanip>
//
//std::vector<std::string> moveHistory;
//
//extern Chess3D::Scene* scene;
//std::mutex boardMutex;
//
//bool parseMove(const std::string& input, int& fromRow, int& fromCol, int& toRow, int& toCol) {
//    if (input.length() != 4) return false;
//    if (!isalpha(input[0]) || !isdigit(input[1]) || !isalpha(input[2]) || !isdigit(input[3]))
//        return false;
//
//    fromCol = input[0] - 'a';
//    fromRow = 8 - (input[1] - '0');
//    toCol = input[2] - 'a';
//    toRow = 8 - (input[3] - '0');
//
//    return fromRow >= 0 && fromRow < 8 && fromCol >= 0 && fromCol < 8 &&
//        toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8;
//}
//
//void movePieceConsole() {
//    Chess3D::ChessBoard* board = scene->getBoard();
//    Chess3D::ChessLogic logic(board);
//
//    while (true) {
//        if (logic.isGameOver()) {
//            std::cout << "GameOver: " << logic.getResultMessage() << "\n";
//            break;
//        }
//
//        std::string input;
//        std::cout << (logic.isWhiteTurn() ? "White to move" : "Black to move") << " (e.g., e2e4): ";
//        std::cin >> input;
//
//        int fr, fc, tr, tc;
//        if (!parseMove(input, fr, fc, tr, tc)) {
//            std::cout << "Invalid input format!\n";
//            continue;
//        }
//
//        bool moved = logic.movePiece(fr, fc, tr, tc);
//        if (moved) {
//            std::string move = input.substr(0, 2) + " -> " + input.substr(2, 2);
//            moveHistory.push_back(move);
//
//            std::cout << "The move is completed: " << move << "\n";
//        }
//    }
//
//    // После завершения партии создаём таблицу и открываем её
//    std::ofstream out("moves.txt");
//    out << std::left << std::setw(8) << "#"
//        << std::setw(20) << "White"
//        << std::setw(20) << "Black" << "\n";
//    out << "---------------------------------------------\n";
//
//    for (size_t i = 0; i < moveHistory.size(); i += 2) {
//        size_t moveNum = i / 2 + 1;
//        std::string white = moveHistory[i];
//        std::string black = (i + 1 < moveHistory.size()) ? moveHistory[i + 1] : "";
//        out << std::setw(8) << moveNum
//            << std::setw(20) << white
//            << std::setw(20) << black << "\n";
//    }
//
//    out << "\nResult: " << logic.getResultMessage() << "\n";
//    out.close();
//
//    system("start notepad moves.txt");
//}
//
