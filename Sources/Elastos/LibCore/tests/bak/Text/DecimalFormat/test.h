#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H


#include <elastos.h>
using namespace Elastos;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest(){};
    ~CTest(){};

public:
    int test_exponentSeparator(int argc, char* argv[]);
    int test_setMaximumFractionDigitsAffectsRoundingMode(int argc, char* argv[]);
    int test_setMaximumIntegerDigits(int argc, char* argv[]);
    int test_BigDecimalBug1897917(int argc, char* argv[]);
    int testBigDecimalTestBigIntWithMultiplier(int argc, char* argv[]);
    int testBigDecimalICUConsistency(int argc, char* argv[]);

private:
};


#endif //__PATTTERN_TEST_H
