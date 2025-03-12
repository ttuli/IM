#include "imagemanager.h"
#include "sqlitemanager.h"
#include <QImage>
#include <QDateTime>
#include <QTimer>

ImageManager::ImageManager(QObject *parent)
    : QObject{parent}
{

}

ImageManager *ImageManager::getInstance()
{
    static ImageManager obj;
    return &obj;
}

void ImageManager::addImageCache(QString md5, QByteArray imagedata)
{
    QImage image;
    if(!image.loadFromData(QByteArray::fromBase64(imagedata))){
        qDebug()<<__FUNCTION__<<"load fail";
        return;
    }
    SqliteManager::getInstance()->writeImage(md5,imagedata);
    if(!cache_.contains(md5)){
        cache_[md5]=image;
        enterOrder_.enqueue(md5);
        cacheSize_+=image.sizeInBytes();

        while(cacheSize_>=CACHE_IMAGE_LIMIT_SIZE&&!enterOrder_.empty()){
            QString imaMd5=enterOrder_.dequeue();
            cacheSize_-=cache_[imaMd5].sizeInBytes();
            cache_.remove(imaMd5);
        }
    }
}

QImage ImageManager::getImage(QString md5)
{
    if(cache_.contains(md5)){

        return cache_[md5];
    }
    return QImage();
}

// void ImageManager::addHdImageCache(QString md5, QByteArray imagedata)
// {
//     QImage image;
//     if(!image.loadFromData(QByteArray::fromBase64(imagedata))){
//         qDebug()<<__FUNCTION__<<"load fail";
//         return;
//     }
//     if(!hd_cache_.contains(md5)){
//         hd_cache_[md5]=image;
//         hd_enterOrder_.enqueue(md5);
//         hd_cacheSize_+=image.sizeInBytes();

//         while(hd_cacheSize_>=CACHE_IMAGE_HD_LIMIT_SIZE&&!hd_enterOrder_.empty()){
//             QString imaMd5=hd_enterOrder_.dequeue();
//             hd_cacheSize_-=hd_cache_[imaMd5].sizeInBytes();
//             hd_cache_.remove(imaMd5);
//         }
//     }
// }

// QImage ImageManager::getHdImage(QString md5)
// {
//     if(hd_cache_.contains(md5)){

//         return hd_cache_[md5];
//     }
//     return QImage();
// }
