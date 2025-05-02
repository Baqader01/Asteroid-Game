#include "HighScore.h"

void HighScore::Load(const string& filename) {
    scores_.clear();
    ifstream file(filename);
    if (!file.is_open()) return;

    string name;
    int score;
    while (file >> name >> score && scores_.size() < MAX_SCORES) {
        scores_.emplace_back(name, score);
    }
}

void HighScore::Save(const string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (const auto& entry : scores_) {
        file << entry.first << " " << entry.second << "\n";
    }
}

void HighScore::Add(const string& name, int score) {
    // First load current scores 
    Load(filename_);  //

    // Only add if score is high enough to make the list
    if (scores_.size() < MAX_SCORES || score > scores_.back().second) {
        scores_.emplace_back(name, score);
        std::sort(scores_.begin(), scores_.end(),
            [](const auto& a, const auto& b) { return b.second < a.second; });

        if (scores_.size() > MAX_SCORES) {
            scores_.resize(MAX_SCORES);
        }
        Save(filename_);
    }
}

