#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>
#include <QAbstractListModel>
#include <QMap>
#include "customizequickwidget.h"
#include "emojiimageprovider.h"
#include "chatdialogmodel.h"
#include "generalimageprovider.h"
#include "PictureViewer/pictureviewmanager.h"

#define MAX_PIC_WIDTH 380

class ChatDialog : public QWidget
{
    Q_OBJECT
public:
    explicit ChatDialog(QWidget *parent = nullptr,QString tarAccount="",QString tarName="",QString curAccount="");
    ~ChatDialog();
    friend class NetWorkManager;

    QString getCurId(){
        return curAccount_;
    }

private slots:
    void sendMsg(QString msg,int type);
    void sendPic();

private:
    QString tarAccount_;
    QString tarName_;
    QString curAccount_;
    QString curName_;
    int offset_;

    CustomizeQuickWidget *interface_;
    QQuickItem *interfaceItem_;

    EmojiImageProvider *emojiProvider_=nullptr;
    GeneralImageProvider *generalProvider_=nullptr;

    QSharedPointer<ChatDialogModel> model_;
    QString currentFile_;

    QMap<int,QString> chosedList_;
private slots:
    void pullHistoryMsg();
    void pullOfflineMsg();
    void handleImageData(QByteArray byte1);
    void showHdImage(QString md5);
    QByteArray getImageData(QImage image,short quality=-1);

    void updateChosedState(QString id,int position,bool value);
    void deleteMsg();

signals:
    void updateOrderSig(QString id);
};

#endif // CHATDIALOG_H
