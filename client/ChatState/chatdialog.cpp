#include "chatdialog.h"
#include "sqlitemanager.h"
#include "emojimodel.h"
#include "IM_login.pb.h"
#include "threadwork.h"
#include "chatlistmodel.h"
#include "customdialog.h"
#include "imagemanager.h"
#include <QVBoxLayout>
#include <QQuickItem>
#include <QQmlContext>
#include <QDateTime>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QImageReader>
#include <QFuture>
#include <QtConcurrent>
#include <QMessageBox>
#include <QBuffer>
#include <set>

ChatDialog::ChatDialog(QWidget *parent,QString tarAccount,QString tarName,QString curAccount)
    : QWidget{parent},tarAccount_(tarAccount),tarName_(tarName),offset_(0),curAccount_(curAccount)
{
    model_=QSharedPointer<ChatDialogModel>::create(nullptr,tarAccount);
    emojiProvider_=new EmojiImageProvider;
    generalProvider_=new GeneralImageProvider;
    currentFile_="";

    interface_=new CustomizeQuickWidget(this);
    interface_->engine()->addImageProvider("emojiProvider",emojiProvider_);
    interface_->engine()->addImageProvider("generalProvider",generalProvider_);
    interface_->rootContext()->setContextProperty("emojiModel",EmojiModel::getInstance());
    interface_->rootContext()->setContextProperty("chatDialogModel",model_.get());
    interface_->setSource(QUrl("qrc:/ChatState/ChatDialog.qml"));
    interfaceItem_=interface_->rootObject();
    QMetaObject::invokeMethod(interfaceItem_,"setCaption",Q_ARG(QVariant,tarName));
    QMetaObject::invokeMethod(interfaceItem_,"setMaxPicWidth",Q_ARG(QVariant,MAX_PIC_WIDTH));
    connect(interfaceItem_,SIGNAL(sendMsgSig(QString,int)),this,SLOT(sendMsg(QString,int)));
    connect(interfaceItem_,SIGNAL(pullMoreHistorySig()),this,SLOT(pullHistoryMsg()));
    connect(interfaceItem_,SIGNAL(sendPicSig()),this,SLOT(sendPic()));
    connect(interfaceItem_,SIGNAL(showHdImage(QString)),this,SLOT(showHdImage(QString)));
    connect(interfaceItem_,SIGNAL(updateChoiceState(QString,int,bool)),this,SLOT(updateChosedState(QString,int,bool)));
    connect(interfaceItem_,SIGNAL(deleteSig()),this,SLOT(deleteMsg()));

    QVBoxLayout *qvb=new QVBoxLayout(this);
    qvb->setContentsMargins(0,0,0,0);
    qvb->addWidget(interface_);

    User u=NetWorkManager::getInstance()->getInfo(curAccount_);
    curName_=u.name_;

    NetWorkManager::getInstance()->addChatDiaModel(tarAccount,model_);

    pullHistoryMsg();
}

ChatDialog::~ChatDialog()
{
    NetWorkManager::getInstance()->removeChatDiaModel(curAccount_);
    if(emojiProvider_){
        emojiProvider_->deleteLater();
    }
    if(generalProvider_)
        generalProvider_->deleteLater();
}


void ChatDialog::sendMsg(QString msg,int type)
{
    if(type==message::msgType::FRIEND){
        msg=msg.replace("'", "''");
        msg=msg.replace("\\", "\\\\");
    }

    QString currentTime=QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString message_id=curAccount_+tarAccount_+currentTime;
    PullRequest req;
    req.mutable_communication()->set_content(msg.toStdString());
    req.mutable_communication()->set_message_id(message_id.toStdString());
    req.mutable_communication()->set_target_id(tarAccount_.toStdString());
    req.mutable_communication()->set_user_id(curAccount_.toStdString());
    req.mutable_communication()->set_msg_type(type);
    req.mutable_communication()->set_md5("");

    QString timeStemp=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString timeStemp2=QDateTime::currentDateTime().toString("yyyy-MM-dd");

    int position=model_->addRow(-1,QSharedPointer<message>::create(msg,curAccount_,curName_,(message::msgType)type,timeStemp,message::MINE,message_id));
    if(position==-1)
        return;

    QString id=NetWorkManager::getInstance()->addTask(Task(std::move(NetWorkManager::getInstance()->mergeData(req)),[this,position,msg,timeStemp2](QByteArray &rdata){
        PullRespond resq;
        resq.ParseFromString(rdata.toStdString());
        if(resq.has_communication()){
            model_->updateRow(position,(message::Status)resq.communication().result());
            ChatListModel::getinstance()->adjustToTop(tarAccount_,msg,timeStemp2);
        }
    }));
    QTimer::singleShot(MAX_WAIT_TIME,this,[this,position,id]{
        QSharedPointer<message> info=model_->getInfo(position);
        if(info==nullptr)
            return;
        if(info->st_==message::SENDING)
            model_->updateRow(position,message::FAIL);
        NetWorkManager::getInstance()->removeTask(id);
    });

    ChatListModel::getinstance()->adjustToTop(tarAccount_,"","");
    emit updateOrderSig(tarAccount_);
}

void ChatDialog::sendPic()
{
    QString filename=QFileDialog::getOpenFileName(this,"选择文件",QString(),"*.png *.jpg");
    if(filename.isEmpty())
        return;
    currentFile_=filename;

    ThreadWork::getInstance()->execTask([this,filename]{
        QImageReader reader(filename);
        reader.setQuality(80);
        QImage ima = reader.read();

        if(!ima.isNull()) {
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            buffer.open(QIODevice::WriteOnly);
            ima.save(&buffer, "PNG");
            buffer.close();

            byteArray.toBase64();
            QMetaObject::invokeMethod(this, "handleImageData", Qt::QueuedConnection,
                                      Q_ARG(QByteArray, std::move(byteArray)));
        } else {
            qDebug()<<"load fail";
        }
    });
    //handleImageData(std::move(getImageData(ima).toBase64()));
}

void ChatDialog::handleImageData(QByteArray byte1)
{
    QFile file(currentFile_);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file for reading:";
        return;
    }
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(&file);
    file.close();
    QString md5=hash.result().toHex();

    ImageManager::getInstance()->addImageCache(md5,byte1);

    QString currentTime=QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString message_id=curAccount_+tarAccount_+currentTime;
    QString timeStemp=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString timeStemp2=QDateTime::currentDateTime().toString("yyyy-MM-dd");

    QSharedPointer<message> info=QSharedPointer<message>::create(md5,
                                                                   curAccount_,
                                                                   curName_,message::msgType::IMAGE,
                                                                   timeStemp,message::MINE,message_id);

    int position=model_->addRow(-1,info);
    if(position==-1){
        return;
    }

    PullRequest req;
    req.mutable_communication()->set_content(byte1.toStdString());
    req.mutable_communication()->set_message_id(message_id.toStdString());
    req.mutable_communication()->set_target_id(tarAccount_.toStdString());
    req.mutable_communication()->set_user_id(curAccount_.toStdString());
    req.mutable_communication()->set_msg_type((int)(message::IMAGE));
    req.mutable_communication()->set_md5(md5.toStdString());

    QString id=NetWorkManager::getInstance()->addTask(Task(std::move(NetWorkManager::getInstance()->mergeData(req)),[this,position,timeStemp2](QByteArray &rdata){
        PullRespond resq;
        resq.ParseFromString(rdata.toStdString());
        if(resq.has_communication()){
            model_->updateRow(position,(message::Status)resq.communication().result());
            ChatListModel::getinstance()->adjustToTop(tarAccount_,"[图片]",timeStemp2);
        }
    }));
    QTimer::singleShot(MAX_WAIT_TIME,this,[this,position,id]{
        QSharedPointer<message> info=model_->getInfo(position);
        if(info==nullptr)
            return;
        if(info->st_==message::SENDING)
            model_->updateRow(position,message::FAIL);
        NetWorkManager::getInstance()->removeTask(id);
    });

    ChatListModel::getinstance()->adjustToTop(tarAccount_,"","");
    emit updateOrderSig(tarAccount_);
}

void ChatDialog::showHdImage(QString md5)
{
    PictureViewManager::getInstance()->createView(md5);
}

QByteArray ChatDialog::getImageData(QImage image, short quality)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    if (!image.save(&buffer,"PNG",quality)) {
        qWarning() << "Failed to save image to buffer!";
        return QByteArray();
    }
    return byteArray;
}

void ChatDialog::updateChosedState(QString id,int position, bool value)
{
    if(value)
        chosedList_[position]=id;
    else
        chosedList_.remove(position);
}

void ChatDialog::deleteMsg()
{
    CustomDialog dia(this,"确定删除所选内容？");
    if(dia.exec()==QDialog::Rejected)
        return;
    QStringList lists;
    std::set<int,std::greater<int>> order;
    for(QMap<int,QString>::const_iterator it=chosedList_.constBegin();it!=chosedList_.constEnd();++it){
        lists<<it.value();
        order.insert(it.key());
    }
    if(SqliteManager::getInstance()->deleteMsg(lists)){
        for(auto i:order){
            model_->removeRow(i);
        }
        while(1){
            QSharedPointer<message> info=model_->getInfo(model_->rowCount()-1);
            if(info&&info->type_==message::TIME){
                model_->removeRow(model_->rowCount()-1);
            } else {
                if(!info){
                    ChatListModel::getinstance()->adjustToTop(tarAccount_," "," ");
                } else {
                    QString timestemp=info->createTime_;
                    timestemp=timestemp.left(timestemp.indexOf(" "));
                    QString lastMsg=info->content_;
                    if(info->type_==message::IMAGE)
                        lastMsg="[图片]";
                    ChatListModel::getinstance()->adjustToTop(tarAccount_,lastMsg,timestemp);
                }
                break;
            }
        }

        chosedList_.clear();
    }

}

void ChatDialog::pullHistoryMsg()
{
    SqliteManager::getInstance()->asyncRead(pullformat(curAccount_,tarAccount_,offset_),
        [this](const QVector<message>& result){
            QMetaObject::invokeMethod(this,[this,result]{
                    for(auto i:result){
                        i.name_=i.account_==curAccount_?curName_:tarName_;
                        model_->addRow(-1,QSharedPointer<message>::create(i));
                    }
                    if(result.size()){
                        QString timeStemp=result.back().createTime_.left(result.back().createTime_.indexOf(" "));
                        if(ChatListModel::getinstance()->getTextIsNull(tarAccount_))
                            ChatListModel::getinstance()->adjustToTop(tarAccount_,result.back().content_,timeStemp);
                    }
                    pullOfflineMsg();
                },Qt::QueuedConnection);
        });
    offset_+=50;
}

void ChatDialog::pullOfflineMsg()
{
    QList<QSharedPointer<message>> list=NetWorkManager::getInstance()->getOfflineMsg(tarAccount_);
    for(int i=0;i<list.size();i++){
        list[i]->name_=tarName_;
        model_->addRow(-1,list[i]);
    }
}

