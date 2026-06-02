#pragma once
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

        void loadStats();
        void saveStats();

    public:
        StatsManager();

        void addWin(const std::string& player);
        void addLoss(const std::string& player);
        void addDraw(const std::string& player1, const std::string& player2);
        void printLeaderboard();
        void printPlayerStats(const std::string& player);
        void resetStats();
    };
}