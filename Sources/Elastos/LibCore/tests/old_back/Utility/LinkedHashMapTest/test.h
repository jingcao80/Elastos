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
    int test_getLjava_lang_Object(int argc, char* argv[]);
    int test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);
    int test_putPresent(int argc, char* argv[]);
    int test_putAllLjava_util_Map(int argc, char* argv[]);
    int test_putAll_Ljava_util_Map_Null(int argc, char* argv[]);
    int test_entrySet(int argc, char* argv[]);
    int test_entrySetRemove(int argc, char* argv[]);
    int test_keySet(int argc, char* argv[]);
    int test_values(int argc, char* argv[]);
    int test_removeLjava_lang_Object(int argc, char* argv[]);
    int test_clear(int argc, char* argv[]);
    int test_clone(int argc, char* argv[]);
    int test_clone_ordered(int argc, char* argv[]);
    int test_clone_Mock(int argc, char* argv[]);
    int test_removeEldestFromSameBucketAsNewEntry(int argc, char* argv[]);
    int test_containsKeyLjava_lang_Object(int argc, char* argv[]);
    int test_containsValueLjava_lang_Object(int argc, char* argv[]);
    int test_isEmpty(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_ordered_entrySet(int argc, char* argv[]);
    int test_ordered_keySet(int argc, char* argv[]);
    int test_ordered_values(int argc, char* argv[]);
    int test_remove_eldest(int argc, char* argv[]);

private:
    void setUp();
    void tearDown();
    void entrySetRemoveHelper(String key, String value);

    // static final class CacheMap extends LinkedHashMap {
    //     protected boolean removeEldestEntry(Map.Entry e) {
    //         return size() > 5;
    //     }
    // };

    // private static class MockMapNull extends AbstractMap {
    //     @Override
    //     public Set entrySet() {
    //         return null;
    //     }

    //     @Override
    //     public int size() {
    //         return 10;
    //     }
    // };

    // class MockMap extends LinkedHashMap {
    //     int num;

    //     public Object put(Object k, Object v) {
    //         num++;
    //         return super.put(k, v);
    //     }

    //     protected boolean removeEldestEntry(Map.Entry e) {
    //         return num > 1;
    //     }
    // };

private:
    AutoPtr<IMap> hm;

    static int hmSize;

    AutoPtr< ArrayOf<IInterface*> > objArray;

    AutoPtr< ArrayOf<IInterface*> > objArray2;

};

#endif // __HASHTABLETEST_H__