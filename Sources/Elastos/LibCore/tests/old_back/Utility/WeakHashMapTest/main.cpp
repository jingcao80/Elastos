#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_Constructor,   //1, ok
    &CTest::test_ConstructorI,   //2, ok
    &CTest::test_ConstructorIF,   //3, ok
    &CTest::test_ConstructorLjava_util_Map,   //4, not implement
    &CTest::test_clear,   //5, ok
    &CTest::test_containsKeyLjava_lang_Object,   //6, ok
    &CTest::test_containsValueLjava_lang_Object,   //7, ok
    &CTest::test_entrySet,   //8, ok
    &CTest::test_isEmpty,   //9, ok
    &CTest::test_putLjava_lang_ObjectLjava_lang_Object,   //10, ok
    &CTest::test_putAllLjava_util_Map,   //11, ok
    &CTest::test_removeLjava_lang_Object,   //12, ok
    &CTest::test_size,   //13, ok
    &CTest::test_keySet,   //14, ok
    &CTest::test_values,   //15, ok
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
