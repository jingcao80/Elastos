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
    int test_custom_Number_gets_longValue(int argc, char* argv[]);
    int test_small_BigInteger_gets_longValue(int argc, char* argv[]);
    int test_getIntegerInstance_ar(int argc, char* argv[]);
    int test_numberLocalization(int argc, char* argv[]);
    int test_10333(int argc, char* argv[]);
    int test_10514(int argc, char* argv[]);
    int test_10522(int argc, char* argv[]);

private:
};


#endif //__PATTTERN_TEST_H
