#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_isMulticastAddress, // 1.ok
    &CTest::test_isAnyLocalAddress, // 2.ok
    &CTest::test_isLoopbackAddress, // 3.ok
    &CTest::test_isLinkLocalAddress, // 4.ok
    &CTest::test_isSiteLocalAddress, // 5.ok
    &CTest::test_isMCGlobal, // 6.ok
    &CTest::test_isMCNodeLocal, // 7.ok
    &CTest::test_isMCLinkLocal, // 8.ok
    &CTest::test_isMCSiteLocal, // 9.ok
    &CTest::test_isMCOrgLocal, // 10.ok
    &CTest::test_isIPv4CompatibleAddress, // 11.ok
    &CTest::test_getByNameLjava_lang_String, // 12.ok
    &CTest::test_getByAddressLString_BI, // 13.ok
    &CTest::test_getByAddressLString_BLNetworkInterface, // 14.ok
    &CTest::test_getHostAddress_, // 15. not ok
    &CTest::test_getScopeID, // 16. ok
    &CTest::test_getScopedInterface,// 17. ok
    &CTest::test_hashCode, //18. ok
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

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

    return nQuitCode;
}
