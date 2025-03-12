#ifndef CHATDIALOGMODEL_H
#define CHATDIALOGMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QQueue>
#include <QTimer>

#define MSG_WAIT_TIME_SEC 10

class message
{
public:
    enum msgType{
        FRIEND=0,
        TIME=1,
        IMAGE=2,
        WITHDRAW=3,
        DELETED=4
    };
    enum msgWay{
        MINE=0,
        OTHER=1
    };
    enum Status{
        SENDING,
        FINISH,
        FAIL
    };

    message(QString content,QString account,QString name,msgType type,QString createTime,msgWay way,QString message_id)
        :content_(content),account_(account),name_(name),type_(type),way_(way),message_id_(message_id),createTime_(createTime)
    {
        st_=SENDING;
    }
    message(){}

    QString content_;
    QString account_;
    QString name_;
    int type_;
    QString createTime_;
    QString message_id_;
    int way_;
    Status st_;
};

class pendingMsg
{
public:
    pendingMsg(QString timeStamp,int index)
        :timeStamp_(timeStamp),index_(index)
    {}

    QString timeStamp_;
    int index_;
};

class ChatDialogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum{
        CONTENT,
        ACCOUNT,
        NAME,
        TYPE,
        CREATE_TIME,
        WAY,
        STATE,
        MSGID
    };

    explicit ChatDialogModel(QObject *parent = nullptr,QString tarAccount="");

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int addRow(int position,QSharedPointer<message> info,const QModelIndex &parent=QModelIndex());
    void removeRow(int position,bool removeAll=false,const QModelIndex &parent=QModelIndex());
    void updateRow(int index,message::Status st);
    QSharedPointer<message> getInfo(int index);
    QString getNewTime();
    bool judgeTime(QString timeStemp);

    void resetAll();

    QHash<int,QByteArray> roleNames() const;
private:
    QList<QSharedPointer<message>> data_;

    QString newestTime_="";
    QString tarAccount_;
};

#endif // CHATDIALOGMODEL_H
