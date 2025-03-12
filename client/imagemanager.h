#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>
#include <QQueue>
#include <QMap>
#include <QImage>
#include <queue>

#define CACHE_IMAGE_LIMIT_SIZE 300*1024*1024
#define CACHE_IMAGE_HD_LIMIT_SIZE 80*1024*1024

class ImageManager : public QObject
{
    Q_OBJECT
public:
    ImageManager(const ImageManager&)=delete;
    ImageManager operator =(const ImageManager&)=delete;

    static ImageManager* getInstance();

    void addImageCache(QString md5,QByteArray imagedata);
    QImage getImage(QString md5);

    // void addHdImageCache(QString md5,QByteArray imagedata);
    // QImage getHdImage(QString md5);
private:
    ImageManager(QObject *parent = nullptr);
    ~ImageManager()=default;

    QMap<QString,QImage> cache_;
    QQueue<QString> enterOrder_;
    qint64 cacheSize_=0;

    QMap<QString,QImage> hd_cache_;
    QQueue<QString> hd_enterOrder_;
    qint64 hd_cacheSize_=0;
signals:
};

#endif // IMAGEMANAGER_H
