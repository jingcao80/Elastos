#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_useDaylightTime_Taiwan,
    &CTest::test_useDaylightTime_Iceland,
    &CTest::test_clone_SimpleTimeZone,
    &CTest::testCustomTimeZoneDisplayNames,
    &CTest::testPreHistoricInDaylightTime,
    &CTest::testPreHistoricOffsets,
    &CTest::testZeroTransitionZones,
    &CTest::testHelsinkiOverflow,
    &CTest::testHasSameRules,
    &CTest::testNullId,
    &CTest::testCustomZoneIds,
    &CTest::test_getDSTSavings,
    &CTest::testSimpleTimeZoneDoesNotCallOverrideableMethodsFromConstructor,
    &CTest::test_11129,
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

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

//    TPEND;

    return nQuitCode;
}
