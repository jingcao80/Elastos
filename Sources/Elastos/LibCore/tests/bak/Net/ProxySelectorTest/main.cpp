#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testNoProxySystemProperty,   //1,
    &CTest::testProxyHostOnly,   //2,
    &CTest::testProxyHostPort,   //3,
    &CTest::testProxyPortOnly,   //4,
    &CTest::testHttpsDoesNotUseHttpProperties,   //5,
    &CTest::testProxyHost,   //6,
    &CTest::testHttpProxyHostPreferredOverProxyHost,   //7,
    &CTest::testSocksProxyHost,   //8,
    &CTest::testSocksProxyHostAndPort,   //9,
    &CTest::testNonProxyHostsFtp,   //10,
    &CTest::testNonProxyHostsHttp,   //11,
    &CTest::testNonProxyHostsHttps,   //12,
    &CTest::testSchemeCaseSensitive,   //13,
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
