#include "Thread.h"

namespace utility {

Thread::Thread() : m_state(STOPED)
{
	m_lock = new SpinLock();
}

Thread::~Thread()
{
	delete m_lock;
}

void Thread::Start() {
	if (GetState() == RUNING) { return; }

	m_state = RUNING;
#ifdef WIN32
	int ret =  _beginthread(ThreadProc, 1000, this);    
	if(ret < 0) {
		throw runtime_error("chreat thread error");
		SetState(STOPED);
		return;
	}
#else
	int ret = pthread_create(&m_tid, 0, ThreadProc, this);
	if(ret) {
		throw runtime_error("chreat thread error");
		SetState(STOPED);
		return;
	}
#endif 
}

void Thread::Join() {
#ifdef WIN32
	while (GetState() != STOPED) {
		q_sleep(3000);	//�߳��˳�3���Join�Ż��˳���ʱ������ô����Ϊ�����Ч��
	}
#else
	pthread_join(m_tid, 0);
#endif
}

void Thread::Stop() {
	SetState(REQUEST_STOP);
}

bool Thread::IsRuning() {
	return GetState() == RUNING;
}

void Thread::SetState(ThreadState state) {
	Locker locker(m_lock);
	m_state = state;
}

Thread::ThreadState Thread::GetState() {
	Locker locker(m_lock);
	return m_state;
}

}
