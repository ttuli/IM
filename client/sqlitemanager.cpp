#include "sqlitemanager.h"
#include "imagemanager.h"
#include <QThread>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QFile>

SqliteManager::SqliteManager(QObject *parent)
    : QObject(parent)
{

}

void SqliteManager::setCurrentAccount(QString currentAccount)
{
    currentAccount_=currentAccount;
    init();
}

void SqliteManager::endWork()
{
    ChatListModel::getinstance()->writeAllInfo();
    processWriteQueue();
    processImageQueue();
    if (m_db.isOpen()) {
        m_db.close();
    }
    if(globaldb_.isOpen())
        globaldb_.close();
}

SqliteManager::~SqliteManager()
{
}

void SqliteManager::init()
{
    QDir dir("msg");
    if(!dir.exists()){
        dir.mkpath(".");
    }
    QFile file("msg/"+currentAccount_+".db");
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    file.setFileName("msg/imageDatabase.db");
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    static int connectionCount = 0;
    QString connectionName = QString("SQLITE_CONN_%1").arg(connectionCount++);

    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_db.setDatabaseName("msg/"+currentAccount_+".db");

    connectionName = QString("SQLITE_CONN_%1").arg(connectionCount++);
    globaldb_=QSqlDatabase::addDatabase("QSQLITE",connectionName);
    globaldb_.setDatabaseName("msg/imageDatabase.db");

    if (!m_db.open()||!globaldb_.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
    }

    QSqlQuery query(m_db);
    bool initResult=true;
    m_db.transaction();
    if(!query.exec("CREATE TABLE if not exists private_chat (\n"
                    "                        id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                    "                        message_id TEXT NOT NULL,\n"
                    "                        sender_id BIGINT UNSIGNED NOT NULL,\n"
                    "                        receiver_id BIGINT UNSIGNED NOT NULL,\n"
                    "                        message TEXT,\n"
                    "                        message_type TINYINT NOT NULL DEFAULT 0,\n"
                    "                        deleted_by_user TINYINT NOT NULL DEFAULT 0,\n"
                    "                        created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP\n"
                    "                        );")){
        initResult=false;
    }
    if(!query.exec("CREATE INDEX IF NOT EXISTS idx_receiver_id ON private_chat(receiver_id);"))
        initResult=false;
    if(!query.exec("CREATE INDEX IF NOT EXISTS idx_sender_id ON private_chat(sender_id);"))
        initResult=false;
    if(!query.exec("CREATE TABLE IF NOT EXISTS list_info (\n"
                    "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                    "  account BEGIN NOT NULL,\n"
                    "  name varchar(30),\n"
                    "  lastMsg TEXT,\n"
                    "  createtime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\n"
                    "  unreadmsg INTEGER NOT NULL DEFAULT 0);")){
        qCritical() << "Failed to create table list_info:" << query.lastError().text();
        initResult=false;
    }
    if(!initResult){
        m_db.rollback();
        qCritical() << "Failed to create table:" << query.lastError().text();
    } else {
        m_db.commit();
    }

    QSqlQuery sql(globaldb_);
    initResult=true;
    globaldb_.transaction();
    if(!sql.exec("CREATE TABLE IF NOT EXISTS images(\n"
                  "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                  "  md5_hash CHAR (32) NOT NULL UNIQUE,\n"
                  "image_data BLOB NOT NULL);")){
        initResult=false;
    }
    if(!sql.exec("CREATE INDEX IF NOT EXISTS idx_md5 ON images(md5_hash);"))
        initResult=false;
    if(initResult){
        globaldb_.commit();
    } else {
        globaldb_.rollback();
        qCritical() << "Failed to create table:" << sql.lastError().text();
    }

    addImageCache();
}

void SqliteManager::addImageCache()
{
    QSqlQuery sql(globaldb_);
    if(!sql.exec("select * from images;")){
        qDebug()<<__FUNCTION__<<"error";
        return;
    };
    while(sql.next()){
        ImageManager::getInstance()->addImageCache(sql.value("md5_hash").toString(),
                                                   sql.value("image_data").toByteArray());
    }
}

SqliteManager *SqliteManager::getInstance()
{
    static SqliteManager obj;
    return &obj;
}

void SqliteManager::batchWrite(QSharedPointer<message> msg,QString tarAcc)
{
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(this, "batchWrite", Qt::QueuedConnection,
                                  Q_ARG(QSharedPointer<message>, msg),Q_ARG(QString,tarAcc));
        return;
    }

    m_writeQueue.append(QPair<QString,QSharedPointer<message>>(tarAcc,msg));
    if(m_writeQueue.size()>=m_batchSize){
        processWriteQueue();
    }
}

void SqliteManager::asyncRead(pullformat pullf,ReadCallback callback)
{
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(this, "asyncRead", Qt::QueuedConnection,
                    Q_ARG(pullformat, pullf),Q_ARG(ReadCallback, callback));
        return;
    }

    m_readQueue.append(qMakePair(pullf, callback));
    processReadQueue();
}

void SqliteManager::readList(ListReadCallback callback)
{
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(this, "readList", Qt::QueuedConnection,
                                  Q_ARG(ListReadCallback, callback));
        return;
    }
    QSqlQuery sql(m_db);
    if(sql.exec("select * from list_info;")){
        QVector<chatListInfo> info;
        while(sql.next()){
            info<<chatListInfo(sql.value("account").toString(),sql.value("name").toString(),sql.value("lastMsg").toString(),
                                 sql.value("createtime").toString(),sql.value("unreadmsg").toInt());
        }
        if(callback)
            callback(info);
    }

}

void SqliteManager::writeList(QList<QSharedPointer<chatListInfo>> data)
{
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(this, "writeList", Qt::QueuedConnection,
                                  Q_ARG(QList<QSharedPointer<chatListInfo>>, data));
        return;
    }
    QSqlQuery sql(m_db);
    sql.exec("delete from list_info;");
    for(auto i:data){
        i->lastMsg_=i->lastMsg_.replace("'","''");
        QString command=QString("insert into list_info(account,name,lastMsg,"
                                  "createtime,unreadmsg) values(%1,'%2','%3','%4',%5);").arg(i->account_)
                              .arg(i->name_).arg(i->lastMsg_).arg(i->timeStemp_).arg(i->unReadMsg_);
        if(!sql.exec(command)){
            qCritical()<<"list_info insert error";
        }
    }
}

void SqliteManager::writeImage(QString md5, QByteArray imageData)
{
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(this, "writeImage", Qt::QueuedConnection,
                                  Q_ARG(QString, md5),Q_ARG(QByteArray,std::move(imageData)));
        return;
    }

    m_imageQueue<<qMakePair(md5,imageData);
    if(m_imageQueue.size()>=m_batchSize){
        processImageQueue();
    }
}

QByteArray SqliteManager::getSingleImage(QString md5)
{
    QSqlQuery sql(globaldb_);
    if(!sql.exec("select image_data from images where md5_hash="+md5+";")){
        return QByteArray();
    }
    return sql.value("image_data").toByteArray();
}

bool SqliteManager::deleteMsg(QStringList ids)
{
    QSqlQuery query(m_db);
    QSet<QString> set;
    QVector<QPair<QString,QSharedPointer<message>>> result;
    m_db.transaction();
    for(auto i:ids){
        if(!query.exec(QString("delete from private_chat where message_id='%1';").arg(i))){
            m_db.rollback();
            qDebug()<<"error "<<query.lastError().text();
            return false;
        }
        set.insert(i);
    }
    m_db.commit();
    for(int i=0;i<m_writeQueue.size();++i){
        if(!set.contains(m_writeQueue[i].second->message_id_)){
            result.append(m_writeQueue[i]);
        }
    }
    m_writeQueue=result;
    return true;
}


void SqliteManager::processWriteQueue()
{
    if (m_writeQueue.isEmpty()) return;

    if (!m_db.transaction()) {
        qDebug()<<__FUNCTION__<<"transaction"<<m_db.lastError().text();
        return;
    }

    bool result=false;
    QSqlQuery q(m_db);
    for (const auto &msg : m_writeQueue) {
        msg.second->content_.replace("'","''");
        QString sql=QString("insert into "
                "private_chat(sender_id,receiver_id,message,"
                "message_type,deleted_by_user,created_at,message_id) "
                              "values(%1,%2,'%3',%4,%5,'%6','%7');").arg(msg.second->account_)
                          .arg(msg.first).arg(msg.second->content_).arg((int)(msg.second->type_))
                          .arg((int)(msg.second->type_==message::DELETED))
                          .arg(msg.second->createTime_)
                          .arg(msg.second->message_id_);

        result=q.exec(sql);
        if (!result){
            qDebug()<<q.lastError().text()<<sql;
            break;
        }
    }

    if (!result) {
        m_db.rollback();
    } else {
        if (!m_db.commit()) {
            qDebug()<<__FUNCTION__<<m_db.lastError().text();
        }
    }

    m_writeQueue.clear();
}

void SqliteManager::processReadQueue()
{
    while (!m_readQueue.isEmpty()) {
        auto request = m_readQueue.takeFirst();
        const pullformat &info = request.first;
        ReadCallback callback = request.second;

        QVector<message> results;
        QSqlQuery q(m_db);

        QString query=QString("select * from private_chat where receiver_id in (%1,%2) "
                        "order by id asc limit 50 offset %3;").arg(info.curAccount_).arg(info.tarAccount_).arg(info.offset_);
        if (q.exec(query)) {
            while (q.next()) {
                if(q.value("deleted_by_user").toInt())
                    continue;
                message::msgType type=(message::msgType)(message::msgType::FRIEND+q.value("message_type").toInt());
                message msg=message(q.value("message").toString(),q.value("sender_id").toString(),
                                       "",type,q.value("created_at").toString(),
                                       (message::msgWay)(info.curAccount_!=q.value("sender_id").toString()),
                                      q.value("message_id").toString());
                msg.st_=message::FINISH;
                results.append(msg);
            }
        } else {
            qDebug()<<__FUNCTION__<<q.lastError().text();
        }

        QMetaObject::invokeMethod(this, [callback, results]() {
                if(callback!=nullptr)
                    callback(results);
            }, Qt::QueuedConnection);
    }
}

void SqliteManager::processImageQueue()
{
    while(!m_imageQueue.empty()){
        QPair<QString,QByteArray> pair=std::move(m_imageQueue.back());
        m_imageQueue.pop_back();
        QSqlQuery sql(globaldb_);
        QString s=QString("insert or IGNORE into images(md5_hash,image_data) values('%1','%2');")
                        .arg(pair.first)
                        .arg(pair.second);
        if(!sql.exec(s)){
            qDebug()<<sql.lastError().text();
        }
    }
}
