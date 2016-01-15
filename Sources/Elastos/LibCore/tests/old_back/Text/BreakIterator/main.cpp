#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testGetAvailableLocales,
    &CTest::testGetWordInstanceLocale,
    &CTest::testWordBoundaries,
    &CTest::testIsBoundary,
    &CTest::testFollowing,
    &CTest::testPreceding,
};

int main(int argc, char* argv[])
{
    printf("%s %d\n", __FILE__, __LINE__);
    int nIndex = atoi(argv[1]);
    printf("%s %d\n", __FILE__, __LINE__);
    CTest testObj;
    PFL
    (testObj.*TestEntry[nIndex-1])(argc, argv);
    PFL
    return 0;
}
