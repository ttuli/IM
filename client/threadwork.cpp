#include "threadwork.h"

ThreadWork::ThreadWork(QObject *parent)
    : QObject{parent}
{
    pool_.setMaxThreadCount(2);
}

ThreadWork::~ThreadWork()
{
    pool_.waitForDone();
}

ThreadWork *ThreadWork::getInstance()
{
    static ThreadWork obj;
    return &obj;
}

void ThreadWork::execTask(workTask task)
{
    work *w=new work(task);
    pool_.start(w);
}

work::work(workTask task)
    :task_(task)
{
    setAutoDelete(true);
}

void work::run()
{
    if(task_)
        task_();
}
