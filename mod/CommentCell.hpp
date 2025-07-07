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
        if (std::find(moderators.begin(), moderators.end(), name) == moderators.end()) return;

        if (auto menu = m_mainLayer->getChildByIDRecursive("username-menu")) {
            auto sprite = CCSprite::create("admin.png"_spr);
            sprite->setScale(0.7f);
        
            auto item = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(ADBCommentCell::onBadge));
            menu->addChild(item);
            menu->updateLayout();
        }
    }

    void onBadge(CCObject*) {}
};