#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QPair>
#include <QVariant>
#include <QTimer>
#include <functional>
#include "ChatState/chatdialogmodel.h"
#include "ChatState/chatlistmodel.h"

#define MAX_WITHDRAW_TIME 120000

typedef std::function<void(const QVector<message>&)> ReadCallback;
typedef std::function<void(const QVector<chatListInfo>)> ListReadCallback;

class pullformat
{
public:
    pullformat(const QString &curAccount,const QString &tarAccount,int m_offset)
        :curAccount_(curAccount),tarAccount_(tarAccount),offset_(m_offset)
    {

    }

    QString curAccount_;
    QString tarAccount_;
    int offset_;
};

class SqliteManager : public QObject
{
    Q_OBJECT
public:
    static SqliteManager *getInstance();
    SqliteManager(const SqliteManager&)=delete;
    SqliteManager operator =(const SqliteManager&)=delete;

    void setCurrentAccount(QString currentAccount);

public slots:
    void endWork();
    void batchWrite(QSharedPointer<message> msg,QString tarAcc);
    void asyncRead(pullformat pullf,ReadCallback callback);

    void readList(ListReadCallback callback);
    void writeList(QList<QSharedPointer<chatListInfo>> data);

    void writeImage(QString md5,QByteArray imageData);
    QByteArray getSingleImage(QString md5);

    bool deleteMsg(QStringList ids);

private slots:
    void processWriteQueue();
    void processReadQueue();
    void processImageQueue();

private:
    explicit SqliteManager(QObject *parent = nullptr);
    ~SqliteManager();
    void init();
    void addImageCache();

    QSqlDatabase m_db;
    QString currentAccount_;
    QVector<QPair<QString,QSharedPointer<message>>> m_writeQueue;
    QVector<QPair<pullformat, ReadCallback>> m_readQueue;
    QVector<QPair<QString,QByteArray>> m_imageQueue;
    int m_batchSize = 10;

    QSqlDatabase globaldb_;

signals:
};

#endif // SQLITEMANAGER_H
