#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "managers/translatemgr.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    auto& mgr = translatemgr::get();
    mgr.load();
    log::info("asd");

    Mod::get()->setSavedValue("shown-thanks", false);
}

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (!Mod::get()->getSettingValue<bool>("shown-thanks")) {
            FLAlertLayer::create(
                "Thank You!",
                "Thank you for using <ca>The Multilingualist</ca> mod for Geometry Dash!!\n\nIf you would like, you can contribute to translating languages!",
                "OK"
            )->show();

            Mod::get()->setSavedValue("shown-thanks", true);
        }

        return true;
    }
};