#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class FlagLevel : public Popup<> {
    GJGameLevel* m_level{ nullptr };
    TextInput* m_reason{ nullptr };
    TextInput* m_antistupid{ nullptr };
    CCMenuItemSpriteExtra* sendButton{ nullptr };
    EventListener<web::WebTask> m_flagListener;

    bool setup() override {
        setTitle("MOD: Flag Level");
        auto size = m_mainLayer->getContentSize();

        m_reason = TextInput::create(260.f, "Reason", "chatFont.fnt");
        m_reason->setPosition({ size.width / 2, size.height / 2 + 30.f });
        m_reason->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+{}|:\"<>?-=[]\\;',./");
        m_mainLayer->addChild(m_reason);

        m_antistupid = TextInput::create(260.f, "Password", "chatFont.fnt");
        m_antistupid->setPosition({ size.width / 2, size.height / 2 - 10.f });
        m_antistupid->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+{}|:\"<>?-=[]\\;',./");
        m_antistupid->setPasswordMode(true);
        m_mainLayer->addChild(m_antistupid);

        sendButton = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Send Flag"),
            this, menu_selector(FlagLevel::onSend)
        );
        auto menu = CCMenu::createWithItem(sendButton);
        menu->setPosition({ size.width / 2, size.height / 2 - 60.f });
        m_mainLayer->addChild(menu);

        return true;
    }

    void onSend(CCObject*) {
        if (!m_level) { return; }
        if (!GJAccountManager::get()) { return; }
        if (sendButton) sendButton->setEnabled(false);

        auto reason = m_reason->getString();
        auto password = m_antistupid->getString();

        web::WebRequest req;
        req.header("Content-Type", "application/x-www-form-urlencoded")
           .bodyString(fmt::format("level_id={}&reason={}&password={}",
                                   m_level->m_levelID.value(),
                                   reason, password));
        m_flagListener.bind([this](web::WebTask::Event* e) {
            if (e->getProgress()) return;
            if (e->isCancelled()) return;
            if (auto res = e->getValue()) {
                auto body = res->string().unwrapOr("");
                if (body.rfind("msg_", 0) == 0) {
                    FLAlertLayer::create("Message", body.substr(4), "OK")->show();
                    if (sendButton) sendButton->setEnabled(true);
                    return;
                }
                if (res->code() >= 200 && res->code() < 300) {
                    FLAlertLayer::create("Flagged", "The content has been successfully flagged.", "OK")->show();
                    this->onClose(nullptr);
                } else {
                    FLAlertLayer::create("Error", "Failed to flag content. Please try again.", "OK")->show();
                    if (sendButton) sendButton->setEnabled(true);
                }
            }
        });
        m_flagListener.setFilter(req.post("https://api.jarvisdevil.com/abusedb/flagLevelGeode.php"));
    }

public:
    static FlagLevel* create(GJGameLevel* level) {
        auto ret = new FlagLevel();
        ret->m_level = level;
        if (ret && ret->initAnchored(400.f, 200.f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};