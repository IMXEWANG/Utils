
#ifndef EYEGLE_INCLUDE_COMMON_H_ 
#define EYEGLE_INCLUDE_COMMON_H_

#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define q_sleep(milliseconds) Sleep(milliseconds)
#define get_th_id() GetCurrentThreadId()

#elif defined __GNUG__
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>
#include <unistd.h>
#define q_sleep(milliseconds) usleep(milliseconds*1000)
#define get_th_id() syscall(SYS_gettid)

#elif defined _QDEF_UNIX

#endif

#if defined(_WIN32) || defined(_WIN64)
#include <math.h>
#define fmax max
#define fmin min
#define fabs abs
#endif

/* atomic operations defines */
#if defined(_MSC_VER)
#define base_fetch_and_inc(x) InterlockedExchangeAdd((x), 1)
#define base_fetch_and_dec(x) InterlockedExchangeAdd((x), -1)
#define base_fetch_and_add(x, c) InterlockedExchangeAdd((x), c)
#define base_fetch_and_sub(x, c) InterlockedExchangeAdd((x), -(c))
#define base_compare_and_exchange(des, cmp, exch) InterlockedCompareExchange((des), (exch), (cmp))
#define base_exchange(des, exch) InterlockedExchange((des), (exch))
#else
#define base_fetch_and_inc(x) __sync_fetch_and_add((x), 1)
#define base_fetch_and_dec(x) __sync_fetch_and_sub((x), 1)
#define base_fetch_and_add(x, c) __sync_fetch_and_add((x), c)
#define base_fetch_and_sub(x, c) __sync_fetch_and_sub((x), c)
#define base_compare_and_exchange(des, cmp, exch) __sync_val_compare_and_swap((des), (cmp), (exch))
#define base_exchange(des, exch) __sync_lock_test_and_set((des), (exch))
#endif

#endif

