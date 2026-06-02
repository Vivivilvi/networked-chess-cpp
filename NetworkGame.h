// NetworkGame.h
#pragma once
#include <string>
#include <vector>
#include "NetworkClient.h"

namespace Chess3D {
    class Scene;
    class ChessBoard;
    class ChessLogic;

    class NetworkGame {
    private:
        NetworkClient* client;
        Scene* scene;
        ChessBoard* board;
        ChessLogic* logic;
        std::vector<std::string> moveHistory;
        bool isNetworkGame;
        bool isMyTurn;
        std::string myColor;
        std::string playerName;
        bool isRunning;

        void OnGameStart(const std::string& info);
        void OnOpponentMove(const std::string& move);
        void OnGameOver(const std::string& result);
        void OnServerMessage(const std::string& msg);

    public:
        NetworkGame(Scene* scene);
        ~NetworkGame();
        bool ConnectAndStart(const std::string& ip, int port, const std::string& name);
        void RunGameLoop();
        void Disconnect();
        bool IsConnected() const;
    };
}