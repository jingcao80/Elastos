#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testIteratorAndNonStructuralChanges,
    &CTest::testSubListAndNonStructuralChanges,
    &CTest::testSubListAndStructuralChanges,
    &CTest::testSubListAndSizePreservingStructuralChanges,
    &CTest::testRemoveAll,
    &CTest::testSubListClear,
    &CTest::testSubListClearWhenEmpty,
    &CTest::testSubListIteratorGetsSnapshot,
    &CTest::testSubListRemoveByValue,
    &CTest::testSubListRemoveByIndex,
    &CTest::testSubListRetainAll,
    &CTest::testSubListRemoveAll,
    &CTest::testAtomicAdds,
    &CTest::testSubListAtomicAdds,
    &CTest::testSubListAddIsAtEnd,
    &CTest::testSubListAddAll,
    &CTest::testListIterator,
    &CTest::testSerialize,
    &CTest::testDoesNotRetainToArray,
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
