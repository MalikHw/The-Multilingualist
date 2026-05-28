#pragma once

#include <Geode/Geode.hpp>
#include "../managers/translatemgr.hpp"

using namespace geode::prelude;

inline void translatenode(CCNode* node, std::string const& scope) {
    if (!node) return;

    std::string sid = node->getID();

    if (sid.empty()) {
        if (auto lbl = typeinfo_cast<CCLabelBMFont*>(node)) {
            sid = lbl->getString();
        } else if (auto lbl2 = typeinfo_cast<CCLabelTTF*>(node)) {
            sid = lbl2->getString();
        }
    }

    std::string key = scope;

    if (!sid.empty()) {
        key = scope.empty() ? sid : scope + "." + sid;
    }

    auto tryTranslate = [&](std::string const& baseKey, std::string const& textFallback) -> std::string {
        auto res = translatemgr::get().gettr(baseKey);

        if (res != baseKey) return res;

        if (!textFallback.empty()) {
            auto txtKey = baseKey + "." + textFallback;
            auto res2 = translatemgr::get().gettr(txtKey);
            if (res2 != txtKey) return res2;
        }

        return baseKey;
    };

    if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
        std::string txt = label->getString();
        auto translated = tryTranslate(key, txt);

        if (translated != key) {
            label->setString(translated.c_str());
        }
    }

    if (auto label2 = typeinfo_cast<CCLabelTTF*>(node)) {
        std::string txt = label2->getString();
        auto translated = tryTranslate(key, txt);

        if (translated != key) {
            label2->setString(translated.c_str());
        }
    }

    auto children = node->getChildren();
    if (!children) return;

    for (auto obj : CCArrayExt(children)) {
        auto child = typeinfo_cast<CCNode*>(obj);
        translatenode(child, key);
    }
}