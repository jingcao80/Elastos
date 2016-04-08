#ifndef __TREEMAPTEST_H__
#define __TREEMAPTEST_H__

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
    int testEntrySetSetValue(int argc, char* argv[]);
    int testSubMapEntrySetSetValue(int argc, char* argv[]);
    int testExceptionsOnSetValue(int argc, char* argv[]);
    int testExceptionsOnSubMapSetValue(int argc, char* argv[]);
    int testConcurrentModificationDetection(int argc, char* argv[]);
    int testIteratorRemoves(int argc, char* argv[]);
    int testEntrySetUsesComparatorOnly(int argc, char* argv[]);
    int testMapConstructorPassingSortedMap(int argc, char* argv[]);
    int testNullsWithNaturalOrder(int argc, char* argv[]);
    int testClassCastExceptions(int argc, char* argv[]);
    int testClassCastExceptionsOnBounds(int argc, char* argv[]);
    int testClone(int argc, char* argv[]);
    int testEmptyMapSerialization(int argc, char* argv[]);
    int testSerializationWithComparator(int argc, char* argv[]);
    int testSubMapSerialization(int argc, char* argv[]);
    int testNavigableSubMapSerialization(int argc, char* argv[]);
    int testDescendingMapSerialization(int argc, char* argv[]);
    int testJava5SerializationWithComparator(int argc, char* argv[]);
    int testJava5SubMapSerialization(int argc, char* argv[]);
private:
    void assertAllEntryMethodsReturnImmutableEntries(INavigableMap* map);
    void assertImmutable(IEntry* entry);
private:
    AutoPtr<ICharSequence> csA;
    AutoPtr<ICharSequence> csa;
    AutoPtr<ICharSequence> csB;
    AutoPtr<ICharSequence> csb;
    AutoPtr<ICharSequence> csC;
    AutoPtr<ICharSequence> csc;
    AutoPtr<ICharSequence> csD;
    AutoPtr<ICharSequence> csd;
    AutoPtr<ICharSequence> csx;
    AutoPtr<ICharSequence> csy;
    AutoPtr<ICharSequence> csz;
};

#endif // __TREEMAPTEST_H__