#include "chatdialogmodel.h"
#include "sqlitemanager.h"

ChatDialogModel::ChatDialogModel(QObject *parent,QString tarAccount)
    : QAbstractListModel(parent),tarAccount_(tarAccount)
{
}

int ChatDialogModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

QVariant ChatDialogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==NAME)
        return data_[index.row()]->name_;
    if(role==TYPE)
        return data_[index.row()]->type_;
    if(role==CONTENT)
        return data_[index.row()]->content_;
    if(role==ACCOUNT)
        return data_[index.row()]->account_;
    if(role==CREATE_TIME)
        return data_[index.row()]->createTime_;
    if(role==WAY)
        return data_[index.row()]->way_;
    if(role==STATE)
        return data_[index.row()]->st_;
    if(role==MSGID)
        return data_[index.row()]->message_id_;

    return QVariant();
}

int ChatDialogModel::addRow(int position, QSharedPointer<message> info, const QModelIndex &parent)
{
    //position=-1:插末尾
    if(position<-1||position>rowCount())
        return -1;
    if(position==-1){
        position=rowCount();
    }
    if(judgeTime(info->createTime_)){
        beginInsertRows(parent,position,position+1);
        data_.insert(position,info);
        data_.insert(position,QSharedPointer<message>::create("","","",message::TIME,info->createTime_,message::MINE,""));
        endInsertRows();
        position++;
    } else {
        beginInsertRows(parent,position,position);
        data_.insert(position,info);
        endInsertRows();
    }

    newestTime_=info->createTime_;
    return position;
}

void ChatDialogModel::removeRow(int position, bool removeAll, const QModelIndex &parent)
{
    if(removeAll){
        beginRemoveRows(parent,0,rowCount()-1);
        data_.clear();
        endRemoveRows();
        return;
    } else {
        if(position<0||position>=rowCount())
            return;
        beginRemoveRows(parent,position,position);
        data_.removeAt(position);
        endRemoveRows();
    }
}

void ChatDialogModel::updateRow(int index, message::Status st)
{
    data_[index]->st_=st;
    if(st==message::FINISH)
        SqliteManager::getInstance()->batchWrite(data_[index],tarAccount_);
    emit dataChanged(this->index(index),this->index(index),{STATE});
}

QSharedPointer<message> ChatDialogModel::getInfo(int index)
{
    if(index<0||index>=rowCount())
        return nullptr;
    return data_[index];
}

QString ChatDialogModel::getNewTime()
{
    return newestTime_;
}

bool ChatDialogModel::judgeTime(QString timeStemp)
{
    if(newestTime_=="")
        return true;
    QDateTime dateTime=QDateTime::fromString(newestTime_,"yyyy-MM-dd HH:mm:ss");
    QDateTime dateTime1=QDateTime::fromString(timeStemp,"yyyy-MM-dd HH:mm:ss");
    qint64 secondsDiff = qAbs(dateTime1.secsTo(dateTime));

    return secondsDiff>=300;
}

void ChatDialogModel::resetAll()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> ChatDialogModel::roleNames() const
{
    QHash<int,QByteArray> hash;

    hash.insert(NAME,"name");
    hash.insert(ACCOUNT,"account");
    hash.insert(TYPE,"type");
    hash.insert(CONTENT,"content");
    hash.insert(CREATE_TIME,"createtime");
    hash.insert(WAY,"way");
    hash.insert(STATE,"msgstate");
    hash.insert(MSGID,"msgid");

    return hash;
}
