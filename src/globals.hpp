#pragma once
#include <unordered_set>
#include <vector>
#include <string>
#include <filesystem>

extern std::unordered_set<int> hiddenLvls, hiddenAccs;
extern std::vector<std::string> moderators;
extern std::vector<std::string> flaggedUsers;
extern const std::filesystem::path ignoredLvls, ignoredAccs;

void saveLvls();
void saveAccs();