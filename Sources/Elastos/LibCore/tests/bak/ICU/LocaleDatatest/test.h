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
    int testAll(int argc, char* argv[]);
    int test_en_US(int argc, char* argv[]);
    int test_de_DE(int argc, char* argv[]);
    int test_cs_CZ(int argc, char* argv[]);
    int test_ru_RU(int argc, char* argv[]);

private:
    AutoPtr<ILocaleDataHelper> locdatahep;
    AutoPtr<ILocaleHelper> lochep;
};


#endif //__PATTTERN_TEST_H
