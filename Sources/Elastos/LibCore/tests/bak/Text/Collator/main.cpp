#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_setStrength,
    &CTest::test_stackCorruption,
    &CTest::test_collationKeySize,
    &CTest::test_decompositionCompatibility,
    &CTest::test_EqualsObject,
    &CTest::test_Harmony,
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

    CTest TestObject;
    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

    return nQuitCode;
}
