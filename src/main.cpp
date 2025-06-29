#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/web.hpp>
#include "mod/LevelInfoLayer.hpp"
#include "mod/ProfilePage.hpp"
#include "mod/CommentCell.hpp"
#include "mod/MenuLayer.hpp"
#include "popups/ReportLevel.hpp"
#include "popups/ReportAccount.hpp"
#include <thread>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <unordered_set>
#include <vector>
#include <string>
#include "globals.hpp"

using namespace geode::prelude;
namespace fs = std::filesystem;

const fs::path ignoredLvls = Mod::get()->getConfigDir() / "ignored-levels.txt";
const fs::path ignoredAccs = Mod::get()->getConfigDir() / "ignored-accounts.txt";

std::unordered_set<int> hiddenLvls, hiddenAccs;
std::vector<std::string> moderators;

void saveLvls() {
    std::ostringstream oss;
    for (int id : hiddenLvls) oss << id << "\n";
    file::writeString(ignoredLvls, oss.str());
}

void saveAccs() {
    std::ostringstream oss;
    for (int id : hiddenAccs) oss << id << "\n";
    file::writeString(ignoredAccs, oss.str());
}

static void loadLvls() {
    file::createDirectoryAll(ignoredLvls.parent_path());
    auto data = file::readString(ignoredLvls).unwrapOr("");
    std::istringstream iss(data);
    for (int id; iss >> id;) hiddenLvls.insert(id);
}

static void loadAccs() {
    file::createDirectoryAll(ignoredAccs.parent_path());
    auto data = file::readString(ignoredAccs).unwrapOr("");
    std::istringstream iss(data);
    for (int id; iss >> id;) hiddenAccs.insert(id);
}

static void getMods() {
    web::WebRequest req;
    req.timeout(std::chrono::seconds(5));
    static EventListener<web::WebTask> m;
    m.bind([](web::WebTask::Event* e) {
        if (auto res = e->getValue(); res && res->code() >= 200 && res->code() < 300) {
            moderators.clear();
            std::istringstream ss(res->string().unwrapOr(""));
            for (std::string user; std::getline(ss, user, ',');)
                if (!user.empty()) moderators.push_back(user);
            geode::log::info("Successfully fetched AbuseDB moderators.");
        }
    });
    m.setFilter(req.get("https://jarvisdevil.com/abuse/api.php?api=mods"));
}

$execute {
    loadLvls();
    loadAccs();
    getMods();
    geode::log::info("Thanks for using AbuseDB! -jarvisdevil");
}