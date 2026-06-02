#include <glut.h>
#include "GameStats.h"
#include <fstream>
#include <sstream>


namespace Chess3D {

    void GameStats::saveMove(const std::string& from, const std::string& to, char piece, int turn) {
        moveHistory.push_back({ from, to, piece, turn });
    }

    void GameStats::drawMoveHistory() {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);

        int y = 580;
        for (const auto& move : moveHistory) {
            std::stringstream ss;
            ss << move.turn << ". " << move.piece << ": " << move.from << " → " << move.to;
            renderText(610, y, ss.str());
            y -= 20;
        }

        glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    void GameStats::saveStatsToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Wins: " << wins << "\n";
            file << "Losses: " << losses << "\n";
            file << "Draws: " << draws << "\n";
        }
    }

    void GameStats::loadStatsFromFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        if (!file.is_open()) return;

        while (std::getline(file, line)) {
            if (line.find("Wins:") != std::string::npos)
                wins = std::stoi(line.substr(6));
            else if (line.find("Losses:") != std::string::npos)
                losses = std::stoi(line.substr(8));
            else if (line.find("Draws:") != std::string::npos)
                draws = std::stoi(line.substr(7));
        }
    }

    void GameStats::endGame(bool playerWon, bool draw) {
        if (draw) draws++;
        else if (playerWon) wins++;
        else losses++;

        saveStatsToFile();
        moveHistory.clear();
    }

    void GameStats::renderText(float x, float y, const std::string& text) {
        glRasterPos2f(x, y);
        for (char c : text)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

}
