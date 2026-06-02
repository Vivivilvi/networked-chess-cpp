#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <map>
#include <queue>
#include <string>
#include <mutex>

struct Player {
    SOCKET socket;
    std::string name;
    int roomId;
    bool inGame;

    Player() : socket(INVALID_SOCKET), roomId(-1), inGame(false) {}
    Player(SOCKET s, const std::string& n) : socket(s), name(n), roomId(-1), inGame(false) {}
};

struct GameRoom {
    int id;
    SOCKET whitePlayer;
    SOCKET blackPlayer;
    std::string whiteName;
    std::string blackName;
    bool isActive;

    GameRoom() : id(-1), whitePlayer(INVALID_SOCKET), blackPlayer(INVALID_SOCKET), isActive(false) {}
};

class ChessServer {
private:
    SOCKET listenSocket;
    std::map<SOCKET, Player> players;
    std::queue<SOCKET> waitingPlayers;
    std::map<int, GameRoom> rooms;
    int nextRoomId;
    std::mutex mtx;
    bool isRunning;

    void SendToClient(SOCKET client, const std::string& message) {
        if (client != INVALID_SOCKET) {
            send(client, message.c_str(), (int)message.length(), 0);
        }
    }

    void CreateGameRoom(SOCKET player1, SOCKET player2) {
        int roomId = nextRoomId++;

        GameRoom room;
        room.id = roomId;
        room.whitePlayer = player1;
        room.blackPlayer = player2;
        room.whiteName = players[player1].name;
        room.blackName = players[player2].name;
        room.isActive = true;

        rooms[roomId] = room;

        players[player1].roomId = roomId;
        players[player1].inGame = true;
        players[player2].roomId = roomId;
        players[player2].inGame = true;

        // Сообщаем игрокам их стороны
        SendToClient(player1, "GAME_START white " + players[player2].name);
        SendToClient(player2, "GAME_START black " + players[player1].name);

        std::cout << "Game created: " << players[player1].name << " (white) vs "
            << players[player2].name << " (black) in room " << roomId << std::endl;
    }

    void HandleFindGame(SOCKET client) {
        std::lock_guard<std::mutex> lock(mtx);

        if (!waitingPlayers.empty()) {
            SOCKET opponent = waitingPlayers.front();
            waitingPlayers.pop();
            CreateGameRoom(opponent, client);
        }
        else {
            waitingPlayers.push(client);
            SendToClient(client, "SEARCHING Searching for opponent...");
            std::cout << players[client].name << " is waiting for opponent" << std::endl;
        }
    }

    void HandleMove(SOCKET client, const std::string& move) {
        std::lock_guard<std::mutex> lock(mtx);

        auto playerIt = players.find(client);
        if (playerIt == players.end()) return;

        int roomId = playerIt->second.roomId;
        auto roomIt = rooms.find(roomId);
        if (roomIt == rooms.end()) return;

        auto& room = roomIt->second;
        SOCKET opponent = (room.whitePlayer == client) ? room.blackPlayer : room.whitePlayer;

        if (opponent != INVALID_SOCKET) {
            SendToClient(opponent, "OPPONENT_MOVE " + move);
            std::cout << "Move from " << players[client].name << ": " << move << std::endl;
        }
    }

    void HandleSurrender(SOCKET client) {
        std::lock_guard<std::mutex> lock(mtx);

        auto playerIt = players.find(client);
        if (playerIt == players.end()) return;

        int roomId = playerIt->second.roomId;
        auto roomIt = rooms.find(roomId);
        if (roomIt == rooms.end()) return;

        auto& room = roomIt->second;
        SOCKET winner = (room.whitePlayer == client) ? room.blackPlayer : room.whitePlayer;

        if (winner != INVALID_SOCKET) {
            SendToClient(winner, "GAME_OVER Opponent surrendered! You win!");
        }
        SendToClient(client, "GAME_OVER You surrendered!");

        if (room.whitePlayer != INVALID_SOCKET) {
            players[room.whitePlayer].inGame = false;
            players[room.whitePlayer].roomId = -1;
        }
        if (room.blackPlayer != INVALID_SOCKET) {
            players[room.blackPlayer].inGame = false;
            players[room.blackPlayer].roomId = -1;
        }
        rooms.erase(roomId);

        std::cout << "Game ended by surrender" << std::endl;
    }

    void HandleClient(SOCKET client) {
        char buffer[4096];

        // Ожидаем регистрацию
        int recvSize = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (recvSize <= 0) {
            closesocket(client);
            return;
        }

        buffer[recvSize] = '\0';
        std::string cmd(buffer);

        if (cmd.rfind("REGISTER ", 0) == 0) {
            std::string clientName = cmd.substr(9);

            {
                std::lock_guard<std::mutex> lock(mtx);
                players[client] = Player(client, clientName);
                SendToClient(client, "REGISTERED Welcome " + clientName + "!");
                std::cout << clientName << " connected" << std::endl;
            }
        }
        else {
            closesocket(client);
            return;
        }

        // Основной цикл обработки команд
        while (isRunning) {
            recvSize = recv(client, buffer, sizeof(buffer) - 1, 0);
            if (recvSize <= 0) {
                // Клиент отключился
                std::lock_guard<std::mutex> lock(mtx);

                auto playerIt = players.find(client);
                if (playerIt != players.end()) {
                    std::cout << playerIt->second.name << " disconnected" << std::endl;

                    if (playerIt->second.inGame) {
                        int roomId = playerIt->second.roomId;
                        auto roomIt = rooms.find(roomId);
                        if (roomIt != rooms.end()) {
                            auto& room = roomIt->second;
                            SOCKET opponent = (room.whitePlayer == client) ? room.blackPlayer : room.whitePlayer;
                            if (opponent != INVALID_SOCKET) {
                                SendToClient(opponent, "GAME_OVER Opponent disconnected!");
                            }
                            rooms.erase(roomId);
                        }
                    }
                    players.erase(client);
                }
                closesocket(client);
                break;
            }

            buffer[recvSize] = '\0';
            std::string command(buffer);

            if (command == "FIND_GAME") {
                HandleFindGame(client);
            }
            else if (command.rfind("MOVE ", 0) == 0) {
                HandleMove(client, command.substr(5));
            }
            else if (command == "SURRENDER") {
                HandleSurrender(client);
            }
            else if (command == "QUIT") {
                break;
            }
        }
    }

public:
    ChessServer() : listenSocket(INVALID_SOCKET), nextRoomId(1), isRunning(true) {}

    void Start(int port) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return;
        }

        listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed" << std::endl;
            WSACleanup();
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed" << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed" << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        std::cout << "========================================" << std::endl;
        std::cout << "    CHESS SERVER STARTED ON PORT " << port << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Waiting for players..." << std::endl;

        while (isRunning) {
            SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
            if (clientSocket != INVALID_SOCKET) {
                std::thread(&ChessServer::HandleClient, this, clientSocket).detach();
            }
        }

        closesocket(listenSocket);
        WSACleanup();
    }

    void Stop() {
        isRunning = false;
    }
};

int main() {
    ChessServer server;
    server.Start(8888);
    return 0;
}