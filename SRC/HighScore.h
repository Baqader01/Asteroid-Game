#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "GameUtil.h"

class HighScore {
public:
    void Load(const string& filename);
    void Save(const string& filename);
    void Add(const string& name, int score);
    const vector<pair<string, int>>& GetScores() const { return scores_; }

private:
    vector<pair<string, int>> scores_;
    string filename_ = "HighScore.txt";
    static const size_t MAX_SCORES = 5;
};