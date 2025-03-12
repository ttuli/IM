#include "windows_background.h"
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#include <QScreen>
#include <QMouseEvent>
#include <QGuiApplication>

WindowsBackground::WindowsBackground(QWidget *parent)
    : QWidget{parent}
{
    title=nullptr;
    titleItem=nullptr;
}

void WindowsBackground::initInterface()
{
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);

    ::SetWindowLong(hwnd, GWL_STYLE, style|WS_MAXIMIZEBOX | WS_THICKFRAME|WS_CAPTION);
    MARGINS margins = {-1,-1,-1,-1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void WindowsBackground::setTitleWidget(QQuickWidget *widget,QQuickItem *item)
{
    title=widget;
    titleItem=item;
}

void WindowsBackground::showEvent(QShowEvent *)
{
    initInterface();
}


bool WindowsBackground::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = static_cast<MSG *>(message);

    if (eventType == "windows_generic_MSG"){

        if (msg->message == WM_NCCALCSIZE)
        {
            *result = 0;
            return true;
        } else if (msg->message == WM_SYSKEYDOWN){
            //屏蔽alt键按下
        } else if (msg->message == WM_SYSKEYUP){
            //屏蔽alt键松开
        } else if (msg->message == WM_NCHITTEST){
            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);
            QPoint cursorpos(x,y);

            //注意屏幕缩放的问题
            qreal scales=QGuiApplication::primaryScreen()->devicePixelRatio();
            cursorpos/=scales;

            QPoint pos=mapFromGlobal(cursorpos);
            int padding=5;

            bool left = pos.x() < padding;
            bool right = pos.x() > width() - padding;
            bool top = pos.y() < padding;
            bool bottom = pos.y() > height() - padding;

            *result = 0;

            if (left && top) {
                *result = HTTOPLEFT;
            } else if (left && bottom) {
                *result = HTBOTTOMLEFT;
            } else if (right && top) {
                *result = HTTOPRIGHT;
            } else if (right && bottom) {
                *result = HTBOTTOMRIGHT;
            } else if (left) {
                *result = HTLEFT;
            } else if (right) {
                *result = HTRIGHT;
            } else if (top) {
                *result = HTTOP;
            } else if (bottom) {
                *result = HTBOTTOM;
            }

            //先处理掉拉伸
            if (0 != *result) {
                return true;
            }

            if(title&&titleItem&&title->geometry().contains(pos)){
                QVariant res=true;
                QMetaObject::invokeMethod(
                    titleItem,
                    "isInteractiveControl",
                    Qt::DirectConnection,
                    Q_RETURN_ARG(QVariant, res),
                    Q_ARG(QVariant, pos.x() - title->geometry().x()),
                    Q_ARG(QVariant, pos.y()));
                if(!res.toBool()){
                    *result = HTCAPTION;
                    return true;
                }
            }

            // HCURSOR currentCursor = GetCursor();

            // HCURSOR resizeCursor = LoadCursor(NULL, IDC_SIZEALL); // 拉伸光标
            // HCURSOR resizeNSCursor = LoadCursor(NULL, IDC_SIZENS); // 垂直拉伸光标
            // HCURSOR resizeWECursor = LoadCursor(NULL, IDC_SIZEWE); // 水平拉伸光标

            // if(currentCursor==resizeCursor||currentCursor==resizeNSCursor||currentCursor==resizeWECursor)
            //     PostMessage((HWND)this->winId(), WM_SETCURSOR, 0, 0);

        } else if (msg->wParam == PBT_APMSUSPEND && msg->message == WM_POWERBROADCAST){
            //系统休眠的时候自动最小化可以规避程序可能出现的问题
            this->showMinimized();
        } else if (msg->wParam == PBT_APMRESUMEAUTOMATIC){
            //休眠唤醒后自动打开
            this->showNormal();
        } else if (msg->message == WM_NCACTIVATE){
            BOOL composition_enabled = FALSE;
            if (DwmIsCompositionEnabled(&composition_enabled) == S_OK)
            {
                *result = TRUE;
                return true;
            }
            return false;
        }
    }

    return false;
}

void WindowsBackground::mouseMoveEvent(QMouseEvent *event)
{
    if(leftButton_){
        move(windowp_+event->globalPosition().toPoint()-cursorp_);
    }
}

void WindowsBackground::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        leftButton_=true;
        cursorp_=event->globalPosition().toPoint();
        windowp_=pos();
    }
}

void WindowsBackground::mouseReleaseEvent(QMouseEvent *event)
{
    leftButton_=false;
}
