#ifndef EMOJIMODEL_H
#define EMOJIMODEL_H

#include <QAbstractListModel>

class EmojiModel : public QAbstractListModel
{
    Q_OBJECT

public:
    EmojiModel(const EmojiModel&)=delete;
    EmojiModel operator =(const EmojiModel&)=delete;

    static EmojiModel *getInstance();


    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addEmojis(QStringList names,const QModelIndex &parent=QModelIndex());

    QHash<int,QByteArray> roleNames() const;
private:
    EmojiModel(QObject *parent = nullptr);
    ~EmojiModel()=default;

    QStringList data_;
};

#endif // EMOJIMODEL_H
