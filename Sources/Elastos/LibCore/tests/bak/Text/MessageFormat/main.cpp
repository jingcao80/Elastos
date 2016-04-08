#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_applyPattern,
    &CTest::test_formatToCharacterIterator,
    &CTest::test_getLocale,
    &CTest::test_setFormat,
    &CTest::test_formatLjava_lang_ObjectLjava_lang_StringBufferLjava_text_FieldPosition,
    &CTest::test_formatLjava_lang_StringLjava_lang_Object,
    &CTest::test_formatLjava_lang_ObjectLjava_lang_StringBufferLjava_text_FieldPosition2,
    &CTest::test_setFormatsLjava_text_Format,
    &CTest::test_parseLjava_lang_String,
    &CTest::test_parseObjectLjava_lang_StringLjavajava_text_ParsePosition,
    &CTest::test_format_Object,
    &CTest::test_parseLjava_lang_StringLjava_text_ParsePosition,
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
