#ifndef _QADDRESS_H
#define _QADDRESS_H

#include "Global.h"
#include <string>
using namespace std;

namespace utility {

struct COMMON_API QAddress {  
    string host_name;
    string port;
};
}
#endif
