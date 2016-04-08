#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H


#include <elastos.h>
#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Utility;
using namespace Elastos::Core;
using namespace Libcore::ICU;
using namespace Elastos::Text;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest(){};

public:
    int test_useDaylightTime_Taiwan(int argc, char* argv[]);
    int test_useDaylightTime_Iceland(int argc, char* argv[]);
    int test_clone_SimpleTimeZone(int argc, char* argv[]);
    int testCustomTimeZoneDisplayNames(int argc, char* argv[]);
    int testPreHistoricInDaylightTime(int argc, char* argv[]);
    int testPreHistoricOffsets(int argc, char* argv[]);
    int testZeroTransitionZones(int argc, char* argv[]);
    int testHelsinkiOverflow(int argc, char* argv[]);
    int testHasSameRules(int argc, char* argv[]);
    int testNullId(int argc, char* argv[]);
    int testCustomZoneIds(int argc, char* argv[]);
    int test_getDSTSavings(int argc, char* argv[]);
    int testSimpleTimeZoneDoesNotCallOverrideableMethodsFromConstructor(int argc, char* argv[]);
    int test_11129(int argc, char* argv[]);

private:
    void assertNonDaylightOffset(int expectedOffsetSeconds, long epochSeconds, AutoPtr<ITimeZone> tz);

private:
    AutoPtr<ITimeZoneHelper> tzh;
    AutoPtr<ILocaleHelper> lh;
};


#endif //__PATTTERN_TEST_H
