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
    int test_toString(int argc, char* argv[]);
    int test_toGMTString(int argc, char* argv[]);
    int test_10583(int argc, char* argv[]);
    int test_10581(int argc, char* argv[]);
    int test_11090(int argc, char* argv[]);
    int test_11094(int argc, char* argv[]);
    int test_11129(int argc, char* argv[]);

private:
};


#endif //__PATTTERN_TEST_H
