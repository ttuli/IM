#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <memory>
#include <QModelIndex>
#include <QSharedPointer>

class info{
public:
    QString name_;
    QString account_;
    bool applystate_;

    info(QString name,QString account)
        :name_(name),account_(account),applystate_(false)
    {}

    info(){
        name_="";
        account_="";
    }
};

class SearchModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SearchModel(const SearchModel&)=delete;
    SearchModel operator =(const SearchModel&)=delete;

    static SearchModel *getInstance();

    enum DataType{
        ACCOUNT,
        NAME,
        APPLYSTATE
    };


    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool addInfo(QString name,QString account,const QModelIndex &parent=QModelIndex());
    void removeInfo(QString account,bool clearAll=false,const QModelIndex &parent=QModelIndex());
    QSharedPointer<info> getInfo(int index);
    void updateIndexApplyState(int index,bool value);

    QHash<int,QByteArray> roleNames() const;
private:
    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel()=default;

    QList<QSharedPointer<info>> data_;
};

#endif // SEARCHMODEL_H
