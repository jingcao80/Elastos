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
    int test1(int argc, char* argv[]);
    int test2(int argc, char* argv[]);
    int test3(int argc, char* argv[]);
    int test4(int argc, char* argv[]);
    int test5(int argc, char* argv[]);

private:
};


#endif //__PATTTERN_TEST_H
