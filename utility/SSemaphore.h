/************************************************************************/
/* �����ź������������ڽ��̣������ڣ�/�̼߳�ͬ�� 
/************************************************************************/
#ifndef EYEGLE_COMMON_SSEMAPHORE_H_  
#define EYEGLE_COMMON_SSEMAPHORE_H_  

#include <string>

#if defined(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <tchar.h>
#else
#include <semaphore.h>
#include <fcntl.h>           /* Ϊ�� O_* ���� */
#include <sys/stat.h>        /* Ϊ��ģʽ���� */
#endif

#include "Global.h"

namespace utility {

	class COMMON_API SSemaphore
	{
	public:
		SSemaphore(long initial=0, const char *SemName=NULL);
#if defined(WIN32)
		bool Wait(int millisecond_=INFINITE);
		bool Release(int counter_=1);
#else
		bool Wait(int millisecond_=-1);
		bool Release();
#endif
		bool TryWait();
		~SSemaphore();
	private:

#if defined(WIN32)
		HANDLE sem_;
#else
		sem_t* sem_;
		std::string SEMName;
#endif

	};

}
#endif
