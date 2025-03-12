#include "chatstate.h"
#include "IM_login.pb.h"
#include "networkmanager.h"
#include "sqlitemanager.h"
#include "imagemanager.h"
#include "chatlistmodel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QQuickItem>
#include <QApplication>
#include <QTimer>
#include <QQmlEngine>
#include <QSettings>
#include <QMenu>
#include <QProcess>
#include <QFile>
#include <QQueue>

ChatState::ChatState(QWidget *parent)
    :WindowsBackground(parent)
{
    sqliteThread_=new QThread(this);
    SqliteManager::getInstance()->moveToThread(sqliteThread_);
    connect(sqliteThread_,&QThread::finished,SqliteManager::getInstance(),&SqliteManager::endWork);
    sqliteThread_->start();

    infoDia_=QSharedPointer<PersonalInfoDialog>::create(nullptr);
    addFriendDia_=QSharedPointer<AddFriend>::create(nullptr);

    leftChoiceColum_=new CustomizeQuickWidget(this);
    leftChoiceColum_->setSource(QUrl("qrc:/ChatState/LeftChoiceColumn.qml"));
    leftChoiceColum_->setFixedWidth(60);
    leftChoiceColumItem_=leftChoiceColum_->rootObject();
    connect(leftChoiceColumItem_,SIGNAL(checkInfoSig()),this,SLOT(showPersonalInfo()));
    connect(leftChoiceColumItem_,SIGNAL(addFriendSig()),this,SLOT(doAddFriend()));
    connect(leftChoiceColumItem_,SIGNAL(choiceSig(int)),this,SLOT(changeCurrentWidget(int)));
    connect(FriendApplyModel::getInstance(),&FriendApplyModel::showTipSig,[=]{
        QMetaObject::invokeMethod(leftChoiceColumItem_,"setTip",Q_ARG(QVariant,true));
    });
    connect(NetWorkManager::getInstance(),&NetWorkManager::updateInfoSig,[=](QString id){
        if(id!=currentAccount_)
            return;
        QMetaObject::invokeMethod(leftChoiceColumItem_,"setImage",Q_ARG(QVariant,id));
    });
    connect(ChatListModel::getinstance(),&ChatListModel::msgNumChanged,[this](int num){
        QMetaObject::invokeMethod(leftChoiceColumItem_,"setTotalMsgNum",Q_ARG(QVariant,num));
    });


    titleBar_=new CustomizeQuickWidget(this);
    titleBar_->setSource(QUrl("qrc:/ChatState/TitleBar.qml"));
    titleBar_->setFixedHeight(30);
    titleBarItem_=titleBar_->rootObject();
    connect(titleBarItem_,SIGNAL(closeSig()),this,SLOT(hide()));
    connect(titleBarItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(titleBarItem_,SIGNAL(maxSig()),this,SLOT(updateWindow()));
    connect(titleBarItem_,SIGNAL(flushSig()),this,SLOT(flush()));

    chatList_=new ChatList(this);
    chatList_->setMinimumWidth(200);
    chatList_->setMaximumWidth(width()/3);
    connect(chatList_,&ChatList::createChatDiaSig,this,&ChatState::createChatDia);

    pageControl_=new QStackedWidget(this);

    friendlist_=new FriendList(this);
    connect(friendlist_,&FriendList::unShowTipSig,[=]{
        QMetaObject::invokeMethod(leftChoiceColumItem_,"setTip",Q_ARG(QVariant,false));
    });
    connect(friendlist_,&FriendList::createChatDiaSig,this,&ChatState::createChatDia);
    pageControl_->addWidget(friendlist_);


    chatDiaSwitch_=new QStackedWidget(this);
    chatDiaSwitch_->setMinimumWidth(100);
    splitter=new QSplitter(this);
    splitter->addWidget(chatList_);
    splitter->addWidget(chatDiaSwitch_);
    splitter->setHandleWidth(1);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Horizontal);
    pageControl_->addWidget(splitter);
    pageControl_->setCurrentWidget(splitter);


    QVBoxLayout *qvb=new QVBoxLayout;
    qvb->setContentsMargins(0,0,0,0);
    qvb->setSpacing(0);
    qvb->addWidget(titleBar_);
    qvb->addWidget(pageControl_);


    qhb_=new QHBoxLayout(this);
    qhb_->setContentsMargins(0,0,0,0);
    qhb_->setSpacing(0);
    qhb_->addWidget(leftChoiceColum_);
    qhb_->addLayout(qvb);


    setTitleWidget(titleBar_,titleBarItem_);
    setMinimumSize(800,700);
    resize(800,700);

    setConnectInfoVisible("拉取中...",true);
}

ChatState::~ChatState()
{
    sqliteThread_->quit();
    sqliteThread_->wait();
}

void ChatState::setSystemIcon()
{
    systemIcon_=new QSystemTrayIcon(QIcon(":/new/prefix1/res/IM_ICON.svg"),this);
    systemIcon_->setToolTip("IM");
    systemIcon_->setVisible(true);
    connect(systemIcon_,&QSystemTrayIcon::activated,[=](QSystemTrayIcon::ActivationReason reason){
        if(reason==QSystemTrayIcon::Trigger){
            showNormal();
            show();
            activateWindow();
        }
    });

    QMenu *menu=new QMenu(this);

    QAction *loginoutBtn=new QAction("注销",this);
    connect(loginoutBtn,&QAction::triggered,this,&ChatState::restartProgram);
    menu->addAction(loginoutBtn);

    QAction *quitBtn=new QAction("退出",this);
    connect(quitBtn,&QAction::triggered,[=]{
        qApp->quit();
    });
    menu->addAction(quitBtn);


    systemIcon_->setContextMenu(menu);
}

void ChatState::pullOfflineMsg()
{
    PullRequest req;
    req.mutable_offline_request()->set_user_id(currentAccount_.toStdString());

    NetWorkManager::getInstance()->addTask(Task(std::move(NetWorkManager::getInstance()->mergeData(req)),[this](QByteArray &receive){
        PullRespond res;
        res.ParseFromString(receive.toStdString());
        if(res.has_offline_respond()){
            for(int i=0;i<res.offline_respond().info_size();++i){
                const offline_info &info=res.offline_respond().info(i);
                QString tar=QString::fromStdString(info.friend_id());
                QString timeStemp=QString::fromStdString(info.timestemp());
                message::msgType type=(message::msgType)(info.msg_type());
                QString content="";
                if(type==message::FRIEND){
                    content=QString::fromStdString(info.message());
                } else if(type==message::IMAGE){
                    content=QString::fromStdString(info.md5());
                    ImageManager::getInstance()->addImageCache(content,QByteArray::fromStdString(info.message()));
                }

                timeStemp=timeStemp.left(timeStemp.indexOf(" "));
                ChatListModel::getinstance()->adjustToTop(tar,
                                                          (type==message::IMAGE?"[图片]":content),
                                                          timeStemp,true);
                QSharedPointer<message> m=QSharedPointer<message>::create(content,QString::fromStdString(info.friend_id()),"",type,
                                                                            QString::fromStdString(info.timestemp()),
                                                                            message::OTHER,QString::fromStdString(info.message_id()));
                NetWorkManager::getInstance()->addOfflineMsg(tar,m);
                SqliteManager::getInstance()->batchWrite(m,tar);
            }
        }
    }));
}

void ChatState::pullList()
{
    SqliteManager::getInstance()->readList([this](const QVector<chatListInfo> infos){
        if(QThread::currentThread()!=this->thread()){
            QMetaObject::invokeMethod(this,[this,infos]{
                for(auto i:infos){
                    ChatListModel::getinstance()->addRow(-1,QSharedPointer<chatListInfo>::create(i));
                }
            });
            return;
        }
        for(auto i:infos){
            ChatListModel::getinstance()->addRow(-1,QSharedPointer<chatListInfo>::create(i));
        }
    });
}

void ChatState::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::WindowStateChange){
        QWindowStateChangeEvent *stateEvent=dynamic_cast<QWindowStateChangeEvent*>(event);
        qhb_->setContentsMargins(0,0,0,0);
        if(isMaximized())
            qhb_->setContentsMargins(5,5,5,5);
        if(stateEvent->oldState()==Qt::WindowMaximized){
            QMetaObject::invokeMethod(titleBarItem_,"setIsMax",Q_ARG(QVariant,isMinimized()));
        }
        else if(stateEvent->oldState()==Qt::WindowNoState){
            QMetaObject::invokeMethod(titleBarItem_,"setIsMax",Q_ARG(QVariant,isMaximized()));
        }
        NetWorkManager::getInstance()->setIsUnVisible(isMinimized()||!isVisible());
    }
}

void ChatState::resizeEvent(QResizeEvent *event)
{
    chatList_->setMaximumWidth(event->size().width()/3);
}

void ChatState::updateWindow()
{
    if(isMaximized()){
        showNormal();
    } else {
        showMaximized();
    }
}

void ChatState::showPersonalInfo()
{
    if(!infoDia_->isVisible())
        infoDia_->show();
    infoDia_->raise();
}

void ChatState::doAddFriend()
{
    if(!addFriendDia_->isVisible())
        addFriendDia_->show();
    addFriendDia_->raise();
}

void ChatState::changeCurrentWidget(int index)
{
    QMetaObject::invokeMethod(leftChoiceColumItem_,"setCurrentIndex",Q_ARG(QVariant,index));
    if(!index){
        pageControl_->setCurrentWidget(splitter);
        if(chatDiaSwitch_->currentWidget()){
            NetWorkManager::getInstance()->setCurrentTargetAccount(static_cast<ChatDialog*>(chatDiaSwitch_->currentWidget())->getCurId());
        }
    } else if(index==1) {
        pageControl_->setCurrentWidget(friendlist_);
        NetWorkManager::getInstance()->setCurrentTargetAccount("");
    }
}

void ChatState::createChatDia(QString account, QString name)
{
    NetWorkManager::getInstance()->setCurrentTargetAccount(account);
    changeCurrentWidget(0);
    if(!ChatListModel::getinstance()->isContain(account))
        ChatListModel::getinstance()->adjustToTop(account,"","");
    if(chatDias_.contains(account)){
        chatDiaSwitch_->setCurrentWidget(chatDias_[account].get());
        return;
    }
    if(enterOrder_.size()>=MAX_CHAT_IN_CACHE){
        QString id=enterOrder_.front();
        enterOrder_.pop_front();
        QSharedPointer<ChatDialog> cd=chatDias_[id];
        chatDias_.remove(id);
        chatDiaSwitch_->removeWidget(cd.get());
    }
    QSharedPointer<ChatDialog> cdia=QSharedPointer<ChatDialog>::create(this,account,name,currentAccount_);
    enterOrder_.append(account);
    chatDias_[account]=cdia;
    chatDiaSwitch_->addWidget(cdia.get());
    chatDiaSwitch_->setCurrentWidget(cdia.get());

    chatList_->setCurrentIndex(account);

    connect(cdia.get(),&ChatDialog::updateOrderSig,this,&ChatState::updateEnterOrder);
}

void ChatState::updateEnterOrder(QString id)
{
    for(int i=0;i<enterOrder_.size();++i){
        if(enterOrder_[i]==id){
            QString tmp=enterOrder_[i];
            enterOrder_[i]=enterOrder_[enterOrder_.size()-1];
            enterOrder_[enterOrder_.size()-1]=tmp;
            break;
        }
    }
}

void ChatState::setConnectInfoVisible(QString loading_text, bool visible)
{
    QMetaObject::invokeMethod(titleBarItem_,"setloadingCaptionVisible",Q_ARG(QVariant,loading_text),Q_ARG(QVariant,visible));
}




//------------------------拉取任务--------------------------------------------------
void ChatState::flush()
{
    //setCurrentAccount(currentAccount_);
}

void ChatState::restartProgram()
{
    QString program = QCoreApplication::applicationFilePath();

    // 获取当前程序的命令行参数（可选）
    QStringList arguments = QCoreApplication::arguments();
    arguments.removeFirst(); // 移除程序路径本身

    QFile file("config.ini");
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    QSettings setting("config.ini", QSettings::IniFormat);
    setting.setValue("User/password",QVariant(""));
    setting.setValue("remberPwd",QVariant(false));
    setting.setValue("autoLogin",QVariant(false));

    QProcess::startDetached(program, arguments);

    qApp->quit();
}

void ChatState::setCurrentAccount(QString account)
{
    setSystemIcon();
    currentAccount_=account;
    infoDia_->setAccount_P(account);
    QMetaObject::invokeMethod(leftChoiceColumItem_,"setImage",Q_ARG(QVariant,account));
    friendlist_->setCurrentAccount_F(account);
    addFriendDia_->setCurrentAccount_A(account);
    SqliteManager::getInstance()->setCurrentAccount(account);
    setConnectInfoVisible("",false);
    pullList();
    pullOfflineMsg();
}





