#ifndef PICTUREVIEW_H
#define PICTUREVIEW_H

#include <QWidget>
#include <QQuickItem>
#include <QVBoxLayout>
#include "customizequickwidget.h"
#include "windows_background.h"
#include "generalimageprovider.h"

class PictureView : public WindowsBackground
{
    Q_OBJECT
public:
    explicit PictureView(QWidget *parent = nullptr,QString md5="");
    ~PictureView();

protected:
    void changeEvent(QEvent *event);

private:
    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;

    CustomizeQuickWidget *titleBar_;
    QQuickItem *titleBarItem_;

    QVBoxLayout *qvb_;

    QString currentImageMd5_;
    GeneralImageProvider *provider_;
    int screenWidth_;
    int screenHeight_;
private slots:
    void onClose();
    void onMax();
    void adjustWindow(int newWidth,int newHeight);
    void centerWindow();
    void onSave();
signals:
    void doDestory(QString md5);
};

#endif // PICTUREVIEW_H
