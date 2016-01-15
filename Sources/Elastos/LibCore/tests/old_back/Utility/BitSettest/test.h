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
    int test_valueOf_long(int argc, char* argv[]);
    int test_valueOf_byte(int argc, char* argv[]);
    int test_toLongArray(int argc, char* argv[]);
    int test_toByteArray(int argc, char* argv[]);
    int test_previousSetBit(int argc, char* argv[]);
    int test_differentSizes(int argc, char* argv[]);

private:
};


#endif //__PATTTERN_TEST_H
