#ifndef CHATSTATE_H
#define CHATSTATE_H

#include "windows_background.h"
#include "personalinfodialog.h"
#include "friendAgroup/friendlist.h"
#include "customizequickwidget.h"
#include "chatdialog.h"
#include "chatlist.h"
#include "addfriend.h"
#include <QQuickItem>
#include <QTcpSocket>
#include <QStackedWidget>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QQueue>
#include <QQueue>
#include <QHBoxLayout>

#define MAX_CHAT_IN_CACHE 5

class ChatState : public WindowsBackground
{
    Q_OBJECT
public:
    ChatState(QWidget *parent=nullptr);

    ~ChatState();

    void setCurrentAccount(QString account);
    void setConnectInfoVisible(QString loading_text,bool visible1);

protected:
    void changeEvent(QEvent *event);

    void resizeEvent(QResizeEvent *event);

private:
    CustomizeQuickWidget *leftChoiceColum_;
    QQuickItem *leftChoiceColumItem_;

    CustomizeQuickWidget *titleBar_;
    QQuickItem *titleBarItem_;

    QHBoxLayout *qhb_;

    ChatList *chatList_;
    QStackedWidget *chatDiaSwitch_;
    QList<QString> enterOrder_;
    QMap<QString,QSharedPointer<ChatDialog>> chatDias_;
    QSplitter *splitter;

    FriendList *friendlist_;
    QSharedPointer<PersonalInfoDialog> infoDia_;

    QStackedWidget *pageControl_;
    QSharedPointer<AddFriend> addFriendDia_;
    QSystemTrayIcon *systemIcon_;

    QString currentAccount_="1000000001";

    QThread *sqliteThread_;

    void setSystemIcon();
    void pullOfflineMsg();
    void pullList();

private slots:
    void updateWindow();
    void showPersonalInfo();
    void doAddFriend();
    void changeCurrentWidget(int index);
    void createChatDia(QString account,QString name);
    void updateEnterOrder(QString id);
    void flush();
    void restartProgram();
};

#endif // CHATSTATE_H
