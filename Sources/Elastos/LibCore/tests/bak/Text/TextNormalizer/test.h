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
    int testTextNormalize(int argc, char* argv[]);
    int testIsNormalized(int argc, char* argv[]);


private:
};


#endif //__PATTTERN_TEST_H
