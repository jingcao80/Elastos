#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testSocketSimple,   //1,
    &CTest::testWildcardAddress,   //2,
    &CTest::testServerSocketClose,   //3,
    &CTest::testSetReuseAddress,   //4, not implement
    &CTest::testTimeoutException,   //5,
    &CTest::testNativeSocketChannelOpen,   //6, not implement
    &CTest::disable_testConnectWithIP4IPAddr,   //7,
    &CTest::disable_testSocketConnectClose,   //8, not implement
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
