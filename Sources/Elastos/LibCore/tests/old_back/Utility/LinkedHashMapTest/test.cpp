#include "test.h"

int CTest::hmSize = 1000;

CTest::CTest()
{
    setUp();
}

CTest::~CTest()
{
    tearDown();
}

int CTest::test_Constructor(int argc, char* argv[])
{
    PEL("CTest::test_Constructor")
    // Test for method java.util.LinkedHashMap()
    // new Support_MapTest2(new LinkedHashMap()).runTest();

    AutoPtr<IMap> hm2;
    CLinkedHashMap::New((IMap**)&hm2);
    Int32 sizelen = 0;
    assert(0 == (hm2->GetSize(&sizelen), sizelen));
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    // Test for method java.util.LinkedHashMap(int)
    AutoPtr<IMap> hm2;
    CLinkedHashMap::New(5, (IMap**)&hm2);
    Int32 sizelen = 0;
    assert(0 == (hm2->GetSize(&sizelen), sizelen));
    // try {
    ECode ec = CLinkedHashMap::New(-1, (IMap**)&hm2);
    if (ec != NOERROR) {
        printf("Failed to throw IllegalArgumentException for initial capacity < 0\n");
    }

    // } catch (IllegalArgumentException e) {
    //     //expected
    // }

    AutoPtr<IMap> empty;
    CLinkedHashMap::New(0, (IMap**)&empty);
    AutoPtr<IInterface> outface;
    AutoPtr<ICharSequence> nothing;
    CString::New(String("nothing"), (ICharSequence**)&nothing);
    assert((empty->Get(nothing, (IInterface**)&outface), outface) == NULL);
    AutoPtr<ICharSequence> something;
    CString::New(String("something"), (ICharSequence**)&something);
    AutoPtr<ICharSequence> here;
    CString::New(String("here"), (ICharSequence**)&here);
    empty->Put(something, here, (IInterface**)&outface);
    empty->Get(something, (IInterface**)&outface);
    String str= Object::ToString(outface);
    assert(str == String("here"));
    return 0;
}

int CTest::test_ConstructorIF(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorIF")
    // Test for method java.util.LinkedHashMap(int, float)
    AutoPtr<IMap> hm2;
    CLinkedHashMap::New(5, (Float)0.5, (IMap**)&hm2);
    Int32 sizelen = 0;
    assert(0 == (hm2->GetSize(&sizelen), sizelen));
    // try {
    ECode ec = CLinkedHashMap::New(0, 0, (IMap**)&hm2);
    if (ec != NOERROR) {
        printf("Failed to throw IllegalArgumentException for initial load factor <= 0");
    }

    // } catch (IllegalArgumentException e) {
    //     //expected
    // }

    AutoPtr<IMap> empty;
    CLinkedHashMap::New(0, 0.75f, (IMap**)&empty);
    AutoPtr<IInterface> outface;
    AutoPtr<ICharSequence> nothing;
    CString::New(String("nothing"), (ICharSequence**)&nothing);
    AutoPtr<ICharSequence> something;
    CString::New(String("something"), (ICharSequence**)&something);
    AutoPtr<ICharSequence> here;
    CString::New(String("here"), (ICharSequence**)&here);
    assert((empty->Get(nothing, (IInterface**)&outface), outface) == NULL);
    empty->Put(something, here, (IInterface**)&outface);
    empty->Get(something, (IInterface**)&outface);
    String str = Object::ToString(outface);
    assert(str == String("here"));
    return 0;
}

int CTest::test_ConstructorLjava_util_Map(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_util_Map")
    // Test for method java.util.LinkedHashMap(java.util.Map)
    AutoPtr<IMap> myMap;
    AutoPtr<INavigableMap> nvmapy;
    CTreeMap::New((INavigableMap**)&nvmapy);
    myMap = IMap::Probe(nvmapy);
    AutoPtr<IInterface> outface;
    for (int counter = 0; counter < hmSize; counter++) {
        myMap->Put((*objArray2)[counter], (*objArray)[counter], (IInterface**)&outface);
    }
    AutoPtr<IMap> hm2;
    CLinkedHashMap::New(myMap, (IMap**)&hm2);
    for (int counter = 0; counter < hmSize; counter++) {
        AutoPtr<IInterface> hmface;
        AutoPtr<IInterface> hm2face;
        hm->Get((*objArray2)[counter], (IInterface**)&hmface);
        hm2->Get((*objArray2)[counter], (IInterface**)&hm2face);
        Boolean isflag = FALSE;
        assert((IObject::Probe(hmface)->Equals(hm2face, &isflag), isflag));
    }
    return 0;
}

int CTest::test_getLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_getLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.LinkedHashMap.get(java.lang.Object)
    AutoPtr<IInterface> outface;
    AutoPtr<ICharSequence> sqT;
    CString::New(String("T"), (ICharSequence**)&sqT);
    hm->Get(sqT, (IInterface**)&outface);
    assert(outface == NULL);
    AutoPtr<ICharSequence> HELLO;
    CString::New(String("HELLO"), (ICharSequence**)&HELLO);
    hm->Put(sqT, HELLO, (IInterface**)&outface);
    hm->Get(sqT, (IInterface**)&outface);
    String str = Object::ToString(outface);
    assert(String("HELLO") == str);

    AutoPtr<IMap> m;
    CLinkedHashMap::New((IMap**)&m);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    m->Put(NULL, test, (IInterface**)&outface);
    m->Get(NULL, (IInterface**)&outface);
    str = Object::ToString(outface);
    assert(String("test") == str);
    AutoPtr<IInteger32> int0;
    CInteger32::New(0, (IInteger32**)&int0);
    assert((m->Get(int0, (IInterface**)&outface), outface) == NULL);
    return 0;
}

int CTest::test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_putLjava_lang_ObjectLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.LinkedHashMap.put(java.lang.Object, java.lang.Object)
    AutoPtr<ICharSequence> KEY;
    CString::New(String("KEY"), (ICharSequence**)&KEY);
    AutoPtr<ICharSequence> VALUE;
    CString::New(String("VALUE"), (ICharSequence**)&VALUE);
    AutoPtr<IInterface> outface;
    hm->Put(KEY, VALUE, (IInterface**)&outface);
    String str;
    hm->Get(KEY, (IInterface**)&outface);
    str = Object::ToString(outface);
    assert(String("VALUE") == str);

    AutoPtr<IMap> m;
    CLinkedHashMap::New((IMap**)&m);
    AutoPtr<IInteger16> short0;
    CInteger16::New(0, (IInteger16**)&short0);
    AutoPtr<ICharSequence> sqshort;
    CString::New(String("short"), (ICharSequence**)&sqshort);
    m->Put(short0, sqshort, (IInterface**)&outface);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    m->Put(NULL, test, (IInterface**)&outface);
    AutoPtr<IInteger32> int0;
    CInteger32::New(0, (IInteger32**)&int0);
    AutoPtr<ICharSequence> sqint;
    CString::New(String("int"), (ICharSequence**)&sqint);
    m->Put(int0, sqint, (IInterface**)&outface);
    m->Get(short0, (IInterface**)&outface);
    str = Object::ToString(outface);
    assert(String("short") == str);
    m->Get(int0, (IInterface**)&outface);
    str = Object::ToString(outface);
    assert(String("int") == str);
    return 0;
}

int CTest::test_putPresent(int argc, char* argv[])
{
    PEL("CTest::test_putPresent")
    AutoPtr<IMap> m;
    CLinkedHashMap::New(8, 0.75f, true, (IMap**)&m);
    AutoPtr<ICharSequence> KEY;
    CString::New(String("KEY"), (ICharSequence**)&KEY);
    AutoPtr<ICharSequence> VALUE;
    CString::New(String("VALUE"), (ICharSequence**)&VALUE);
    AutoPtr<ICharSequence> WOMBAT;
    CString::New(String("WOMBAT"), (ICharSequence**)&WOMBAT);
    AutoPtr<ICharSequence> COMBAT;
    CString::New(String("COMBAT"), (ICharSequence**)&COMBAT);
    AutoPtr<IInterface> outface;
    m->Put(KEY, VALUE, (IInterface**)&outface);
    m->Put(WOMBAT, COMBAT, (IInterface**)&outface);
    m->Put(KEY, VALUE, (IInterface**)&outface);
    AutoPtr< ArrayOf<IInterface*> > outarr;
    AutoPtr<ISet> outset;
    m->EntrySet((ISet**)&outset);
    outset->ToArray((ArrayOf<IInterface*>**)&outarr);
    AutoPtr<IMapEntry> newest;
    for (int i = 0; i < outarr->GetLength(); i++) {
        newest = IMapEntry::Probe((*outarr)[i]);
    }
    newest->GetKey((IInterface**)&outface);
    String str = Object::ToString(outface);
    assert(String("KEY") == str);
    newest->GetValue((IInterface**)&outface);
    str = Object::ToString(outface);
    assert(String("VALUE") == str);
    return 0;
}

int CTest::test_putAllLjava_util_Map(int argc, char* argv[])
{
    PEL("CTest::test_putAllLjava_util_Map")
    // Test for method void java.util.LinkedHashMap.putAll(java.util.Map)
    AutoPtr<IMap> hm2;
    CLinkedHashMap::New((IMap**)&hm2);
    Boolean isflag = FALSE;
    hm2->PutAll(hm);
    for (int i = 0; i < 1000; i++) {
        AutoPtr<IInteger32> midint;
        CInteger32::New(i, (IInteger32**)&midint);
        AutoPtr<IInterface> outface;
        String str;
        midint->ToString(&str);
        AutoPtr<ICharSequence> midsq;
        CString::New(str, (ICharSequence**)&midsq);
        hm2->Get(midsq, (IInterface**)&outface);
        IObject::Probe(outface)->Equals(midint, &isflag);
        String str2;
        IObject::Probe(outface)->ToString(&str2);
        assert(isflag);
    }
    return 0;
}

int CTest::test_putAll_Ljava_util_Map_Null(int argc, char* argv[])
{
    PEL("CTest::test_putAll_Ljava_util_Map_Null")
    AutoPtr<IMap> linkedHashMap;
    CLinkedHashMap::New((IMap**)&linkedHashMap);
    // try {
    //     linkedHashMap.putAll(new MockMapNull());
    //     fail("Should throw NullPointerException");
    // } catch (NullPointerException e) {
    //     // expected.
    // }

    // try {
    //     linkedHashMap = new LinkedHashMap(new MockMapNull());
    //     fail("Should throw NullPointerException");
    // } catch (NullPointerException e) {
    //     // expected.
    // }
    return 0;
}

int CTest::test_entrySet(int argc, char* argv[])
{
    PEL("CTest::test_entrySet")
    // Test for method java.util.Set java.util.LinkedHashMap.entrySet()
    AutoPtr<ISet> s;
    hm->EntrySet((ISet**)&s);
    AutoPtr<IIterator> i;
    s->GetIterator((IIterator**)&i);
    Int32 len = 0;
    Int32 len1 = 0;
    hm->GetSize(&len);
    s->GetSize(&len1);
    assert(len == len1);
    Boolean isflag = FALSE;
    while (i->HasNext(&isflag), isflag) {
        AutoPtr<IMapEntry> m;
        AutoPtr<IInterface> outface;
        i->Next((IInterface**)&outface);
        m = IMapEntry::Probe(outface);
        AutoPtr<IInterface> keyface;
        m->GetKey((IInterface**)&keyface);
        AutoPtr<IInterface> valueface;
        m->GetValue((IInterface**)&valueface);
        Boolean isflag2 = FALSE;
        hm->ContainsKey(keyface, &isflag);
        hm->ContainsValue(valueface, &isflag2);
        assert(isflag && isflag2);
    }
    return 0;
}

int CTest::test_entrySetRemove(int argc, char* argv[])
{
    PEL("CTest::test_entrySetRemove")
    entrySetRemoveHelper(String("military"), String("intelligence"));
    entrySetRemoveHelper(String(NULL), String("hypothesis"));
    return 0;
}

int CTest::test_keySet(int argc, char* argv[])
{
    PEL("CTest::test_keySet")
    // Test for method java.util.Set java.util.LinkedHashMap.keySet()
    AutoPtr<ISet> s;
    hm->KeySet((ISet**)&s);
    Int32 len = 0;
    Int32 len1 = 0;
    assert((s->GetSize(&len), len) == (hm->GetSize(&len1), len1));
    Boolean isflag = FALSE;
    for (int i = 0; i < objArray2->GetLength(); i++) {
        assert((s->Contains((*objArray2)[i], &isflag), isflag));
    }

    AutoPtr<IMap> m;
    CLinkedHashMap::New((IMap**)&m);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    AutoPtr<IInterface> outface;
    m->Put(NULL, test, (IInterface**)&outface);
    AutoPtr<ISet> outsetm;
    m->KeySet((ISet**)&outsetm);
    assert((outsetm->Contains(NULL, &isflag), isflag));
    AutoPtr<IIterator> outit;
    outsetm->GetIterator((IIterator**)&outit);
    outit->Next((IInterface**)&outface);
    assert(outface == NULL);

    AutoPtr<IMap> map;
    CLinkedHashMap::New(101, (IMap**)&map);
    AutoPtr<IInteger32> int1;
    CInteger32::New(1, (IInteger32**)&int1);
    AutoPtr<IInteger32> int102;
    CInteger32::New(102, (IInteger32**)&int102);
    AutoPtr<IInteger32> int203;
    CInteger32::New(203, (IInteger32**)&int203);
    AutoPtr<ICharSequence> sq1;
    CString::New(String("1"), (ICharSequence**)&sq1);
    AutoPtr<ICharSequence> sq102;
    CString::New(String("102"), (ICharSequence**)&sq102);
    AutoPtr<ICharSequence> sq203;
    CString::New(String("203"), (ICharSequence**)&sq203);
    map->Put(int1, sq1, (IInterface**)&outface);
    map->Put(int102, sq102, (IInterface**)&outface);
    map->Put(int203, sq203, (IInterface**)&outface);
    AutoPtr<IIterator> it;
    outsetm = NULL;
    map->KeySet((ISet**)&outsetm);
    outsetm->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> remove1;
    it->Next((IInterface**)&remove1);
    it->HasNext(&isflag);
    it->Remove();
    AutoPtr<IInterface> remove2;
    it->Next((IInterface**)&remove2);
    it->Remove();
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    list->Add(int1, &isflag);
    list->Add(int102, &isflag);
    list->Add(int203, &isflag);
    list->Remove(remove1, &isflag);
    list->Remove(remove2, &isflag);
    AutoPtr<IInterface> outface2;
    it->Next((IInterface**)&outface);
    list->Get(0, (IInterface**)&outface2);
    assert((Object::Equals(outface, outface2)));
    map->GetSize(&len);
    assert(1 == len);
    outsetm = NULL;
    map->KeySet((ISet**)&outsetm);
    outsetm->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
    assert((Object::Equals(outface, outface2)));

    AutoPtr<IMap> map2;
    CLinkedHashMap::New(101, (IMap**)&map2);
    AutoPtr<IInteger32> int4;
    CInteger32::New(4, (IInteger32**)&int4);
    AutoPtr<ICharSequence> sq4;
    CString::New(String("4"), (ICharSequence**)&sq4);
    map2->Put(int1, sq1, (IInterface**)&outface);
    map2->Put(int4, sq4, (IInterface**)&outface);
    AutoPtr<IIterator> it2;
    outsetm = NULL;
    map2->KeySet((ISet**)&outsetm);
    outsetm->GetIterator((IIterator**)&it2);
    AutoPtr<IInterface> remove3;
    it2->Next((IInterface**)&remove3);
    AutoPtr<IInteger32> next;
    IInteger32::Probe(remove3)->GetValue(&len);
    if (len == 1)
        CInteger32::New(4, (IInteger32**)&next);
    else
        CInteger32::New(1, (IInteger32**)&next);
    it2->HasNext(&isflag);
    it2->Remove();
    it2->Next((IInterface**)&outface);
    assert((Object::Equals(outface, next)));
    map2->GetSize(&len);
    assert(1 == len);
    outsetm = NULL;
    map2->KeySet((ISet**)&outsetm);
    outsetm->GetIterator((IIterator**)&it2);
    it2->Next((IInterface**)&outface);
    assert((Object::Equals(outface, next)));
    return 0;
}

int CTest::test_values(int argc, char* argv[])
{
    PEL("CTest::test_values")
    // Test for method java.util.Collection java.util.LinkedHashMap.values()
    AutoPtr<ICollection> c;
    hm->Values((ICollection**)&c);
    Int32 len = 0;
    Int32 len1 = 0;
    c->GetSize(&len);
    hm->GetSize(&len1);
    assert(len1 == len);
    Boolean isflag = FALSE;
    for (int i = 0; i < objArray->GetLength(); i++) {
        assert((c->Contains((*objArray)[i], &isflag), isflag));
    }

    AutoPtr<IMap> myLinkedHashMap;
    CLinkedHashMap::New((IMap**)&myLinkedHashMap);
    AutoPtr<IInterface> outface;
    for (int i = 0; i < 100; i++) {
        myLinkedHashMap->Put((*objArray2)[i], (*objArray)[i], (IInterface**)&outface);
    }
    AutoPtr<ICollection> values;
    myLinkedHashMap->Values((ICollection**)&values);
    // new Support_UnmodifiableCollectionTest(
    //         "Test Returned Collection From LinkedHashMap.values()", values)
    //         .runTest();
    AutoPtr<IInteger32> int0;
    CInteger32::New(0, (IInteger32**)&int0);
    values->Remove(int0, &isflag);
    assert(!(myLinkedHashMap->ContainsValue(int0, &isflag), isflag));
    return 0;
}

int CTest::test_removeLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_removeLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.LinkedHashMap.remove(java.lang.Object)
    int size = 0;
    hm->GetSize(&size);
    AutoPtr<IInteger32> y;
    CInteger32::New(9, (IInteger32**)&y);
    AutoPtr<IInterface> outface;
    String str;
    y->ToString(&str);
    AutoPtr<ICharSequence> sqy;
    CString::New(str, (ICharSequence**)&sqy);
    hm->Remove(sqy, (IInterface**)&outface);
    AutoPtr<IInteger32> x = IInteger32::Probe(outface);
    Boolean isflag = FALSE;
    assert((x->Equals(y, &isflag), isflag));
    hm->Get(y, (IInterface**)&outface);
    assert(outface == NULL);
    int len = 0;
    hm->GetSize(&len);
    assert(len == (size - 1));
    AutoPtr<ICharSequence> LCLCLC;
    CString::New(String("LCLCLC"), (ICharSequence**)&LCLCLC);
    assert((hm->Remove(LCLCLC, (IInterface**)&outface), outface) == NULL);

    AutoPtr<IMap> m;
    CLinkedHashMap::New((IMap**)&m);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    m->Put(NULL, test, (IInterface**)&outface);
    AutoPtr<IInteger32> int0;
    CInteger32::New(0, (IInteger32**)&int0);
    assert((m->Remove(int0, (IInterface**)&outface), outface) == NULL);
    m->Remove(NULL, (IInterface**)&outface);
    str = Object::ToString(outface);
    assert(String("test") == str);
    return 0;
}

int CTest::test_clear(int argc, char* argv[])
{
    PEL("CTest::test_clear")
    // Test for method void java.util.LinkedHashMap.clear()
    hm->Clear();
    Int32 len = 0;
    hm->GetSize(&len);
    assert(0 == len);
    AutoPtr<IInterface> outface;
    for (int i = 0; i < hmSize; i++)
        assert((hm->Get((*objArray2)[i], (IInterface**)&outface), outface) == NULL);
    return 0;
}

int CTest::test_clone(int argc, char* argv[])
{
    PEL("CTest::test_clone")
    // Test for method java.lang.Object java.util.LinkedHashMap.clone()
    AutoPtr<IMap> hm2;
    AutoPtr<IInterface> outface;
    ICloneable::Probe(hm)->Clone((IInterface**)&outface);
    hm2 = IMap::Probe(outface);
    assert(hm2 != hm);
    AutoPtr<IInterface> outface2;
    for (int counter = 0; counter < hmSize; counter++) {
        hm->Get((*objArray2)[counter], (IInterface**)&outface);
        hm2->Get((*objArray2)[counter], (IInterface**)&outface2);
        assert(outface == outface2);
    }

    AutoPtr<IMap> map;
    CLinkedHashMap::New((IMap**)&map);
    AutoPtr<ICharSequence> key;
    CString::New(String("key"), (ICharSequence**)&key);
    AutoPtr<ICharSequence> value;
    CString::New(String("value"), (ICharSequence**)&value);
    map->Put(key, value, (IInterface**)&outface);
    // get the keySet() and values() on the original Map
    AutoPtr<ISet> keys;
    map->KeySet((ISet**)&keys);
    AutoPtr<ICollection> values;
    map->Values((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
    String str = Object::ToString(outface);
PFL_EX("value == %s", str.string())
    assert(String("value") == str);
PFL
    keys->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
PFL
    str = Object::ToString(outface);
    assert(String("key") == str);
    AutoPtr<IMap> map2;
    ICloneable::Probe(map)->Clone((IInterface**)&outface);
PFL
    map2 = IMap::Probe(outface);
    AutoPtr<ICharSequence> value2;
    CString::New(String("value2"), (ICharSequence**)&value2);
    map2->Put(key, value2, (IInterface**)&outface);
    AutoPtr<ICollection> values2;
    map2->Values((ICollection**)&values2);
    assert(values2 != values);
    values2->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
    str = Object::ToString(outface);
    // values() and keySet() on the cloned() map should be different
    assert(String("value2") == str);
    map2->Clear();
PFL
    AutoPtr<ICharSequence> key2;
    CString::New(String("key2"), (ICharSequence**)&key2);
    AutoPtr<ICharSequence> value3;
    CString::New(String("value3"), (ICharSequence**)&value3);
    map2->Put(key2, value3, (IInterface**)&outface);
    AutoPtr<ISet> keys2;
    map2->KeySet((ISet**)&keys2);
PFL
    assert(keys2 != keys);
    keys2->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
    str = Object::ToString(outface);
    assert(String("key2") == str);
    return 0;
}

int CTest::test_clone_ordered(int argc, char* argv[])
{
    PEL("CTest::test_clone_ordered")
    // Test for method java.lang.Object java.util.LinkedHashMap.clone()
    AutoPtr<IMap> hm1;
    CLinkedHashMap::New(10, 0.75f, true, (IMap**)&hm1);
    AutoPtr<ICharSequence> sqa;
    CString::New(String("a"), (ICharSequence**)&sqa);
    AutoPtr<ICharSequence> sqb;
    CString::New(String("b"), (ICharSequence**)&sqb);
    AutoPtr<ICharSequence> sqc;
    CString::New(String("c"), (ICharSequence**)&sqc);
    AutoPtr<IInterface> outface;
    hm1->Put(sqa, sqa, (IInterface**)&outface);
    hm1->Put(sqb, sqb, (IInterface**)&outface);
    hm1->Put(sqc, sqc, (IInterface**)&outface);
    AutoPtr<IMap> hm2;
    ICloneable::Probe(hm1)->Clone((IInterface**)&outface);
    hm2 = IMap::Probe(outface);
    hm1->Get(sqa, (IInterface**)&outface);

    AutoPtr< ArrayOf<IMapEntry> > set = ArrayOf<IMapEntry>::Alloc(3);
    AutoPtr<IIterator> iterator;
    AutoPtr<ISet> hm1set;
    hm1->EntrySet((ISet**)&hm1set);
    hm1set->GetIterator((IIterator**)&iterator);
    iterator->Next((IInterface**)&outface);
    AutoPtr<IMapEntry> outent = IMapEntry::Probe(outface);
    AutoPtr<IInterface> keyface;
    outent->GetKey((IInterface**)&keyface);
    String str;
    ICharSequence::Probe(keyface)->ToString(&str);
    assert(String("b") == str);

    iterator->Next((IInterface**)&outface);
    outent = IMapEntry::Probe(outface);
    outent->GetKey((IInterface**)&keyface);
    ICharSequence::Probe(keyface)->ToString(&str);
    assert(String("c") == str);

    iterator->Next((IInterface**)&outface);
    outent = IMapEntry::Probe(outface);
    outent->GetKey((IInterface**)&keyface);
    ICharSequence::Probe(keyface)->ToString(&str);
    assert(String("a") == str);

    hm2->EntrySet((ISet**)&hm1set);
    hm1set->GetIterator((IIterator**)&iterator);
    iterator->Next((IInterface**)&outface);
    outent = IMapEntry::Probe(outface);
    outent->GetKey((IInterface**)&keyface);
    ICharSequence::Probe(keyface)->ToString(&str);
    assert(String("a") == str);

    iterator->Next((IInterface**)&outface);
    outent = IMapEntry::Probe(outface);
    outent->GetKey((IInterface**)&keyface);
    ICharSequence::Probe(keyface)->ToString(&str);
    assert(String("b") == str);

    iterator->Next((IInterface**)&outface);
    outent = IMapEntry::Probe(outface);
    outent->GetKey((IInterface**)&keyface);
    ICharSequence::Probe(keyface)->ToString(&str);
    assert(String("c") == str);
    return 0;
}

int CTest::test_clone_Mock(int argc, char* argv[])
{
    PEL("CTest::test_clone_Mock")
    assert(0 && "TODO");
    // LinkedHashMap hashMap = new MockMap();
    // String value = "value a";
    // hashMap.put("key", value);
    // MockMap cloneMap = (MockMap) hashMap.clone();
    // assertEquals(value, cloneMap.get("key"));
    // assertEquals(hashMap, cloneMap);
    // assertEquals(1, cloneMap.num);

    // hashMap.put("key", "value b");
    // assertFalse(hashMap.equals(cloneMap));
    return 0;
}

int CTest::test_removeEldestFromSameBucketAsNewEntry(int argc, char* argv[])
{
    PEL("CTest::test_removeEldestFromSameBucketAsNewEntry")
    assert(0 && "TODO");
    // LinkedHashMap<String, String> map
    //         = new LinkedHashMap<String, String>(6, 0.75F, true) {
    //     @Override
    //     protected boolean removeEldestEntry(Entry<String, String> e) {
    //         return true;
    //     }
    // };
    // map.put("N", "E");
    // map.put("F", "I");
    // assertEquals(null, map.get("N"));
    return 0;
}

int CTest::test_containsKeyLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsKeyLjava_lang_Object")
    // Test for method boolean
    // java.util.LinkedHashMap.containsKey(java.lang.Object)
    AutoPtr<IInteger32> int876;
    CInteger32::New(876, (IInteger32**)&int876);
    String str;
    int876->ToString(&str);
    AutoPtr<ICharSequence> sq876;
    CString::New(str, (ICharSequence**)&sq876);
    Boolean isflag = FALSE;
    assert((hm->ContainsKey(sq876, &isflag), isflag));
    AutoPtr<ICharSequence> KKDKDKD;
    CString::New(String("KKDKDKD"), (ICharSequence**)&KKDKDKD);
    assert(!(hm->ContainsKey(KKDKDKD, &isflag), isflag));

    AutoPtr<IMap> m;
    CLinkedHashMap::New((IMap**)&m);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    AutoPtr<IInterface> outface;
    m->Put(NULL, test, (IInterface**)&outface);
    assert((m->ContainsKey(NULL, &isflag), isflag));
    AutoPtr<IInteger32> int0;
    CInteger32::New(0, (IInteger32**)&int0);
    assert(!(m->ContainsKey(int0, &isflag), isflag));
    return 0;
}

int CTest::test_containsValueLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsValueLjava_lang_Object")
    // Test for method boolean
    // java.util.LinkedHashMap.containsValue(java.lang.Object)
    AutoPtr<IInteger32> int875;
    CInteger32::New(875, (IInteger32**)&int875);
    Boolean isflag = FALSE;
    assert((hm->ContainsValue(int875, &isflag), isflag));
    AutoPtr<IInteger32> int_9;
    CInteger32::New(-9, (IInteger32**)&int_9);
    assert(!(hm->ContainsValue(int_9, &isflag), isflag));
    return 0;
}

int CTest::test_isEmpty(int argc, char* argv[])
{
    PEL("CTest::test_isEmpty")
    // Test for method boolean java.util.LinkedHashMap.isEmpty()
    AutoPtr<IMap> empmap;
    CLinkedHashMap::New((IMap**)&empmap);
    Boolean isflag = FALSE;
    assert((empmap->IsEmpty(&isflag), isflag));
    assert(!(hm->IsEmpty(&isflag), isflag));
    return 0;
}

int CTest::test_size(int argc, char* argv[])
{
    PEL("CTest::test_size")
    // Test for method int java.util.LinkedHashMap.size()
    Int32 len = 0;
    hm->GetSize(&len);
    assert(len == (objArray->GetLength() + 2));
    return 0;
}

int CTest::test_ordered_entrySet(int argc, char* argv[])
{
    PEL("CTest::test_ordered_entrySet")
    int i = 0;
    int sz = 100;
    AutoPtr<IMap> lhm;
    CLinkedHashMap::New((IMap**)&lhm);
    AutoPtr<IInterface> outface;
    for (i = 0; i < sz; i++) {
        AutoPtr<IInteger32> ii;
        CInteger32::New(i, (IInteger32**)&ii);
        String str;
        ii->ToString(&str);
        AutoPtr<ICharSequence> sqi;
        CString::New(str, (ICharSequence**)&sqi);
        lhm->Put(ii, sqi, (IInterface**)&outface);
    }

    AutoPtr<ISet> s1;
    lhm->EntrySet((ISet**)&s1);
    AutoPtr<IIterator> it1;
    s1->GetIterator((IIterator**)&it1);
    int len = 0;
    int len1 = 0;
    lhm->GetSize(&len);
    s1->GetSize(&len1);
    assert(len1 == len);
    Boolean isflag = FALSE;
    for (i = 0; it1->HasNext(&isflag), isflag; i++) {
        AutoPtr<IMapEntry> m;
        it1->Next((IInterface**)&outface);
        m = IMapEntry::Probe(outface);
        AutoPtr<IInteger32> jj;
        m->GetKey((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i);
    }

    AutoPtr<IMap> lruhm;
    CLinkedHashMap::New(200, .75f, true, (IMap**)&lruhm);
    for (i = 0; i < sz; i++) {
        AutoPtr<IInteger32> ii;
        CInteger32::New(i, (IInteger32**)&ii);
        String str;
        ii->ToString(&str);
        AutoPtr<ICharSequence> sqi;
        CString::New(str, (ICharSequence**)&sqi);
        lruhm->Put(ii, sqi, (IInterface**)&outface);
    }

    AutoPtr<ISet> s3;
    lruhm->EntrySet((ISet**)&s3);
    AutoPtr<IIterator> it3;
    s3->GetIterator((IIterator**)&it3);
    assert((lruhm->GetSize(&len), len) == (s3->GetSize(&len1), len1));
    for (i = 0; i < sz && (it3->HasNext(&isflag), isflag); i++) {
        AutoPtr<IMapEntry> m;
        it3->Next((IInterface**)&outface);
        m = IMapEntry::Probe(outface);
        AutoPtr<IInteger32> jj;
        m->GetKey((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        Int32 outvalue = 0;
        jj->GetValue(&outvalue);
        assert(outvalue == i);
    }

    /* fetch the even numbered entries to affect traversal order */
    int p = 0;
    for (i = 0; i < sz; i += 2) {
        AutoPtr<IInteger32> inti;
        CInteger32::New(i, (IInteger32**)&inti);
        lruhm->Get(inti, (IInterface**)&outface);
        String ii = Object::ToString(outface);
        p = p + StringUtils::ParseInt32(ii);
    }
    assert(2450 == p);

    AutoPtr<ISet> s2;
    lruhm->EntrySet((ISet**)&s2);
    AutoPtr<IIterator> it2;
    s2->GetIterator((IIterator**)&it2);
    assert((lruhm->GetSize(&len), len) == (s2->GetSize(&len1), len1));
    for (i = 1; i < sz && (it2->HasNext(&isflag), isflag); i += 2) {
        AutoPtr<IMapEntry> m;
        it2->Next((IInterface**)&outface);
        m = IMapEntry::Probe(outface);
        AutoPtr<IInteger32> jj;
        m->GetKey((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i);
    }
    for (i = 0; i < sz && (it2->HasNext(&isflag), isflag); i += 2) {
        AutoPtr<IMapEntry> m;
        it2->Next((IInterface**)&outface);
        m = IMapEntry::Probe(outface);
        AutoPtr<IInteger32> jj;
        m->GetKey((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i);
    }
    assert(!(it2->HasNext(&isflag), isflag));
    return 0;
}

int CTest::test_ordered_keySet(int argc, char* argv[])
{
    PEL("CTest::test_ordered_keySet")
    int i = 0;
    int sz = 100;
    AutoPtr<IMap> lhm;
    CLinkedHashMap::New((IMap**)&lhm);
    AutoPtr<IInterface> outface;
    for (i = 0; i < sz; i++) {
        AutoPtr<IInteger32> ii;
        CInteger32::New(i, (IInteger32**)&ii);
        String str;
        ii->ToString(&str);
        AutoPtr<ICharSequence> sqi;
        CString::New(str, (ICharSequence**)&sqi);
        lhm->Put(ii, sqi, (IInterface**)&outface);
    }

    AutoPtr<ISet> s1;
    lhm->KeySet((ISet**)&s1);
    AutoPtr<IIterator> it1;
    s1->GetIterator((IIterator**)&it1);
    Boolean isflag = FALSE;
    int len = 0;
    int len1 = 0;
    assert((lhm->GetSize(&len), len) == (s1->GetSize(&len1), len1));
    for (i = 0; (it1->HasNext(&isflag), isflag); i++) {
        AutoPtr<IInteger32> jj;
        it1->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i);
    }

    AutoPtr<IMap> lruhm;
    CLinkedHashMap::New(200, .75f, true, (IMap**)&lruhm);
    for (i = 0; i < sz; i++) {
        AutoPtr<IInteger32> ii;
        CInteger32::New(i, (IInteger32**)&ii);
        String str;
        ii->ToString(&str);
        AutoPtr<ICharSequence> sqi;
        CString::New(str, (ICharSequence**)&sqi);
        lruhm->Put(ii, sqi, (IInterface**)&outface);
    }

    AutoPtr<ISet> s3;
    lruhm->KeySet((ISet**)&s3);
    AutoPtr<IIterator> it3;
    s3->GetIterator((IIterator**)&it3);
    assert((lruhm->GetSize(&len), len) == (s3->GetSize(&len1), len1));
    for (i = 0; i < sz && (it3->HasNext(&isflag), isflag); i++) {
        AutoPtr<IInteger32> jj;
        it3->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i);
    }

    /* fetch the even numbered entries to affect traversal order */
    int p = 0;
    for (i = 0; i < sz; i += 2) {
        AutoPtr<IInteger32> inti;
        CInteger32::New(i, (IInteger32**)&inti);
        lruhm->Get(inti, (IInterface**)&outface);
        String ii = Object::ToString(outface);
        p = p + StringUtils::ParseInt32(ii);
    }
    assert(2450 == p);

    AutoPtr<ISet> s2;
    lruhm->KeySet((ISet**)&s2);
    AutoPtr<IIterator> it2;
    s2->GetIterator((IIterator**)&it2);
    assert((lruhm->GetSize(&len), len) == (s2->GetSize(&len1), len1));
    for (i = 1; i < sz && (it2->HasNext(&isflag), isflag); i += 2) {
        AutoPtr<IInteger32> jj;
        it2->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i);
    }
    for (i = 0; i < sz && (it2->HasNext(&isflag), isflag); i += 2) {
        AutoPtr<IInteger32> jj;
        it2->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i);
    }
    assert(!(it2->HasNext(&isflag), isflag));
    return 0;
}

int CTest::test_ordered_values(int argc, char* argv[])
{
    PEL("CTest::test_ordered_values")
    int i = 0;
    int sz = 100;
    AutoPtr<IMap> lhm;
    CLinkedHashMap::New((IMap**)&lhm);
    AutoPtr<IInterface> outface;
    for (i = 0; i < sz; i++) {
        AutoPtr<IInteger32> ii;
        CInteger32::New(i, (IInteger32**)&ii);
        AutoPtr<IInteger32> ii2;
        CInteger32::New(i * 2, (IInteger32**)&ii2);
        lhm->Put(ii, ii2, (IInterface**)&outface);
    }

    AutoPtr<ICollection> s1;
    lhm->Values((ICollection**)&s1);
    AutoPtr<IIterator> it1;
    s1->GetIterator((IIterator**)&it1);
    Int32 len = 0;
    Int32 len1 = 0;
    assert((lhm->GetSize(&len), len) == (s1->GetSize(&len1), len1));
    Boolean isflag = FALSE;
    for (i = 0; it1->HasNext(&isflag), isflag; i++) {
        AutoPtr<IInteger32> jj;
        it1->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i * 2);
    }

    AutoPtr<IMap> lruhm;
    CLinkedHashMap::New(200, .75f, true, (IMap**)&lruhm);
    for (i = 0; i < sz; i++) {
        AutoPtr<IInteger32> ii;
        CInteger32::New(i, (IInteger32**)&ii);
        AutoPtr<IInteger32> ii2;
        CInteger32::New(i * 2, (IInteger32**)&ii2);
        lruhm->Put(ii, ii2, (IInterface**)&outface);
    }

    AutoPtr<ICollection> s3;
    lruhm->Values((ICollection**)&s3);
    AutoPtr<IIterator> it3;
    s3->GetIterator((IIterator**)&it3);
    assert((lruhm->GetSize(&len), len) == (s3->GetSize(&len1), len1));
    for (i = 0; i < sz && (it3->HasNext(&isflag), isflag); i++) {
        AutoPtr<IInteger32> jj;
        it3->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i * 2);
    }

    // fetch the even numbered entries to affect traversal order
    int p = 0;
    for (i = 0; i < sz; i += 2) {
        AutoPtr<IInteger32> inti;
        CInteger32::New(i, (IInteger32**)&inti);
        AutoPtr<IInteger32> ii;
        lruhm->Get(inti, (IInterface**)&outface);
        ii = IInteger32::Probe(outface);
        ii->GetValue(&len);
        p = p + len;
    }
    assert(p == 2450 * 2);

    AutoPtr<ICollection> s2;
    lruhm->Values((ICollection**)&s2);
    AutoPtr<IIterator> it2;
    s2->GetIterator((IIterator**)&it2);
    assert((lruhm->GetSize(&len), len) == (s2->GetSize(&len1), len1));
    for (i = 1; i < sz && (it2->HasNext(&isflag), isflag); i += 2) {
        AutoPtr<IInteger32> jj;
        it2->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i * 2);
    }
    for (i = 0; i < sz && (it2->HasNext(&isflag), isflag); i += 2) {
        AutoPtr<IInteger32> jj;
        it2->Next((IInterface**)&outface);
        jj = IInteger32::Probe(outface);
        jj->GetValue(&len);
        assert(len == i * 2);
    }
    assert(!(it2->HasNext(&isflag), isflag));
    return 0;
}

int CTest::test_remove_eldest(int argc, char* argv[])
{
    PEL("CTest::test_remove_eldest")
    assert(0 && "TODO");
    // int i = 0;
    // int sz = 10;
    // CacheMap lhm = new CacheMap();
    // for (i = 0; i < sz; i++) {
    //     Integer ii = new Integer(i);
    //     lhm.put(ii, new Integer(i * 2));
    // }

    // Collection s1 = lhm.values();
    // Iterator it1 = s1.iterator();
    // assertTrue("Returned set of incorrect size 1", lhm.size() == s1.size());
    // for (i = 5; it1.hasNext(); i++) {
    //     Integer jj = (Integer) it1.next();
    //     assertTrue("Returned incorrect entry set 1", jj.intValue() == i * 2);
    // }
    // assertTrue("Entries left in map", !it1.hasNext());
    return 0;
}

void CTest::setUp()
{
    PEL("CTest::setUp")
    objArray = ArrayOf<IInterface*>::Alloc(hmSize);
    objArray2 = ArrayOf<IInterface*>::Alloc(hmSize);
    for (int i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<IInteger32> inti;
        CInteger32::New(i, (IInteger32**)&inti);
        objArray->Set(i, inti);
        String str;
        inti->ToString(&str);
        AutoPtr<ICharSequence> sqi;
        CString::New(str, (ICharSequence**)&sqi);
        objArray2->Set(i, sqi);
    }

    CLinkedHashMap::New((IMap**)&hm);
    AutoPtr<IInterface> outface;
    for (int i = 0; i < objArray->GetLength(); i++) {
        hm->Put((*objArray2)[i], (*objArray)[i], (IInterface**)&outface);
    }
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    hm->Put(test, NULL, (IInterface**)&outface);
    hm->Put(NULL, test, (IInterface**)&outface);
}

void CTest::tearDown()
{
    PEL("CTest::tearDown")
    objArray = NULL;
    objArray2 = NULL;
    hm = NULL;
}

void CTest::entrySetRemoveHelper(String key, String value)
{
    PEL("CTest::entrySetRemoveHelper")
    AutoPtr<IMap> m1;
    CLinkedHashMap::New((IMap**)&m1);
    AutoPtr<ICharSequence> sqkey;
    CString::New(key, (ICharSequence**)&sqkey);
    AutoPtr<ICharSequence> sqvalue;
    CString::New(value, (ICharSequence**)&sqvalue);
    AutoPtr<IInterface> outface;
    m1->Put(sqkey, sqvalue, (IInterface**)&outface);
    AutoPtr<ICharSequence> jumbo;
    CString::New(String("jumbo"), (ICharSequence**)&jumbo);
    AutoPtr<ICharSequence> shrimp;
    CString::New(String("shrimp"), (ICharSequence**)&shrimp);
    m1->Put(jumbo, shrimp, (IInterface**)&outface);
    AutoPtr<IMap> m2;
    CLinkedHashMap::New(m1, (IMap**)&m2);
    AutoPtr<ISet> s1;
    m1->EntrySet((ISet**)&s1);
    AutoPtr<ISet> s2;
    m2->EntrySet((ISet**)&s2);
    AutoPtr<IIterator> it2;
    s2->GetIterator((IIterator**)&it2);
    it2->Next((IInterface**)&outface);
    Boolean isflag = FALSE;
    s1->Remove(outface, &isflag);
    AutoPtr<IIterator> it1;
    s1->GetIterator((IIterator**)&it1);
    it1->Next((IInterface**)&outface);
    AutoPtr<IInterface> outface2;
    IMapEntry::Probe(outface)->GetKey((IInterface**)&outface2);
    String str;
    ICharSequence::Probe(outface2)->ToString(&str);
    assert(String("jumbo") == str);
}
