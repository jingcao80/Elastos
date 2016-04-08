#ifndef __INETADDRESSHELPER_TEST_H_
#define __INETADDRESSHELPER_TEST_H_

#include <elastos.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest();

public:
    int test_isMulticastAddress(int argc, char* argv[]);
    int test_isAnyLocalAddress(int argc, char* argv[]);
    int test_isLoopbackAddress(int argc, char* argv[]);
    int test_isLinkLocalAddress(int argc, char* argv[]);
    int test_isSiteLocalAddress(int argc, char* argv[]);
    int test_isMCGlobal(int argc, char* argv[]);
    int test_isMCNodeLocal(int argc, char* argv[]);
    int test_isMCLinkLocal(int argc, char* argv[]);
    int test_isMCSiteLocal(int argc, char* argv[]);
    int test_isMCOrgLocal(int argc, char* argv[]);
    int test_isIPv4CompatibleAddress(int argc, char* argv[]);
    int test_getByNameLjava_lang_String(int argc, char* argv[]);
    int test_getByAddressLString_BI(int argc, char* argv[]);
    int test_getByAddressLString_BLNetworkInterface(int argc, char* argv[]);
    int test_getHostAddress_(int argc, char* argv[]);
    int test_getScopeID(int argc, char* argv[]);
    int test_getScopedInterface(int argc, char* argv[]);
    int test_hashCode(int argc, char* argv[]);
};

#endif // __INETADDRESSHELPER_TEST_H_