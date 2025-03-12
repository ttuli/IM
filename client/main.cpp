#include "login.h"
#include "ChatState/chatstate.h"
#include "ChatState/storeemoji.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/new/prefix1/res/IM_ICON.svg"));
    a.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    a.setQuitOnLastWindowClosed(false);

    StoreEmoji::getInstance();

    ChatState cs;
    Widget *w=new Widget;
    QObject::connect(w,&Widget::loginSuccess,[&cs,w](QString account){
        w->deleteLater();
        cs.setCurrentAccount(account);
        cs.show();
    });

    w->show();

    return a.exec();
}
