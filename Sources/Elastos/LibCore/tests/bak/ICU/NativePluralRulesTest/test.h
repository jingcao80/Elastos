#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H


#include <elastos.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Libcore::ICU;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest(){};

public:
    int testEnglish(int argc, char* argv[]);
    int testCzech(int argc, char* argv[]);
    int testArabic(int argc, char* argv[]);

private:
    AutoPtr<INativePluralRulesHelper> nprhep;
    AutoPtr<INativePluralRules> npr;

};


#endif //__PATTTERN_TEST_H
