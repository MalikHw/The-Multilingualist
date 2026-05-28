#include "translatemgr.hpp"

using namespace geode::prelude;

translatemgr& translatemgr::get() {
    static translatemgr inst;
    return inst;
}

void translatemgr::load() {
    static bool loaded = false;
    if (loaded) return;
    loaded = true;

    auto path = Mod::get()->getResourcesDir() / "pl_PL.json";

    auto res = file::readString(path);
    if (!res) {
        log::error("FAILED TO READ FILE");
        return;
    }

    auto json = matjson::parse(res.unwrap());
    if (!json) {
        log::error("FAILED TO PARSE JSON");
        return;
    }

    m_json = json.unwrap();

    log::info("translator loaded");
}

std::string translatemgr::gettr(std::string const& key) {
    if (m_json.isNull()) return key;

    auto parts = utils::string::split(key, ".");

    matjson::Value cur = m_json;

    for (auto const& p : parts) {
        if (!cur.contains(p))
            return key;

        cur = cur[p];
    }

    if (!cur.isString()) return key;

    return cur.asString().unwrapOr(key);
}