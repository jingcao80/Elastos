#ifndef __CURRENCYTEST_H__
#define __CURRENCYTEST_H__

#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Libcore::ICU;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest(){};
    ~CTest(){};

public:
    int test_getSymbol_fallback(int argc, char* argv[]);
    int test_getInstance_illegal_currency_code(int argc, char* argv[]);
    int testGetAvailableCurrencies(int argc, char* argv[]);
    int test_getDisplayName(int argc, char* argv[]);
    int test_getDefaultFractionDigits(int argc, char* argv[]);
};

#endif // __CURRENCYTEST_H__