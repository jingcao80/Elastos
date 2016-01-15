#ifndef __INETSOCKETADDRESSTEST_H__
#define __INETSOCKETADDRESSTEST_H__

#include "cmdef.h"
#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>
#include <elastos/Mutex.h>

using namespace Elastos;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int testIPv6(int argc, char* argv[]);
    int testLoopback(int argc, char* argv[]);
    int testDumpAll(int argc, char* argv[]);

};

#endif // __INETSOCKETADDRESSTEST_H__