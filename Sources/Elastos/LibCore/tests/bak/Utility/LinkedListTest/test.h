#ifndef __LINKEDLISTTEST_H__
#define __LINKEDLISTTEST_H__

#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using Elastos::Utility::ILinkedList;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    // int test_ConstructorLjava_util_Collection(int argc, char* argv[]);
    int test_addILjava_lang_Object(int argc, char* argv[]);
    int test_addLjava_lang_Object(int argc, char* argv[]);
    int test_addAllILjava_util_Collection(int argc, char* argv[]);
    int test_addAllILjava_util_Collection_2(int argc, char* argv[]);
    int test_addAllLjava_util_Collection(int argc, char* argv[]);
    int test_addFirstLjava_lang_Object(int argc, char* argv[]);
    int test_addLastLjava_lang_Object(int argc, char* argv[]);
    int test_clear(int argc, char* argv[]);
    int test_clone(int argc, char* argv[]);
    int test_containsLjava_lang_Object(int argc, char* argv[]);
    int test_getI(int argc, char* argv[]);
    int test_getFirst(int argc, char* argv[]);
    int test_getLast(int argc, char* argv[]);
    int test_indexOfLjava_lang_Object(int argc, char* argv[]);
    int test_lastIndexOfLjava_lang_Object(int argc, char* argv[]);
    int test_listIteratorI(int argc, char* argv[]);
    int test_removeI(int argc, char* argv[]);
    int test_removeLjava_lang_Object(int argc, char* argv[]);
    int test_removeFirst(int argc, char* argv[]);
    int test_removeLast(int argc, char* argv[]);
    int test_setILjava_lang_Object(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_toArray(int argc, char* argv[]);
    int test_toArrayLjava_lang_Object(int argc, char* argv[]);
    int test_offer(int argc, char* argv[]);
    int test_poll(int argc, char* argv[]);
    int test_remove(int argc, char* argv[]);
    int test_element(int argc, char* argv[]);
    int test_peek(int argc, char* argv[]);

protected:
    void setUp();
    void tearDown();

private:
    AutoPtr<ILinkedList> ll;
    AutoPtr<ArrayOf<IInterface*> > objArray;
};

#endif // __LINKEDLISTTEST_H__