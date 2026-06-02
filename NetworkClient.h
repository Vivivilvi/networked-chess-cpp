// NetworkClient.h
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <functional>
#include <queue>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

namespace Chess3D {
    class NetworkClient {
    private:
        SOCKET sock;
        bool connected;
        std::string playerName;
        std::string opponentName;
        std::string myColor;
        std::queue<std::string> messageQueue;
        std::mutex queueMutex;

        std::function<void(const std::string&)> onGameStart;
        std::function<void(const std::string&)> onOpponentMove;
        std::function<void(const std::string&)> onGameOver;
        std::function<void(const std::string&)> onMessage;

        void ReceiveThread();
        void ProcessMessages();

    public:
        NetworkClient();
        ~NetworkClient();

        bool Connect(const std::string& ip, int port, const std::string& name);
        void Disconnect();
        void FindGame();
        void SendMove(const std::string& from, const std::string& to);
        void SendSurrender();
        void Update();

        bool IsConnected() const { return connected; }
        std::string GetOpponentName() const { return opponentName; }

        void SetOnGameStart(std::function<void(const std::string&)> cb) { onGameStart = cb; }
        void SetOnOpponentMove(std::function<void(const std::string&)> cb) { onOpponentMove = cb; }
        void SetOnGameOver(std::function<void(const std::string&)> cb) { onGameOver = cb; }
        void SetOnMessage(std::function<void(const std::string&)> cb) { onMessage = cb; }
    };
}