#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QWindow>
#include <QQuickWindow>
#include <QQuickItem>
#include <QAbstractNativeEventFilter>
#include <QSurfaceFormat>
#include <QFile>
#include <QElapsedTimer>

#include "headers/player.h"
#include "headers/database.h"
#include "models/songmodel.h"
#include "headers/eventbus.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// These DWM attributes are available starting in Windows 10 (build 1809+).
// If the user is on an older Windows, the calls will no‐op and leave the bar white.

/// Sets the title‐bar background color (DWMWA_CAPTION_COLOR) and
/// the title‐bar text color (DWMWA_TEXT_COLOR) for the given HWND.
static void setTitleBarColor(HWND hwnd, const QColor &bgColor, const QColor &textColor)
{
    if (!IsWindow(hwnd))
        return;

    // Convert Qt's QColor to a COLORREF (0x00BBGGRR).  We mask to 24 bits.
    DWORD bg  = static_cast<DWORD>(bgColor.rgb() & 0x00FFFFFF);
    DWORD txt = static_cast<DWORD>(textColor.rgb() & 0x00FFFFFF);

    // DWMWA_CAPTION_COLOR = 35, DWMWA_TEXT_COLOR = 36
    // (Defined in newer Windows SDKs; if your sdk doesn't have the enums, use the literals.)
    const DWORD DWMWA_CAPTION_COLOR = 35;
    const DWORD DWMWA_TEXT_COLOR    = 36;

    // Request Windows to set our caption (title‐bar) to bg, and the text to txt.
    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &bg,  sizeof(bg));
    DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR,   &txt, sizeof(txt));
}
#endif

// void removeTitleBar(QWindow *window) {
// #ifdef Q_OS_WIN
//     HWND hwnd = (HWND)window->winId();

//     // 1) Remove only the caption (title bar) bit:
//     LONG style = GetWindowLong(hwnd, GWL_STYLE);
//     style &= ~WS_CAPTION;
//     style |= WS_THICKFRAME;
//     SetWindowLong(hwnd, GWL_STYLE, style);

//     // 2) Push the style change:
//     SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
//                  SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);

//     // 3) Re‑enable DWM transitions (animations):
// #ifndef DWMWA_TRANSITIONS_FORCEDISABLED
// # define DWMWA_TRANSITIONS_FORCEDISABLED 3
// #endif
//     BOOL disableTransitions = FALSE;  // <— FALSE = transitions ON
//     DwmSetWindowAttribute(hwnd,
//                           DWMWA_TRANSITIONS_FORCEDISABLED,
//                           &disableTransitions,
//                           sizeof(disableTransitions));

//     // 4) Extend the glass frame *only* 1px at the top to hide that white line:
//     MARGINS margins = { -1, 0, 0, 0 };  // top, left, right, bottom
//     DwmExtendFrameIntoClientArea(hwnd, &margins);
// #endif
// }

// void removeTitleBar(QWindow *window) {
// #ifdef Q_OS_WIN
//     HWND hwnd = (HWND)window->winId();

//     // 1. Enable DWM animations FIRST (critical ordering fix) :cite[1]:cite[5]
//     BOOL disableTransitions = FALSE;  // FALSE enables animations
//     DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED,
//                           &disableTransitions, sizeof(disableTransitions));

//     // 2. Remove title bar while preserving resizing borders
//     LONG style = GetWindowLong(hwnd, GWL_STYLE);
//     style &= ~WS_CAPTION;
//     style |= WS_THICKFRAME;
//     SetWindowLong(hwnd, GWL_STYLE, style);

//     // 3. Apply style changes
//     SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
//                  SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

//     // 4. Calculate precise top margin (DPI-aware) :cite[2]
//     const UINT dpi = GetDpiForWindow(hwnd);
//     const int frameY = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
//     const int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
//     const int totalTop = frameY + padding;  // Critical for Windows 10/11

//     // 5. Extend glass frame to cover white area :cite[2]:cite[5]
//     MARGINS margins = { totalTop, 0, 0, 0 };
//     DwmExtendFrameIntoClientArea(hwnd, &margins);

//     // 6. Set black background brush to prevent artifacts :cite[2]:cite[10]
//     HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
//     SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)blackBrush);
//     // Note: Brush is intentionally not deleted (tied to window lifetime)
// #endif
// }

void get_folders(database* db);

int main(int argc, char *argv[])
{
    QElapsedTimer timer;
    timer.start();

    QGuiApplication app(argc, argv);

    player *mediaplayer = new player();
    database *db = new database();
    songmodel* queuemodel = new songmodel();
    eventbus* bus = new eventbus();

    QQmlApplicationEngine engine;
    mediaplayer->init(queuemodel);
    db->init();


    qmlRegisterSingletonInstance<player>("Nebula.Media", 1, 0, "MediaPlayer", mediaplayer);
    qmlRegisterSingletonInstance<database>("Nebula.Database", 1, 0, "Keeper", db);
    qmlRegisterSingletonInstance<songmodel>("Nebula.SongModel", 1, 0, "Model", queuemodel);
    qmlRegisterSingletonInstance<eventbus>("Nebula.Events", 1, 0, "EventBus", bus);

    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/nebula-music/Main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    // 4) Grab the QML-created window (must be a QWindow-derived type):
    QObject* topLevel = engine.rootObjects().first();
    QWindow* rootWindow = qobject_cast<QWindow*>(topLevel);
    if (!rootWindow) {
        qWarning("ERROR: Top-level QML object is not a QWindow!");
        return -1;
    }

    // 5) Show the window (in case QML didn't already set visible:true):
    rootWindow->show();

#ifdef Q_OS_WIN
    // 6) Now that the window is visible, get its HWND and override the title-bar color:
    HWND hwnd = reinterpret_cast<HWND>(rootWindow->winId());
    if (IsWindow(hwnd)) {
        // Example: dark gray background with white text:
        QColor bg    = QColor(41, 41, 41);   // a dark gray (almost black)
        QColor text  = QColor(255, 255, 255); // pure white

        setTitleBarColor(hwnd, bg, text);
    }
#endif

    qDebug() << "Window creation time: " << timer.elapsed() << " ms.";

    return app.exec();
}

