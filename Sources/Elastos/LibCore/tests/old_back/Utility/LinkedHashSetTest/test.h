#ifndef __LINKEDHASHSETTEST_H__
#define __LINKEDHASHSETTEST_H__

#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>

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
    int test_Constructor(int argc, char* argv[]);
    int test_ConstructorI(int argc, char* argv[]);
    int test_ConstructorIF(int argc, char* argv[]);
    int test_ConstructorLjava_util_Collection(int argc, char* argv[]);
    int test_addLjava_lang_Object(int argc, char* argv[]);
    int test_clear(int argc, char* argv[]);
    int test_clone(int argc, char* argv[]);
    int test_containsLjava_lang_Object(int argc, char* argv[]);
    int test_isEmpty(int argc, char* argv[]);
    int test_iterator(int argc, char* argv[]);
    int test_removeLjava_lang_Object(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_retainAllLjava_util_Collection(int argc, char* argv[]);
    int test_toArray(int argc, char* argv[]);
    int test_toArrayLjava_lang_Object(int argc, char* argv[]);

protected:
    void setUp();
    void tearDown();

private:
    // class Mock_LinkedHashSet extends LinkedHashSet {
    //     @Override
    //     public boolean retainAll(Collection c) {
    //         throw new UnsupportedOperationException();
    //     }
    // };

    AutoPtr<ISet> hs;

    AutoPtr< ArrayOf<IInterface*> > objArray;
};

#endif // __LINKEDHASHSETTEST_H__