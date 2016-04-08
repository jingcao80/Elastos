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
    int test_ConstructorLjava_util_Map(int argc, char* argv[]);
    int test_clear(int argc, char* argv[]);
    int test_clone(int argc, char* argv[]);
    int test_containsKeyLjava_lang_Object(int argc, char* argv[]);
    int test_containsValueLjava_lang_Object(int argc, char* argv[]);
    int test_entrySet(int argc, char* argv[]);
    int test_getLjava_lang_Object(int argc, char* argv[]);
    int test_isEmpty(int argc, char* argv[]);
    int test_keySet(int argc, char* argv[]);
    int test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);
    int test_putAllLjava_util_Map(int argc, char* argv[]);
    int test_removeLjava_lang_Object(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_equalsLjava_lang_Object(int argc, char* argv[]);
    int test_values(int argc, char* argv[]);
    int test_Serialization(int argc, char* argv[]);
    int test_rehash(int argc, char* argv[]);

private:
    void setUp();
    void tearDown();

private:
    static String ID;

    // class MockMap extends AbstractMap {
    //     public Set entrySet() {
    //         return null;
    //     }
    //     public int size(){
    //         return 0;
    //     }
    // };
    /*
     * TODO: change all the statements testing the keys and values with equals()
     * method to check for reference equality instead
     */

    AutoPtr<IMap> hm;

    static int hmSize;

    AutoPtr< ArrayOf<IInterface*> > objArray;

    AutoPtr< ArrayOf<IInterface*> > objArray2;

    // private static final SerializationTest.SerializableAssert comparator = new
    //                          SerializationTest.SerializableAssert() {

    //     public void assertDeserialized(Serializable initial, Serializable deserialized) {
    //         IdentityHashMap<String, String> initialMap = (IdentityHashMap<String, String>) initial;
    //         IdentityHashMap<String, String> deseriaMap = (IdentityHashMap<String, String>) deserialized;
    //         assertEquals("should be equal", initialMap.size(), deseriaMap.size());
    //     }

    // };
};

#endif // __HASHTABLETEST_H__