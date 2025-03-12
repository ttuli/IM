#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <framless_dialog.h>
#include "customizequickwidget.h"
#include <QQuickItem>

class CustomDialog : public FramlessDialog
{
    Q_OBJECT
public:
    CustomDialog(QWidget *parent=nullptr,QString text="",bool showAccept=true,bool showReject=true);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;
};

#endif // CUSTOMDIALOG_H
