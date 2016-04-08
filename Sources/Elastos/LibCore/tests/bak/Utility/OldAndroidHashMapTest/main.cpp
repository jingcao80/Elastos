#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testBug17184,
    &CTest::testAdd,
    &CTest::testClear,
    &CTest::testRemove,
    &CTest::testManipulate,
    &CTest::testKeyIterator,
    &CTest::testValueIterator,
    &CTest::testEntryIterator,
    &CTest::testEquals,
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
    PFL_EX("start===============================")

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

//    TPEND;

    PFL_EX("end=================================nQuitCode: %d", nQuitCode)
    return nQuitCode;
}
