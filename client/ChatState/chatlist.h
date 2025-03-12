#ifndef CHATLIST_H
#define CHATLIST_H

#include <QWidget>
#include "customizequickwidget.h"
#include "emojiimageprovider.h"

class ChatList : public QWidget
{
    Q_OBJECT
public:
    explicit ChatList(QWidget *parent = nullptr);
    ~ChatList();

    void setCurrentIndex(QString account);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
    EmojiImageProvider *provider_;
private slots:
    void onTalkRequest(QString account,QString name);
    void onClear(int index);

signals:
    void createChatDiaSig(QString account,QString name);
};

#endif // CHATLIST_H
