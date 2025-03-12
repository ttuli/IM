#include "emojiimageprovider.h"

EmojiImageProvider::EmojiImageProvider() {}

QQuickImageResponse *EmojiImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    EmojiRespond *respond=new EmojiRespond(id,requestedSize);
    return respond;
}

EmojiRespond::EmojiRespond(const QString &id, const QSize &requestedSize)
{
    image_=QImage("emoji/"+id);
    emit finished();
}

QQuickTextureFactory *EmojiRespond::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(image_);
}
