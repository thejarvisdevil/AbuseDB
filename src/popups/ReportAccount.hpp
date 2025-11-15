#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>
#include <fmt/format.h>

using namespace geode::prelude;

class ReportAccount : public Popup<>, public FLAlertLayerProtocol {
    int accID{ 0 };
    TextInput* input{ nullptr };
    CCMenuItemSpriteExtra* sendButton{ nullptr };
    EventListener<web::WebTask> reportListener;

    void onInfo(CCObject*) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        FLAlertLayer::create(
            this,
            "Information",
            "<cg>AbuseDB is a project created by jarvisdevil that serves as an alternative source of in-game moderation and we get work done ASAP. "
            "Make sure to report as much content as you can, this helps AbuseDB users stay safe across all of Geometry Dash without them having to worry about harmful content.</c>\n\n"
            "When an account is flagged by AbuseDB mods, there will be a popup "
            "when you go to the users profile which contains the reason for the flag.\n"
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
                "- Report accounts that are repeatedly harassing others or sharing inappropriate content.\n"
                "- Report accounts that are made to impersonate other people for the purpose of scams or similar.\n"
                "- Report accounts that are involved in botting or heavy amounts of spamming. Likebotting is an example.\n"
                "- Report accounts that are only made for a purpose to troll, upload NSFW or harmful levels or be a menace to society.\n"
                "- Report accounts that have done illegal stuff or anything morally wrong off platform such as homophobia, racism or (insert stuff that gets the mod declined here because its so bad)\n"
                "- Report accounts that are compromised meaning someone has unauthorized access to the account via malware or stolen information.\n\n"
                "<cr>DONT:</c>\n"
                "- Report accounts that are just uploading levels or other content that you dont like.\n"
                "- Report accounts that are just being slightly annoying or if you both were in some argument where its some high school drama type thing.\n"
                "- Report accounts that are posting comments that contain mild profanity (however, repeated usage of slurs is a DO).\n"
                "- Report accounts that have cheated stats. <cy>(Contact Leaderboard mods, they are reliable IMO)</c>\n\n\n"
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

    bool setup() override {
        setTitle("Report Account to AbuseDB");

        auto size = m_mainLayer->getContentSize();

        auto infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSprite, this, menu_selector(ReportAccount::onInfo)
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
        auto discordLabel = CCLabelBMFont::create("Join our Discord Server at https://dsc.gg/devlin\nfor updates or for questions about AbuseDB.", "chatFont.fnt");
        discordLabel->setColor({255, 255, 255});
        discordLabel->setScale(0.8f);
        discordLabel->setAnchorPoint({0.5f, 0.0f});
        discordLabel->setPosition({ winSize.width / 2, 35.f });
        discordLabel->setOpacity(220);
        this->addChild(discordLabel, 1000);

        input = TextInput::create(260.f, "Enter a reason for your report. Please add detail so we don't decline your report.", "chatFont.fnt");
        input->setPosition({ size.width / 2, size.height / 2 + 20.f });
        input->setFilter(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+{}|:\"<>?-=[]\\;',./");
        m_mainLayer->addChild(input);

        sendButton = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Send Report"),
            this, menu_selector(ReportAccount::onSend)
        );
        auto menu = CCMenu::createWithItem(sendButton);
        menu->setPosition({ size.width / 2, size.height / 2 - 40.f });
        m_mainLayer->addChild(menu);

        return true;
    }

    void onSend(CCObject*) {
        if (!GJAccountManager::get()) { return; }

        if (sendButton) sendButton->setEnabled(false);

        auto reason = input->getString();

        std::string reporter = "unknown";
        int reporterID = 0;
        if (GJAccountManager::get()) {
            reporter = GJAccountManager::get()->m_username;
            reporterID = GJAccountManager::get()->m_accountID;
        }

        web::WebRequest req;
        req.header("Content-Type", "application/x-www-form-urlencoded")
           .bodyString(fmt::format(
                "account_id={}&account_reason={}&my_username={}&my_playerID={}",
                accID,
                reason,
                reporter,
                reporterID
            ));
        reportListener.bind([this](web::WebTask::Event* e) {
            if (e->getProgress())
                return;
            if (e->isCancelled())
                return;
            if (auto res = e->getValue()) {
                auto body = res->string().unwrapOr("");
                if (body.rfind("msg_", 0) == 0) {
                    FLAlertLayer::create("Message", body.substr(4), "OK")->show();
                    if (sendButton) sendButton->setEnabled(true);
                    return;
                }
                if (res->code() == 403) {
                    FLAlertLayer::create("Banned", "You have been banned from reporting.\nYou can appeal by joining our Discord Server at dsc.gg/devlin and begging on your knees.", "OK")->show();
                    this->onClose(nullptr);
                    return;
                }
                if (res->code() >= 200 && res->code() < 300) {
                    FLAlertLayer::create("Success", "Report sent successfully, we are taking a look at it now. Just in case we need more information, please have your messages on.", "OK")->show();
                    this->onClose(nullptr);
                } else {
                    FLAlertLayer::create("Error", "Failed to send report. Please try again later.", "OK")->show();
                    if (sendButton) sendButton->setEnabled(true);
                }
            }
        });
        reportListener.setFilter(req.post("https://abuse.jarvisdevil.com/report.php"));
    }

public:
    static ReportAccount* create(int accountID) {
        auto ret = new ReportAccount();
        ret->accID = accountID;
        if (ret && ret->initAnchored(400.f, 200.f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};