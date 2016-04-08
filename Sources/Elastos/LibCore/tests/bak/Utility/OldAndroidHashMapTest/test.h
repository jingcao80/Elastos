#ifndef __OLDABSTRACTMAPTEST_H__
#define __OLDABSTRACTMAPTEST_H__

#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int testBug17184(int argc, char* argv[]);
    int testAdd(int argc, char* argv[]);
    int testClear(int argc, char* argv[]);
    int testRemove(int argc, char* argv[]);
    int testManipulate(int argc, char* argv[]);
    int testKeyIterator(int argc, char* argv[]);
    int testValueIterator(int argc, char* argv[]);
    int testEntryIterator(int argc, char* argv[]);
    int testEquals(int argc, char* argv[]);

private:
    void addItems(IMap* map);

private:
    static const Int32 ONE = 1;
    static const Int32 TWO = 2;
    static const Int32 THREE = 3;
    static const Int32 FOUR = 4;
};

#endif // __OLDABSTRACTMAPTEST_H__