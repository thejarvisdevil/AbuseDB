#pragma once
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/binding/GJUserScore.hpp>
#include <Geode/utils/web.hpp>
#include "../globals.hpp"
#include "popups/ReportAccount.hpp"

using namespace geode::prelude;

class $modify(ADBProfilePage, ProfilePage) {
    struct Fields {
        EventListener<web::WebTask> accListener;
        bool addedReportBtn{ false };
        bool addedBadge{ false };
    };

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);
        int accID = score->m_accountID;

        if (!hiddenAccs.count(accID)) {
            web::WebRequest req;
            req.param("api", "account")
               .param("id", std::to_string(accID))
               .timeout(std::chrono::seconds(5));

            m_fields->accListener.bind([accID](web::WebTask::Event* e) {
                if (auto res = e->getValue(); res && res->code() >= 200 && res->code() < 300) {
                    auto body = res->string().unwrapOr("");
                    if (body != "-1") {
                        std::thread([body, accID]() {
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                            geode::queueInMainThread([body, accID]() {
                                geode::createQuickPopup(
                                    "AbuseDB",
                                    fmt::format("This account has been flagged by AbuseDB.\n\nReason: {}", body),
                                    "I Understand", "Always Hide",
                                    [accID](auto, bool btn2) {
                                        if (btn2) {
                                            hiddenAccs.insert(accID);
                                            saveAccs();
                                        }
                                    }
                                );
                            });
                        }).detach();
                    }
                }
            });
            m_fields->accListener.setFilter(req.get("https://jarvisdevil.com/abuse/api.php"));
        }

        if (!m_fields->addedReportBtn) {
            if (auto menu = static_cast<CCMenu*>(m_mainLayer->getChildByIDRecursive("left-menu"))) {
                auto reportBtn = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
                reportBtn->setScale(0.7f);
                auto btn = CCMenuItemSpriteExtra::create(
                    reportBtn, this, menu_selector(ADBProfilePage::onReportAcc)
                );
                menu->addChild(btn);
                menu->updateLayout();
                m_fields->addedReportBtn = true;
            }
        }

        auto& name = score->m_userName;
        if (!m_fields->addedBadge
            && std::find(moderators.begin(), moderators.end(), name) != moderators.end())
        {
            this->scheduleOnce(
                schedule_selector(ADBProfilePage::addBadge),
                0.1f
            );
            m_fields->addedBadge = true;
        }
    }

    void onReportAcc(CCObject*) {
        if (auto popup = ReportAccount::create(m_score->m_accountID)) {
            popup->show();
        }
    }

private:
    void addBadge(float) {
        auto menu = static_cast<CCMenu*>(m_mainLayer->getChildByIDRecursive("username-menu"));
        if (!menu) return;
        if (menu->getChildByIDRecursive("abusedb-badge")) return;

        auto badge = CCSprite::create("admin.png"_spr);
        badge->setScale(0.95f);
        badge->setID("abusedb-badge"_spr);

        menu->addChild(badge);
        menu->updateLayout();
    }
};