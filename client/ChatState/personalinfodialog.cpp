#include "personalinfodialog.h"
#include "networkmanager.h"
#include <QQuickItem>
#include <QVBoxLayout>
#include <QQmlEngine>
#include <QQmlContext>
#include <QBuffer>
#include <QTimer>

PersonalInfoDialog::PersonalInfoDialog(QWidget *parent)
    :FramlessDialog(parent)
{
    infoState_=new CustomizeQuickWidget(this);
    infoState_->setSource(QUrl("qrc:/ChatState/PersonalInfoDialog.qml"));
    infoState_->rootContext()->setContextProperty("InfoDialog",this);

    infoStateItem_=infoState_->rootObject();
    connect(infoStateItem_,SIGNAL(closeSig()),this,SLOT(hide()));
    connect(infoStateItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));


    QVBoxLayout *qvb=new QVBoxLayout(this);
    qvb->setContentsMargins(0,0,0,0);
    qvb->addWidget(infoState_);

    timer_=new QTimer(this);
    timer_->setInterval(10000);
    connect(timer_,&QTimer::timeout,[=]{
        setMsg(false,"响应超时");
    });

    setMinimumSize(540,800);
    setMaximumSize(540,800);

    connect(NetWorkManager::getInstance(),&NetWorkManager::updateInfoSig,[=](QString id){
        if(id!=currentAccount_)
            return;
        setInfortion();
    });
}

PersonalInfoDialog::~PersonalInfoDialog()
{
    timer_->stop();
}

void PersonalInfoDialog::setInfortion()
{
    User u=NetWorkManager::getInstance()->getInfo(currentAccount_);
    QMetaObject::invokeMethod(infoStateItem_,"setInfomation",Q_ARG(QVariant,u.name_),
                              Q_ARG(QVariant,u.email_),Q_ARG(QVariant,u.account_),Q_ARG(QVariant,u.createTime_));
}

void PersonalInfoDialog::setMsg(bool isSuccess, QString text)
{
    QMetaObject::invokeMethod(infoStateItem_,"setError",Q_ARG(QVariant,isSuccess),Q_ARG(QVariant,text));
    timer_->stop();
}

void PersonalInfoDialog::setAccount_P(QString account)
{
    currentAccount_=account;
}

void PersonalInfoDialog::updateUserImage(QQuickItemGrabResult *result)
{
    QImage ima=result->image();
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    if (!ima.save(&buffer,"PNG",80)) {
        qWarning() << "Failed to save image to buffer!";
        return;
    }
    ima.loadFromData(byteArray);
    byteArray=byteArray.toBase64();

    PullRequest ureq;
    ureq.mutable_ima_request()->set_user_id(currentAccount_.toStdString());
    ureq.mutable_ima_request()->set_img_date(byteArray.toStdString());

    NetWorkManager::getInstance()->addTask(Task(
        std::move(NetWorkManager::getInstance()->mergeData(ureq)),
        [this,ima](QByteArray &respondData){
            PullRespond res;
            res.ParseFromString(respondData.toStdString());

            if(res.has_ima_respond()){
                if(res.ima_respond().success()){
                    setMsg(true,"更新成功");
                    User u=NetWorkManager::getInstance()->getInfo(currentAccount_);
                    u.image_=ima;
                    NetWorkManager::getInstance()->setInfo(currentAccount_,u);
                    NetWorkManager::getInstance()->resetAllModel();
                } else {
                    setMsg(true,"更新失败");
                }
            }
        }));
    timer_->start();
}
