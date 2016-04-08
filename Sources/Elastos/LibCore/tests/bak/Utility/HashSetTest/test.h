#ifndef __HASHSETTEST_H__
#define __HASHSETTEST_H__

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
    int test_Serialization(int argc, char* argv[]);

private:
    void setUp();
    void tearDown();
    // private static final SerializationTest.SerializableAssert comparator = new
    //                                    SerializationTest.SerializableAssert() {
    //     public void assertDeserialized(Serializable initial, Serializable deserialized) {
    //         HashSet<String> initialHs = (HashSet<String>) initial;
    //         HashSet<String> deseriaHs = (HashSet<String>) deserialized;
    //         assertEquals("should be equal", initialHs.size(), deseriaHs.size());
    //         assertEquals("should be equal", initialHs, deseriaHs);
    //     }

    // };
    AutoPtr<ISet> hs;

    AutoPtr< ArrayOf<IInterface*> > objArray;
};

#endif // __HASHSETTEST_H__