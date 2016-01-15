
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

using namespace Elastos;
using namespace Elastos::Text;

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_custom_Number_gets_longValue,
    &CTest::test_small_BigInteger_gets_longValue,
    &CTest::test_getIntegerInstance_ar,
    &CTest::test_numberLocalization,
    &CTest::test_10333,
    &CTest::test_10514,
    &CTest::test_10522,

};


#ifndef ASSERT_SUCCEEDED
#define ASSERT_SUCCEEDED(expr) \
   do { \
       ECode ec = expr; \
       assert(SUCCEEDED(ec)); \
   } while(0);
#endif

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

    // Double num = 12345.321;
    // Double percent = 0.123;
    // AutoPtr<INumberFormatHelper> helper;
    // ASSERT_SUCCEEDED(CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&helper));

    // AutoPtr<INumberFormat> format;
    // String result;
    // ASSERT_SUCCEEDED(helper->GetInstance((INumberFormat**)&format));
    // printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    // ASSERT_SUCCEEDED(format->FormatDouble(num, &result));
    // printf("==== File: %s, Line: %d, result: %s ====\n", __FILE__, __LINE__, (const char*)result);

    // ASSERT_SUCCEEDED(helper->GetCurrencyInstance((INumberFormat**)&format));
    // printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    // ASSERT_SUCCEEDED(format->FormatDouble(num, &result));
    // printf("==== File: %s, Line: %d, result: %s ====\n", __FILE__, __LINE__, (const char*)&result);

    // ASSERT_SUCCEEDED(helper->GetPercentInstance((INumberFormat**)&format));
    // printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    // ASSERT_SUCCEEDED(format->FormatDouble(percent, &result));
    // printf("==== File: %s, Line: %d, result: %s ====\n", __FILE__, __LINE__, (const char*)&result);

    return 0;
}
