#ifndef EMOJIIMAGEPROVIDER_H
#define EMOJIIMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>

class EmojiRespond : public QQuickImageResponse
{
    Q_OBJECT
public:
    EmojiRespond(const QString& id, const QSize& requestedSize);
    ~EmojiRespond()=default;

    QQuickTextureFactory* textureFactory() const override;

private:
    QString id_;
    QImage image_;
};

class EmojiImageProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    EmojiImageProvider();

    QQuickImageResponse* requestImageResponse(const QString &id, const QSize &requestedSize) override;
};

#endif // EMOJIIMAGEPROVIDER_H
