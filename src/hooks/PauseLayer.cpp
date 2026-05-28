#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

#include "../helper/idontknowwhattocallthis.hpp"

class $modify(PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        translatenode(this, "");
    }
};