#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"
#include <elastos/Math.h>

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
    int test_setTimeZone(int argc, char* argv[]);
    int testAddOneDayOverDstForwardAdds23HoursAt0100(int argc, char* argv[]);
    int testAddOneDayOverDstForwardAdds24HoursAt0000(int argc, char* argv[]);
    int testAddOneDayOverDstBackAdds25HoursAt0000(int argc, char* argv[]);
    int testAddOneDayOverDstBackAdds25HoursAt0100(int argc, char* argv[]);
    int testAddTwoHalfDaysOverDstForwardAdds23HoursAt0100(int argc, char* argv[]);
    int testAdd24HoursOverDstForwardAdds24Hours(int argc, char* argv[]);
    int testAddOneDayAndOneDayOver30MinuteDstForwardAdds48Hours(int argc, char* argv[]);
    int testAddTwoDaysOver30MinuteDstForwardAdds47AndAHalfHours(int argc, char* argv[]);
    int testNewCalendarKoreaIsSelfConsistent(int argc, char* argv[]);
    int testSetTimeInZoneWhereDstIsNoLongerUsed(int argc, char* argv[]);
    int testCalendarSerialization(int argc, char* argv[]);
    int test_timebug(int argc, char* argv[]);

public:
    AutoPtr<ITimeZoneHelper> tzh;
    AutoPtr<ICalendarHelper> cdh;
    AutoPtr<ILocaleHelper> lch;
    AutoPtr<ITimeZone> AMERICA_SAO_PAULO;
    AutoPtr<ITimeZone> AUSTRALIA_LORD_HOWE;
    AutoPtr<ITimeZone> ASIA_KUALA_LUMPUR;
    AutoPtr<ITimeZone> ASIA_SEOUL;
};


#endif //__PATTTERN_TEST_H
