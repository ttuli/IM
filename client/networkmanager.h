#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <functional>
#include <QQueue>
#include <QMutex>
#include <QWidget>
#include "IM_login.pb.h"
#include "friendAgroup/friendapplymodel.h"
#include "ChatState/chatdialogmodel.h"

#define SERVER_ADDR "your server addr"
#define PORT 50051
#define MAX_WAIT_TIME 5000
#define RECONNECT_TIME 2000

enum CustomType{
    MESSAGE=0,
    HEART=1
};

typedef struct packageHead_t{
    uint32_t type;
    uint32_t dataSize;
}packageHead;

using DataCallBack=std::function<void(QByteArray&)>;

class Task{
public:
    Task(QByteArray &&dataToSend,DataCallBack callback){
        id_="";
        receiveDataSize_=0;
        callback_=nullptr;
        if(callback!=nullptr)
            callback_=std::move(callback);
        data_=std::move(dataToSend);
        fetch_=false;
    }
    Task(){}
    QString id_;
    QByteArray data_;
    DataCallBack callback_;
    qint64 receiveDataSize_;
    QByteArray byteArray_;
    uint32_t type_;
    bool fetch_;
};

class User{
public:
    User(QString account,QString name,QString email,QByteArray image,QString ctime)
        :account_(account),name_(name),email_(email),createTime_(ctime)
    {
        if(image.size()<10)
            image_=QImage(":/new/prefix1/res/pexels-stywo-1054218.jpg");
        else{
            if(!image_.loadFromData(QByteArray::fromBase64(image))){
                qDebug()<<"load fail";
                image_=QImage(":/new/prefix1/res/pexels-stywo-1054218.jpg");
            }

        }
    }

    User(){

    }

    QString account_;
    QString name_;
    QString email_;
    QImage image_;
    QString createTime_;
};

class NetWorkManager : public QObject
{
    Q_OBJECT
public:
    static NetWorkManager* getInstance();

    NetWorkManager operator =(NetWorkManager&) = delete;
    NetWorkManager(NetWorkManager&) = delete;

    QString addTask(Task task);
    bool removeTask(QString id);
    void execTask();
    QByteArray mergeData(PullRequest &data);

    User getInfo(QString id);
    void setInfo(QString id,User u);
    void setCurrentTargetAccount(QString account);
    void setIsUnVisible(bool value);

    void addChatDiaModel(QString id,QSharedPointer<ChatDialogModel> model);
    void removeChatDiaModel(QString id);
    void resetAllModel();

    void addOfflineMsg(QString id,QSharedPointer<message> info);
    QList<QSharedPointer<message>> getOfflineMsg(QString id);

public slots:
    void sendClientId(QString account);

private:
    NetWorkManager(QObject *parent = nullptr);
    ~NetWorkManager();
    void init();

    QTcpSocket *msgSocket_;
    QQueue<Task> taskQueue_;
    QString currentAccount_;
    QString currentTargetAccount_="";
    bool isUnVisible_=false;

    QHash<QString,User> cache_;
    QMap<QString,QSharedPointer<ChatDialogModel>> chatDiaModelList_;
    QMap<QString,QList<QSharedPointer<message>>> offlineMsg_;

    Task tempTask_;
private slots:
    void handleRespond();
    void processRespond();

signals:
    void initResult(bool value);
    void updateInfoSig(QString id);
};

#endif // NETWORKMANAGER_H
