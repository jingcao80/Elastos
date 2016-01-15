#ifndef __HASHTABLETEST_H__
#define __HASHTABLETEST_H__

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
    int test_ConstructorLjava_util_Map(int argc, char* argv[]);
    int test_ConversionConstructorNullValue(int argc, char* argv[]);
    int test_clear(int argc, char* argv[]);
    int test_clone(int argc, char* argv[]);
    int test_containsLjava_lang_Object(int argc, char* argv[]);
    int test_containsKeyLjava_lang_Object(int argc, char* argv[]);
    int test_containsValueLjava_lang_Object(int argc, char* argv[]);
    int test_elements(int argc, char* argv[]);
    int test_entrySet(int argc, char* argv[]);
    int test_equalsLjava_lang_Object(int argc, char* argv[]);
    int test_getLjava_lang_Object(int argc, char* argv[]);
    int test_hashCode(int argc, char* argv[]);
    int test_isEmpty(int argc, char* argv[]);
    int test_keys(int argc, char* argv[]);
    int test_keys_subtest0(int argc, char* argv[]);
    int test_keySet(int argc, char* argv[]);
    int test_keySet_subtest0(int argc, char* argv[]);
    int test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);
    int test_putAllLjava_util_Map(int argc, char* argv[]);
    int test_removeLjava_lang_Object(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_toString(int argc, char* argv[]);
    int test_values(int argc, char* argv[]);
    int test_entrySet_remove(int argc, char* argv[]);
    int test_rehash(int argc, char* argv[]);

    // class Mock_Hashtable : public CHashTable
    // {
    // public:
    //     Mock_Hashtable(int i) {
    //         super(i);
    //     }

    //     @Override
    //     protected void rehash() {
    //         flag = true;
    //         super.rehash();
    //     }

    //     public boolean isRehashed() {
    //         return flag;
    //     }
    // public:
    //     boolean flag = false;
    // };

protected:
    AutoPtr<IDictionary> hashtableClone(AutoPtr<IDictionary> s);

private:
    void setUp();
    void tearDown();

private:
    AutoPtr<IDictionary> ht10;

    AutoPtr<IDictionary> ht100;

    AutoPtr<IDictionary> htfull;

    AutoPtr<IVector> keyVector;

    AutoPtr<IVector> elmVector;

    String h10sVal;
};

#endif // __HASHTABLETEST_H__