#ifndef __TREESETTEST_H__
#define __TREESETTEST_H__

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
    int testEmptySetSerialization(int argc, char* argv[]);
    int testSerializationWithComparator(int argc, char* argv[]);
    int testSubSetSerialization(int argc, char* argv[]);
    int testNavigableSubSetSerialization(int argc, char* argv[]);
    int testDescendingSetSerialization(int argc, char* argv[]);
    int testJava5SerializationWithComparator(int argc, char* argv[]);

private:
    void assertBounded(ISortedSet* deserialized, Boolean bounded);
};

#endif // __TREESETTEST_H__