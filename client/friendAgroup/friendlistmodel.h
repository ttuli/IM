#ifndef FRIENDLISTMODEL_H
#define FRIENDLISTMODEL_H

#include <QAbstractListModel>
#include "networkmanager.h"

class FriendListInfo{
public:
    FriendListInfo(QString name,QString account)
        :name_(name),account_(account)
    {}

    QString name_;
    QString account_;
};

class FriendListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    static FriendListModel* getInstance();
    FriendListModel(const FriendListModel&)=delete;
    FriendListModel operator =(const FriendListModel&)=delete;

    enum{
        NAME,
        ACCOUNT
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int,QByteArray> roleNames() const;

    void waitForInfomation(QString id);

    void addRow(int position,QSharedPointer<FriendListInfo> info,const QModelIndex &parent = QModelIndex());

    void removeRow(int position,bool removeAll=false,const QModelIndex &parent = QModelIndex());
public slots:
    void handleInfoUpdate(QString id);

private:
    explicit FriendListModel(QObject *parent = nullptr);
    ~FriendListModel()=default;

    QSet<QString> waitForm_;
    QList<QSharedPointer<FriendListInfo>> data_;
};

#endif // FRIENDLISTMODEL_H
