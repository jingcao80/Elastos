#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_getSymbol_fallback,
    &CTest::test_getInstance_illegal_currency_code,
    &CTest::testGetAvailableCurrencies,
    &CTest::test_getDisplayName,
    &CTest::test_getDefaultFractionDigits,
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
