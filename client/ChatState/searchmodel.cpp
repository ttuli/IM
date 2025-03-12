#include "searchmodel.h"

SearchModel::SearchModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

SearchModel *SearchModel::getInstance()
{
    static SearchModel obj;
    return &obj;
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==DataType::ACCOUNT)
        return data_[index.row()]->account_;
    if(role==DataType::NAME)
        return data_[index.row()]->name_;
    if(role==DataType::APPLYSTATE)
        return data_[index.row()]->applystate_;

    return QVariant();
}

bool SearchModel::addInfo(QString name,QString account,const QModelIndex &parent)
{
    beginInsertRows(parent, rowCount(), rowCount());
    data_<<QSharedPointer<info>::create(name,account);
    endInsertRows();
}

void SearchModel::removeInfo(QString account,bool clearAll,const QModelIndex &parent)
{
    if(clearAll){
        beginRemoveColumns(parent, 0, rowCount()-1);
        data_.clear();
        endRemoveRows();
    } else {
        for(int i=0;i<data_.size();++i){
            if(data_[i]->account_==account){
                beginRemoveColumns(parent,i,i);
                data_.removeAt(i);
                endRemoveRows();
            }
        }
    }
}

QSharedPointer<info> SearchModel::getInfo(int index)
{
    if(index<0||index>data_.size()-1)
        return nullptr;
    return data_[index];
}

void SearchModel::updateIndexApplyState(int index, bool value)
{
    if(index<0||index>data_.size()-1)
        return;
    data_[index]->applystate_=value;
    emit dataChanged(this->index(index),this->index(index),{DataType::APPLYSTATE});
}

QHash<int, QByteArray> SearchModel::roleNames() const
{
    QHash<int,QByteArray> hash;
    hash.insert(DataType::ACCOUNT,"account");
    hash.insert(DataType::NAME,"name");
    hash.insert(DataType::APPLYSTATE,"applystate");

    return hash;
}
