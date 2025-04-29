#include "HighScore.h"

void HighScoreSystem::Load(const std::string& filename) {
    scores_.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string name;
    int score;
    while (file >> name >> score && scores_.size() < MAX_SCORES) {
        scores_.emplace_back(name, score);
    }
}

void HighScoreSystem::Save(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (const auto& entry : scores_) {
        file << entry.first << " " << entry.second << "\n";
    }
}

void HighScoreSystem::Add(const std::string& name, int score) {
    scores_.emplace_back(name, score);
    std::sort(scores_.begin(), scores_.end(),
        [](const auto& a, const auto& b) { return b.second < a.second; });

    if (scores_.size() > MAX_SCORES) {
        scores_.resize(MAX_SCORES);
    }
    Save("HighScore.txt");
}