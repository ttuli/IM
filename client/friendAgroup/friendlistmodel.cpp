#include "friendlistmodel.h"

FriendListModel::FriendListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(NetWorkManager::getInstance(),&NetWorkManager::updateInfoSig,this,&FriendListModel::handleInfoUpdate,Qt::QueuedConnection);
}

FriendListModel *FriendListModel::getInstance()
{
    static FriendListModel obj;
    return &obj;
}

int FriendListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

QVariant FriendListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role==NAME)
        return data_[index.row()]->name_;
    if(role==ACCOUNT)
        return data_[index.row()]->account_;

    return QVariant();
}

QHash<int, QByteArray> FriendListModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash.insert(NAME,"name");
    hash.insert(ACCOUNT,"account");

    return hash;

}

void FriendListModel::waitForInfomation(QString id)
{
    User u=NetWorkManager::getInstance()->getInfo(id);
    if(!u.image_.isNull()){
        addRow(rowCount(),QSharedPointer<FriendListInfo>::create(u.name_,id));
        return;
    }
    waitForm_.insert(id);
    addRow(rowCount(),QSharedPointer<FriendListInfo>::create("",id));
}

void FriendListModel::handleInfoUpdate(QString id)
{
    if(waitForm_.contains(id)){
        User u=NetWorkManager::getInstance()->getInfo(id);
        for(int i=0;i<data_.size();++i){
            if(data_[i]->account_==id){
                data_[i]->name_=u.name_;
                emit dataChanged(index(i),index(i),{NAME});
                break;
            }
        }
        waitForm_.remove(id);
    }
}

void FriendListModel::addRow(int position, QSharedPointer<FriendListInfo> info,const QModelIndex &parent)
{
    if(position<0||position>rowCount())
        return;
    beginInsertRows(parent,position,position);
    data_.insert(position,info);
    endInsertRows();
}

void FriendListModel::removeRow(int position, bool removeAll,const QModelIndex &parent)
{
    if(removeAll){
        if(data_.size()==0)
            return;
        beginRemoveColumns(parent, 0, rowCount()-1);
        data_.clear();
        endRemoveRows();
    } else {
        if(position<0||position>=rowCount())
            return;
        beginRemoveColumns(parent, position, position);
        data_.removeAt(position);
        endRemoveRows();
    }
}
