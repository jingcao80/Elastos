#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_ConstructorLjava_lang_Class,
    &CTest::test_ConstructorLjava_util_EnumMap,
    &CTest::test_ConstructorLjava_util_Map,
    &CTest::test_clear,
    &CTest::test_containsKeyLjava_lang_Object,
    &CTest::test_clone,
    &CTest::test_containsValueLjava_lang_Object,
    &CTest::test_entrySet,
    &CTest::test_equalsLjava_lang_Object,
    &CTest::test_keySet,
    &CTest::test_getLjava_lang_Object,
    &CTest::test_putLjava_lang_ObjectLjava_lang_Object,
    &CTest::test_putAllLjava_util_Map,
    &CTest::test_removeLjava_lang_Object,
    &CTest::test_size,
    &CTest::test_values,
    &CTest::testSerializationSelf,
    &CTest::testSerializationCompatibility,
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
