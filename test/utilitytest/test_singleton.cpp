#include <stdio.h>
#include <iostream>
#include <sstream>
#include <map>
#include <signal.h>
#include <assert.h>
#include <memory>
#include <vector>
#include <set>
#include <queue> 
#include <functional>
#include <numeric>

#include "utility/singleton.h"

using namespace std;

class MySingletonClass : public utility::singleton<MySingletonClass> {
public:
    MySingletonClass() {}
    ~MySingletonClass() {}

public:
    void DoPrint() {
        printf("This is MySingletonClass.\n");
    }
};

int test_singleton_main()
{
    MySingletonClass::create_instance();
    MySingletonClass::get_instance()->DoPrint();
    MySingletonClass::destory_instance();
    
	getchar();
	return 0;
}

