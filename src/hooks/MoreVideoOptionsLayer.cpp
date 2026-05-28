#include <Geode/Geode.hpp>
#include <Geode/modify/MoreVideoOptionsLayer.hpp>

#include "../helper/idontknowwhattocallthis.hpp"

using namespace geode::prelude;

#define FORK false

// only works with my own fork of nodeIDs

#ifdef FORK
class $modify(MoreVideoOptionsLayer) {
    bool init() {
        if (!MoreVideoOptionsLayer::init()) return false;

        auto main = this->getChildByType<CCLayer>(0);
        if (!main) return false;

        translatenode(main, "MoreVideoOptionsLayer");

        return true;
    }
};
#endif