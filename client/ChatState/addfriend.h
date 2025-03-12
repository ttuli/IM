#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <windows_background.h>
#include "customizequickwidget.h"
#include "searchmodel.h"

class AddFriend : public WindowsBackground
{
    Q_OBJECT
public:
    AddFriend(QWidget *parent=nullptr);
    ~AddFriend();

    enum SearchType{
        BY_ACCOUNT,
        BY_NAME
    };

    void setCurrentAccount_A(QString account);

protected:

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
    QQmlContext *interfaceContext_;

    QTimer *timeout_;
    QString currentAccount_;

private slots:
    void doSearch(QString mode,int type,QString content);

    void doAddFriend(QString account,int index);
};

#endif // ADDFRIEND_H
