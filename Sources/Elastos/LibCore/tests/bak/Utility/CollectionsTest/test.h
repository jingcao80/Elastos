#ifndef __WEAKHASHMAP_TEST_H
#define __WEAKHASHMAP_TEST_H

#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Core;
using Elastos::Utility::IIterator;
using Elastos::Utility::IListIterator;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int testEmptyEnumeration(int argc, char* argv[]);

    int testEmptyIterator(int argc, char* argv[]);

    int testEmptyListIterator(int argc, char* argv[]);

private:
    void test_EmptyIterator(IIterator* i);

    void test_EmptyListIterator(IListIterator* i);
};

#endif //__WEAKHASHMAP_TEST_H
