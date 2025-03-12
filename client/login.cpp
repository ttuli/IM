#include "login.h"
#include "ChatState/chatstate.h"
#include <QVBoxLayout>
#include <QQuickWidget>
#include <QTimer>
#include <QFile>
#include <QSettings>
#include <QQuickItem>
#include "IM_login.pb.h"
#include "networkmanager.h"


Widget::Widget(QWidget *parent)
    : WindowsBackground(parent)
{
    loginInterface_=new CustomizeQuickWidget(this);
    loginInterface_->setSource(QUrl("qrc:/Login.qml"));
    loginInterface_item_=loginInterface_->rootObject();
    connect(loginInterface_item_,SIGNAL(closeSig()),qApp,SLOT(quit()));
    connect(loginInterface_item_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(loginInterface_item_,SIGNAL(loginSig(QString,QString,bool,bool)),this,SLOT(handleLogin(QString,QString,bool,bool)));
    connect(loginInterface_item_,SIGNAL(registerSig()),this,SLOT(handleRegister()));

    vLayout_=new QVBoxLayout(this);
    vLayout_->setContentsMargins(0,0,0,0);
    vLayout_->addWidget(loginInterface_);

    timeout_=new QTimer(this);
    timeout_->setInterval(MAX_WAIT_TIME);
    connect(timeout_,&QTimer::timeout,[this]{
        timeout_->stop();
        NetWorkManager::getInstance()->removeTask("top");
        setCaptionVisible(FAIL,"响应超时",3000);
    });

    setMinimumSize(350,480);
    setMaximumSize(350,480);

    connect(this,&Widget::loginSuccess,NetWorkManager::getInstance(),&NetWorkManager::sendClientId);

    regDia=new RegisterDialog(this);
    connect(regDia,&RegisterDialog::finishSig,[=](QString acc){
        QMetaObject::invokeMethod(loginInterface_item_,"setInfo",Q_ARG(QVariant,acc)
                                  ,Q_ARG(QVariant,""),Q_ARG(QVariant,false),Q_ARG(QVariant,false),Q_ARG(QVariant,false));
    });

    QTimer::singleShot(200,this,SLOT(readInitFile()));
}

Widget::~Widget()
{
    timeout_->stop();
}

void Widget::setCaptionVisible(int type,QString text,int duration)
{
    QMetaObject::invokeMethod(loginInterface_item_,"showCaption",
                              Q_ARG(QVariant,type),Q_ARG(QVariant,text),Q_ARG(QVariant,duration));
}

void Widget::writeInitFile(QString acc, QString pwd, bool remberPwd, bool autoLogin)
{
    QFile file("config.ini");
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    QSettings setting("config.ini", QSettings::IniFormat);
    setting.setValue("User/account",QVariant(acc));
    setting.setValue("User/password",QVariant(pwd));
    setting.setValue("remberPwd",QVariant(remberPwd));
    setting.setValue("autoLogin",QVariant(autoLogin));

    setting.sync();
}

void Widget::readInitFile()
{
    QFile file("config.ini");
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    QSettings setting("config.ini", QSettings::IniFormat);
    QString account=setting.value("User/account").toString();
    QString pwd=setting.value("User/password").toString();
    bool rember=setting.value("remberPwd").toBool();
    bool autoLogin=setting.value("autoLogin").toBool();

    if(!rember)pwd="";

    QMetaObject::invokeMethod(loginInterface_item_,"setInfo",Q_ARG(QVariant,account),Q_ARG(QVariant,pwd),
                              Q_ARG(QVariant,rember),Q_ARG(QVariant,autoLogin),Q_ARG(QVariant,true));
}

void Widget::handleLogin(QString account,QString password,bool remberPwd,bool autoLogin)
{
    currentAccount_=account;
    PullRequest request;
    request.mutable_login_request()->set_account(account.toStdString());
    request.mutable_login_request()->set_password(password.toStdString());

    NetWorkManager::getInstance()->addTask(Task(
    std::move(NetWorkManager::getInstance()->mergeData(request)),[=](QByteArray &respondData){
        PullRespond res;
        res.ParseFromString(respondData.toStdString());

        if(res.has_lginreg_respond()){
            if(res.lginreg_respond().success()){
                writeInitFile(account,password,remberPwd,autoLogin);
                setCaptionVisible(SUCCESS,"登录成功",3000);
                emit loginSuccess(currentAccount_);
            }
            else {
                QString str=QString::fromStdString(res.lginreg_respond().msg());
                if(str=="wrong")
                    str="用户名或密码错误";
                else if(str=="otherlogin")
                    str="账号已登录!";
                setCaptionVisible(FAIL,str,3000);
            }
        }
        timeout_->stop();
    }));
    timeout_->start();
}

void Widget::handleRegister()
{
    regDia->exec();
}





