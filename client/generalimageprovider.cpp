#include "generalimageprovider.h"
#include "imagemanager.h"
#include "sqlitemanager.h"
#include "networkmanager.h"

GeneralImageProvider::GeneralImageProvider() {}

QQuickImageResponse *GeneralImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    GeneralImageRespond *respond=new GeneralImageRespond(id,requestedSize);
    return respond;
}

GeneralImageRespond::GeneralImageRespond(const QString &id, const QSize &requestedSize)
{
    image_=ImageManager::getInstance()->getImage(id);
    if(image_.isNull()){
        QMetaObject::invokeMethod(SqliteManager::getInstance(),[this,id]{
                QImage image;
                image.loadFromData(SqliteManager::getInstance()->getSingleImage(id));
                QMetaObject::invokeMethod(this,[this,image]{
                    image_=image;
                    if(image_.isNull()){
                        image_=QImage(":/new/prefix1/res/mountains-9372609_1280.jpg");
                    }
                    emit finished();
                });
            },Qt::QueuedConnection);
    }else{
        emit finished();
    }
}

QQuickTextureFactory *GeneralImageRespond::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(image_);
}
