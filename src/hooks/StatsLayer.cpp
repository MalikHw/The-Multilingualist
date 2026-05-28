#include <Geode/Geode.hpp>
#include <Geode/modify/StatsLayer.hpp>

using namespace geode::prelude;

#include "../helper/idontknowwhattocallthis.hpp"

class $modify(StatsLayer) {
    void customSetup() {
        StatsLayer::customSetup();

        translatenode(this, "");
    }
};