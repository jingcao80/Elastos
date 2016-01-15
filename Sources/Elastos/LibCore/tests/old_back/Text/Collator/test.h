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
    int test_setStrength(int argc, char* argv[]);
    int test_stackCorruption(int argc, char* argv[]);
    int test_collationKeySize(int argc, char* argv[]);
    int test_decompositionCompatibility(int argc, char* argv[]);
    int test_EqualsObject(int argc, char* argv[]);
    int test_Harmony(int argc, char* argv[]);

private:
};


#endif //__PATTTERN_TEST_H
