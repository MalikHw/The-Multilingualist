#include <Geode/Geode.hpp>
#include <Geode/modify/OptionsLayer.hpp>

using namespace geode::prelude;

#include "../helper/idontknowwhattocallthis.hpp"

class $modify(OptionsLayer) {
    void customSetup() {
        OptionsLayer::customSetup();

        auto main = this->getChildByType<CCLayer>(0);
        if (!main) return;

        translatenode(main, "OptionsLayer");
    }
};