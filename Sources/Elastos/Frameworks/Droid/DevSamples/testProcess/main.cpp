#include "test.h"

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::Test1,
    &CTest::Test2,
    &CTest::Test3,
    &CTest::Test4,
    &CTest::Test5,
    &CTest::Test6,
    &CTest::Test7,
    &CTest::Test8,
    &CTest::Test9,
    &CTest::Test10,
    &CTest::Test11,
    &CTest::Test12,
    &CTest::Test13,
};

TPDECL;

int main(int argc, char * argv[])
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

    TPINIT;

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

    if (ISRUNOK) TPOK;
    TPEND;
    return nQuitCode;
}
