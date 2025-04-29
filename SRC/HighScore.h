#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

class HighScoreSystem {
public:
    void Load(const std::string& filename);
    void Save(const std::string& filename);
    void Add(const std::string& name, int score);
    const std::vector<std::pair<std::string, int>>& GetScores() const { return scores_; }

private:
    std::vector<std::pair<std::string, int>> scores_;
    static const size_t MAX_SCORES = 5;
};