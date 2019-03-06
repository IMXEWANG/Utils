
#include "QThread.h"
#include "Applog.h"
#include <iostream>
using namespace std;

#ifdef _MSC_VER
#include <process.h>    
#endif

namespace utility 
{
    IThreadRunner::IThreadRunner():id_(0), state_(eReady)
    {
#ifdef _MSC_VER
        join_sem_ = new QCSemaphore(0,1);
#endif
    }

    IThreadRunner::~IThreadRunner()
    {
#ifdef _MSC_VER
        if(join_sem_) {
            delete join_sem_;
        }
#endif
    }

    void IThreadRunner::requestTerminate()
    {
        if(state_ == eRunning || state_ == eReady) {
            state_ = eRequestStop;
        }
    }

    void IThreadRunner::join()
    {
        if(state_!= eRunning && state_ != eRequestStop) {
            return ;
        }
#ifdef _MSC_VER
        while(!join_sem_->try_wait() && state_ != eStop) {
            Sleep(100);
        }
#elif defined __GNUG__
        pthread_join(tid_, 0);
#endif
    }

    void IThreadRunner::beforeRun()
    {
        if(state_ == eReady) {
            state_ = eRunning;
        }
    }

    void IThreadRunner::afterTerminate()
    {
        state_ = eStop;

#ifdef _MSC_VER
        join_sem_->release(1);
#elif defined __GNUG__
        // pthread_detach(pthread_self());
#endif
        QCThreadManager::removeThread(*this);
    }

    QCThreadManager* QCThreadManager::instance_ = 0;
    unsigned long QCThreadManager::count_ = 1;
    map<unsigned long,IThreadRunner*> QCThreadManager::thread_map_;
    Mutex* QCThreadManager::mutex_ = new Mutex();

    QCThreadManager* QCThreadManager::get_instance_()
    {
        if(!instance_) {
            MutexLocker locker(mutex_);
            if(!instance_) {
                instance_ = new QCThreadManager();
            }
        }
        return instance_;
    }

    unsigned int QCThreadManager::createThread(IThreadRunner & t_obj)
    {
        int ret = 0;
        MutexLocker locker(mutex_);
        count_++;
        t_obj.id_ = (unsigned long)&t_obj;
        thread_map_[t_obj.id_] = &t_obj;
#ifdef _MSC_VER
        ret =  _beginthread(QCThreadManager::thread_func, 1000, &t_obj);
        if(ret < 0) {
            APP_LOG(Applog::LOG_CRITICAL)<<"Create thread failed";
        }
#elif defined __GNUG__
        ret = pthread_create(&t_obj.tid_, 0, QCThreadManager::thread_func, &t_obj);
        if(ret) {
            APP_LOG(Applog::LOG_CRITICAL)<<"Create thread failed, error code:" << ret;
        }
#endif 
        return ret;
    }

    void QCThreadManager::removeThread(IThreadRunner & t_obj)
    {
        unsigned long thread_id = t_obj.get_id_();
        if(thread_id == 0) { return; }
        MutexLocker locker(mutex_);
        map<unsigned long, IThreadRunner*>::iterator it = thread_map_.find(thread_id);
        if(it != thread_map_.end()) {
            IThreadRunner* t = thread_map_[thread_id];
            thread_map_.erase(thread_id);
            if(t->state_ == IThreadRunner::eRunning) {
                t->requestTerminate();
                t->join();
            }

            count_--;
        } else {
            APP_LOG(Applog::LOG_WARNING)<< "not found thread id : " << (unsigned int)thread_id ;
        }
    }

#ifdef _MSC_VER
    void QCThreadManager::thread_func(void* arg)
#elif defined __GNUG__
    void* QCThreadManager::thread_func(void* arg)
#endif
    {
        IThreadRunner* runner = (IThreadRunner*) arg;
        runner->beforeRun();
        runner->run();
        runner->afterTerminate();
#ifdef _MSC_VER
        return ;
#elif defined __GNUG__
        return 0;
#endif
    }

    QCSemaphore::QCSemaphore(int init_count, int max_count)
    {
#ifdef _MSC_VER
        sem = CreateSemaphore(0, init_count, max_count, 0);
#elif defined __GNUG__
        sem_init(&sem, 0 ,init_count);
#endif
    }

    QCSemaphore::~QCSemaphore()
    {
#ifdef _MSC_VER
        CloseHandle(sem);
#elif defined __GNUG__
        sem_destroy(&sem);
#endif
    }

    /*
    void QCSemaphore::wait() {
    #ifdef _MSC_VER
    WaitForSingleObject(sem, INFINITE);
    #elif defined __GNUG__
    sem_wait(&sem);
    #endif
    }*/

    bool QCSemaphore::try_wait()
    {
#ifdef _MSC_VER
        return (WaitForSingleObject(sem, 1) != WAIT_TIMEOUT );
#elif defined __GNUG__
        return (sem_trywait(&sem) == 0);
#endif
    }

    void QCSemaphore::release(int count)
    {
#ifdef _MSC_VER    
        ReleaseSemaphore(sem, count, 0);
#elif defined __GNUG__
        sem_post(&sem);
#endif
    }
}
