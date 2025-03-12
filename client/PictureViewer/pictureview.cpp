#include "pictureview.h"
#include "networkmanager.h"
#include "threadwork.h"
#include "imagemanager.h"
#include <QScreen>
#include <QVBoxLayout>
#include <QQmlEngine>
#include <QImage>
#include <QFileDialog>
#include <QDateTime>

PictureView::PictureView(QWidget *parent,QString md5)
    : WindowsBackground{parent},currentImageMd5_(md5)
{
    provider_=new GeneralImageProvider;

    interface_=new CustomizeQuickWidget(this);
    interface_->engine()->addImageProvider("hdimageprovider",provider_);
    interface_->setSource(QUrl("qrc:/PictureViewer/PictureView.qml"));
    interfaceItem_=interface_->rootObject();
    connect(interfaceItem_,SIGNAL(centerSig()),this,SLOT(centerWindow()));
    connect(interfaceItem_,SIGNAL(resizeSig(int,int)),this,SLOT(adjustWindow(int,int)));
    connect(interfaceItem_,SIGNAL(saveSig()),this,SLOT(onSave()));

    titleBar_=new CustomizeQuickWidget(this);
    titleBar_->setSource(QUrl("qrc:/PictureViewer/ViewTitle.qml"));
    titleBarItem_=titleBar_->rootObject();
    titleBar_->setFixedHeight(35);
    connect(titleBarItem_,SIGNAL(closeSig()),this,SLOT(onClose()));
    connect(titleBarItem_,SIGNAL(minSig()),this,SLOT(showMinimized()));
    connect(titleBarItem_,SIGNAL(maxSig()),this,SLOT(onMax()));


    qvb_=new QVBoxLayout(this);
    qvb_->setContentsMargins(0,0,0,0);
    qvb_->addWidget(titleBar_);
    qvb_->addWidget(interface_);
    qvb_->setSpacing(0);

    QMetaObject::invokeMethod(interfaceItem_,"setImageSource",Q_ARG(QVariant,md5));

    resize(800,600);
    setTitleWidget(titleBar_,titleBarItem_);
}

PictureView::~PictureView()
{
    if(provider_)
        provider_->deleteLater();
}

void PictureView::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::WindowStateChange){
        QWindowStateChangeEvent *stateEvent=dynamic_cast<QWindowStateChangeEvent*>(event);
        qvb_->setContentsMargins(0,0,0,0);
        if(isMaximized())
            qvb_->setContentsMargins(5,5,5,5);
        if(stateEvent->oldState()==Qt::WindowMaximized){
            QMetaObject::invokeMethod(titleBarItem_,"setIsMax",Q_ARG(QVariant,isMinimized()));
        }
        else if(stateEvent->oldState()==Qt::WindowNoState){
            QMetaObject::invokeMethod(titleBarItem_,"setIsMax",Q_ARG(QVariant,isMaximized()));
        }
    }
}

void PictureView::onClose()
{
    emit doDestory(currentImageMd5_);
}

void PictureView::onMax()
{
    isMaximized()?showNormal():showMaximized();
}

void PictureView::adjustWindow(int newWidth, int newHeight)
{
    resize(newWidth,newHeight);
}

void PictureView::centerWindow()
{
    screenWidth_=QGuiApplication::primaryScreen()->size().width();
    screenHeight_=QGuiApplication::primaryScreen()->size().height();
    move(screenWidth_/2-width()/2,screenHeight_/2-height()/2);
}

void PictureView::onSave()
{
    QString timestemp=QDateTime::currentDateTime().toString("yyMMddHHms");
    QString position=QFileDialog::getSaveFileName(this,"选择存储位置",QDir::currentPath() + "/"+timestemp+".png");
    ThreadWork::getInstance()->execTask([this,position]{
        QImage ima=ImageManager::getInstance()->getImage(currentImageMd5_);
        ima.save(position,"PNG");
    });
}

