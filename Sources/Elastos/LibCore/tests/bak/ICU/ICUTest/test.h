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
    int test_getISOLanguages(int argc, char* argv[]);
    int test_getISOCountries(int argc, char* argv[]);
    int test_getAvailableLocales(int argc, char* argv[]);
    int test_localeFromString(int argc, char* argv[]);
    int test_getScript_addLikelySubtags(int argc, char* argv[]);
    int test_11152(int argc, char* argv[]);

private:
    AutoPtr<IICUUtil> icuhep;
};


#endif //__PATTTERN_TEST_H
