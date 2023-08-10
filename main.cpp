
#include <QtWidgets/QApplication>
#include <windows.h>
#include <tchar.h>
#include <qdebug.h>
#include "QWinHost.h"
#include "qrandom.h"

const char* WinMsgToStr(UINT msg)
{
    switch (msg)
    {
    case WM_CREATE:
        return "WM_CREATE";
    case WM_MOVE:
        return "WM_MOVE";
    case WM_SIZE:
        return "WM_SIZE";
    case WM_ACTIVATE:
        return "WM_ACTIVATE";
    case WM_ACTIVATEAPP:
        return "WM_ACTIVATEAPP";
    case WM_DESTROY:
        return "WM_DESTROY";
    case WM_NCDESTROY:
        return "WM_NCDESTROY";
    case WM_ERASEBKGND:
        return "WM_ERASEBKGND";
    case WM_NCCREATE:
        return "WM_NCCREATE";
    case WM_NCHITTEST:
        return "WM_NCHITTEST";
    case WM_NCCALCSIZE:
        return "WM_NCCALCSIZE";
    case WM_NCPAINT:
        return "WM_NCPAINT";
    case WM_NCACTIVATE:
        return "WM_NCACTIVATE";
    case WM_PAINT:
        return "WM_PAINT";
    case WM_SETCURSOR:
        return "WM_SETCURSOR";
    case WM_MOUSEMOVE:
        return "WM_MOUSEMOVE";
    case WM_NCMOUSEMOVE:
        return "WM_NCMOUSEMOVE";
    case WM_NCLBUTTONDOWN:
        return "WM_NCLBUTTONDOWN";
    case WM_NCMOUSELEAVE:
        return "WM_NCMOUSELEAVE";
    case WM_SETFOCUS:
        return "WM_SETFOCUS";
    case WM_KILLFOCUS:
        return "WM_KILLFOCUS";
    case WM_IME_SETCONTEXT:
        return "WM_IME_SETCONTEXT";
    case WM_IME_NOTIFY:
        return "WM_IME_NOTIFY";
    case WM_WINDOWPOSCHANGING:
        return "WM_WINDOWPOSCHANGING";
    case WM_WINDOWPOSCHANGED:
        return "WM_WINDOWPOSCHANGED";
    case WM_CHILDACTIVATE:
        return "WM_CHILDACTIVATE";
    case WM_SHOWWINDOW:
        return "WM_SHOWWINDOW";
    case WM_SETICON:
        return "WM_SETICON";
    case WM_GETICON:
        return "WM_GETICON";
    case WM_SETTEXT:
        return "WM_SETTEXT";
    case WM_PARENTNOTIFY:
        return "WM_PARENTNOTIFY";
    case WM_GETMINMAXINFO:
        return "WM_GETMINMAXINFO";
    case WM_DWMCOMPOSITIONCHANGED:
        return "WM_DWMCOMPOSITIONCHANGED";
    case WM_DWMNCRENDERINGCHANGED:
        return "WM_DWMNCRENDERINGCHANGED";
    case WM_SYSCOMMAND:
        return "WM_SYSCOMMAND";
    case WM_CAPTURECHANGED:
        return "WM_CAPTURECHANGED";
    case WM_MOVING:
        return "WM_MOVING";
    case WM_ENTERSIZEMOVE:
        return "WM_ENTERSIZEMOVE";
    case WM_EXITSIZEMOVE:
        return "WM_EXITSIZEMOVE";

    default:
        static QString unknown;
        unknown = QString("0x%1").arg(msg, 4, 16, QLatin1Char('0'));
        return unknown.toLatin1().constData();
    }
}
void PrintWinMsg(const char* wndName, UINT msg)
{
    qDebug() << wndName << WinMsgToStr(msg);
}

LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT: {
        PrintWinMsg("Embedded Win32 Window \t", message);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        //RECT r = ps.rcPaint;
        RECT r{};
        GetClientRect(hwnd, &r);
        FillRect(hdc, &r, (HBRUSH)(COLOR_WINDOW + 1));

        quint32 colValR = QRandomGenerator::global()->bounded(0, 255);
        quint32 colValG = QRandomGenerator::global()->bounded(0, 255);
        quint32 colValB = QRandomGenerator::global()->bounded(0, 255);

        HBRUSH b = CreateSolidBrush(RGB(colValR, colValG, colValB));
        int hMargin = (r.right - r.left) * 0.25;
        int vMargin = (r.bottom - r.top) * 0.25;
        r.left += hMargin;
        r.right -= hMargin;
        r.top += vMargin;
        r.bottom -= vMargin;

        FillRect(hdc, &r, b);
        DeleteObject(b);
        EndPaint(hwnd, &ps);

        return 0;
    }
    default:
        PrintWinMsg("Embedded Win32 Window \t", message);
        break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}


// Help tool to unset legacy environment variables
bool UnsetLegacyEnvironmentVars(const std::vector<const char*>& vEnvironmentVars)
{
    bool allUnset = false;
    for (auto varName : vEnvironmentVars)
    {
        allUnset = allUnset || qunsetenv(varName);
    }
    return allUnset;
}

int main( int argc, char *argv[] )
{
    SetDllDirectory(_T(""));

    // Register our embedded test window class
    auto hInstance = GetModuleHandle(NULL);
    WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 255));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"EmbeddedWindow";

    if (!RegisterClass(&wc))
        return FALSE;

    //---------------------------------------------------------------------
    // Before creating QAplication instance, we need to unset Qt legacy
    // environment variables (if any), so they won't introduce incorrect
    // behavior such as wrong dpi scaling management [vergel-2019/05/09]
    std::vector<const char*> envVars = { "QT_DEVICE_PIXEL_RATIO" };
    UnsetLegacyEnvironmentVars(envVars);
    // Set correct Qt dpi scaling environment variable introduced with Qt 5.6
    // cf. MAXX-50419:Create an immunity to "QT_DEVICE_PIXEL_RATIO = auto" [vergel-2019/05/09]
    qputenv("QT_SCALE_FACTOR", "1");
    // In Qt6 the HDPI scaling is by default enabled, so we disable it here to match our Qt5 behaviour
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");

    QApplication app( argc, argv );

    QWinHost winHost;
    winHost.setWindowTitle(QString("QtWidgetFlickerTest Qt%1.%2.%3").arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR).arg(QT_VERSION_PATCH));

    HWND embededWin = CreateWindow(L"EmbeddedWindow", // Predefined class;
        L"EmbeddedWindow", // caption
        WS_TABSTOP | WS_VISIBLE | WS_CHILD, // Styles
        0, // x position
        0, // y position
        100, // Button width
        25, // Button height
        (HWND)winHost.winId(), // Parent window
        NULL, // No menu.
        hInstance,
        NULL); // Pointer not needed.

    winHost.setWindow(embededWin);
    winHost.show();

    // Run Qt message loop
    return app.exec();

    // Standard win32 message loop test
    /*MSG msg;
    BOOL bRet;

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;*/
}
