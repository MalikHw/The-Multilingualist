#include <Geode/Geode.hpp>
#include <Geode/modify/InfoLayer.hpp>

using namespace geode::prelude;

#include "../helper/idontknowwhattocallthis.hpp"

class $modify(InfoLayer) {
    bool init(GJGameLevel* level, GJUserScore* score, GJLevelList* list) {
        if (!InfoLayer::init(level,score,list)) return false;
        translatenode(this, "");

        return true;
    }
};