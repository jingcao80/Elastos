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
    int test_getInstance_unknown_locale(int argc, char* argv[]);
    int test_getInstance_invalid_locale(int argc, char* argv[]);
    int testSerialization(int argc, char* argv[]);


private:
};


#endif //__PATTTERN_TEST_H
