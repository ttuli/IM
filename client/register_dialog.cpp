#include "register_dialog.h"
#include "IM_login.pb.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QQuickWidget>
#include <QQuickItem>
#include <string>
#include <QTimer>

RegisterDialog::RegisterDialog(QWidget *parent)
    :FramlessDialog(parent)
{
    setWindowFlag(Qt::Dialog);

    registerInterface_=new CustomizeQuickWidget(this);
    registerInterface_->setSource(QUrl("qrc:/Register.qml"));

    registerInterfaceItem_=registerInterface_->rootObject();
    connect(registerInterfaceItem_,SIGNAL(closeSig()),this,SLOT(close()));
    connect(registerInterfaceItem_,SIGNAL(registerSig(QString,QString,QString)),this,SLOT(doRegister(QString,QString,QString)));

    vlayout_=new QVBoxLayout(this);
    vlayout_->setContentsMargins(0,0,0,0);
    vlayout_->addWidget(registerInterface_);

    setMinimumSize(400,500);
    setMaximumSize(400,500);
}

RegisterDialog::~RegisterDialog()
{
}

void RegisterDialog::showEvent(QShowEvent *)
{
    initInterface();
    QMetaObject::invokeMethod(registerInterfaceItem_,"init");
}


void RegisterDialog::doRegister(QString username, QString pwd, QString email)
{
    PullRequest req;
    req.mutable_register_request()->set_username(username.toStdString());
    req.mutable_register_request()->set_password(pwd.toStdString());
    req.mutable_register_request()->set_email(email.toStdString());

    QString id=NetWorkManager::getInstance()->addTask(Task(
    std::move(NetWorkManager::getInstance()->mergeData(req)),[=](QByteArray &respondData){
        PullRespond res;
        res.ParseFromString(respondData.toStdString());

        if(res.has_lginreg_respond()){
            if(res.lginreg_respond().success()){
                QMetaObject::invokeMethod(registerInterfaceItem_,"showFinish",
                        Q_ARG(QVariant,QString::fromStdString(res.lginreg_respond().msg())));
                emit finishSig(QString::fromStdString(res.lginreg_respond().msg()));
            }
            else
                QMetaObject::invokeMethod(registerInterfaceItem_,"showError",Q_ARG(QVariant,"注册失败!"),Q_ARG(QVariant,false));
        }
    }));

    QTimer::singleShot(MAX_WAIT_TIME,[this,id]{
        if(NetWorkManager::getInstance()->removeTask(id))
            QMetaObject::invokeMethod(registerInterfaceItem_,"showError",Q_ARG(QVariant,"响应超时!"),Q_ARG(QVariant,false));
    });
}

