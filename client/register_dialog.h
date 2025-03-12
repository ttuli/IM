#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include "framless_dialog.h"
#include "register_dialog.h"
#include "customizequickwidget.h"
#include <QVBoxLayout>
#include <QTcpSocket>


class RegisterDialog : public FramlessDialog
{
    Q_OBJECT
public:
    RegisterDialog(QWidget *parent=nullptr);

    ~RegisterDialog();

protected:
    void showEvent(QShowEvent*) override;

private:
    CustomizeQuickWidget *registerInterface_;
    QQuickItem *registerInterfaceItem_;
    QVBoxLayout *vlayout_;

private slots:
    void doRegister(QString username,QString pwd,QString email);

signals:
    void finishSig(QString account);
};

#endif // REGISTER_DIALOG_H
