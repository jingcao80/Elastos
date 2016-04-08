#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testEntrySetSetValue,
    &CTest::testSubMapEntrySetSetValue,
    &CTest::testExceptionsOnSetValue,
    &CTest::testExceptionsOnSubMapSetValue,
    &CTest::testConcurrentModificationDetection,
    &CTest::testIteratorRemoves,
    &CTest::testEntrySetUsesComparatorOnly,

    &CTest::testMapConstructorPassingSortedMap,
    &CTest::testNullsWithNaturalOrder,
    &CTest::testClassCastExceptions,
    &CTest::testClassCastExceptionsOnBounds,
    &CTest::testClone,
    &CTest::testEmptyMapSerialization,
    &CTest::testSerializationWithComparator,

    &CTest::testSubMapSerialization,
    &CTest::testNavigableSubMapSerialization,
    &CTest::testDescendingMapSerialization,
    &CTest::testJava5SerializationWithComparator,
    &CTest::testJava5SubMapSerialization,
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
