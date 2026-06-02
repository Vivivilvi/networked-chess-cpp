#pragma once
#include <vector>
#include <string>

namespace Chess3D {

    struct Move {
        std::string from;
        std::string to;
        char piece;
        int turn;
    };

    class GameStats {
    public:
        void saveMove(const std::string& from, const std::string& to, char piece, int turn);
        void drawMoveHistory();
        void saveStatsToFile(const std::string& filename = "stats.txt");
        void loadStatsFromFile(const std::string& filename = "stats.txt");
        void endGame(bool playerWon, bool draw);

        void renderText(float x, float y, const std::string& text);
        int getWins() const { return wins; }
        int getLosses() const { return losses; }
        int getDraws() const { return draws; }

    private:
        std::vector<Move> moveHistory;
        int wins = 0;
        int losses = 0;
        int draws = 0;
    };

}
