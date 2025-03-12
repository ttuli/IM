#ifndef FRIENDAPPLYMODEL_H
#define FRIENDAPPLYMODEL_H

#include <QAbstractListModel>
#include <QSharedPointer>
#include "networkmanager.h"
#include <QMap>

class applyInfo{
public:
    enum applyType{
        REQUEST,
        ACCEPT,
        REJECT,
        VERIFY
    };

    applyInfo(applyType type,QString account,QString name)
        :account_(account),name_(name)
    {
        type_=type;
        if(type==REQUEST)
            state_="request";
        else if(type==ACCEPT)
            state_="accept";
        else if(type==REJECT)
            state_="reject";
        else if(type==VERIFY)
            state_="verify";
        time_="";
    }

    applyInfo(QString createtime)
    {
        type_=REQUEST;
        state_="time";
        time_=createtime;
        account_="";
        name_="";
    }

    applyType type_;
    QString state_;
    QString account_;
    QString name_;
    QString time_;
};

class FriendApplyModel : public QAbstractListModel
{
    Q_OBJECT

public:
    FriendApplyModel(const FriendApplyModel&)=delete;
    FriendApplyModel operator =(const FriendApplyModel&)=delete;

    static FriendApplyModel *getInstance();

    enum Mapping{
        ACCOUNT,
        NAME,
        STATE,
        TIME
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QSharedPointer<applyInfo> getData(QString id);
    QSharedPointer<applyInfo> getData(int position);

    void addAtHead(QSharedPointer<applyInfo> info,const QModelIndex &parent = QModelIndex());

    void removeRow(int position,bool removeAll=false);

    void waitForInfomation(QString id,applyInfo::applyType type);

    void updateState(int position,applyInfo::applyType type);
    void updateState(QString id,applyInfo::applyType type);

    QString getLastTime();

    QHash<int,QByteArray> roleNames() const;
public slots:
    void handleInfoUpdate(QString id);

private:
    explicit FriendApplyModel(QObject *parent = nullptr);
    ~FriendApplyModel()=default;

    QSet<QString> waitForm_;
    QList<QSharedPointer<applyInfo>> data_;
signals:
    void showTipSig();
};

#endif // FRIENDAPPLYMODEL_H
