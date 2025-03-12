#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>
#include <QMutex>
#include "networkmanager.h"

class ImageRespond : public QQuickImageResponse
{
    Q_OBJECT
public:
    ImageRespond(const QString& id, const QSize& requestedSize);
    ~ImageRespond();

    QQuickTextureFactory* textureFactory() const override;

private:
    QString id_;
    QImage image_;
};

class ImageProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    ImageProvider();
    ~ImageProvider();

    QQuickImageResponse* requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
};

#endif // IMAGEPROVIDER_H
