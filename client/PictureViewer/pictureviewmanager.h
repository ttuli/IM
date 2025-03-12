#ifndef PICTUREVIEWMANAGER_H
#define PICTUREVIEWMANAGER_H

#include <QObject>
#include <QMap>
#include "pictureview.h"

class PictureViewManager : public QObject
{
    Q_OBJECT
public:
    PictureViewManager(const PictureViewManager&)=delete;
    PictureViewManager operator =(const PictureViewManager&)=delete;

    static PictureViewManager *getInstance();

    void createView(QString md5);

private:
    PictureViewManager(QObject *parent = nullptr);
    ~PictureViewManager();

    QMap<QString,PictureView*> viewManager_;
signals:
};

#endif // PICTUREVIEWMANAGER_H
