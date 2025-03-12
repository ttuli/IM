#include "pictureviewmanager.h"

PictureViewManager::PictureViewManager(QObject *parent)
    : QObject{parent}
{
}

PictureViewManager::~PictureViewManager()
{
    for(auto i:viewManager_){
        i->deleteLater();
    }
}

PictureViewManager *PictureViewManager::getInstance()
{
    static PictureViewManager obj;
    return &obj;
}

void PictureViewManager::createView(QString md5)
{
    if(viewManager_.contains(md5)){
        if(viewManager_[md5]->isMinimized())
            viewManager_[md5]->showNormal();
        viewManager_[md5]->show();
        viewManager_[md5]->activateWindow();
        return;
    }
    PictureView *view=new PictureView(nullptr,md5);
    connect(view,&PictureView::doDestory,[this](QString md5){
        if(viewManager_.contains(md5)){
            viewManager_[md5]->close();
            viewManager_[md5]->deleteLater();
            viewManager_.remove(md5);
        }
    });
    viewManager_[md5]=view;
    view->show();
}
