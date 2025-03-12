#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTcpSocket>
#include "register_dialog.h"
#include "windows_background.h"
#include "ChatState/chatstate.h"
#include "customizequickwidget.h"

class Widget : public WindowsBackground
{
    Q_OBJECT

public:
    enum{
      LOADING,
        SUCCESS,
        FAIL
    };

    Widget(QWidget *parent = nullptr);

    ~Widget();

    void setCaptionVisible(int type,QString text,int duration);

protected:

private:
    //成员函数
    void writeInitFile(QString acc,QString pwd,bool remberPwd,bool autoLogin);
    //静态常量

    //成员变量
    CustomizeQuickWidget *loginInterface_;
    QQuickItem *loginInterface_item_;
    QVBoxLayout *vLayout_;
    QTimer *timeout_;

    QString currentAccount_;

    RegisterDialog *regDia;
    QSharedPointer<ChatState> chatState_;

private slots:
    void handleLogin(QString account,QString password,bool remberPwd,bool autoLogin);

    void handleRegister();

    void readInitFile();

signals:
    void loginSuccess(QString account);

    void registerSig();
};
#endif // WIDGET_H
