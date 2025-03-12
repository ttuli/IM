#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H

#include <QAbstractListModel>
#include <QSharedPointer>

#define MAX_SESSION_NB 30

class chatListInfo{
public:
    chatListInfo(QString account,QString name,QString lastMsg,QString timeStemp)
        :account_(account),name_(name),lastMsg_(lastMsg),timeStemp_(timeStemp)
    {
        unReadMsg_=0;
    }
    chatListInfo(QString account,QString name,QString lastMsg,QString timeStemp,int unReadMsg)
        :account_(account),lastMsg_(lastMsg),timeStemp_(timeStemp),unReadMsg_(unReadMsg),name_(name)
    {

    }

    QString account_;
    QString name_;
    QString lastMsg_;
    QString timeStemp_;
    int unReadMsg_;
};

class ChatListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ChatListModel(const ChatListModel&)=delete;
    ChatListModel operator =(const ChatListModel&)=delete;

    static ChatListModel* getinstance();

    enum {
      ACCOUNT,
        NAME,
        LASTMSG,
        TIME,
        UNREAD
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int,QByteArray> roleNames() const;

    void waitForInfomation(QString id,QString lastMsg,QString timeStemp,bool addUnReadMsg=false);

    void addRow(int position,QSharedPointer<chatListInfo> info,const QModelIndex &parent = QModelIndex());
    void removeRow(int position,bool removeAll=false,const QModelIndex &parent=QModelIndex());

    void adjustToTop(QString id,QString lastMsg,QString timeStemp,bool addUnReadMsg=false,const QModelIndex &parent = QModelIndex());
    bool isContain(QString id);

    bool getTextIsNull(QString id);
    void setMsgNum(QString account,bool doClear);

    void writeAllInfo();
public slots:
    void handleInfoUpdate(QString id);

private:
    explicit ChatListModel(QObject *parent = nullptr);
    ~ChatListModel()=default;

    QSet<QString> waitForm_;
    QList<QSharedPointer<chatListInfo>> data_;
    int totalMsgNum_=0;
signals:
    void msgNumChanged(int num);
};

#endif // CHATLISTMODEL_H
