#ifndef __CONCURRENTCLOASE_TEST_H
#define __CONCURRENTCLOASE_TEST_H

#include <elastos.h>
#include <Elastos.CoreLibrary.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest(){};
    ~CTest(){};

public:
    int test_accept(int argc, char* argv[]);
    int test_connect(int argc, char* argv[]);
    int test_connect_timeout(int argc, char* argv[]);
    int test_connect_nonBlocking(int argc, char* argv[]);
    int test_read(int argc, char* argv[]);
    int test_read_multiple(int argc, char* argv[]);
    int test_recv(int argc, char* argv[]);
    int test_write(int argc, char* argv[]);
};

#endif //__CONCURRENTCLOASE_TEST_H
