#include "addfriend.h"
#include "IM_login.pb.h"
#include "networkmanager.h"
#include "personalinfodialog.h"
#include <QVBoxLayout>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QDate>
#include <QTimer>

AddFriend::AddFriend(QWidget *parent)
    :WindowsBackground(parent)
{
    interface_=new CustomizeQuickWidget(this);
    interfaceContext_=interface_->rootContext();
    interfaceContext_->setContextProperty("searchModel",SearchModel::getInstance());
    interface_->setSource(QUrl("qrc:/ChatState/AddFriend.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(closeSig()),this,SLOT(close()));
    connect(interfaceItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(interfaceItem_,SIGNAL(searchSig(QString,int,QString)),this,SLOT(doSearch(QString,int,QString)));
    connect(interfaceItem_,SIGNAL(addSig(QString,int)),this,SLOT(doAddFriend(QString,int)));

    QVBoxLayout *qvb=new QVBoxLayout(this);
    qvb->setContentsMargins(0,0,0,0);
    qvb->addWidget(interface_);

    setMinimumSize(640,480);
    setMaximumSize(640,480);

    timeout_=new QTimer(this);
    timeout_->setInterval(MAX_WAIT_TIME);
    connect(timeout_,&QTimer::timeout,[=]{
        timeout_->stop();

    });
}

AddFriend::~AddFriend()
{
    timeout_->stop();
}

void AddFriend::setCurrentAccount_A(QString account)
{
    currentAccount_=account;
}


void AddFriend::doSearch(QString mode, int type, QString content)
{
    SearchModel::getInstance()->removeInfo("",true);
    PullRequest req;
    if(mode=="person"){
        req.mutable_search_request()->set_st(SearchRequest_State_FRIEND);
        req.mutable_search_request()->set_way((type==SearchType::BY_ACCOUNT?SearchRequest_Way_BY_ACCOUNT:SearchRequest_Way_BY_NAME));
        req.mutable_search_request()->set_content(content.toStdString());

    } else {
        return;
    }
    QString id=NetWorkManager::getInstance()->addTask(Task(
        std::move(NetWorkManager::getInstance()->mergeData(req)),[this](QByteArray &respondData){
            PullRespond res;
            res.ParseFromString(respondData.toStdString());
            QMetaObject::invokeMethod(interfaceItem_,"setFinishVisible",Q_ARG(QVariant,false));
            QMetaObject::invokeMethod(interfaceItem_,"setMsg",Q_ARG(QVariant,"搜索成功"),Q_ARG(QVariant,true));
            if(res.has_search_respond()){
                const SearchRespond& r=res.search_respond();
                for(int i=0;i<r.infos_size();++i){
                    const SearchInfo& info=r.infos(i);
                    SearchModel::getInstance()->addInfo(
                        QString::fromStdString(info.name()),
                        QString::fromStdString(info.account()));
                }
            }
        }));
    QTimer::singleShot(MAX_WAIT_TIME,[this,id]{
        if(!NetWorkManager::getInstance()->removeTask(id))
            return;
        QMetaObject::invokeMethod(interfaceItem_,"setFinishVisible",Q_ARG(QVariant,false));
        QMetaObject::invokeMethod(interfaceItem_,"setMsg",Q_ARG(QVariant,"执行失败"),Q_ARG(QVariant,false));
    });
}

void AddFriend::doAddFriend(QString account, int index)
{
    QSharedPointer<info> f=SearchModel::getInstance()->getInfo(index);

    if(f==nullptr||f->account_==""){
        qDebug()<<"AddFriend::doAddFriend:f null";
        QMetaObject::invokeMethod(interfaceItem_,"setFinishVisible",Q_ARG(QVariant,false));
        QMetaObject::invokeMethod(interfaceItem_,"setMsg",Q_ARG(QVariant,"添加失败"),Q_ARG(QVariant,false));
        return;
    }

    PullRequest req;
    req.mutable_add_request()->set_st(AddRequest_State_FRIEND);
    req.mutable_add_request()->set_user_id(currentAccount_.toStdString());
    req.mutable_add_request()->set_friend_id(account.toStdString());

    QString id=NetWorkManager::getInstance()->addTask(Task(
        std::move(NetWorkManager::getInstance()->mergeData(req)),[this,index,f](QByteArray &respondData){
            PullRespond res;
            res.ParseFromString(respondData.toStdString());
            if(res.has_add_respond()){
                QMetaObject::invokeMethod(interfaceItem_,"setFinishVisible",Q_ARG(QVariant,false));
                bool result=res.add_respond().result();
                QMetaObject::invokeMethod(interfaceItem_,"setMsg",Q_ARG(QVariant,result?"添加成功":"添加失败"),
                                          Q_ARG(QVariant,result));
                SearchModel::getInstance()->updateIndexApplyState(index,result);

                QString currentDate=QDate::currentDate().toString("yyyy-MM-dd");
                QString lastDay=FriendApplyModel::getInstance()->getLastTime();
                if(lastDay!=currentDate)
                    lastDay=currentDate;
                FriendApplyModel::getInstance()->removeRow(0);
                FriendApplyModel::getInstance()->waitForInfomation(f->account_,applyInfo::REQUEST);
                FriendApplyModel::getInstance()->addAtHead(QSharedPointer<applyInfo>::create(lastDay));
            }
        }));
    QTimer::singleShot(MAX_WAIT_TIME,[this,id]{
        if(!NetWorkManager::getInstance()->removeTask(id))
            return;
        QMetaObject::invokeMethod(interfaceItem_,"setFinishVisible",Q_ARG(QVariant,false));
        QMetaObject::invokeMethod(interfaceItem_,"setMsg",Q_ARG(QVariant,"执行失败"),Q_ARG(QVariant,false));
    });
}
