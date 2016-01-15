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
    int test_parseNumericAddress(int argc, char* argv[]);
    int test_isNumeric(int argc, char* argv[]);
    int test_getByName(int argc, char* argv[]);
protected:
    void setUp();
    void tearDown();
private:
    AutoPtr<ArrayOf<Byte> > LOOPBACK6_BYTES;
    AutoPtr<ArrayOf<String> > INVALID_IPv4_NUMERIC_ADDRESSES;
};

#endif // __INETADDRESSHELPER_TEST_H_