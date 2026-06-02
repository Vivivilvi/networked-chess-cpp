// NetworkClient.cpp
#include "NetworkClient.h"
#include <iostream>
#include <thread>
#include <sstream>

namespace Chess3D {
    NetworkClient::NetworkClient() : sock(INVALID_SOCKET), connected(false) {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    NetworkClient::~NetworkClient() {
        Disconnect();
        WSACleanup();
    }

    bool NetworkClient::Connect(const std::string& ip, int port, const std::string& name) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) return false;

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

        if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            closesocket(sock);
            return false;
        }

        playerName = name;
        std::string registerMsg = "REGISTER " + name;
        send(sock, registerMsg.c_str(), (int)registerMsg.length(), 0);

        connected = true;
        std::thread(&NetworkClient::ReceiveThread, this).detach();
        return true;
    }

    void NetworkClient::Disconnect() {
        connected = false;
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
    }

    void NetworkClient::ReceiveThread() {
        char buffer[4096];
        while (connected) {
            int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) {
                connected = false;
                break;
            }
            buffer[bytesReceived] = '\0';
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push(std::string(buffer));
        }
    }

    void NetworkClient::ProcessMessages() {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!messageQueue.empty()) {
            std::string msg = messageQueue.front();
            messageQueue.pop();

            if (msg.rfind("GAME_START", 0) == 0) {
                std::stringstream ss(msg);
                std::string cmd, color, opponent;
                ss >> cmd >> color >> opponent;
                myColor = color;
                opponentName = opponent;
                if (onGameStart) onGameStart(color + " " + opponent);
            }
            else if (msg.rfind("OPPONENT_MOVE", 0) == 0) {
                if (onOpponentMove) onOpponentMove(msg.substr(14));
            }
            else if (msg.rfind("GAME_OVER", 0) == 0) {
                if (onGameOver) onGameOver(msg.substr(9));
                myColor.clear();
            }
            else if (msg.rfind("REGISTERED", 0) == 0) {
                if (onMessage) onMessage(msg);
            }
            else if (msg.rfind("SEARCHING", 0) == 0) {
                if (onMessage) onMessage("Searching for opponent...");
            }
            else {
                if (onMessage) onMessage(msg);
            }
        }
    }

    void NetworkClient::Update() {
        ProcessMessages();
    }

    void NetworkClient::FindGame() {
        if (!connected) return;
        send(sock, "FIND_GAME", 9, 0);
    }

    void NetworkClient::SendMove(const std::string& from, const std::string& to) {
        if (!connected) return;
        std::string moveCmd = "MOVE " + from + to;
        send(sock, moveCmd.c_str(), (int)moveCmd.length(), 0);
    }

    void NetworkClient::SendSurrender() {
        if (!connected) return;
        send(sock, "SURRENDER", 9, 0);
    }
}