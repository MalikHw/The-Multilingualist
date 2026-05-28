#include <Geode/Geode.hpp>
#include <Geode/modify/VideoOptionsLayer.hpp>

#include "../helper/idontknowwhattocallthis.hpp"

using namespace geode::prelude;

#define FORK false

// only works with my own fork of nodeIDs

#ifdef FORK
class $modify(VideoOptionsLayer) {
    bool init() {
        if (!VideoOptionsLayer::init()) return false;

        auto main = this->getChildByType<CCLayer>(0);
        if (!main) return false;

        translatenode(main, "VideoOptionsLayer");

        return true;
    }
};
#endif