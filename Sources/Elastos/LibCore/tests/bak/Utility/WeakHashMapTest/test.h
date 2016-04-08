#ifndef __WEAKHASHMAP_TEST_H
#define __WEAKHASHMAP_TEST_H

#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Core;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::CWeakHashMap;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    class Support_MapTest2
    {
    public:
        Support_MapTest2(IMap* m);
        void runTest();

    public:
        AutoPtr<IMap> map;
    };

    // class MockMap
    //     : public IMap
    //     , public AbstractMap
    // {
    // public:
    //     CARAPI EntrySet(
    //         /* [out, callee] */ ISet** entries);
    //     CARAPI GetSize(
    //         /* [out] */ Int32* size);
    // };

public:
    CTest();
    ~CTest();

public:
    int test_Constructor(int argc, char* argv[]);

    int test_ConstructorI(int argc, char* argv[]);

    int test_ConstructorIF(int argc, char* argv[]);

    int test_ConstructorLjava_util_Map(int argc, char* argv[]);

    int test_clear(int argc, char* argv[]);

    int test_containsKeyLjava_lang_Object(int argc, char* argv[]);

    int test_containsValueLjava_lang_Object(int argc, char* argv[]);

    int test_entrySet(int argc, char* argv[]);

    int test_isEmpty(int argc, char* argv[]);

    int test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);

    int test_putAllLjava_util_Map(int argc, char* argv[]);

    int test_removeLjava_lang_Object(int argc, char* argv[]);

    int test_size(int argc, char* argv[]);

    int test_keySet(int argc, char* argv[]);

    int test_values(int argc, char* argv[]);

protected:
    void setUp();

    void tearDown();

public:
    AutoPtr< ArrayOf<IInterface*> > mKeyArray;

    AutoPtr< ArrayOf<IInterface*> > mValueArray;

    AutoPtr<IMap> mWhm;
};

#endif //__WEAKHASHMAP_TEST_H
