#ifndef Q_COMMON_QTHREAD_H
#define Q_COMMON_QTHREAD_H

#include "Mutex.h"
#include <map>

#ifdef __GNUG__
#include <pthread.h>
#include <semaphore.h> 
#endif

using namespace std;

namespace utility 
{

    class QCSemaphore;

    class COMMON_API IThreadRunner
    {
    public:
        enum state{
            eReady,
            eRunning,
            eRequestStop,
            eStop,
        };
        IThreadRunner();
        virtual ~IThreadRunner();

        void join();
        void requestTerminate();
        int getState(){return state_;}
        bool isAlive(){return (state_ != eStop);}
        inline const unsigned long get_id_() const { return id_; }
        friend class QCThreadManager;

    protected:
        virtual void beforeRun();
        virtual void afterTerminate();
        virtual void run() = 0;

        unsigned long id_;
        volatile state state_;

    private:
#ifdef __GNUG__
        pthread_t tid_;
#elif defined _MSC_VER
        QCSemaphore* join_sem_;
#endif
    };

    class COMMON_API QCThreadManager {
    public:
        static QCThreadManager* get_instance_();
        static unsigned int createThread(IThreadRunner & t_obj);
        static void removeThread(IThreadRunner & t_obj);

    private:
        static QCThreadManager* instance_;
        static unsigned long count_;
        static map<unsigned long,IThreadRunner*> thread_map_;
        static Mutex* mutex_;
#ifdef _MSC_VER
        static void thread_func(void* arg);
#elif defined __GNUG__
        static void* thread_func(void* arg);
#endif
    };

    class COMMON_API ISemaphore {
    public:
        virtual ~ISemaphore() {};
        //virtual void wait() = 0;
        virtual bool try_wait() = 0;
        virtual void release(int count) = 0;
    };

    class COMMON_API QCSemaphore : public ISemaphore {
    public:
        QCSemaphore(int init_count, int max_count);
        virtual ~QCSemaphore();
        //virtual void wait();
        virtual bool try_wait();
        virtual void release(int count);
    private:
#ifdef _MSC_VER
        HANDLE sem;
#elif defined __GNUG__
        sem_t sem;
#endif
    };

}
#endif
