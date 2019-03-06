/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: test_alarm.h
 Version: 1.0
 Date: 2015.9.25
 
 History:
 jeffguo     2015.9.25   1.0     Create
 ******************************************************************************/

#ifndef BASE_TEST_ALARM_H_
#define BASE_TEST_ALARM_H_

#include "base/alarm.h"
#include "base/sigslot.h"

class test_alarm
	: public sigslot::has_slots<>
{
public:
	test_alarm();
	~test_alarm();
    void set_alarm(int hour, int min, int sec);

private:
	void alarm_callback(base::alarm_info& ainfo, struct tm* t);

private:
    base::alarm* m_alarm;
};

#endif
