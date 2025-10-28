#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include "globals.hpp"

using namespace geode::prelude;

class $modify(ADBMenuLayer, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        auto menu = this->getChildByID("right-side-menu");
        auto icon = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
        icon->setScale(0.75f);
        auto item = CCMenuItemSpriteExtra::create(icon, this, menu_selector(ADBMenuLayer::adbMenu));
        menu->addChild(item);
        menu->updateLayout();
        setKeypadEnabled(true);

        return true;
    }

    void adbMenu(CCObject*) {
        constexpr float boxW = 336, boxH = 270, btnW = 120, btnH = 60;
        struct Btn { const char* label; const char* link; };
        Btn btns[] = {
            {"Flags", "https://abuse.jarvisdevil.com/flags.php"},
            {"Website", "https://abuse.jarvisdevil.com/"},
            {"Discord", "https://dsc.gg/devlin"},
            {"Source", "https://github.com/thejarvisdevil/AbuseDB"},
            {"YouTube", "https://www.youtube.com/@jarvisdevlin"},
            {"Credits", ""},
        };

        if (CCDirector::sharedDirector()->getRunningScene()->getChildByTag(0xADB)) return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto mmm = CCLayerColor::create({0,0,0,0});
        mmm->setTag(0xADB);

        auto bg = CCScale9Sprite::create("GJ_square01.png");
        bg->setContentSize({boxW, boxH});
        bg->setPosition({winSize.width / 2, winSize.height / 2});
        bg->setScale(0.7f);
        mmm->addChild(bg);

        bg->runAction(CCEaseBackOut::create(CCScaleTo::create(0.25f, 1.0f)));
        auto titleLabel = CCLabelBMFont::create("AbuseDB Menu", "goldFont.fnt");
        titleLabel->setPosition({boxW/2, boxH - 32});
        bg->addChild(titleLabel);

        auto menu = CCMenu::create();
        for (int i = 0; i < 6; ++i) {
            float x = 84 + (i % 2) * 168;
            float y = boxH - 80 - (i / 2) * 60;
            auto btnSprite = ButtonSprite::create(btns[i].label, btnW, true, "goldFont.fnt", "GJ_button_01.png", 30, 1.f);
            auto btn = CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(ADBMenuLayer::onADBLink));
            btn->setTag(i);
            btn->setPosition({x, y});
            menu->addChild(btn);
        }
        menu->setPosition({0, 0});
        bg->addChild(menu);

        auto closeBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"), this, menu_selector(ADBMenuLayer::diediedie));
        closeBtn->setPosition({0, boxH});
        auto closeMenu = CCMenu::createWithItem(closeBtn);
        closeMenu->setPosition({10, -10});
        bg->addChild(closeMenu);

        auto refreshBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"), this, menu_selector(ADBMenuLayer::ADBRefreshAll));
        refreshBtn->setPosition({boxW, 0});
        auto refreshMenu = CCMenu::createWithItem(refreshBtn);
        refreshMenu->setPosition({-10, 10});
        bg->addChild(refreshMenu);

        CCDirector::sharedDirector()->getRunningScene()->addChild(mmm, 999);
    }

    void keyBackClicked() {
        if (CCDirector::sharedDirector()->getRunningScene()->getChildByTag(0xADB)) {
            diediedie(nullptr);
            return;
        }
        MenuLayer::keyBackClicked();
    }

    void ADBRefreshAll(CCObject* sender) {
        refreshAll();
        FLAlertLayer::create(
            "Refreshed",
            "All AbuseDB data has been refreshed.",
            "OK"
        )->show();
    }

    void onADBLink(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        int tag = btn->getTag();
        switch (tag) {
            case 0: web::openLinkInBrowser("https://abuse.jarvisdevil.com/flags.php"); break;
            case 1: web::openLinkInBrowser("https://abuse.jarvisdevil.com/"); break;
            case 2: web::openLinkInBrowser("https://dsc.gg/devlin"); break;
            case 3: web::openLinkInBrowser("https://github.com/thejarvisdevil/AbuseDB"); break;
            case 4: web::openLinkInBrowser("https://www.youtube.com/@jarvisdevlin"); break;
            case 5:
                FLAlertLayer::create(
                    "Thanks!",
                    "<cg>AbuseDB (+ Geode Integration) created by jarvisdevil.</c> (jarvisdevil.com)\n"
                    "<cy>and a very special thanks to all the AbuseDB Moderators for helping out!</c>",
                    "OK"
                )->show();
                break;
        }
    }

    void diediedie(CCObject*) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (auto layer = scene->getChildByTag(0xADB)) {
            layer->removeFromParentAndCleanup(true);
        }
    }
};