#include "storeemoji.h"
#include "emojimodel.h"
#include <QDir>
#include <QImage>

StoreEmoji::StoreEmoji(QObject *parent)
    : QObject{parent}
{
    QDir dir("emoji");

    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo &fileInfo : fileList) {
        QImage ima("emoji/"+fileInfo.fileName());
        EmojiList_.append(fileInfo.fileName());
        storages_.insert(fileInfo.fileName(),ima);
    }

    EmojiModel::getInstance()->addEmojis(EmojiList_);
}

StoreEmoji *StoreEmoji::getInstance()
{
    static StoreEmoji obj;
    return &obj;
}
