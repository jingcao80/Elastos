#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_setTimeZone,
    &CTest::testAddOneDayOverDstForwardAdds23HoursAt0100,
    &CTest::testAddOneDayOverDstForwardAdds24HoursAt0000,
    &CTest::testAddOneDayOverDstBackAdds25HoursAt0000,
    &CTest::testAddOneDayOverDstBackAdds25HoursAt0100,
    &CTest::testAddTwoHalfDaysOverDstForwardAdds23HoursAt0100,
    &CTest::testAdd24HoursOverDstForwardAdds24Hours,
    &CTest::testAddOneDayAndOneDayOver30MinuteDstForwardAdds48Hours,
    &CTest::testAddTwoDaysOver30MinuteDstForwardAdds47AndAHalfHours,
    &CTest::testNewCalendarKoreaIsSelfConsistent,
    &CTest::testSetTimeInZoneWhereDstIsNoLongerUsed,
    &CTest::testCalendarSerialization,
    &CTest::test_timebug,
};

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("*ERROR* Require test case number\n");
        return -1;
    }

    int nIndex = atoi(argv[1]) - 1;
    int nLength = sizeof(TestEntry) / sizeof(TestEntry[0]);

    if (nIndex < 0 || nIndex >= nLength) {
        printf("*ERROR* Invalid testing case number\n");
        return -1;
    }

//    TPINIT;

    CTest TestObject;
PFL
    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);
PFL
//    TPEND;

    return nQuitCode;
}
