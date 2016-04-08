#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testAll,
    &CTest::test_en_US,
    &CTest::test_de_DE,
    &CTest::test_cs_CZ,
    &CTest::test_ru_RU,
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
