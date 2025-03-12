#ifndef GENERALIMAGEPROVIDER_H
#define GENERALIMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>

class GeneralImageRespond : public QQuickImageResponse
{
    Q_OBJECT
public:
    GeneralImageRespond(const QString& id, const QSize& requestedSize);

    QQuickTextureFactory* textureFactory() const override;

private:
    QString id_;
    QImage image_;
};

class GeneralImageProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    GeneralImageProvider();

    QQuickImageResponse* requestImageResponse(const QString &id, const QSize &requestedSize) override;
};

#endif // GENERALIMAGEPROVIDER_H
