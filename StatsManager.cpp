// StatsManager.cpp
#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <iomanip>

namespace Chess3D {
    class StatsManager {
    private:
        std::string statsFile;
        std::map<std::string, int> wins;
        std::map<std::string, int> losses;
        std::map<std::string, int> draws;

        void loadStats() {
            std::ifstream file(statsFile);
            if (file.is_open()) {
                std::string line;
                std::string player;
                while (std::getline(file, line)) {
                    if (line.find("Player:") != std::string::npos) {
                        size_t pos = line.find(":") + 2;
                        player = line.substr(pos);
                        wins[player] = 0;
                        losses[player] = 0;
                        draws[player] = 0;
                    }
                    else if (line.find("Wins:") != std::string::npos) {
                        wins[player] = std::stoi(line.substr(6));
                    }
                    else if (line.find("Losses:") != std::string::npos) {
                        losses[player] = std::stoi(line.substr(8));
                    }
                    else if (line.find("Draws:") != std::string::npos) {
                        draws[player] = std::stoi(line.substr(7));
                    }
                }
                file.close();
            }
        }

        void saveStats() {
            std::ofstream file(statsFile);
            if (file.is_open()) {
                for (std::map<std::string, int>::iterator it = wins.begin(); it != wins.end(); ++it) {
                    std::string player = it->first;
                    file << "Player: " << player << "\n";
                    file << "Wins: " << wins[player] << "\n";
                    file << "Losses: " << losses[player] << "\n";
                    file << "Draws: " << draws[player] << "\n";
                    file << "-------------------\n";
                }
                file.close();
            }
        }

    public:
        StatsManager() : statsFile("chess_scores.txt") {
            loadStats();
        }

        void addWin(const std::string& player) {
            wins[player]++;
            saveStats();
        }

        void addLoss(const std::string& player) {
            losses[player]++;
            saveStats();
        }

        void addDraw(const std::string& player1, const std::string& player2) {
            draws[player1]++;
            draws[player2]++;
            saveStats();
        }

        void printLeaderboard() {
            std::cout << "\n========================================\n";
            std::cout << "            LEADERBOARD                \n";
            std::cout << "========================================\n";
            std::cout << "Player                 Wins    Losses    Draws\n";
            std::cout << "----------------------------------------\n";

            for (std::map<std::string, int>::iterator it = wins.begin(); it != wins.end(); ++it) {
                std::string player = it->first;
                std::cout << std::left << std::setw(22) << player
                    << std::right << std::setw(5) << wins[player]
                    << std::setw(9) << losses[player]
                    << std::setw(9) << draws[player] << "\n";
            }
            std::cout << "========================================\n";
        }

        void printPlayerStats(const std::string& player) {
            if (wins.find(player) == wins.end()) {
                std::cout << "\n[STATS] No stats for player: " << player << std::endl;
                return;
            }

            int total = wins[player] + losses[player] + draws[player];
            float winRate = total > 0 ? (wins[player] * 100.0f / total) : 0;

            std::cout << "\n========================================\n";
            std::cout << "      STATISTICS FOR " << player << "\n";
            std::cout << "========================================\n";
            std::cout << "  Wins:   " << wins[player] << "\n";
            std::cout << "  Losses: " << losses[player] << "\n";
            std::cout << "  Draws:  " << draws[player] << "\n";
            std::cout << "  Total:  " << total << "\n";
            std::cout << "  Win Rate: " << std::fixed << std::setprecision(1) << winRate << "%\n";
            std::cout << "========================================\n";
        }

        void resetStats() {
            wins.clear();
            losses.clear();
            draws.clear();
            saveStats();
            std::cout << "[STATS] Statistics reset!" << std::endl;
        }
    };
}