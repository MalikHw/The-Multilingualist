#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../ui/LanguageLayer.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    void onLanguagesBtn(CCObject*) {
        CCDirector::sharedDirector()->pushScene(
            CCTransitionFade::create(0.5f, LanguageLayer::scene())
        );
    }
    bool init() {
        if (!MenuLayer::init()) return false;
        auto menu = this->getChildByID("bottom-menu");
        if (!menu) return false;
        auto spr = CCSprite::create("button.png");
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MyMenuLayer::onLanguagesBtn));
        menu->addChild(btn);
        menu->updateLayout();
        return true;
    }
};