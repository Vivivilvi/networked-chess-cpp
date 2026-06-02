#include "NetworkGame.h"
#include "Scene.h"
#include "ChessBoard.h"
#include "ChessLogic.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

namespace Chess3D {
    NetworkGame::NetworkGame(Scene* scene)
        : client(nullptr), scene(scene), board(nullptr), logic(nullptr),
        isNetworkGame(false), isMyTurn(false), isRunning(true) {
        board = scene->getBoard();
        logic = new ChessLogic(board);
    }

    NetworkGame::~NetworkGame() {
        delete client;
        delete logic;
    }

    bool NetworkGame::ConnectAndStart(const std::string& ip, int port, const std::string& name) {
        playerName = name;
        client = new NetworkClient();

        client->SetOnGameStart([this](const std::string& info) { OnGameStart(info); });
        client->SetOnOpponentMove([this](const std::string& move) { OnOpponentMove(move); });
        client->SetOnGameOver([this](const std::string& result) { OnGameOver(result); });
        client->SetOnMessage([this](const std::string& msg) { OnServerMessage(msg); });

        if (!client->Connect(ip, port, name)) {
            delete client;
            client = nullptr;
            return false;
        }
        return true;
    }

    void NetworkGame::OnGameStart(const std::string& info) {
        myColor = (info.find("white") != std::string::npos) ? "white" : "black";
        isNetworkGame = true;
        isMyTurn = (myColor == "white");

        std::cout << "\n========================================\n";
        std::cout << "           GAME STARTED!                \n";
        std::cout << "========================================\n";
        std::cout << "  You play: " << myColor << "\n";
        std::cout << "========================================\n";

        if (isMyTurn) {
            std::cout << "\n[YOUR TURN] Enter move (e.g., e2e4): ";
        }
        else {
            std::cout << "\n[WAITING] Waiting for opponent's move..." << std::endl;
        }
        scene->updateBoardDisplay();
    }

    void NetworkGame::OnOpponentMove(const std::string& move) {
        if (!logic || move.length() != 4) return;

        if (logic->isGameOver()) {
            std::cout << "\n[GAME OVER] Game already finished.\n";
            return;
        }

        int fromCol = move[0] - 'a';
        int fromRow = 8 - (move[1] - '0');
        int toCol = move[2] - 'a';
        int toRow = 8 - (move[3] - '0');

        if (logic->movePiece(fromRow, fromCol, toRow, toCol)) {
            std::cout << "\n[OPPONENT] moved: " << move << std::endl;

            if (logic->isGameOver()) {
                std::string result = logic->getResultMessage();
                std::cout << "\n========================================\n";
                std::cout << "           GAME OVER!                 \n";
                std::cout << "    " << result << "\n";
                std::cout << "========================================\n";
                isNetworkGame = false;
                isMyTurn = false;

                std::ofstream out("moves.txt", std::ios::app);
                out << "\nGame: " << playerName << " vs " << client->GetOpponentName() << "\n";
                out << "Result: " << result << "\n";
                out << "========================================\n";
                out.close();
                return;
            }

            isMyTurn = true;
            std::cout << "[YOUR TURN] Enter move: ";
            scene->updateBoardDisplay();
        }
    }

    void NetworkGame::OnGameOver(const std::string& result) {
        std::cout << "\n========================================\n";
        std::cout << "           GAME OVER!                 \n";
        std::cout << "    " << result << "\n";
        std::cout << "========================================\n";
        isNetworkGame = false;
        isMyTurn = false;
    }

    void NetworkGame::OnServerMessage(const std::string& msg) {
        std::cout << "[Server] " << msg << std::endl;
    }

    bool NetworkGame::IsConnected() const {
        return client && client->IsConnected();
    }

    void NetworkGame::RunGameLoop() {
        if (!client || !client->IsConnected()) {
            std::cout << "Not connected!\n";
            return;
        }

        std::cout << "\n========================================\n";
        std::cout << "        NETWORK GAME MODE             \n";
        std::cout << "========================================\n";
        std::cout << "  Commands: /ready, /surr, /quit\n";
        std::cout << "========================================\n";

        bool waitingForGame = true;

        std::thread inputThread([this, &waitingForGame]() {
            while (isRunning && client && client->IsConnected()) {
                std::string input;
                std::getline(std::cin, input);

                if (input == "/ready" && waitingForGame) {
                    client->FindGame();
                    std::cout << "[SEARCH] Searching for opponent...\n";
                }
                else if (input == "/quit") {
                    isRunning = false;
                    client->Disconnect();
                    break;
                }
                else if (input == "/surr" && isNetworkGame && isMyTurn) {
                    client->SendSurrender();
                    std::cout << "[SURRENDER] You surrendered!\n";
                    isMyTurn = false;
                    isNetworkGame = false;
                }
                else if (input.length() == 4 && isNetworkGame && isMyTurn && !logic->isGameOver()) {
                    int fromCol = input[0] - 'a';
                    int fromRow = 8 - (input[1] - '0');
                    int toCol = input[2] - 'a';
                    int toRow = 8 - (input[3] - '0');

                    if (fromRow >= 0 && fromRow < 8 && fromCol >= 0 && fromCol < 8 &&
                        toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8) {

                        if (logic->movePiece(fromRow, fromCol, toRow, toCol)) {
                            client->SendMove(input.substr(0, 2), input.substr(2, 2));
                            std::cout << "[MOVE SENT] " << input << std::endl;

                            if (logic->isGameOver()) {
                                std::string result = logic->getResultMessage();
                                std::cout << "\n========================================\n";
                                std::cout << "           GAME OVER!                 \n";
                                std::cout << "    " << result << "\n";
                                std::cout << "========================================\n";
                                isNetworkGame = false;
                                isMyTurn = false;

                                std::ofstream out("moves.txt", std::ios::app);
                                out << "\nGame: " << playerName << " vs " << client->GetOpponentName() << "\n";
                                out << "Result: " << result << "\n";
                                out << "========================================\n";
                                out.close();
                            }
                            else {
                                isMyTurn = false;
                                std::cout << "[WAITING] Waiting for opponent's move...\n";
                            }
                            scene->updateBoardDisplay();
                        }
                        else {
                            std::cout << "[ERROR] Invalid move!\n";
                        }
                    }
                }
            }
            });

        while (client->IsConnected() && isRunning) {
            client->Update();

            if (waitingForGame && isNetworkGame) {
                waitingForGame = false;
            }

            if (!waitingForGame && !isNetworkGame) {
                std::cout << "\n[INFO] Game ended. Type /ready for new game\n";
                waitingForGame = true;
                isMyTurn = false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        inputThread.detach();
        std::cout << "[DISCONNECTED]\n";
    }

    void NetworkGame::Disconnect() {
        isRunning = false;
        if (client) client->Disconnect();
        isNetworkGame = false;
    }
}