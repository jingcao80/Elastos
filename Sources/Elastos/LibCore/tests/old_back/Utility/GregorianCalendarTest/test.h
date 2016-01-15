#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H


#include <elastos.h>
#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Utility;
using namespace Elastos::Core;
using namespace Libcore::ICU;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest(){};

public:
    int testGetHour(int argc, char* argv[]);
    int test_computeTime(int argc, char* argv[]);
    int test_computeFields(int argc, char* argv[]);
    int test_hashCode(int argc, char* argv[]);
    int test_setFirstDayOfWeekI(int argc, char* argv[]);
    int test_setMinimalDaysInFirstWeekI(int argc, char* argv[]);

private:
    AutoPtr<ITimeZoneHelper> tzh;
    AutoPtr<ILocaleHelper> lh;
};


#endif //__PATTTERN_TEST_H
