#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "nodes/GeodeTabSprite.hpp"
#include "nodes/CCLayerMultiplexR.hpp"
#include "nodes/DSBorder.hpp"
#include "../managers/translatemgr.hpp"

using namespace geode::prelude;

class LanguageLayer : public CCLayer, public FLAlertLayerProtocol {
public:
    enum class Tab {
        Downloaded = 0,
        Browse = 1,
    };
    struct TabWidgets {
        CCLayer* layer = nullptr;
        LoadingSpinner* loading = nullptr;
        ScrollLayer* list = nullptr;
        CCClippingNode* clipping = nullptr;
        DSBorder* border = nullptr;
        CCLabelBMFont* error = nullptr;
        CCMenu* errorMenu = nullptr;
        CCMenuItemSpriteExtra* resetToFirstPageBtn = nullptr;
        CCMenuItemSpriteExtra* infoButton = nullptr;
        std::string lastErrorCode;
    };
    static LanguageLayer* create();
    static CCScene* scene();
    bool init() override;
    void keyBackClicked() override;
    void FLAlert_Clicked(FLAlertLayer* alert, bool btn2) override;

private:
    Tab m_activeTab = Tab::Browse;

    TabWidgets m_downloadedWidgets;
    TabWidgets m_browseWidgets;

    CCLayerMultiplexR* m_tabHost = nullptr;

    GeodeTabSprite* m_downloadedTabSprite = nullptr;
    CCMenuItemSpriteExtra* m_downloadedTabBtn = nullptr;

    GeodeTabSprite* m_browseTabSprite = nullptr;
    CCMenuItemSpriteExtra* m_browseTabBtn = nullptr;

    CCMenuItemSpriteExtra* m_prevPageBtn = nullptr;
    CCMenuItemSpriteExtra* m_nextPageBtn = nullptr;
    CCLabelBMFont* m_pageLabel = nullptr;

    int m_browsePage = 1;
    int m_downloadedPage = 1;

    matjson::Value m_cachedLanguages;
    async::TaskHolder<web::WebResponse> m_fetchTask;

    std::map<int, std::pair<std::string, std::string>> m_langEntries;
    std::map<int, std::string> m_downloadedEntries;

    TabWidgets createTabWidgets();
    TabWidgets& widgetsForTab(Tab tab);
    TabWidgets& activeWidgets();

    void setTabLoading(TabWidgets& widgets);
    void setTabContentVisible(TabWidgets& widgets, bool showPagination);
    void setTabError(TabWidgets& widgets, char const* text);
    void setPaginationVisible(bool visible);
    void setPageText(std::string const& text);
    void clearList(TabWidgets& widgets);
    void updateTabVisuals();
    void switchTab(Tab tab);
    void refreshPage(CCObject*);

    void showBrowseResults(const matjson::Value& langs);
    void showDownloadedResults();
    void fetchLanguages();

    int& pageForTab(Tab tab);

    void downloadLanguage(std::string const& langName, std::string const& url);
    void applyLanguage(std::string const& langName);

    void selectDownloadedTab(CCObject*);
    void selectBrowseTab(CCObject*);
    void prevPage(CCObject*);
    void nextPage(CCObject*);
    void onBack(CCObject*);
    void onFork(CCObject*);
    void onDownloadBtn(CCObject*);
    void onApplyBtn(CCObject*);
};
// fuck