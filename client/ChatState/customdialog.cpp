#include "customdialog.h"
#include <QHBoxLayout>

CustomDialog::CustomDialog(QWidget *parent,QString text,bool showAccept,bool showReject)
    :FramlessDialog(parent)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->setSource(QUrl("qrc:/ChatState/CustomDialog.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(rejectSig()),this,SLOT(reject()));
    connect(interfaceItem_,SIGNAL(acceptSig()),this,SLOT(accept()));

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(interface_);

    QMetaObject::invokeMethod(interfaceItem_,"init",Q_ARG(QVariant,text)
                              ,Q_ARG(QVariant,showAccept)
                              ,Q_ARG(QVariant,showReject));

    setMinimumSize(270,150);
    setMaximumSize(270,150);
}

