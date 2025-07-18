#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class ReportLevel : public Popup<>, public FLAlertLayerProtocol {
    GJGameLevel* m_level{ nullptr };
    TextInput* m_input{ nullptr };
    CCMenuItemSpriteExtra* sendButton{ nullptr };
    EventListener<web::WebTask> m_reportListener;

    bool setup() override {
        setTitle("Report Level to AbuseDB");

        auto size = m_mainLayer->getContentSize();

        auto infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSprite, this, menu_selector(ReportLevel::onInfo)
        );
        infoBtn->setPosition({ size.width - 20.f, size.height - 20.f });
        auto infoMenu = CCMenu::createWithItem(infoBtn);
        infoMenu->setPosition({0, 0});
        m_mainLayer->addChild(infoMenu);

        auto info = CCLabelBMFont::create(
            "Only report for serious issues and use the correct report type.\nPress the INFO button for more info.",
            "chatFont.fnt"
        );
        info->setColor({255, 255, 255});
        info->setScale(0.7f);
        info->setPosition({ size.width / 2, size.height / 2 + 55.f });
        m_mainLayer->addChild(info);

        auto warning = CCLabelBMFont::create("Repeated misuse of AbuseDB will lead to a permanent ban.", "chatFont.fnt");
        warning->setColor({0, 255, 255});
        warning->setScale(0.9f);
        warning->setPosition({ size.width / 2, 30.f });
        m_mainLayer->addChild(warning);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto discordLabel = CCLabelBMFont::create("Join our Discord Server at https://dsc.gg/devlin for updates or for questions about AbuseDB.", "chatFont.fnt");
        discordLabel->setColor({255, 255, 255});
        discordLabel->setScale(0.8f);
        discordLabel->setAnchorPoint({0.5f, 0.0f});
        discordLabel->setPosition({ winSize.width / 2, 40.f });
        discordLabel->setOpacity(220);
        this->addChild(discordLabel, 1000);

        m_input = TextInput::create(260.f, "Enter a reason for your report. Please add detail so we don't decline your report.", "chatFont.fnt");
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

    void onInfo(CCObject*) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        FLAlertLayer::create(
            this,
            "Information",
            "<cg>AbuseDB is a project created by jarvisdevil that serves as an alternative source of in-game moderation and we get work done ASAP. "
            "Make sure to report as much content as you can, this helps AbuseDB users stay safe across all of Geometry Dash without them having to worry about harmful content.</c>\n\n"
            "When a level is flagged by AbuseDB mods, there will be a popup "
            "when you go to the level which contains the reason for the flag.\n"
            "<cr>To avoid abuse of this system, we will ban accounts from reporting that repeatedly "
            "misuse this form and we also have filters to prevent abuse.</c>\n"
            "<co>To make sure we can properly handle your report, please make sure that you provide a valid reason, "
            "must not be gibberish and contain detail so we don't get lost/confused.</c>\n"
            "When reporting please make sure that your standards are high and that you only report content that is truly harmful to "
            "the Geometry Dash community.\n <cj>To help with this, here are some DO and DONTs when it comes to reporting so...</c>",
            "Next Page",
            nullptr,
            winSize.width - 40.f,
            true,
            winSize.height - 20.f,
            1.0f
        )->show();
    }

    void FLAlert_Clicked(FLAlertLayer*, bool btn2) override {
        // this is so goofy
        if (!btn2) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            FLAlertLayer::create(
                nullptr,
                "DOs and DON'Ts",
                "<cg>DO:</c>\n"
                "- Report levels that is made to harass someone.\n"
                "- Report levels that are uploaded by bots.\n"
                "- Report levels that are only made for a purpose to troll, includes NSFW or harmful content or anything else that is as bad.\n"
                "- Report levels that are stolen.\n\n"
                "<cr>DONT:</c>\n"
                "- Report levels for deletion or for a star rating, we are NOT mods.\n"
                "- Report levels meant to crash your game.\n"
                "- Report levels that have mild profanity (except use of slurs), no one cares about it anyways.\n"
                "- Report levels that are way too hard for you or impossible.\n\n\n"
                "<cg>If you have any questions about what to report or not, please join our Discord Server at dsc.gg/devlin and ask there. Thank you for using AbuseDB!</c>",
                "OK",
                nullptr,
                winSize.width - 40.f,
                true,
                winSize.height - 20.f,
                1.0f
            )->show();
        }
    }

    void onSend(CCObject*) {
        if (!m_level) { return; }
        if (!GJAccountManager::get()) { return; }

        if (sendButton) sendButton->setEnabled(false);

        auto reason = m_input->getString();

        std::string reporter = "unknown";
        int reporterID = 0;
        if (GJAccountManager::get()) {
            reporter = GJAccountManager::get()->m_username;
            reporterID = GJAccountManager::get()->m_accountID;
        }

        web::WebRequest req;
        req.header("Content-Type", "application/x-www-form-urlencoded")
           .bodyString(fmt::format("level_id={}&level_reason={}&my_username={}&my_playerID={}",
                                   m_level->m_levelID.value(),
                                   reason, reporter, reporterID));
        m_reportListener.bind([this](web::WebTask::Event* e) {
            if (e->getProgress())
                return;
            if (e->isCancelled())
                return;
            if (auto res = e->getValue()) {
                if (res->code() == 403) {
                    FLAlertLayer::create("Banned", "You have been banned from reporting.\nYou can appeal by joining our Discord Server at dsc.gg/devlin and begging on your knees.", "OK")->show();
                    this->onClose(nullptr);
                    return;
                }
                if (res->code() >= 200 && res->code() < 300) {
                    FLAlertLayer::create("Success", "Report sent successfully, we are taking a look at it now. Just in case we need more information, please have your messages on.", "OK")->show();
                    this->onClose(nullptr);
                } else {
                    FLAlertLayer::create("Error", "Failed to send report. Please try again laters.", "OK")->show();
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