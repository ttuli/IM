#include "emojimodel.h"

EmojiModel::EmojiModel(QObject *parent)
    : QAbstractListModel(parent)
{}

EmojiModel *EmojiModel::getInstance()
{
    static EmojiModel obj;
    return &obj;
}

int EmojiModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return data_.size();
    // FIXME: Implement me!
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==Qt::DisplayRole)
        return data_[index.row()];

    // FIXME: Implement me!
    return QVariant();
}

void EmojiModel::addEmojis(QStringList names, const QModelIndex &parent)
{
    beginInsertRows(parent,rowCount(),rowCount()+names.size()-1);
    for(QString &i:names){
        data_.append(i);
    }
    endInsertRows();
}

QHash<int, QByteArray> EmojiModel::roleNames() const
{
    QHash<int,QByteArray> hash;
    hash.insert(Qt::DisplayRole,"name");

    return hash;
}
