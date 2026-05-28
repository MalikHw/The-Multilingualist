#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

#include "../helper/idontknowwhattocallthis.hpp"

class $modify(LevelSearchLayer) {
    bool init(int type) {
        if (!LevelSearchLayer::init(type)) return false;

        translatenode(this, ""); // i swear dude listen

        return true;
    }
};