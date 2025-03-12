#include "chatlist.h"
#include "customdialog.h"
#include "chatlistmodel.h"
#include <QQuickItem>
#include <QVBoxLayout>
#include <QQmlEngine>
#include <QQmlContext>

ChatList::ChatList(QWidget *parent)
    : QWidget{parent}
{
    provider_=new EmojiImageProvider;

    interface_=new CustomizeQuickWidget(this);
    interface_->engine()->addImageProvider("emojiprovider",provider_);
    interface_->rootContext()->setContextProperty("chatlistModel",ChatListModel::getinstance());
    QVBoxLayout *qvb=new QVBoxLayout(this);
    qvb->setContentsMargins(0,0,0,0);
    qvb->addWidget(interface_);
    interface_->setSource(QUrl("qrc:/ChatState/ChatList.qml"));
    interfaceItem_=interface_->rootObject();

    connect(interfaceItem_,SIGNAL(talkRequestSig(QString,QString)),this,SLOT(onTalkRequest(QString,QString)));
    connect(interfaceItem_,SIGNAL(clearSig(int)),this,SLOT(onClear(int)));
}

ChatList::~ChatList()
{
    if(provider_)
        provider_->deleteLater();
}

void ChatList::setCurrentIndex(QString account)
{
    QMetaObject::invokeMethod(interfaceItem_,"setIndex",Q_ARG(QVariant,account));
}

void ChatList::onTalkRequest(QString account, QString name)
{
    ChatListModel::getinstance()->setMsgNum(account,true);
    emit createChatDiaSig(account,name);
}

void ChatList::onClear(int index)
{
    QString content=(index==-1?"确定清空列表?":"确定移除所选项?");
    CustomDialog dia(this,content);
    if(dia.exec()==QDialog::Rejected)
        return;
    ChatListModel::getinstance()->removeRow(index,index==-1);
}
