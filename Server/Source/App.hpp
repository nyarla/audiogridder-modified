/*
 * Copyright (c) 2020 Andreas Pohl
 * Licensed under MIT (https://github.com/apohl79/audiogridder/blob/master/COPYING)
 *
 * Author: Andreas Pohl
 */

#ifndef App_hpp
#define App_hpp

#include <JuceHeader.h>

#include "ProcessorWindow.hpp"
#include "Utils.hpp"

namespace e47 {

class Server;
class Processor;
class PluginListWindow;
class ServerSettingsWindow;
class PluginListWindow;
class StatisticsWindow;
class SplashWindow;
class MenuBarWindow;

class App : public JUCEApplication, public MenuBarModel, public LogTag {
  public:
    enum ExitCodes : uint32 {
        EXIT_OK = 0,
        EXIT_RESTART = 1,
        EXIT_SANDBOX_INIT_ERROR = 101,
        EXIT_SANDBOX_BIND_ERROR = 102,
        EXIT_SANDBOX_NO_MASTER = 103,
        EXIT_SANDBOX_PARAM_ERROR = 104
    };

    App();
    ~App() override;

    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }
    void initialise(const String& commandLineParameters) override;
    void shutdown() override;
    void systemRequestedQuit() override { quit(); }

    PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& /* menuName */) override;
    void menuItemSelected(int /* menuItemID */, int /* topLevelMenuIndex */) override {}

    // MenuBarModel
    StringArray getMenuBarNames() override {
        StringArray names;
        names.add("Settings");
        return names;
    }

    void prepareShutdown(uint32 exitCode = 0);
    const KnownPluginList& getPluginList();
    void restartServer(bool rescan = false);
    std::shared_ptr<Server> getServer() { return m_server; }

    void showEditor(std::shared_ptr<Processor> proc, Thread::ThreadID tid, ProcessorWindow::CaptureCallbackFFmpeg func,
                    int x = 0, int y = 0);
    void showEditor(std::shared_ptr<Processor> proc, Thread::ThreadID tid, ProcessorWindow::CaptureCallbackNative func,
                    int x = 0, int y = 0);

    void hideEditor(Thread::ThreadID tid = nullptr, bool updateMacOSDock = true);
    void resetEditor(Thread::ThreadID tid);
    void bringEditorToFront(Thread::ThreadID tid);
    void moveEditor(Thread::ThreadID tid, int x, int y);
    std::shared_ptr<Processor> getCurrentWindowProc(Thread::ThreadID tid);
    void restartEditor(Thread::ThreadID tid);
    void updateScreenCaptureArea(Thread::ThreadID tid, int val = 0);
    Point<float> localPointToGlobal(Thread::ThreadID tid, Point<float> lp);
    void addKeyListener(Thread::ThreadID tid, KeyListener* l);

    void hidePluginList();
    void hideServerSettings();
    void hideStatistics();
    void showSplashWindow(std::function<void(bool)> onClick = nullptr);
    void hideSplashWindow(int wait = 0);
    void setSplashInfo(const String& txt);

    void updateDockIcon() {
#ifdef JUCE_MAC
        bool show = nullptr != m_srvSettingsWindow || nullptr != m_statsWindow || nullptr != m_pluginListWindow ||
                    nullptr != m_splashWindow;
        Process::setDockIconVisible(show);
#endif
    }

  private:
    std::shared_ptr<Server> m_server;
    std::unique_ptr<std::thread> m_child;
    std::atomic_bool m_stopChild{false};

    struct ProcessorWindowHelper {
        std::unique_ptr<ProcessorWindow> window;
        std::shared_ptr<Processor> processor;
        ProcessorWindow::CaptureCallbackFFmpeg callbackFFmpeg;
        ProcessorWindow::CaptureCallbackNative callbackNative;
        void reset();
    };

    std::unordered_map<uint64, ProcessorWindowHelper> m_windows;
    std::mutex m_windowsMtx;

    std::unique_ptr<PluginListWindow> m_pluginListWindow;
    std::unique_ptr<ServerSettingsWindow> m_srvSettingsWindow;
    std::unique_ptr<StatisticsWindow> m_statsWindow;
    std::shared_ptr<SplashWindow> m_splashWindow;
    std::unique_ptr<MenuBarWindow> m_menuWindow;

    uint32 m_exitCode = 0;

    template <typename T>
    void showEditorInternal(std::shared_ptr<Processor> proc, Thread::ThreadID tid, T func, int x, int y);

    ENABLE_ASYNC_FUNCTORS();
};

}  // namespace e47

#endif /* App_hpp */
