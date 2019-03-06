/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: dispatch.cpp
 Version: 1.0
 Date: 2015.9.14

 History:
 jeffguo     2015.9.14   1.0     Create
 ******************************************************************************/

#include "dispatch.h"
#include "timer.h"
#include "util.h"

namespace base
{

void dispatch(void* obj, dispatch_callback cb, void* param, int delay)
{
	dispatch_scheduler::shared_instance()->dispatch(obj, cb, param, delay);
}

void dispatch_sync(void* obj, dispatch_callback cb, void* param)
{
	dispatch_scheduler::shared_instance()->dispatch_sync(obj, cb, param);
}

void dispatch_cancel(void* obj)
{
	dispatch_scheduler::shared_instance()->dispatch_cancel(obj);
}

dispatch_scheduler* dispatch_scheduler::instance_ = NULL;
int dispatch_scheduler::dispatch_thread_count_ = 2;
mutex dispatch_scheduler::create_mutex_;

dispatch_scheduler::dispatch_scheduler()
	: process_threads_(NULL)
	, stop_process_threads_(false)
	, op_queue_(NULL)
	, op_event_(NULL)
	, schedule_event_(NULL)
{

}

dispatch_scheduler::~dispatch_scheduler()
{

}

dispatch_scheduler* dispatch_scheduler::shared_instance()
{
	if (instance_ == NULL) {
		mutex_scope ms(&create_mutex_);
		if (instance_ == NULL) {
			instance_ = new dispatch_scheduler();
			instance_->start();
		}
	}
	return instance_;
}

void dispatch_scheduler::release_instance()
{
	if (instance_ != NULL) {
		mutex_scope ms(&create_mutex_);
		if (instance_ != NULL) {
			instance_->stop();
			delete instance_;
			instance_ = NULL;
		}
	}
}

void dispatch_scheduler::dispatch(void* obj, dispatch_callback cb, void* param, int delay)
{
	assert(cb != NULL);

	if (delay == 0) {
		dispatch_op op;
		op.cb = cb;
		op.obj = obj;
		op.param = param;
		op_queue_->post_msg_mul(op);
		op_event_->set();
	}
	else {
		attach_dispatch(obj, cb, param, delay);
	}
}

void dispatch_scheduler::dispatch_sync(void* obj, dispatch_callback cb, void* param)
{
	assert(cb != NULL);

	dispatch_op op;
	event e;
	op.cb = cb;
	op.obj = obj;
	op.param = param;
	op.op_event = &e;

	op_queue_->post_msg_mul(op);
	e.wait();

	op_event_->set();
}

void dispatch_scheduler::dispatch_cancel(void* obj)
{
	mutex_scope ms(&mutex_);

	std::vector<tdispatch_info>::iterator it = dispatchs_.begin();
	for ( ; it != dispatchs_.end(); ) {
		if ((*it).op.obj == obj) {
			it = dispatchs_.erase(it);
		}
		else {
			++it;
		}
	}
}

void dispatch_scheduler::run()
{
	while (is_running_)
	{
		int ret = check_timers();
		ret += check_dispatchs();
		if (ret == 0) {
			schedule_event_->reset();
			schedule_event_->wait(20);
		}
		else {
			thread::sleep(1);
		}
	}
}

void dispatch_scheduler::start()
{
	schedule_event_ = new event();
	process_thread::start();

	op_queue_ = new tqueue<dispatch_op>(8);
	op_event_ = new event();

	stop_process_threads_ = false;
	process_threads_ = new thread[dispatch_thread_count_];
	for (int i = 0; i < dispatch_thread_count_; i++) {
		process_threads_[i].create((thread::thread_proc_t)message_process_thread, this);
	}
}

void dispatch_scheduler::stop()
{
	process_thread::stop();

	if (schedule_event_ != NULL) {
		delete schedule_event_;
		schedule_event_ = NULL;
	}

	stop_process_threads_ = true;
	if (process_threads_ != NULL) {
		for (int i = 0; i < dispatch_thread_count_; i++) {
			process_threads_[i].join();
		}
		delete[]process_threads_;
		process_threads_ = NULL;
	}

	if (op_queue_ != NULL) {
		delete op_queue_;
		op_queue_ = NULL;
	}

	if (op_event_ != NULL) {
		delete op_event_;
		op_event_ = NULL;
	}
}

void dispatch_scheduler::attach_timer(timer* t)
{
	mutex_scope ms(&mutex_);

	t->next_fire_time_ = util::clock() + t->timeout_;
	timers_.push_back(t);

	schedule_event_->set();
}

void dispatch_scheduler::detach_timer(timer* t)
{
	mutex_scope ms(&mutex_);

	for (int i = 0; i < (int)timers_.size(); i++) {
		if (timers_.at(i) == t) {
			timers_.erase(timers_.begin() + i);
			break;
		}
	}
}

void dispatch_scheduler::fire_timer(timer* t)
{
	mutex_scope ms(&mutex_);

	this->dispatch(t->obj_, t->cb_, t->param_);
	t->next_fire_time_ = util::clock() + t->timeout_;
}

void dispatch_scheduler::attach_dispatch(void* obj, dispatch_callback cb, void* param, int delay)
{
	mutex_scope ms(&mutex_);

	tdispatch_info tdi;
	tdi.op.obj = obj;
	tdi.op.cb = cb;
	tdi.op.param = param;
	tdi.fire_time = util::clock() + delay;
	tdi.timeout = delay;

	dispatchs_.push_back(tdi);
	schedule_event_->set();
}

int dispatch_scheduler::check_timers()
{
	mutex_scope ms(&mutex_);

	for (int i = 0; i < (int)timers_.size(); ) {
		timer* t = timers_.at(i);
		if (t->next_fire_time_ > 0 && t->next_fire_time_ < util::clock()) {
			this->dispatch(t->obj_, t->cb_, t->param_);
			t->next_fire_time_ = 0;
		}
		if (t->next_fire_time_ == 0) {
			if (t->repeat_) {
				t->next_fire_time_ = util::clock() + t->timeout_;
			}
			else {
				timers_.erase(timers_.begin() + i);
				continue;
			}
		}
		i++;
	}
	return (int)timers_.size();
}

int dispatch_scheduler::check_dispatchs()
{
	mutex_scope ms(&mutex_);

	for (int i = 0; i < (int)dispatchs_.size(); ) {
		tdispatch_info& tdi = dispatchs_[i];
		if (tdi.fire_time > 0 && tdi.fire_time < util::clock()) {
			this->dispatch(tdi.op.obj, tdi.op.cb, tdi.op.param);
			dispatchs_.erase(dispatchs_.begin() + i);
		}
		else {
			i++;
		}
	}
	return (int)dispatchs_.size();
}

#if defined(PLATFORM_WINDOWS)
DWORD WINAPI dispatch_scheduler::message_process_thread(void* param)
#else
void* dispatch_scheduler::message_process_thread(void* param)
#endif
{
	assert(param != NULL);

	dispatch_scheduler* dscheduler = (dispatch_scheduler*)param;
	dscheduler->process_message();

	return NULL;
}

void dispatch_scheduler::process_message()
{
	dispatch_op dop;
	while (!stop_process_threads_) {
		 if (op_queue_->get_msg_mul(dop)) {
			 dop.cb(dop.obj, dop.param);
			 if (dop.op_event != NULL) {
				 dop.op_event->set();
			 }
		 }
		 else {
			 op_event_->reset();
			 op_event_->wait(20);
		 }
	}
}

}
