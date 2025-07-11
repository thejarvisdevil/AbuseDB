#pragma once
#include <Geode/modify/CommentCell.hpp>
#include <Geode/binding/GJComment.hpp>
#include "../globals.hpp"

using namespace geode::prelude;

class $modify(ADBCommentCell, CommentCell) {
    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);
        if (comment->m_levelID <= 0) return;

        auto& name = comment->m_userScore->m_userName;

        if (std::find(moderators.begin(), moderators.end(), name) != moderators.end()) {
            if (auto menu = m_mainLayer->getChildByIDRecursive("username-menu")) {
                auto sprite = CCSprite::create("admin.png"_spr);
                sprite->setScale(0.7f);
                auto item = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(ADBCommentCell::onBadge));
                menu->addChild(item);
                menu->updateLayout();
            }

            if (auto commentLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByIDRecursive("comment-text-label"))) {
                commentLabel->setColor({255, 199, 0});
            }
        }

        if (std::find(flaggedUsers.begin(), flaggedUsers.end(), name) != flaggedUsers.end()) {
            if (auto menu = m_mainLayer->getChildByIDRecursive("username-menu")) {
                for (auto child : CCArrayExt<CCNode*>(menu->getChildren())) {
                    if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(child)) {
                        if (auto label = typeinfo_cast<CCLabelBMFont*>(btn->getChildren()->objectAtIndex(0))) {
                            label->setColor({220, 0, 0});
                        }
                    }
                }
            }
        }
    }

    void onBadge(CCObject*) {
        /* modify the code here, what exactly are you trying to do? */
        FLAlertLayer::create("AbuseDB Mod", "This badge <cl>indicates</c> an <cp>AbuseDB mod.</c> Their main task is to <cf>read reports</c> or <cr>flag</c> users and levels.", "OK")->show();
    }
};
