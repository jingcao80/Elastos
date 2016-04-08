#ifndef __INETSOCKETADDRESSTEST_H__
#define __INETSOCKETADDRESSTEST_H__

#include "Elastos.CoreLibrary.h"
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
    int test_ConstructorLjava_lang_StringI(int argc, char* argv[]);
    int test_ConstructorLInetAddressI(int argc, char* argv[]);
    int test_ConstructorI(int argc, char* argv[]);
    int test_equals(int argc, char* argv[]);
    int test_getAddress(int argc, char* argv[]);
    int test_hashCode(int argc, char* argv[]);
    int test_isUnresolved(int argc, char* argv[]);
    int test_getHostString(int argc, char* argv[]);
};

#endif // __INETSOCKETADDRESSTEST_H__