
#include "test.h"
#include "elastos/StringBuilder.h"

using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IEntry;
using Elastos::Utility::Arrays;

//====================================================================
// CTest::MockMap::
//====================================================================

// ECode CTest::MockMap::EntrySet(
//     /* [out, callee] */ ISet** entries)
// {
//     VALIDATE_NOT_NULL(entries)
//     *entries = NULL;
//     REFCOUNT_ADD(*entries)
//     return NOERROR;
// }

// ECode CTest::MockMap::GetSize(
//     /* [out] */ Int32* size)
// {
//     *size = 0;
//     return NOERROR;
// }

//====================================================================
// CTest::Support_MapTest2::
//====================================================================

CTest::Support_MapTest2::Support_MapTest2(
    /* [in] */ IMap* m)
{
    CTest* mOwner = new CTest();
    map = m;
    Boolean b;
    map->IsEmpty(&b);
    if (!b) {
//        fail("Map must be empty");
        assert(0);
    }
}

void CTest::Support_MapTest2::runTest()
{
    AutoPtr<IInterface> ret;
    AutoPtr<IStringBuilder> k1 = new StringBuilder("one");
    AutoPtr<IStringBuilder> v1 = new StringBuilder("1");
    map->Put(k1, v1, (IInterface**)&ret);

    Int32 size;
    map->GetSize(&size);
    assert(1 == size);
    map->Clear();
    map->GetSize(&size);
    assert(0 == size);

    AutoPtr<ISet> st;
    map->EntrySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean b;
    it->HasNext(&b);
    assert(b == FALSE);

    AutoPtr<ISet> kt;
    map->KeySet((ISet**)&kt);
    AutoPtr<IIterator> it2;
    kt->GetIterator((IIterator**)&it2);
    Boolean b2;
    it2->HasNext(&b2);
    assert(b2 == FALSE);

    AutoPtr<ICollection> val;
    map->Values((ICollection**)&val);
    AutoPtr<IIterator> it3;
    val->GetIterator((IIterator**)&it3);
    Boolean b3;
    it3->HasNext(&b3);
    assert(b3 == FALSE);

    map->Put(k1, v1, (IInterface**)&ret);
    map->GetSize(&size);
    assert(1 == size);
    AutoPtr<IInterface> res;
    map->Remove(k1, (IInterface**)&res);
    map->GetSize(&size);
    assert(0 == size);

    AutoPtr<ISet> st2;
    map->EntrySet((ISet**)&st2);
    AutoPtr<IIterator> it4;
    st2->GetIterator((IIterator**)&it4);
    Boolean b4;
    it4->HasNext(&b4);
    assert(b4 == FALSE);

    AutoPtr<ISet> kt2;
    map->KeySet((ISet**)&kt2);
    AutoPtr<IIterator> it5;
    kt2->GetIterator((IIterator**)&it5);
    Boolean b5;
    it5->HasNext(&b5);
    assert(b5 == FALSE);

    AutoPtr<ICollection> val2;
    map->Values((ICollection**)&val2);
    AutoPtr<IIterator> it6;
    val2->GetIterator((IIterator**)&it6);
    Boolean b6;
    it6->HasNext(&b6);
    assert(b6 == FALSE);
}

//====================================================================
// CTest::
//====================================================================

CTest::CTest()
{
    mKeyArray = ArrayOf<IInterface*>::Alloc(100);
    mValueArray = ArrayOf<IInterface*>::Alloc(100);
    setUp();
}

CTest::~CTest()
{
    tearDown();
}

// test 1
int CTest::test_Constructor(int argc, char* argv[])
{
    // Test for method java.util.WeakHashMap()
    AutoPtr<IMap> p;
    CWeakHashMap::New((IMap**)&p);
    Support_MapTest2* map2 = new Support_MapTest2(p);
    map2->runTest();

    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> res;
        mWhm->Get((*mKeyArray)[i], (IInterface**)&res);
        assert(res.Get() == (*mValueArray)[i]);
    }
    return 0;
}

// test 2
int CTest::test_ConstructorI(int argc, char* argv[])
{
    // Test for method java.util.WeakHashMap(int)
    mWhm = NULL;
    CWeakHashMap::New(50, (IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> res;
        mWhm->Get((*mKeyArray)[i], (IInterface**)&res);
        assert(res.Get() == (*mValueArray)[i]);
    }

    AutoPtr<IMap> empty;
    CWeakHashMap::New(0, (IMap**)&empty);
    AutoPtr<IStringBuilder> k = new StringBuilder("nothing");
    AutoPtr<IInterface> value;
    empty->Get(k, (IInterface**)&value);
    assert(value == NULL);
    AutoPtr<IStringBuilder> k1 = new StringBuilder("something");
    AutoPtr<IStringBuilder> v1 = new StringBuilder("here");
    AutoPtr<IInterface> o;
    empty->Put(k1, v1, (IInterface**)&o);
    AutoPtr<IInterface> val;
    empty->Get(k1, (IInterface**)&val);
    assert(val.Get() == v1.Get());

    AutoPtr<IMap> obj;
    ECode ec = CWeakHashMap::New(-50, (IMap**)&obj);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
    return 0;
}

// test 3
int CTest::test_ConstructorIF(int argc, char* argv[])
{
    // Test for method java.util.WeakHashMap(int, float)
    mWhm = NULL;
    CWeakHashMap::New(50, 0.5f, (IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> res;
        mWhm->Get((*mKeyArray)[i], (IInterface**)&res);
        assert(res.Get() == (*mValueArray)[i]);
    }

    AutoPtr<IMap> empty;
    CWeakHashMap::New(0, 0.75f, (IMap**)&empty);
    AutoPtr<IStringBuilder> k = new StringBuilder("nothing");
    AutoPtr<IInterface> value;
    empty->Get(k, (IInterface**)&value);
    assert(value == NULL);
    AutoPtr<IStringBuilder> k1 = new StringBuilder("something");
    AutoPtr<IStringBuilder> v1 = new StringBuilder("here");
    AutoPtr<IInterface> o;
    empty->Put(k1, v1, (IInterface**)&o);
    AutoPtr<IInterface> val;
    empty->Get(k1, (IInterface**)&val);
    assert(val.Get() == v1.Get());

    AutoPtr<IMap> obj1;
    ECode ec = CWeakHashMap::New(50, -0.5f, (IMap**)&obj1);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    AutoPtr<IMap> obj2;
    ec = CWeakHashMap::New(-50, 0.5f, (IMap**)&obj2);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
    return 0;
}

// test 4
int CTest::test_ConstructorLjava_util_Map(int argc, char* argv[])
{
    AutoPtr<IMap> mockMap;// = new MockMap();
    AutoPtr<IMap> map;
    CWeakHashMap::New(mockMap, (IMap**)&map);
    Int32 size;
    map->GetSize(&size);
    assert(0 == size);

    AutoPtr<IMap> p = NULL;
    AutoPtr<IMap> obj;
    ECode ec = CWeakHashMap::New(p, (IMap**)&obj);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return 0;
}

// test 5
int CTest::test_clear(int argc, char* argv[])
{
    // Test for method boolean java.util.WeakHashMap.clear()
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }
    mWhm->Clear();
    Boolean b;
    mWhm->IsEmpty(&b);
    assert(b == TRUE);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> res;
        mWhm->Get((*mKeyArray)[i], (IInterface**)&res);
        assert(res.Get() == NULL);
    }
    return 0;
}

// test 6
int CTest::test_containsKeyLjava_lang_Object(int argc, char* argv[])
{
    // Test for method boolean java.util.WeakHashMap.containsKey()
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }
    for (int i = 0; i < 100; i++) {
        Boolean ret;
        mWhm->ContainsKey((*mKeyArray)[i], &ret);
        assert(ret == TRUE);
    }
    mKeyArray->Set(25, NULL);
    mKeyArray->Set(50, NULL);
    return 0;
}

// test 7
int CTest::test_containsValueLjava_lang_Object(int argc, char* argv[])
{
    // Test for method boolean java.util.WeakHashMap.containsValue()
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }
    for (int i = 0; i < 100; i++) {
        Boolean res;
        mWhm->ContainsValue((*mValueArray)[i], &res);
        assert(res == TRUE);
    }
    mKeyArray->Set(25, NULL);
    mKeyArray->Set(50, NULL);
    return 0;
}

// test 8
int CTest::test_entrySet(int argc, char* argv[])
{
    // Test for method java.util.Set java.util.WeakHashMap.entrySet()
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }
    AutoPtr<IList> keys;
    AutoPtr<IList> values;
    Arrays::AsList(mKeyArray, (IList**)&keys);
    Arrays::AsList(mValueArray, (IList**)&values);
    AutoPtr<ISet> entrySet;
    mWhm->EntrySet((ISet**)&entrySet);
    Int32 size;
    entrySet->GetSize(&size);
    assert(size == 100);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> obj;
        it->Next((IInterface**)&obj);
        AutoPtr<IEntry> entry = IEntry::Probe(obj);
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        Boolean bKey;
        keys->Contains(key, &bKey);
        assert(bKey == TRUE);
        AutoPtr<IInterface> val;
        entry->GetValue((IInterface**)&val);
        Boolean bVal;
        values->Contains(val, &bVal);
        assert(bVal == TRUE);
    }
    keys = NULL;
    values = NULL;
    mKeyArray->Set(50, NULL);

//     int count = 0;
//     do {
// //        System.gc();
// //        System.gc();
// //        FinalizationTester.induceFinalization();
//         count++;
//     } while (count <= 5 && (entrySet->GetSize(&size), size == 100));

//     entrySet->GetSize(&size);
//     assert(size == 99);
    return 0;
}

// test 9
int CTest::test_isEmpty(int argc, char* argv[])
{
    // Test for method boolean java.util.WeakHashMap.isEmpty()
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    Boolean b;
    mWhm->IsEmpty(&b);
    assert(b == TRUE);
    AutoPtr<IStringBuilder> myObject = new StringBuilder();
    AutoPtr<IInterface> ret;
    mWhm->Put(myObject, myObject, (IInterface**)&ret);
    mWhm->IsEmpty(&b);
    assert(b == FALSE);
    AutoPtr<IInterface> res;
    mWhm->Remove(myObject, (IInterface**)&res);
    mWhm->IsEmpty(&b);
    assert(b == TRUE);
    return 0;
}

// test 10
int CTest::test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[])
{
    // Test for method java.lang.Object
    // java.util.WeakHashMap.put(java.lang.Object, java.lang.Object)
    AutoPtr<IMap> map;
    CWeakHashMap::New((IMap**)&map);
    AutoPtr<IStringBuilder> val = new StringBuilder("value");
    AutoPtr<IInterface> ret;
    map->Put(NULL, val, (IInterface**)&ret); // add null key
//    System.gc();
//    System.gc();
//    FinalizationTester.induceFinalization();
    AutoPtr<IStringBuilder> key = new StringBuilder("nothing");
    ret = NULL;
    map->Remove(key, (IInterface**)&ret); // Cause objects in queue to be removed
    Int32 size;
    map->GetSize(&size);
    assert(1 == size);
    return 0;
}

// test 11
int CTest::test_putAllLjava_util_Map(int argc, char* argv[])
{
    AutoPtr<IMap> mockMap;// = new MockMap();
    AutoPtr<IMap> map;
    CWeakHashMap::New((IMap**)&map);
    map->PutAll(mockMap);
    Int32 size;
    map->GetSize(&size);
    assert(0 == size);

    ECode ec = map->PutAll(NULL);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return 0;
}

// test 12
int CTest::test_removeLjava_lang_Object(int argc, char* argv[])
{
    // Test for method java.lang.Object
    // java.util.WeakHashMap.remove(java.lang.Object)
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }

    AutoPtr<IInterface> ret;
    mWhm->Remove((*mKeyArray)[25], (IInterface**)&ret);
    assert(ret.Get() == (*mValueArray)[25]);
    ret = NULL;
    mWhm->Remove((*mKeyArray)[25], (IInterface**)&ret);
    assert(ret == NULL);
    Int32 size;
    mWhm->GetSize(&size);
    assert(99 == size);
    return 0;
}

// test 13
int CTest::test_size(int argc, char* argv[])
{
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    Int32 size;
    mWhm->GetSize(&size);
    assert(0 == size);
    return 0;
}

// test 14
int CTest::test_keySet(int argc, char* argv[])
{
    // Test for method java.util.Set java.util.WeakHashMap.keySet()
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }

    AutoPtr<IList> keys;
    AutoPtr<IList> values;

    Arrays::AsList(mKeyArray, (IList**)&keys);
    Arrays::AsList(mValueArray, (IList**)&values);
    AutoPtr<ISet> keySet;
    mWhm->KeySet((ISet**)&keySet);
    Int32 size;
    keySet->GetSize(&size);
    assert(100 == size);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> key;
        it->Next((IInterface**)&key);
        Boolean res;
        keys->Contains(key, &res);
        assert(res == TRUE);
    }
    keys = NULL;
    values = NULL;
    mKeyArray->Set(50, NULL);

//     int count = 0;
//     do {
// //        System.gc();
// //        System.gc();
// //        FinalizationTester.induceFinalization();
//         count++;
//     } while (count <= 5 && (keySet->GetSize(&size), size == 100));

//     keySet->GetSize(&size);
//     assert(99 == size);
    return 0;
}

// test 15
int CTest::test_values(int argc, char* argv[])
{
    // Test for method java.util.Set java.util.WeakHashMap.values()
    mWhm = NULL;
    CWeakHashMap::New((IMap**)&mWhm);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInterface> ret;
        mWhm->Put((*mKeyArray)[i], (*mValueArray)[i], (IInterface**)&ret);
    }

    AutoPtr<IList> keys;
    AutoPtr<IList> values;

    Arrays::AsList(mKeyArray, (IList**)&keys);
    Arrays::AsList(mValueArray, (IList**)&values);
    AutoPtr<ICollection> valuesCollection;
    mWhm->Values((ICollection**)&valuesCollection);
    Int32 size;
    valuesCollection->GetSize(&size);
    assert(100 == size);
    AutoPtr<IIterator> it;
    valuesCollection->GetIterator((IIterator**)&it);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> value;
        it->Next((IInterface**)&value);
        Boolean res;
        values->Contains(value, &res);
        assert(res == TRUE);
    }
    keys = NULL;
    values = NULL;
    mKeyArray->Set(50, NULL);

//     int count = 0;
//     do {
// //        System.gc();
// //        System.gc();
// //        FinalizationTester.induceFinalization();
//         count++;
//     } while (count <= 5 && (valuesCollection->GetSize(&size), size == 100));

//     valuesCollection->GetSize(&size);
//     assert(99 == size);
    return 0;
}

void CTest::setUp()
{
    for (int i = 0; i < 100; i++) {
        AutoPtr<IStringBuilder> key = new StringBuilder();
        mKeyArray->Set(i, key);
        AutoPtr<IStringBuilder> val = new StringBuilder();
        mValueArray->Set(i, val);
    }

}

void CTest::tearDown()
{
}