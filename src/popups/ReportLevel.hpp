#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class ReportLevel : public Popup<> {
    GJGameLevel* m_level{ nullptr };
    TextInput* m_input{ nullptr };
    EventListener<web::WebTask> m_reportListener;

    bool setup() override {
        setTitle("Report Level to AbuseDB");

        auto size = m_mainLayer->getContentSize();
        m_input = TextInput::create(260.f, "Enter reason for report", "chatFont.fnt");
        m_input->setPosition({ size.width / 2, size.height / 2 + 20.f });
        m_mainLayer->addChild(m_input);

        auto menu = CCMenu::createWithItem(CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Send Report"),
            this, menu_selector(ReportLevel::onSend)
        ));
        menu->setPosition({ size.width / 2, size.height / 2 - 40.f });
        m_mainLayer->addChild(menu);

        return true;
    }

    void onSend(CCObject*) {
        if (!m_level) { return; }
        if (!GJAccountManager::get()) { return; }

        auto reason = m_input->getString();

        web::WebRequest req;
        req.header("Content-Type", "application/x-www-form-urlencoded")
           .bodyString(fmt::format("level_id={}&level_reason={}",
                                   m_level->m_levelID.value(),
                                   reason));
        m_reportListener.bind([](web::WebTask::Event* e) {
            if (e->getProgress())
                return;
            if (e->isCancelled())
                return;
            if (auto res = e->getValue(); res && res->code() >= 200 && res->code() < 300) {
                FLAlertLayer::create("Success", "Report sent successfully, we are taking a look at it.", "OK")->show();
            } else {
                FLAlertLayer::create("Error", "Failed to send report. Please try again.", "OK")->show();
            }
        });
        m_reportListener.setFilter(req.post("https://jarvisdevil.com/abuse/report.php"));
    }

public:
    static ReportLevel* create(GJGameLevel* level) {
        auto ret = new ReportLevel();
        ret->m_level = level;
        if (ret && ret->initAnchored(400.f, 200.f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};