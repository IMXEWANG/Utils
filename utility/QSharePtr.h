#ifndef _QSHAREPTR_H
#define _QSHAREPTR_H

#include "include_common.h"

namespace utility {

template<class T>
class QSharePtr 
{
public:
    class PointerCount 
	{
	public:
		PointerCount(T*  p_obj)
			: objPtr(p_obj), refCount(1) 
		{
		}

		~PointerCount()
		{
			if(objPtr) {
				delete objPtr;
			}
		}

        void add_ref()
		{
			base_fetch_and_inc(&refCount);
		}

        void release_ref()
		{
			if(refCount <= 1) {   
				delete this;   
				return;
			} else {
				base_fetch_and_dec(&refCount);
			}
		}

        int use_count()
		{
			return refCount;
		}

	private:
		volatile int refCount;
		T* objPtr;
    };
    
    QSharePtr()
		: p_obj_(0)
	{
		p_count_ = new PointerCount(0);
	}

    explicit QSharePtr(T*  ptr)
		: p_obj_(ptr)
	{
		p_count_ = new PointerCount(ptr);
	}

    QSharePtr(const QSharePtr<T> & source)
	{
		p_count_ = source.p_count_;    
		p_count_->add_ref();
		p_obj_ = source.p_obj_;
	}

    virtual ~QSharePtr()
	{
		p_count_->release_ref();
	}

    QSharePtr<T> & operator=(const QSharePtr<T> & source)
	{
		if(this == &source)
		{
			return *this;
		}
		p_count_->release_ref();

		p_count_ = source.p_count_;
		p_obj_ = source.p_obj_;
		if(p_count_)
		{
			p_count_->add_ref();
		}
		return *this;
	}

    int getRefCount() { return p_count_->use_count(); }

    T* operator->() { return p_obj_; }

	const T* operator->() const { return p_obj_; }

    T& operator*() { return *(p_obj_); }

	operator bool() const { return (p_obj_ != 0); }

	bool operator!() const { return (p_obj_ == 0); }

	bool operator<(const QSharePtr<T>& a) const { return this->p_obj_ < a.p_obj_; }

    inline T* get_pointer() {return p_obj_;}
	inline const T* get_pointer() const { return p_obj_;}

private:
    PointerCount* p_count_;
    T* p_obj_;
};

}
#endif
