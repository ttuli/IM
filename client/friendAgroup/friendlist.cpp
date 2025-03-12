#include "friendlist.h"
#include "networkmanager.h"
#include "friendlistmodel.h"
#include "IM_login.pb.h"
#include "ChatState/chatlistmodel.h"
#include <QVBoxLayout>
#include <QQuickItem>
#include <QTimer>
#include <QQmlContext>
#include <QQmlEngine>
#include <QString>

FriendList::FriendList(QWidget *parent)
    : QWidget{parent}
{
    interface_=new CustomizeQuickWidget(this);
    interface_->rootContext()->setContextProperty("applyModel",FriendApplyModel::getInstance());
    interface_->rootContext()->setContextProperty("friendListModel",FriendListModel::getInstance());
    interface_->setSource(QUrl("qrc:/FriendAGroup/MainControl.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(reactSig(int,QString,bool)),this,SLOT(handleApply(int,QString,bool)));
    connect(interfaceItem_,SIGNAL(readyTalkSig(QString,QString)),this,SLOT(handleTalk(QString,QString)));
    connect(FriendApplyModel::getInstance(),&FriendApplyModel::showTipSig,[=]{
        QMetaObject::invokeMethod(interfaceItem_,"setTip",Q_ARG(QVariant,true));
    });
    connect(interfaceItem_,SIGNAL(unShowTipSig()),this,SLOT(emitUnshowTip()));
    connect(interfaceItem_,SIGNAL(deleteFriendSig(QString,int)),this,SLOT(deleteFriend(QString,int)));

    QVBoxLayout *qvb=new QVBoxLayout(this);
    qvb->setContentsMargins(0,0,0,0);
    qvb->addWidget(interface_);

    timer_=new QTimer(this);
    timer_->setInterval(MAX_WAIT_TIME);
    connect(timer_,&QTimer::timeout,[=]{

        timer_->stop();
    });
}

FriendList::~FriendList()
{
    timer_->stop();
}

void FriendList::setCurrentAccount_F(QString account)
{
    currentAccount_=account;
    pullApplyMsg();
    pullFriends();
}

void FriendList::pullApplyMsg()
{
    PullRequest req;
    req.mutable_apply_request()->set_user_id(currentAccount_.toStdString());

    NetWorkManager::getInstance()->addTask(Task(
        std::move(NetWorkManager::getInstance()->mergeData(req)),[this](QByteArray &respondData){
            PullRespond res;
            res.ParseFromString(respondData.toStdString());
            QString applyTimeFinal="";
            if(res.has_apply_respond()){
                FriendApplyModel::getInstance()->removeRow(0,true);
                const ApplyRespond &ar=res.apply_respond();
                for(int i=0;i<ar.infos_size();++i){
                    const ApplyInfo &info=ar.infos(i);
                    const FriendApply &fapply=info.friend_apply();

                    QString applyTime=QString::fromStdString(info.apply_time());
                    applyTime=applyTime.left(applyTime.indexOf(' '));

                    applyInfo::applyType type=applyInfo::applyType::REQUEST;
                    if(fapply.state()==0){
                        type=applyInfo::applyType::VERIFY;
                    }
                    else if(fapply.state()==-1)
                        type=applyInfo::applyType::REQUEST;
                    else if(fapply.state()==-2)
                        type=applyInfo::applyType::REJECT;
                    else if(fapply.state()==1)
                        type=applyInfo::applyType::ACCEPT;
                    FriendApplyModel::getInstance()->waitForInfomation(QString::fromStdString(fapply.user_id()),type);
                    if(applyTimeFinal!=applyTime){
                        FriendApplyModel::getInstance()->addAtHead(QSharedPointer<applyInfo>::create(applyTime));
                        applyTimeFinal=applyTime;
                    }
                }
            }
        }));
}

void FriendList::pullFriends()
{
    PullRequest req;
    req.mutable_friend_list()->set_user_id(currentAccount_.toStdString());
    NetWorkManager::getInstance()->addTask(Task(std::move(NetWorkManager::getInstance()->mergeData(req)),
        [this](QByteArray &respondData){
            PullRespond res;
            res.ParseFromString(respondData.toStdString());
            if(res.has_friend_list()){
                FriendListModel::getInstance()->removeRow(0,true);
                const FriendListRespond &fr=res.friend_list();
                for(int i=0;i<fr.user_id_size();++i){
                    const std::string &si=fr.user_id(i);
                    FriendListModel::getInstance()->waitForInfomation(QString::fromStdString(si));
                }
            }
        }));
}

void FriendList::handleApply(int index, QString account,bool result)
{
    PullRequest req;
    req.mutable_update_rela()->set_user_id(currentAccount_.toStdString());
    req.mutable_update_rela()->set_friend_id(account.toStdString());
    req.mutable_update_rela()->set_result(result);
    QString id=NetWorkManager::getInstance()->addTask(Task(
        std::move(NetWorkManager::getInstance()->mergeData(req)),[this,index,result,account](QByteArray &respondData)
            {
                QMetaObject::invokeMethod(interfaceItem_,"verifyDia_setFinishVisible",Q_ARG(QVariant,false));
                PullRespond res;
                res.ParseFromString(respondData.toStdString());
                if(res.has_add_respond()){
                    bool r=res.add_respond().result();
                    QMetaObject::invokeMethod(interfaceItem_,"verifyDia_showMsg",Q_ARG(QVariant,"执行成功"),Q_ARG(QVariant,true));
                    FriendApplyModel::getInstance()->updateState(index,result?applyInfo::applyType::ACCEPT:applyInfo::REJECT);
                    if(result){
                        FriendListModel::getInstance()->waitForInfomation(account);
                        ChatListModel::getinstance()->waitForInfomation(account,"","");
                    }
                } else
                    QMetaObject::invokeMethod(interfaceItem_,"verifyDia_showMsg",Q_ARG(QVariant,"执行失败"),Q_ARG(QVariant,false));

            }));
    QTimer::singleShot(MAX_WAIT_TIME,[this,id]{
        if(!NetWorkManager::getInstance()->removeTask(id))
            return;
        QMetaObject::invokeMethod(interfaceItem_,"verifyDia_showMsg",Q_ARG(QVariant,"执行失败"),Q_ARG(QVariant,false));
        QMetaObject::invokeMethod(interfaceItem_,"verifyDia_setFinishVisible",Q_ARG(QVariant,false));
    });
}

void FriendList::handleTalk(QString name,QString account)
{
    emit createChatDiaSig(account,name);
}

void FriendList::emitUnshowTip()
{
    emit unShowTipSig();
}

void FriendList::deleteFriend(QString target, int position)
{

}
