#pragma once

#include <Geode/Geode.hpp>

class translatemgr {
    public:
        static translatemgr& get();
        void load();
        std::string gettr(std::string const& key);
    private:
        matjson::Value m_json;
};