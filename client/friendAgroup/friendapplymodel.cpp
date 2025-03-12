#include "friendapplymodel.h"

FriendApplyModel::FriendApplyModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(NetWorkManager::getInstance(),&NetWorkManager::updateInfoSig,this,&FriendApplyModel::handleInfoUpdate);
}

FriendApplyModel *FriendApplyModel::getInstance()
{
    static FriendApplyModel obj;
    return &obj;
}

int FriendApplyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return data_.size();
}

QVariant FriendApplyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==Mapping::ACCOUNT)
        return data_[index.row()]->account_;
    if(role==Mapping::NAME)
        return data_[index.row()]->name_;
    if(role==Mapping::STATE)
        return data_[index.row()]->state_;
    if(role==Mapping::TIME)
        return data_[index.row()]->time_;

    return QVariant();
}

QSharedPointer<applyInfo> FriendApplyModel::getData(QString id)
{
    for(int i=0;i<data_.size();++i){
        if(data_[i]->account_==id)
        {
            return data_[i];
        }
    }
    return nullptr;
}

QSharedPointer<applyInfo> FriendApplyModel::getData(int position)
{
    if(position<0||position>=rowCount())
        return nullptr;
    return data_[position];
}

void FriendApplyModel::addAtHead(QSharedPointer<applyInfo> info, const QModelIndex &parent)
{
    beginInsertRows(parent, 0, 0);
    data_.insert(0,info);
    endInsertRows();
}

void FriendApplyModel::removeRow(int position, bool removeAll)
{
    if(removeAll){
        beginRemoveColumns(QModelIndex(), 0, rowCount()-1);
        data_.clear();
        endRemoveRows();
    } else {
        if(position<0||position>=rowCount())
            return;
        beginRemoveColumns(QModelIndex(), position, position);
        data_.removeAt(position);
        endRemoveRows();
    }
}

void FriendApplyModel::waitForInfomation(QString id,applyInfo::applyType type)
{
    if(type==applyInfo::VERIFY)
        emit showTipSig();
    for(int i=0;i<rowCount();++i){
        if(data_[i]->account_==id){
            removeRow(i);
            break;
        }
    }
    User u=NetWorkManager::getInstance()->getInfo(id);
    if(!u.image_.isNull()){
        addAtHead(QSharedPointer<applyInfo>::create(type,id,u.name_));
        return;
    }
    waitForm_.insert(id);
    addAtHead(QSharedPointer<applyInfo>::create(type,id,""));
}

void FriendApplyModel::handleInfoUpdate(QString id)
{
    if(waitForm_.contains(id)){
        User u=NetWorkManager::getInstance()->getInfo(id);
        for(int i=0;i<data_.size();++i){
            if(data_[i]->account_==id){
                data_[i]->name_=u.name_;
                emit dataChanged(index(i),index(i),{Mapping::NAME});
                break;
            }
        }
        waitForm_.remove(id);
    }
}

void FriendApplyModel::updateState(int position, applyInfo::applyType type)
{
    if(position<0||position>=rowCount())
        return;
    data_[position]->type_=type;
    QString st="";
    if(type==applyInfo::REQUEST)
        st="request";
    else if(type==applyInfo::ACCEPT)
        st="accept";
    else if(type==applyInfo::REJECT)
        st="reject";
    else if(type==applyInfo::VERIFY)
        st="verify";
    data_[position]->state_=st;
    emit dataChanged(this->index(position),this->index(position),{Mapping::STATE});
}

void FriendApplyModel::updateState(QString id, applyInfo::applyType type)
{
    for(int i=0;i<data_.size();++i){
        if(data_[i]->account_==id)
        {
            updateState(i,type);
            return;
        }
    }
}

QString FriendApplyModel::getLastTime()
{
    if(data_.size()==0)
        return "";
    return data_[0]->time_;
}


QHash<int, QByteArray> FriendApplyModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(Mapping::ACCOUNT,"account");
    roles.insert(Mapping::NAME,"name");
    roles.insert(Mapping::STATE,"msgstate");
    roles.insert(Mapping::TIME,"msgtime");

    return roles;
}
