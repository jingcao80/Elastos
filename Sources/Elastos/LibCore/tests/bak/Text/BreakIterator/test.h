#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H

#include <stdio.h>
#include <elastos.h>
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"
#include "elastos/StringBuilder.h"

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Text;
using namespace Libcore::ICU;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest();

public:
    int testGetAvailableLocales(int argc, char* argv[]);
    int testGetWordInstanceLocale(int argc, char* argv[]);
    int testWordBoundaries(int argc, char* argv[]);
    int testIsBoundary(int argc, char* argv[]);
    int testFollowing(int argc, char* argv[]);
    int testPreceding(int argc, char* argv[]);

public:
    AutoPtr<IBreakIterator> iterator;
    AutoPtr<IBreakIteratorHelper> breakhelper;
    AutoPtr<ILocale> locUS;
    AutoPtr<ILocaleHelper> lochelper;
};


#endif //__PATTTERN_TEST_H
