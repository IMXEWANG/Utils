#include <stdio.h>
#include <iostream>

#include "utility/nocopyable.h"

using namespace std;

class test_nocopyable : public utility::nocopyable {
public:
    test_nocopyable() {}
    ~test_nocopyable() {}

public:
    void do_print() {
        printf("This is test_nocopyable.\n");
    }
};

int test_nocopyable_main()
{
    test_nocopyable one;
    one.do_print();
    //test_nocopyable two = one;
    //two.do_print();

    getchar();
    return 0;
}

