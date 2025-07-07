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
    CCMenuItemSpriteExtra* sendButton{ nullptr };
    EventListener<web::WebTask> m_reportListener;

    bool setup() override {
        setTitle("Report Level to AbuseDB");

        auto size = m_mainLayer->getContentSize();

        auto info = CCLabelBMFont::create(
            "Only report for serious issues such as inappropriate content, harassment, etc.\nDO NOT beg for rates here, we aren't associated with RobTop.",
            "chatFont.fnt"
        );
        info->setColor({255, 255, 255});
        info->setScale(0.7f);
        info->setPosition({ size.width / 2, size.height / 2 + 55.f });
        m_mainLayer->addChild(info);

        auto warning = CCLabelBMFont::create("Repeated misuse of AbuseDB will lead to a permanant ban.", "chatFont.fnt");
        warning->setColor({0, 255, 255});
        warning->setScale(0.9f);
        warning->setPosition({ size.width / 2, 30.f });
        m_mainLayer->addChild(warning);

        m_input = TextInput::create(260.f, "Enter reason for report. You should also provide your username here\nin case we need to contact you. (turn messages on)", "chatFont.fnt");
        m_input->setPosition({ size.width / 2, size.height / 2 + 20.f });
        m_mainLayer->addChild(m_input);

        sendButton = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Send Report"),
            this, menu_selector(ReportLevel::onSend)
        );
        auto menu = CCMenu::createWithItem(sendButton);
        menu->setPosition({ size.width / 2, size.height / 2 - 40.f });
        m_mainLayer->addChild(menu);

        return true;
    }

    void onSend(CCObject*) {
        if (!m_level) { return; }
        if (!GJAccountManager::get()) { return; }

        if (sendButton) sendButton->setEnabled(false);

        auto reason = m_input->getString();

        web::WebRequest req;
        req.header("Content-Type", "application/x-www-form-urlencoded")
           .bodyString(fmt::format("level_id={}&level_reason={}",
                                   m_level->m_levelID.value(),
                                   reason));
        m_reportListener.bind([this](web::WebTask::Event* e) {
            if (e->getProgress())
                return;
            if (e->isCancelled())
                return;
            if (auto res = e->getValue()) {
                if (res->code() == 403) {
                    FLAlertLayer::create("Banned", "You have been banned from reporting.\nIf you have any real reports then consider joining our Discord Server at dsc.gg/devlin.", "OK")->show();
                    this->onClose(nullptr);
                    return;
                }
                if (res->code() >= 200 && res->code() < 300) {
                    FLAlertLayer::create("Success", "Report sent successfully, we are taking a look at it.", "OK")->show();
                    this->onClose(nullptr);
                } else {
                    FLAlertLayer::create("Error", "Failed to send report. Please try again.", "OK")->show();
                    if (sendButton) sendButton->setEnabled(true);
                }
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