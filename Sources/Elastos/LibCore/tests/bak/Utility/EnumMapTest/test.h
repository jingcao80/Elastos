#ifndef __ENUMMAPTEST_H__
#define __ENUMMAPTEST_H__

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
    int test_ConstructorLjava_lang_Class(int argc, char* argv[]);
    int test_ConstructorLjava_util_EnumMap(int argc, char* argv[]);
    int test_ConstructorLjava_util_Map(int argc, char* argv[]);
    int test_clear(int argc, char* argv[]);
    int test_containsKeyLjava_lang_Object(int argc, char* argv[]);
    int test_clone(int argc, char* argv[]);
    int test_containsValueLjava_lang_Object(int argc, char* argv[]);
    int test_entrySet(int argc, char* argv[]);
    int test_equalsLjava_lang_Object(int argc, char* argv[]);
    int test_keySet(int argc, char* argv[]);
    int test_getLjava_lang_Object(int argc, char* argv[]);
    int test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);
    int test_putAllLjava_util_Map(int argc, char* argv[]);
    int test_removeLjava_lang_Object(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_values(int argc, char* argv[]);
    int testSerializationSelf(int argc, char* argv[]);
    int testSerializationCompatibility(int argc, char* argv[]);

private:
    void setUp();
    void tearDown();

private:
    enum Size {
        Small, Middle, Big,
    };

    enum Color {
        Red, Green, Blue,
    };

    enum Empty {
        //Empty
    };

    // private static class MockEntry<K, V> implements Map.Entry<K, V> {
    //     private K key;

    //     private V value;

    //     public MockEntry(K key, V value) {
    //         this.key   = key;
    //         this.value = value;
    //     }

    //     @Override
    //     public int hashCode() {
    //         return (key == null ? 0 : key.hashCode())
    //                 ^ (value == null ? 0 : value.hashCode());
    //     }

    //     public K getKey() {
    //         return key;
    //     }

    //     public V getValue() {
    //         return value;
    //     }

    //     public V setValue(V object) {
    //         V oldValue = value;
    //         value = object;
    //         return oldValue;
    //     }
    // };

};

#endif // __ENUMMAPTEST_H__