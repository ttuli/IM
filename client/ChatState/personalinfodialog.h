#ifndef PERSONALINFODIALOG_H
#define PERSONALINFODIALOG_H

#include <framless_dialog.h>
#include "customizequickwidget.h"
#include <QQuickItemGrabResult>

class PersonalInfoDialog : public FramlessDialog
{
    Q_OBJECT
public:
    PersonalInfoDialog(QWidget *parent=nullptr);
    ~PersonalInfoDialog();

    void setInfortion();

    void setMsg(bool isSuccess,QString text);

    void setAccount_P(QString account);

protected:

private:
    CustomizeQuickWidget *infoState_;
    QQuickItem *infoStateItem_;

    QTimer *timer_;
    QString currentAccount_;

public slots:
    void updateUserImage(QQuickItemGrabResult *result);

signals:
};

#endif // PERSONALINFODIALOG_H
