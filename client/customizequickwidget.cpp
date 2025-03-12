#include "customizequickwidget.h"
#include <QQmlEngine>

CustomizeQuickWidget::CustomizeQuickWidget(QWidget *parent)
    :QQuickWidget(parent)
{
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setClearColor(Qt::transparent);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    imageProvider_=new ImageProvider;
    engine()->addImageProvider("imageprovider",imageProvider_);
}

CustomizeQuickWidget::~CustomizeQuickWidget()
{
    if(imageProvider_)
        imageProvider_->deleteLater();
}

