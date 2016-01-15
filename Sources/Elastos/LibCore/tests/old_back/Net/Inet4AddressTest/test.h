#ifndef __INET4ADDRESS_TEST_H_
#define __INET4ADDRESS_TEST_H_

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

protected:
    void setUp();
    void tearDown();
private:
    AutoPtr<ArrayOf<Byte> > LOOPBACK6_BYTES;
    AutoPtr<ArrayOf<String> > INVALID_IPv4_NUMERIC_ADDRESSES;
};

#endif //__INET4ADDRESS_TEST_H_