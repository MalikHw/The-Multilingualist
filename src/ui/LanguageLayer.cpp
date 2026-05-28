#include "LanguageLayer.hpp"
#include <Geode/utils/web.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

LanguageLayer::TabWidgets& LanguageLayer::widgetsForTab(Tab tab) {
    if (tab == Tab::Downloaded) return m_downloadedWidgets;
    return m_browseWidgets;
}
LanguageLayer::TabWidgets& LanguageLayer::activeWidgets() {
    return widgetsForTab(m_activeTab);
}
int& LanguageLayer::pageForTab(Tab tab) {
    if (tab == Tab::Downloaded) return m_downloadedPage;
    return m_browsePage;
}
void LanguageLayer::setTabLoading(TabWidgets& widgets) {
    widgets.loading->setVisible(true);
    widgets.clipping->setVisible(true);
    widgets.border->setVisible(true);
    widgets.error->setVisible(false);
    if (widgets.errorMenu) widgets.errorMenu->setVisible(false);
    if (m_pageLabel) m_pageLabel->setVisible(true);
    if (widgets.infoButton) widgets.infoButton->setVisible(false);
}
void LanguageLayer::setTabContentVisible(TabWidgets& widgets, bool showPagination) {
    widgets.loading->setVisible(false);
    widgets.error->setVisible(false);
    if (widgets.errorMenu) widgets.errorMenu->setVisible(false);
    widgets.clipping->setVisible(true);
    widgets.border->setVisible(true);
    setPaginationVisible(showPagination);
    if (m_pageLabel) m_pageLabel->setVisible(true);
    if (widgets.infoButton) widgets.infoButton->setVisible(false);
}
void LanguageLayer::setTabError(TabWidgets& widgets, char const* text) {
    widgets.loading->setVisible(false);
    widgets.error->setString(text);
    widgets.lastErrorCode = text ? text : "";
    widgets.error->setVisible(true);
    if (widgets.errorMenu) widgets.errorMenu->setVisible(false);
    clearList(widgets);
    widgets.clipping->setVisible(true);
    widgets.border->setVisible(true);
    setPaginationVisible(false);
    if (m_pageLabel) m_pageLabel->setVisible(false);
}
void LanguageLayer::setPaginationVisible(bool visible) {
    if (m_prevPageBtn) m_prevPageBtn->setVisible(visible);
    if (m_nextPageBtn) m_nextPageBtn->setVisible(visible);
}
void LanguageLayer::setPageText(std::string const& text) {
    if (m_pageLabel) {
        m_pageLabel->setString(text.c_str());
        m_pageLabel->setVisible(true);
    }
}
void LanguageLayer::clearList(TabWidgets& widgets) {
    widgets.list->m_contentLayer->removeAllChildrenWithCleanup(true);
    widgets.list->m_contentLayer->setContentSize({ widgets.list->m_contentLayer->getContentWidth(), 0.f });
}
LanguageLayer::TabWidgets LanguageLayer::createTabWidgets() {
    TabWidgets widgets;
    constexpr float kTabContentYOffset = -18.f;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    widgets.layer = CCLayer::create();
    widgets.layer->setContentSize(winSize);
    widgets.layer->setPosition({ winSize.width / 2, (winSize.height / 2) + kTabContentYOffset });

    widgets.loading = LoadingSpinner::create(100.f);
    widgets.layer->addChildAtPosition(widgets.loading, Anchor::Center);

    widgets.list = ScrollLayer::create({ 340.f, 220.f });

    auto columnLayout = ColumnLayout::create();
    columnLayout->setAxisReverse(true)
        ->setAutoGrowAxis(widgets.list->getContentHeight())
        ->setCrossAxisOverflow(false)
        ->setAxisAlignment(AxisAlignment::Center)
        ->setGap(0.f);
    widgets.list->m_contentLayer->setLayout(columnLayout);

    widgets.clipping = CCClippingNode::create();
    widgets.clipping->setContentSize({ 340.f, 220.f });
    widgets.clipping->setAnchorPoint({ 0.f, 0.f });
    widgets.clipping->setPosition({ 50.f, 27.f });

    auto stencil = CCLayerColor::create(ccc4(255, 255, 255, 255), 340.f, 220.f);
    stencil->setAnchorPoint({ 0.f, 0.f });
    widgets.clipping->setStencil(stencil);

    widgets.list->setPosition({ 0.f, 0.f });
    widgets.clipping->addChild(widgets.list);

    widgets.border = DSBorder::create(widgets.list, { 75, 75, 75, 255 }, { 340.f, 220.f });
    widgets.border->setPosition({ 0.f, 0.f });
    widgets.border->setVisible(false);
    widgets.clipping->addChild(widgets.border);
    widgets.layer->addChild(widgets.clipping);
    widgets.loading->setZOrder(10);

    widgets.error = CCLabelBMFont::create("", "bigFont.fnt", 350.f);
    widgets.error->setPosition(winSize / 2);
    widgets.error->setScale(0.5f);
    widgets.error->setVisible(false);
    widgets.layer->addChild(widgets.error);

    widgets.errorMenu = CCMenu::create();
    widgets.errorMenu->setPosition({ 0.f, 0.f });
    widgets.errorMenu->setVisible(false);
    widgets.layer->addChild(widgets.errorMenu);

    widgets.infoButton = nullptr;
    widgets.lastErrorCode = "";
    return widgets;
}
void LanguageLayer::updateTabVisuals() {
    if (m_downloadedTabSprite) {
        m_downloadedTabSprite->select(m_activeTab == Tab::Downloaded);
        m_downloadedTabSprite->disable(m_activeTab != Tab::Downloaded);
    }
    if (m_browseTabSprite) {
        m_browseTabSprite->select(m_activeTab == Tab::Browse);
        m_browseTabSprite->disable(m_activeTab != Tab::Browse);
    }
}
void LanguageLayer::switchTab(Tab tab) {
    if (m_activeTab == tab) return;
    m_activeTab = tab;
    m_tabHost->switchTo(static_cast<unsigned>(tab));
    updateTabVisuals();
    refreshPage(nullptr);
}
void LanguageLayer::fetchLanguages() {
    auto& widgets = widgetsForTab(Tab::Browse);
    setTabLoading(widgets);

    auto req = web::WebRequest();
    m_fetchTask.spawn(
        req.get("https://miskaa.pl/themultilinguist/languages.json"),
        [this](web::WebResponse res) {
            if (!res.ok()) {
                setTabError(widgetsForTab(Tab::Browse), fmt::format("HTTP Error {}", res.code()).c_str());
                return;
            }
            auto json = res.json();
            if (!json) {
                setTabError(widgetsForTab(Tab::Browse), "Failed to parse languages.");
                return;
            }
            m_cachedLanguages = json.unwrap();
            if (m_activeTab == Tab::Browse) {
                showBrowseResults(m_cachedLanguages);
            }
        }
    );
}
void LanguageLayer::showBrowseResults(const matjson::Value& langs) {
    auto& widgets = widgetsForTab(Tab::Browse);
    setTabContentVisible(widgets, false);
    clearList(widgets);

    if (!langs.isObject()) {
        setTabError(widgets, "Invalid language list.");
        return;
    }
    int index = 0;
    float totalHeight = 0.f;

    for (auto it = langs.begin(); it != langs.end(); ++it) {
        auto keyOpt = it->getKey();
        if (!keyOpt) continue;
        std::string langName = *keyOpt;
        std::string url = it->asString().unwrapOr("");

        auto row = CCLayerColor::create(index % 2 == 0 ? ccc4(75, 75, 75, 255) : ccc4(50, 50, 50, 255));
        row->setContentSize({ 340.f, 40.f });

        auto nameLabel = CCLabelBMFont::create(langName.c_str(), "bigFont.fnt");
        nameLabel->setAnchorPoint({ 0.f, 0.5f });
        nameLabel->setScale(0.45f);
        nameLabel->setPosition({ 10.f, 20.f });
        row->addChild(nameLabel);

        auto rowMenu = CCMenu::create();
        rowMenu->setPosition({ 0.f, 0.f });

        auto downloadSprite = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
        downloadSprite->setScale(0.6f);
        auto downloadBtn = CCMenuItemSpriteExtra::create(
            downloadSprite,
            this,
            menu_selector(LanguageLayer::onDownloadBtn)
        );
        downloadBtn->setPosition({ 315.f, 20.f });
        m_langEntries[index] = { langName, url };
        downloadBtn->setTag(index);

        rowMenu->addChild(downloadBtn);
        row->addChild(rowMenu);

        widgets.list->m_contentLayer->addChild(row);
        totalHeight += 40.f;
        widgets.list->m_contentLayer->setContentSize({ widgets.list->m_contentLayer->getContentSize().width, totalHeight });
        ++index;
    }

    widgets.list->m_contentLayer->updateLayout();
    widgets.list->scrollToTop();
    widgets.border->setVisible(true);

    if (index == 0) {
        setTabError(widgets, "No languages available.");
    }
}
void LanguageLayer::showDownloadedResults() {
    auto& widgets = widgetsForTab(Tab::Downloaded);
    setTabContentVisible(widgets, false);
    clearList(widgets);

    auto langDir = Mod::get()->getConfigDir() / "languages";
    if (!std::filesystem::exists(langDir)) {
        setTabError(widgets, "No downloaded languages yet.");
        return;
    }

    std::vector<std::string> langNames;
    for (auto const& entry : std::filesystem::directory_iterator(langDir)) {
        if (!entry.is_regular_file()) continue;
        auto ext = geode::utils::string::pathToString(entry.path().extension());
        if (ext != ".json") continue;
        langNames.push_back(geode::utils::string::pathToString(entry.path().stem()));
    }

    std::sort(langNames.begin(), langNames.end());

    if (langNames.empty()) {
        setTabError(widgets, "No downloaded languages yet.");
        return;
    }

    int index = 0;
    float totalHeight = 0.f;

    for (auto const& langName : langNames) {
        auto row = CCLayerColor::create(index % 2 == 0 ? ccc4(75, 75, 75, 255) : ccc4(50, 50, 50, 255));
        row->setContentSize({ 340.f, 40.f });

        auto nameLabel = CCLabelBMFont::create(langName.c_str(), "bigFont.fnt");
        nameLabel->setAnchorPoint({ 0.f, 0.5f });
        nameLabel->setScale(0.45f);
        nameLabel->setPosition({ 10.f, 20.f });
        row->addChild(nameLabel);

        auto rowMenu = CCMenu::create();
        rowMenu->setPosition({ 0.f, 0.f });

        auto applySprite = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", 0.7f);
        applySprite->setScale(0.6f);
        auto applyBtn = CCMenuItemSpriteExtra::create(
            applySprite,
            this,
            menu_selector(LanguageLayer::onApplyBtn)
        );
        applyBtn->setPosition({ 305.f, 20.f });
        m_downloadedEntries[index] = langName;
        applyBtn->setTag(index);

        rowMenu->addChild(applyBtn);
        row->addChild(rowMenu);

        widgets.list->m_contentLayer->addChild(row);
        totalHeight += 40.f;
        widgets.list->m_contentLayer->setContentSize({ widgets.list->m_contentLayer->getContentSize().width, totalHeight });
        ++index;
    }

    widgets.list->m_contentLayer->updateLayout();
    widgets.list->scrollToTop();
    widgets.border->setVisible(true);
}
void LanguageLayer::onDownloadBtn(CCObject* sender) {
    auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
    if (!btn) return;
    int tag = btn->getTag();
    auto it = m_langEntries.find(tag);
    if (it == m_langEntries.end()) return;
    downloadLanguage(it->second.first, it->second.second);
}
void LanguageLayer::onApplyBtn(CCObject* sender) {
    auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
    if (!btn) return;
    int tag = btn->getTag();
    auto it = m_downloadedEntries.find(tag);
    if (it == m_downloadedEntries.end()) return;
    applyLanguage(it->second);
}
void LanguageLayer::downloadLanguage(std::string const& langName, std::string const& url) {
    auto langDir = Mod::get()->getConfigDir() / "languages";
    std::error_code ec;
    std::filesystem::create_directories(langDir, ec);

    auto fullUrl = url.starts_with("http") ? url : "https://" + url;

    auto* holder = new async::TaskHolder<web::WebResponse>();
    holder->spawn(
        web::WebRequest().get(fullUrl),
        [this, langName, holder](web::WebResponse res) {
            if (!res.ok()) {
                FLAlertLayer::create("Download Failed", fmt::format("Failed to download {}.\nHTTP {}", langName, res.code()).c_str(), "OK")->show();
                delete holder;
                return;
            }
            auto data = res.string().unwrapOr("");
            auto outPath = Mod::get()->getConfigDir() / "languages" / (langName + ".json");
            std::ofstream f(outPath, std::ios::out | std::ios::binary);
            f.write(data.data(), data.size());
            f.close();
            FLAlertLayer::create("Downloaded", fmt::format("{} downloaded!", langName).c_str(), "OK")->show();
            if (m_activeTab == Tab::Downloaded) {
                showDownloadedResults();
            }
            delete holder;
        }
    );
}

void LanguageLayer::applyLanguage(std::string const& langName) {
    auto path = Mod::get()->getConfigDir() / "languages" / (langName + ".json");
    Mod::get()->setSavedValue("active-language", geode::utils::string::pathToString(path));
    translatemgr::get().load();
}
bool LanguageLayer::init() {
    if (!CCLayer::init()) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = CCSprite::create("GJ_gradientBG.png");
    bg->setPosition(winSize / 2);
    bg->setScaleX(winSize.width / bg->getContentWidth());
    bg->setScaleY(winSize.height / bg->getContentHeight());
    bg->setColor({ 0, 102, 255 });
    this->addChild(bg);

    auto sideArtTL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtTL->setPosition({ 0.f, winSize.height });
    sideArtTL->setAnchorPoint({ 0.f, 1.f });
    this->addChild(sideArtTL);

    auto sideArtTR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtTR->setPosition({ winSize.width, winSize.height });
    sideArtTR->setAnchorPoint({ 1.f, 1.f });
    sideArtTR->setFlipX(true);
    this->addChild(sideArtTR);

    auto sideArtBL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtBL->setPosition({ 0.f, 0.f });
    sideArtBL->setAnchorPoint({ 0.f, 0.f });
    sideArtBL->setFlipY(true);
    this->addChild(sideArtBL);

    auto sideArtBR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtBR->setPosition({ winSize.width, 0.f });
    sideArtBR->setAnchorPoint({ 1.f, 0.f });
    sideArtBR->setFlipX(true);
    sideArtBR->setFlipY(true);
    this->addChild(sideArtBR);

    auto title = CCLabelBMFont::create("Languages", "goldFont.fnt");
    title->setPosition({ winSize.width / 2, winSize.height - 20.f });
    title->setScale(0.9f);
    this->addChild(title);

    auto menu = CCMenu::create();
    menu->setPosition({ 0.f, 0.f });
    this->addChild(menu);

    auto backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto backBtn = CCMenuItemSpriteExtra::create(backSpr, this, menu_selector(LanguageLayer::onBack));
    backBtn->setPosition({ 25.f, winSize.height - 20.f });
    menu->addChild(backBtn);

    constexpr float kTabButtonY = 248.f;

    m_downloadedTabSprite = GeodeTabSprite::create("GJ_sDownloadIcon_001.png", "Downloaded", 96.f);
    m_downloadedTabBtn = CCMenuItemSpriteExtra::create(
        m_downloadedTabSprite, this, menu_selector(LanguageLayer::selectDownloadedTab)
    );
    m_downloadedTabBtn->setPosition({ winSize.width / 2 - 70.f, kTabButtonY });
    menu->addChild(m_downloadedTabBtn);

    m_browseTabSprite = GeodeTabSprite::create("geode.loader/globe.png", "Browse", 96.f, true);
    m_browseTabBtn = CCMenuItemSpriteExtra::create(
        m_browseTabSprite, this, menu_selector(LanguageLayer::selectBrowseTab)
    );
    m_browseTabBtn->setPosition({ winSize.width / 2 + 70.f, kTabButtonY });
    menu->addChild(m_browseTabBtn);

    m_downloadedWidgets = createTabWidgets();
    m_browseWidgets = createTabWidgets();

    m_tabHost = CCLayerMultiplex::create(m_downloadedWidgets.layer, m_browseWidgets.layer, nullptr);
    m_tabHost->setContentSize(winSize);
    m_tabHost->setPosition({ 0.f, -12.5f });
    this->addChild(m_tabHost);

    auto nextPageSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextPageSprite->setFlipX(true);
    m_nextPageBtn = CCMenuItemSpriteExtra::create(nextPageSprite, this, menu_selector(LanguageLayer::nextPage));
    m_nextPageBtn->setPosition({ 415.f, 145.f });
    menu->addChild(m_nextPageBtn);

    auto prevPageSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    m_prevPageBtn = CCMenuItemSpriteExtra::create(prevPageSprite, this, menu_selector(LanguageLayer::prevPage));
    m_prevPageBtn->setPosition({ 25.f, 145.f });
    menu->addChild(m_prevPageBtn);

    m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_pageLabel->setScale(0.5f);
    m_pageLabel->setAnchorPoint({ 1.f, 1.f });
    m_pageLabel->setPosition({ winSize.width - 10.f, winSize.height - 8.f });
    this->addChild(m_pageLabel);

    auto forkSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    auto forkBtn = CCMenuItemSpriteExtra::create(forkSprite, this, menu_selector(LanguageLayer::onFork));
    forkBtn->setPosition({ winSize.width - 20.f, 20.f });
    menu->addChild(forkBtn);

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);

    m_activeTab = Tab::Downloaded;
    switchTab(Tab::Browse);

    return true;
}
void LanguageLayer::keyBackClicked() {
    onBack(nullptr);
}
void LanguageLayer::refreshPage(CCObject*) {
    if (m_activeTab == Tab::Downloaded) {
        showDownloadedResults();
        return;
    }
    if (!m_cachedLanguages.isNull()) {
        showBrowseResults(m_cachedLanguages);
    } else {
        fetchLanguages();
    }
}
void LanguageLayer::selectDownloadedTab(CCObject*) {
    switchTab(Tab::Downloaded);
}
void LanguageLayer::selectBrowseTab(CCObject*) {
    switchTab(Tab::Browse);
}
void LanguageLayer::prevPage(CCObject* sender) {
    int& page = pageForTab(m_activeTab);
    page = std::max(1, page - 1);
    refreshPage(sender);
}
void LanguageLayer::nextPage(CCObject* sender) {
    pageForTab(m_activeTab) += 1;
    refreshPage(sender);
}
void LanguageLayer::onBack(CCObject*) {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}
void LanguageLayer::onFork(CCObject*) {
    auto alert = FLAlertLayer::create(
        nullptr,
        "Contribute",
        "Fork the repository and add your language to <cy>resources/LANG</c> from <cy>base.json</c>.",
        "Cancel",
        "Open",
        300.f
    );
    alert->m_scene = this;
    alert->show();
}
void LanguageLayer::FLAlert_Clicked(FLAlertLayer* alert, bool btn2) {
    if (btn2) {
        utils::web::openLinkInBrowser("https://github.com/miskkaaa/The-Multilingualist/fork");
    }
}
LanguageLayer* LanguageLayer::create() {
    auto ret = new LanguageLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
CCScene* LanguageLayer::scene() {
    auto scene = CCScene::create();
    scene->addChild(LanguageLayer::create());
    return scene;
}