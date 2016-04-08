#ifndef __DATAGRAMSOCKET_TEST_H_
#define __DATAGRAMSOCKET_TEST_H_

#include "cmdef.h"
#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Net;
using namespace Elastos::Utility;
using Elastos::Net::Http::IRawHeaders;
using Elastos::Net::Http::IRawHeadersHelper;
using Elastos::Net::Http::CRawHeadersHelper;
using Elastos::Net::Http::CRawHeaders;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest();

public:
    // http://code.google.com/p/android/issues/detail?id=6684
    int test_caseInsensitiveButCasePreserving(int argc, char* argv[]);

    // The copy constructor used to be broken for headers with multiple values.
    // http://code.google.com/p/android/issues/detail?id=6722
    int test_copyConstructor(int argc, char* argv[]);

};

#endif // __DATAGRAMSOCKET_TEST_H_