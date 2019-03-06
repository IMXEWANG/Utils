/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: dispatch.h
 Version: 1.0
 Date: 2015.9.14

 History:
 jeffguo     2015.9.14   1.0     Create
 ******************************************************************************/

#ifndef __VBASE_DISPATCH_H__
#define __VBASE_DISPATCH_H__

#include "tqueue.h"
#include "thread.h"
#include "event.h"
#include "timer.h"

namespace base
{

typedef timer_callback dispatch_callback;

/*
 * dispatch operation to the process thread
 * @param obj the obj that will be passed back to the function
 * @param cb target callback function
 * @param param the param that will be passed back to the function
 * @param delay the delay time before calling the function, in milliseconds.
 */
void dispatch(void* obj, dispatch_callback cb, void* param, int delay = 0);
/*
 * dispatch operation to the process thread, and wait until it's done
 * @param obj the obj that will be passed back to the function
 * @param cb target callback function
 * @param param the param that will be passed back to the function
 */
void dispatch_sync(void* obj, dispatch_callback cb, void* param);
/*
 * cancel all the unfired dispatched operation about obj
 * @param obj
 */
void dispatch_cancel(void* obj);

/****************************************************************************/

struct dispatch_op
{
	void* obj;
	dispatch_callback cb;
	void* param;
	event* op_event;

	dispatch_op()
		: obj(NULL)
		, cb(NULL)
		, param(NULL)
		, op_event(NULL)
	{}
};

struct tdispatch_info
{
	dispatch_op op;
	int timeout;
	long fire_time;

	tdispatch_info()
		: timeout(0)
		, fire_time(0)
	{}
};

class dispatch_scheduler
	: public base::process_thread
{
protected:
	dispatch_scheduler();
	virtual ~dispatch_scheduler();

public:
	static dispatch_scheduler* shared_instance();
	static void release_instance();

public:
	void dispatch(void* obj, dispatch_callback cb, void* param, int delay = 0);
	void dispatch_sync(void* obj, dispatch_callback cb, void* param);
	void dispatch_cancel(void* obj);

public:
	virtual void run();
	virtual void start();
	virtual void stop();

public:
	void attach_timer(timer* t);
	void detach_timer(timer* t);
	void fire_timer(timer* t);

	void attach_dispatch(void* obj, dispatch_callback cb, void* param, int delay);

protected:
	int check_timers();
	int check_dispatchs();

private:
#if defined(PLATFORM_WINDOWS)
	static DWORD WINAPI message_process_thread(void* param);
#else
	static void* message_process_thread(void* param);
#endif
	void process_message();

private:
	thread* process_threads_;
	bool stop_process_threads_;
	tqueue<dispatch_op>* op_queue_;
	event* op_event_;

	mutex mutex_;
	std::vector<timer*> timers_;
	std::vector<tdispatch_info> dispatchs_;
	event* schedule_event_;

	static dispatch_scheduler* instance_;
	static int dispatch_thread_count_;
	static mutex create_mutex_;
};

}

#endif
