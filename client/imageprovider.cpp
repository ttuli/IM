#include "imageprovider.h"

ImageProvider::ImageProvider()
{
}

ImageProvider::~ImageProvider()
{
}


QQuickImageResponse* ImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    ImageRespond *respond=new ImageRespond(id,requestedSize);
    return respond;
}



ImageRespond::ImageRespond(const QString &id, const QSize &requestedSize)
    :id_(id)
{
    User u=NetWorkManager::getInstance()->getInfo(id);
    if(!u.image_.isNull()){
        image_=u.image_;
        emit finished();
        return;
    }

    QMetaObject::invokeMethod(NetWorkManager::getInstance(),[this]{
        PullRequest req;
        req.mutable_user_info()->set_user_id(id_.toStdString());
        NetWorkManager::getInstance()->addTask(Task(std::move(NetWorkManager::getInstance()->mergeData(req)),[this](QByteArray &receive){
            PullRespond res;
            if(!res.ParseFromString(receive.toStdString())){
            }
            if(res.has_user_info()){
                NetWorkManager::getInstance()->setInfo(id_,{QString::fromStdString(res.user_info().account()),
                                                             QString::fromStdString(res.user_info().name()),
                                                             QString::fromStdString(res.user_info().email()),
                                                             QByteArray::fromStdString(res.user_info().image()),
                                                             QString::fromStdString(res.user_info().createtime())});
                QImage ima=NetWorkManager::getInstance()->getInfo(id_).image_;

                QMetaObject::invokeMethod(this,[this,ima]{
                    image_=ima;
                    emit finished();
                },Qt::QueuedConnection);
            }
        }));
    },Qt::QueuedConnection);
}

ImageRespond::~ImageRespond()
{

}

QQuickTextureFactory *ImageRespond::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(image_);
}
