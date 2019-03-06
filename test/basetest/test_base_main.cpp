/*****************************************************************************
VBase Copyright (c) 2015. All Rights Reserved.

FileName: main.cpp
Version: 1.0
Date: 2016.11.25

History:
eric     2016.11.25   1.0     Create
******************************************************************************/
#include <map>
#include <stdio.h>

#include "base/trace.h"
#include "base/util.h"
#include "test_sigslot.h"
#include "test_dictionary.h"
#include "test_alarm.h"
#include "test_log_binder.h"
#include "test_zip.h"
#include "test_aes.h"
#include "test_rsa.h"

void test_rsa1(int argc, char* argv[])
{
    test_rsa t;
    t.test_single_rsa();
}

void test_aes1(int argc, char* argv[])
{
    test_aes t;
    t.test_single_aes();
}

void test_zlib(int argc, char* argv[])
{
	test_zip t;
	t.test_flate(argc, argv);
}

void test_log_binder(int argc, char* argv[])
{
    if (!base::default_log_binder::bind_trace("log_config.xml")) {
        std::cout << "log bind_trace failed" << std::endl;
        return;
    }
    base::trace::enable_std_output(true);
    base::trace::set_trace_level_filter(base::TRACE_LEVEL_SYSTEM);

    TRACE_ERROR("test_log_binder", 10000, "%s", "this is log4cplus packet");
    TRACE_SYSTEM("test_log_binder", "test_log_binder (%s:%d)\n", "hello", 10001);
}

void test_sigslot(int argc, char* argv[])
{
    alarm_clock alarm_clock;
    student midschool_stu;
    alarm_clock.tick.connect(&midschool_stu, &student::weak_up);
    for (int i = 0; i < 10; i++) {
        alarm_clock.send_msg(i);
        base::util::sleep(1000);
    }
    alarm_clock.tick.disconnect(&midschool_stu);
}

void test_alarm1(int argc, char* argv[])
{
    test_alarm t;
    t.set_alarm(15, 55, 0);
    getchar(); //»»ÐÐ·û
    getchar(); //µÈ´ý³¬Ê±
}

void test_dictionarys(int argc, char* argv[])
{
    test_dictionay d;
    d.test();
}

int main(int argc, char* argv[])
{
    int ch = '1';
    do{
        printf("the list: \n");
        printf("0: exit \n");
        printf("1: test base::timer \n");
        printf("2: test base::dispatch \n");
        printf("3: test base::log \n");
        printf("4: test base::event \n");
        printf("5: test base::tqueue \n");
        printf("6: test base::alarm \n");
        printf("7: test base::rsa \n");
        printf("8: test base::aes \n");
        printf("9: test base::trace \n");
        printf("a: test std::map\n");
        printf("b: test base::dictonary\n");
        printf("c: test base::sigslot\n");
		printf("d: test base::zlib\n");
        printf("please select your decide: ");
        ch = getchar();
        switch (ch) {
        case '0':
            printf("exit OK ~~\n");
            break;
        case '3':
            test_log_binder(argc, argv);
            break;
        case '6':
            test_alarm1(argc, argv);
            break;
        case '7':
            test_rsa1(argc, argv);
            break;
        case '8':
            test_aes1(argc, argv);
            break;
        case 'b':
            test_dictionarys(argc, argv);
            break;
        case 'c':
            test_sigslot(argc, argv);
            break;
		case 'd':
			test_zlib(argc, argv);
			break;
        default:
            printf("please input right decide~~\n");
            break;
        }
    } while (0);
    //} while (ch != '0');

    getchar();
    getchar();
    return 0;
}
