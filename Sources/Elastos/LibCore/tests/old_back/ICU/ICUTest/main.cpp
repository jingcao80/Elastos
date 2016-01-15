#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_getISOLanguages,
    &CTest::test_getISOCountries,
    &CTest::test_getAvailableLocales,
    &CTest::test_localeFromString,
    &CTest::test_getScript_addLikelySubtags,
    &CTest::test_11152,
};

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("*ERROR* Require test case number\n");
        return -1;
    }
    int nIndex = atoi(argv[1]);
    CTest testObj;
    (testObj.*TestEntry[nIndex-1])(argc, argv);

    return 0;
}
