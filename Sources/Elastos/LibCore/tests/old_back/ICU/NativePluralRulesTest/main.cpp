#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testEnglish,
    &CTest::testCzech,
    &CTest::testArabic,
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
