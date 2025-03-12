#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include <QWidget>
#include "customizequickwidget.h"
#include "imageprovider.h"
#include "friendapplymodel.h"

class FriendList : public QWidget
{
    Q_OBJECT
public:
    explicit FriendList(QWidget *parent = nullptr);
    ~FriendList();

    void setCurrentAccount_F(QString account);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
    QString currentAccount_="1000000001";

    QTimer *timer_;
    QTimer *timeout_;

    void pullApplyMsg();
    void pullFriends();

private slots:
    void handleApply(int index,QString account,bool result);
    void handleTalk(QString name,QString account);
    void emitUnshowTip();
    void deleteFriend(QString target,int position);

signals:
    void unShowTipSig();
    void createChatDiaSig(QString account,QString name);
};

#endif // FRIENDLIST_H
