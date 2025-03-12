#ifndef THREADWORK_H
#define THREADWORK_H

#include <QObject>
#include <QThreadPool>
#include <QRunnable>
#include <functional>

using workTask = std::function<void()>;

class work : public QRunnable
{
public:
    work(workTask task=nullptr);

protected:
    void run() override;
private:
    workTask task_;
};

class ThreadWork : public QObject
{
    Q_OBJECT
public:
    ThreadWork(const ThreadWork&)=delete;
    ThreadWork operator =(const ThreadWork&)=delete;
    static ThreadWork *getInstance();

    void execTask(workTask task);

private:
    ThreadWork(QObject *parent = nullptr);
    ~ThreadWork();

    QThreadPool pool_;
signals:
};

#endif // THREADWORK_H
