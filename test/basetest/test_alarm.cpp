/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: test_dictionary.h
 Version: 1.0
 Date: 2015.9.25

 History:
 jeffguo     2015.9.25   1.0     Create
 ******************************************************************************/

#include "test_alarm.h"
#include <stdio.h>
#include <base/util.h>

test_alarm::test_alarm()
: m_alarm(NULL)
{

}

test_alarm::~test_alarm()
{
    if (m_alarm) {
        m_alarm->turn_off();
        delete m_alarm;
        m_alarm = NULL;
    }
}

void test_alarm::set_alarm(int hour, int min, int sec)
{
    printf("alarm begin:%s\n", base::util::local_time_string().c_str());
    m_alarm = new base::alarm(hour, min, sec);
    m_alarm->signal_alarm_.connect(this, &test_alarm::alarm_callback);
    m_alarm->turn_on();
}

void test_alarm::alarm_callback(base::alarm_info& ainfo, struct tm* t)
{
    printf("alarm fired,hour:%d, min:%d, sec:%d\n", 
        t->tm_hour, t->tm_min, t->tm_sec);
}


