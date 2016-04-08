#ifndef __WEAKHASHMAP_TEST_H
#define __WEAKHASHMAP_TEST_H

#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Elastos::Utility::Concurrent;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int testIteratorAndNonStructuralChanges(int argc, char* argv[]);
    int testSubListAndNonStructuralChanges(int argc, char* argv[]);
    int testSubListAndStructuralChanges(int argc, char* argv[]);
    int testSubListAndSizePreservingStructuralChanges(int argc, char* argv[]);
    int testRemoveAll(int argc, char* argv[]);
    int testSubListClear(int argc, char* argv[]);
    int testSubListClearWhenEmpty(int argc, char* argv[]);
    int testSubListIteratorGetsSnapshot(int argc, char* argv[]);
    int testSubListRemoveByValue(int argc, char* argv[]);
    int testSubListRemoveByIndex(int argc, char* argv[]);
    int testSubListRetainAll(int argc, char* argv[]);
    int testSubListRemoveAll(int argc, char* argv[]);
    int testAtomicAdds(int argc, char* argv[]);
    int testSubListAtomicAdds(int argc, char* argv[]);
    int testSubListAddIsAtEnd(int argc, char* argv[]);
    int testSubListAddAll(int argc, char* argv[]);
    int testListIterator(int argc, char* argv[]);
    int testSerialize(int argc, char* argv[]);
    int testDoesNotRetainToArray(int argc, char* argv[]);

private:
    void testAddAllIsAtomic(IList* list);

private:
    AutoPtr<IArrays> arrhelp;
    AutoPtr< ArrayOf<IInterface*> > mabcde;
    AutoPtr< ArrayOf<IInterface*> > autoarr;
    AutoPtr<ICharSequence> sqa;
    AutoPtr<ICharSequence> sqb;
    AutoPtr<ICharSequence> sqc;
    AutoPtr<ICharSequence> sqd;
    AutoPtr<ICharSequence> sqe;
    AutoPtr<ICharSequence> sqf;
    AutoPtr<ICharSequence> sqg;
    AutoPtr<ICharSequence> sqh;
    AutoPtr<ICharSequence> sqi;

    AutoPtr<ICharSequence> sqA;
    AutoPtr<ICharSequence> sqB;
    AutoPtr<ICharSequence> sqC;
    AutoPtr<ICharSequence> sqD;
    AutoPtr<ICharSequence> sqE;

    AutoPtr<IInterface> outface;
    Boolean isflag;
};

#endif //__WEAKHASHMAP_TEST_H
