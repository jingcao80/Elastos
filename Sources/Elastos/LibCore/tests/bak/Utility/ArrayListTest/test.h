#ifndef __ARRAYLIST_TEST_H
#define __ARRAYLIST_TEST_H


#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <cmdef.h>
#include <elastos/List.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest();

public:
    int test_Constructor(int argc, char* argv[]);
    int test_ConstructorI(int argc, char* argv[]);
    int test_ConstructorLjava_util_Collection(int argc, char* argv[]);
    int test_addILjava_lang_Object(int argc, char* argv[]);
    int test_addLjava_lang_Object(int argc, char* argv[]);
    int test_addAllILjava_util_Collection(int argc, char* argv[]);
    int test_addAllLjava_util_Collection(int argc, char* argv[]);
    int test_clear(int argc, char* argv[]);
    int test_clone(int argc, char* argv[]);
    int test_containsLjava_lang_Object(int argc, char* argv[]);
    int test_ensureCapacityI(int argc, char* argv[]);
    int test_getI(int argc, char* argv[]);
    int test_indexOfLjava_lang_Object(int argc, char* argv[]);
    int test_isEmpty(int argc, char* argv[]);
    int test_lastIndexOfLjava_lang_Object(int argc, char* argv[]);
    int test_removeI(int argc, char* argv[]);
    int test_setILjava_lang_Object(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_toArray(int argc, char* argv[]);
    int test_toArrayLjava_lang_Object(int argc, char* argv[]);
    int test_trimToSize_01(int argc, char* argv[]);
    int test_trimToSize_02(int argc, char* argv[]);
    int test_addAll(int argc, char* argv[]);
    int test_removeLjava_lang_Object(int argc, char* argv[]);
    int test_removeRangeII(int argc, char* argv[]);


    class Mock_ArrayList : public CArrayList {
    public:
        Mock_ArrayList(){}

        void removeRange(int begin, int end) {
            // CArrayList::RemoveRange(begin, end);
        }
    };
protected:
    void setUp();

    void tearDown();

private:
    AutoPtr<IArrayList> alist;

    AutoPtr< ArrayOf<IInterface*> > objArray;
    AutoPtr<ICharSequence> youink;
    AutoPtr<ICharSequence> kazoo;
};


#endif //__ARRAYLIST_TEST_H
