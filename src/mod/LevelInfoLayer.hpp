#pragma once
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/utils/web.hpp>
#include "../globals.hpp"
#include "popups/ReportLevel.hpp"
#include "popups/FlagLevel.hpp"

using namespace geode::prelude;

class $modify(ADBLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        GJGameLevel* level{ nullptr };
        EventListener<web::WebTask> listener;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        m_fields->level = level;

        int levelID = level->m_levelID.value();
        if (hiddenLvls.count(levelID)) return true;

        web::WebRequest req;
        req.param("api", "geode")
           .param("level", std::to_string(levelID))
           .param("account", std::to_string(level->m_accountID.value()))
           .timeout(std::chrono::seconds(5));

        m_fields->listener.bind([levelID](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->code() >= 200 && res->code() < 300) {
                auto body = res->string().unwrapOr("");
                if (body != "-1") {
                    std::thread([body, levelID]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        geode::queueInMainThread([body, levelID]() {
                            geode::createQuickPopup(
                                "AbuseDB",
                                gd::string(std::string("This level has been flagged by AbuseDB for inappropriate, misleading or harmful content.\n\nReason: ") + body),
                                "I Understand", "Always Hide",
                                [levelID](auto, bool btn2) {
                                    if (btn2) {
                                        hiddenLvls.insert(levelID);
                                        saveLvls();
                                    }
                                }
                            );
                        });
                    }).detach();
                }
            }
        });

        m_fields->listener.setFilter(req.get("https://jarvisdevil.com/abuse/api.php"));

        if (auto leftMenu = getChildByID("left-side-menu")) {
            auto btn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png"),
                this, menu_selector(ADBLevelInfoLayer::onReport)
            );
            leftMenu->addChild(btn);
            leftMenu->updateLayout();

            const auto& username = GJAccountManager::get()->m_username;
            if (!username.empty()) { // its 9:14pm and i am tired
                if (std::find(moderators.begin(), moderators.end(), username) != moderators.end()) {
                    auto adbMod = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
                    adbMod->setColor({0, 255, 0});
                    auto modBtn = CCMenuItemSpriteExtra::create(
                        adbMod,
                        this,
                        menu_selector(ADBLevelInfoLayer::onFlag)
                    );
                    leftMenu->addChild(modBtn);
                    leftMenu->updateLayout();
                }
            }
        }
        return true;
    }

    void onReport(CCObject*) {
        if (auto popup = ReportLevel::create(m_level)) popup->show();
    }

    void onFlag(CCObject*) {
        if (auto popup = FlagLevel::create(m_level)) popup->show();
    }
};