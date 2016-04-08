#ifndef __COOKIESTORETEST_H__
#define __COOKIESTORETEST_H__

#include "Elastos.CoreLibrary.h"
#include "cmdef.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int test_add_LURI_LHttpCookie(int argc, char* argv[]);
    int test_get_LURI(int argc, char* argv[]);
    int test_getCookies(int argc, char* argv[]);
    int test_getURIs(int argc, char* argv[]);
    int test_remove_LURI_LHttpCookie(int argc, char* argv[]);
    int test_removeAll(int argc, char* argv[]);

private:
    void setUp();
    void tearDown();

private:
    AutoPtr<ICookieManager> cookieManager;
    AutoPtr<ICookieStore> cookieStore;
};

#endif // __COOKIESTORETEST_H__