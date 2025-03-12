#include "chatlistmodel.h"
#include "networkmanager.h"
#include "sqlitemanager.h"

ChatListModel::ChatListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(NetWorkManager::getInstance(),&NetWorkManager::updateInfoSig,this,&ChatListModel::handleInfoUpdate);
}

ChatListModel *ChatListModel::getinstance()
{
    static ChatListModel obj;
    return &obj;
}

int ChatListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

QVariant ChatListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role==ACCOUNT)
        return data_[index.row()]->account_;
    if(role==NAME)
        return data_[index.row()]->name_;
    if(role==LASTMSG)
        return data_[index.row()]->lastMsg_;
    if(role==TIME)
        return data_[index.row()]->timeStemp_;
    if(role==UNREAD)
        return data_[index.row()]->unReadMsg_;

    return QVariant();
}

QHash<int, QByteArray> ChatListModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash.insert(ACCOUNT,"account");
    hash.insert(NAME,"name");
    hash.insert(LASTMSG,"lastMsg");
    hash.insert(TIME,"timestemp");
    hash.insert(UNREAD,"unreadMsg");
    return hash;
}

void ChatListModel::waitForInfomation(QString id,QString lastMsg,QString timeStemp,bool addUnReadMsg)
{
    for(int i=0;i<rowCount();i++){
        if(data_[i]->account_==id){
            removeRow(i);
            break;
        }
    }
    User u=NetWorkManager::getInstance()->getInfo(id);
    QSharedPointer<chatListInfo> info=QSharedPointer<chatListInfo>::create(id,u.name_,lastMsg,timeStemp);
    if(addUnReadMsg){
        info->unReadMsg_++;
        totalMsgNum_++;
    }else{
        totalMsgNum_-=info->unReadMsg_;
        info->unReadMsg_=0;
    }
    emit msgNumChanged(totalMsgNum_);
    if(!u.image_.isNull()){
        addRow(0,info);
        return;
    }
    waitForm_.insert(id);
    addRow(0,info);
}

void ChatListModel::addRow(int position, QSharedPointer<chatListInfo> info,const QModelIndex &parent)
{
    if(position<-1||position>rowCount())
        return;
    if(position==-1)
        position=rowCount();
    beginInsertRows(parent,position,position);
    data_.insert(position,info);
    endInsertRows();
}

void ChatListModel::removeRow(int position, bool removeAll,const QModelIndex &parent)
{
    if(removeAll){
        beginRemoveRows(parent,0,rowCount()-1);
        data_.clear();
        endRemoveRows();
        return;
    }
    if(position<0||position>=rowCount())
        return;
    beginRemoveRows(parent,position,position);
    data_.removeAt(position);
    endRemoveRows();
}


void ChatListModel::adjustToTop(QString id,QString lastMsg,QString timeStemp,bool addUnReadMsg,const QModelIndex &parent)
{
    bool isin=false;
    QList<int> role;
    for(int i=0;i<data_.size();++i){
        if(data_[i]->account_==id){
            isin=true;

            if(lastMsg!=""){
                data_[i]->lastMsg_=lastMsg;
                role<<LASTMSG;
            }
            if(timeStemp!=""){
                data_[i]->timeStemp_=timeStemp;
                role<<TIME;
            }
            if(addUnReadMsg){
                data_[i]->unReadMsg_++;
                totalMsgNum_++;
                role<<UNREAD;
            }else{
                totalMsgNum_-=data_[i]->unReadMsg_;
                data_[i]->unReadMsg_=0;
                role<<UNREAD;
            }
            emit msgNumChanged(totalMsgNum_);

            if(i != 0) {
                beginMoveRows(QModelIndex(), i, i, QModelIndex(), 0);
                data_.move(i, 0);
                endMoveRows();
            }
            if(role.size())
                emit dataChanged(this->index(0),this->index(0),role);
            break;
        }
    }
    if(!isin){
        waitForInfomation(id,lastMsg,timeStemp,addUnReadMsg);
    }
}

bool ChatListModel::isContain(QString id)
{
    for(int i=0;i<data_.size();++i){
        if(data_[i]->account_==id)
            return true;
    }
    return false;
}

bool ChatListModel::getTextIsNull(QString id)
{
    for(int i=0;i<data_.size();++i){
        if(data_[i]->account_==id){
            return data_[i]->lastMsg_=="";
        }
    }
    return true;
}

void ChatListModel::setMsgNum(QString account,bool doClear)
{
    for(int i=0;i<data_.size();++i){
        if(data_[i]->account_==account){
            if(doClear){
                totalMsgNum_-=data_[i]->unReadMsg_;
                data_[i]->unReadMsg_=0;
            }else{
                data_[i]->unReadMsg_++;
                totalMsgNum_++;
            }
            emit msgNumChanged(totalMsgNum_);
            emit dataChanged(index(i),index(i),{UNREAD});
            break;
        }
    }
}

void ChatListModel::writeAllInfo()
{
    SqliteManager::getInstance()->writeList(data_);
}

void ChatListModel::handleInfoUpdate(QString id)
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
