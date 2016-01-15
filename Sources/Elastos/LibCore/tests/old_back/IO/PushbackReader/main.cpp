#include "test.h"

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::TestConstructor,
    &CTest::TestConstructor2,
    &CTest::TestClose,
    &CTest::TestMark,
    &CTest::TestIsMarkSupported,
    &CTest::TestRead,
    &CTest::TestReadBuffer,
    &CTest::TestReadBufferEx,
    &CTest::TestIsReady,
    &CTest::TestReset,
    &CTest::TestSkip,
    &CTest::TestUnRead,
    &CTest::TestUnReadBuffer,
    &CTest::TestUnReadBufferEx
};

//TPDECL;

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
