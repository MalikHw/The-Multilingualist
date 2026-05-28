#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>

using namespace geode::prelude;

#include "../helper/idontknowwhattocallthis.hpp"

class $modify(LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;

        //auto main = this->getChildByID("main-layer");
        //if (!main) return false;

        //translatenode(menu, "LevelSelectLayer");
        translatenode(this, "LevelSelectLayer");

        return true;
    }
};