#ifndef STOREEMOJI_H
#define STOREEMOJI_H

#include <QObject>
#include <QMap>
#include <QImage>

class StoreEmoji : public QObject
{
    Q_OBJECT
public:
    StoreEmoji(const StoreEmoji&)=delete;
    StoreEmoji operator =(const StoreEmoji&)=delete;

    static StoreEmoji *getInstance();

private:
    StoreEmoji(QObject *parent = nullptr);
    ~StoreEmoji()=default;

    QMap<QString,QImage> storages_;
    QStringList EmojiList_;
signals:
};

#endif // STOREEMOJI_H
