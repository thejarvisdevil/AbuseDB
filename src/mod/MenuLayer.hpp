#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(ADBMenuLayer, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        auto menu = this->getChildByID("bottom-menu");
        auto icon = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
        icon->setScale(1.05f);
        auto item = CCMenuItemSpriteExtra::create(icon, this, menu_selector(ADBMenuLayer::onOpenFlags));
        menu->addChild(item);
        menu->updateLayout();

        return true;
    }

    void onOpenFlags(CCObject*) {
        geode::createQuickPopup(
            "AbuseDB",
            "Do you want to view flags? (This will open a website in your default browser)",
            "No",
            "Yes",
            [](auto, bool btn2) {
                if (btn2) {
                    web::openLinkInBrowser("https://jarvisdevil.com/abuse/flags.php");
                }
            }
        );
    }
};