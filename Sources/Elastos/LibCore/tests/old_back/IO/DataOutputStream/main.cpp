#include "test.h"

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::TestConstructor,
    &CTest::TestClose,
    &CTest::TestFlush,
    &CTest::TestWrite,
    &CTest::TestWriteBuffer,
    &CTest::TestWriteBufferEx,
    &CTest::TestCheckError,
    &CTest::TestWriteBoolean,
    &CTest::TestWriteByte,
    &CTest::TestWriteBytes,
    &CTest::TestWriteBytesEx,
    &CTest::TestWriteChar,
    &CTest::TestWriteDouble,
    &CTest::TestWriteFloat,
    &CTest::TestWriteInt32,
    &CTest::TestWriteInt64,
    &CTest::TestWriteInt16,
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
