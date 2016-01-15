#include "test.h"

String CTest::ID = String("hello");

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
    // // Test for method java.util.IdentityHashMap()
    // new Support_MapTest2(new IdentityHashMap()).runTest();

    AutoPtr<IMap> hm2;
    CIdentityHashMap::New((IMap**)&hm2);
    Int32 sizelen = 0;
    assert(0 == (hm2->GetSize(&sizelen), sizelen));
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    // Test for method java.util.IdentityHashMap(int)
    AutoPtr<IMap> hm2;
    CIdentityHashMap::New(5, (IMap**)&hm2);
    Int32 sizelen = 0;
    assert(0 == (hm2->GetSize(&sizelen), sizelen));
    // try {
    ECode ec = CIdentityHashMap::New(-1, (IMap**)&hm2);
    if (ec != NOERROR) {
        printf("Failed to throw IllegalArgumentException for initial capacity < 0\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }

    AutoPtr<IMap> empty;
    CIdentityHashMap::New(0, (IMap**)&empty);
    AutoPtr<IInterface> outface;
    AutoPtr<ICharSequence> nothing;
    CString::New(String("nothing"), (ICharSequence**)&nothing);
    assert((empty->Get(nothing, (IInterface**)&outface), outface) == NULL);
    AutoPtr<ICharSequence> something;
    CString::New(String("something"), (ICharSequence**)&something);
    AutoPtr<ICharSequence> here;
    CString::New(String("here"), (ICharSequence**)&here);
    empty->Put(something, here, (IInterface**)&outface);
    String str;
    empty->Get(something, (IInterface**)&outface);
    ICharSequence::Probe(outface)->ToString(&str);
    assert(str == "here");
    return 0;
}

int CTest::test_ConstructorLjava_util_Map(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_util_Map")
    // Test for method java.util.IdentityHashMap(java.util.Map)
    AutoPtr<IMap> myMap;
    AutoPtr<INavigableMap> nmap;
    CTreeMap::New((INavigableMap**)&nmap);
    myMap = IMap::Probe(nmap);
    AutoPtr<IInterface> outface;
    for (int counter = 0; counter < hmSize; counter++) {
        myMap->Put((*objArray2)[counter], (*objArray)[counter], (IInterface**)&outface);
    }
    AutoPtr<IMap> hm2;
    CIdentityHashMap::New(myMap, (IMap**)&hm2);
    AutoPtr<IInterface> outface2;
    Boolean isflag = FALSE;
    for (int counter = 0; counter < hmSize; counter++) {
        hm->Get((*objArray2)[counter], (IInterface**)&outface);
        hm2->Get((*objArray2)[counter], (IInterface**)&outface2);
        assert(Object::Equals(outface, outface2));
    }

    // AutoPtr<IMap> mockMap;
    // CMockMap();
    // hm2 = new IdentityHashMap(mockMap);
    // assertEquals("Size should be 0", 0, hm2.size());

    // try {
    //     new IdentityHashMap(null);
    //     fail("NullPointerException expected");
    // } catch (NullPointerException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_clear(int argc, char* argv[])
{
    PEL("CTest::test_clear")
    // Test for method void java.util.IdentityHashMap.clear()
    hm->Clear();
    Int32 sizelen = 0;
    assert(0 == (hm->GetSize(&sizelen), sizelen));
    AutoPtr<IInterface> outface;
    for (int i = 0; i < hmSize; i++)
        assert((hm->Get((*objArray2)[i], (IInterface**)&outface), outface) == NULL);
    return 0;
}

int CTest::test_clone(int argc, char* argv[])
{
    PEL("CTest::test_clone")
    // Test for method java.lang.Object java.util.IdentityHashMap.clone()
    AutoPtr<IMap> hm2;
    AutoPtr<IInterface> outface;
    ICloneable::Probe(hm)->Clone((IInterface**)&outface);
    hm2 = IMap::Probe(outface);
    assert(hm2.Get() != hm.Get());
    AutoPtr<IInterface> outface2;
    for (int counter = 0; counter < hmSize; counter++) {
        assert((hm->Get((*objArray2)[counter], (IInterface**)&outface), outface)
            == (hm2->Get((*objArray2)[counter], (IInterface**)&outface2), outface2));
    }

PFL
    AutoPtr<IMap> map;
    CIdentityHashMap::New((IMap**)&map);
    AutoPtr<ICharSequence> key;
    CString::New(String("key"), (ICharSequence**)&key);
    AutoPtr<ICharSequence> value;
    CString::New(String("value"), (ICharSequence**)&value);
    map->Put(key, value, (IInterface**)&outface);
PFL
    // get the keySet() and values() on the original Map
    AutoPtr<ISet> keys;
    map->KeySet((ISet**)&keys);
PFL
    AutoPtr<ICollection> values;
    map->Values((ICollection**)&values);
PFL
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
PFL_EX("it: %p", it.Get())
    it->Next((IInterface**)&outface);
PFL
    AutoPtr<ICharSequence> sqmid = ICharSequence::Probe(outface);
    String str;
    sqmid->ToString(&str);
    assert(String("value") == str);
    keys->GetIterator((IIterator**)&it);
PFL
    it->Next((IInterface**)&outface);
    sqmid = ICharSequence::Probe(outface);
    sqmid->ToString(&str);
    assert(String("key") == str);
    // AbstractMap map2 = (AbstractMap) map.clone();
    // map2.put("key", "value2");
    // Collection values2 = map2.values();
    // assertTrue("values() is identical", values2 != values);
    // // values() and keySet() on the cloned() map should be different
    // assertEquals("values() was not cloned",
    //         "value2", values2.iterator().next());
    // map2.clear();
    // map2.put("key2", "value3");
    // Set key2 = map2.keySet();
    // assertTrue("keySet() is identical", key2 != keys);
    // assertEquals("keySet() was not cloned",
    //         "key2", key2.iterator().next());
    return 0;
}

int CTest::test_containsKeyLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsKeyLjava_lang_Object")
    // Test for method boolean
    // java.util.IdentityHashMap.containsKey(java.lang.Object)
    Boolean isflag = FALSE;
    AutoPtr<IMap> hmmap = IMap::Probe(hm);
    assert((hmmap->ContainsKey((*objArray2)[23], &isflag), isflag));
    AutoPtr<IInteger32> int23;
    CInteger32::New(23, (IInteger32**)&int23);
    String str;
    int23->ToString(&str);
    AutoPtr<ICharSequence> sq;
    CString::New(str, (ICharSequence**)&sq);
    assert(!(hm->ContainsKey(sq, &isflag), isflag));
    AutoPtr<ICharSequence> KKDKDKD;
    CString::New(String("KKDKDKD"), (ICharSequence**)&KKDKDKD);
    assert(!(hm->ContainsKey(KKDKDKD, &isflag), isflag));

    AutoPtr<IMap> m;
    CIdentityHashMap::New((IMap**)&m);
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
    // java.util.IdentityHashMap.containsValue(java.lang.Object)
    Boolean isflag = FALSE;
    assert((hm->ContainsValue((*objArray)[19], &isflag), isflag));
    AutoPtr<IInteger32> int_9;
    CInteger32::New(-9, (IInteger32**)&int_9);
    assert(!(hm->ContainsValue(int_9, &isflag), isflag));
    return 0;
}

int CTest::test_entrySet(int argc, char* argv[])
{
    PEL("CTest::test_entrySet")
    // Test for method java.util.Set java.util.IdentityHashMap.entrySet()
    AutoPtr<ISet> s;
    hm->EntrySet((ISet**)&s);
    AutoPtr<IIterator> i;
    s->GetIterator((IIterator**)&i);
    Int32 sizelen1 = 0;
    Int32 sizelen2 = 0;
    assert((hm->GetSize(&sizelen1), sizelen1) == (s->GetSize(&sizelen2), sizelen2));
    Boolean isflag = FALSE;
    while (i->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        i->Next((IInterface**)&outface);
        AutoPtr<IEntry> m = IEntry::Probe(outface);
        AutoPtr<IInterface> keyface;
        AutoPtr<IInterface> valueface;
        m->GetKey((IInterface**)&keyface);
        m->GetValue((IInterface**)&valueface);
        assert((hm->ContainsKey(keyface, &isflag), isflag)
                && (hm->ContainsValue(valueface, &isflag), isflag));
    }
    return 0;
}

int CTest::test_getLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_getLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.IdentityHashMap.get(java.lang.Object)
    AutoPtr<ICharSequence> sqT;
    CString::New(String("T"), (ICharSequence**)&sqT);
    AutoPtr<IInterface> outface;
    assert((hm->Get(sqT, (IInterface**)&outface), outface) == NULL);
    AutoPtr<ICharSequence> HELLO;
    CString::New(String("HELLO"), (ICharSequence**)&HELLO);
    hm->Put(sqT, HELLO, (IInterface**)&outface);
    hm->Get(sqT, (IInterface**)&outface);
    String str;
    ICharSequence::Probe(outface)->ToString(&str);
    assert(String("HELLO") == str);

    AutoPtr<IMap> m;
    CIdentityHashMap::New((IMap**)&m);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    m->Put(NULL, test, (IInterface**)&outface);
    m->Get(NULL, (IInterface**)&outface);
    ICharSequence::Probe(outface)->ToString(&str);
    assert(String("test") == str);
    AutoPtr<IInteger32> int0;
    CInteger32::New(0, (IInteger32**)&int0);
    assert((m->Get(int0, (IInterface**)&outface), outface) == NULL);
    return 0;
}

int CTest::test_isEmpty(int argc, char* argv[])
{
    PEL("CTest::test_isEmpty")
    // Test for method boolean java.util.IdentityHashMap.isEmpty()
    AutoPtr<IMap> em;
    CIdentityHashMap::New((IMap**)&em);
    Boolean isflag = FALSE;
    assert((em->IsEmpty(&isflag), isflag));
    assert(!(hm->IsEmpty(&isflag), isflag));
    return 0;
}

int CTest::test_keySet(int argc, char* argv[])
{
    PEL("CTest::test_keySet")
    // Test for method java.util.Set java.util.IdentityHashMap.keySet()
    AutoPtr<ISet> s;
    hm->KeySet((ISet**)&s);
    Int32 sizelen1 = 0;
    Int32 sizelen2 = 0;
    assert((s->GetSize(&sizelen1), sizelen1) == (hm->GetSize(&sizelen2), sizelen2));
    Boolean isflag = FALSE;
    for (int i = 0; i < objArray->GetLength(); i++) {
        assert((s->Contains((*objArray2)[i], &isflag), isflag));
    }

    AutoPtr<IMap> m;
    CIdentityHashMap::New((IMap**)&m);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    AutoPtr<IInterface> outface;
    m->Put(NULL, test, (IInterface**)&outface);
    m->KeySet((ISet**)&s);
    assert((s->Contains(NULL, &isflag), isflag));
    AutoPtr<IIterator> it;
    s->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
    assert(outface == NULL);

    AutoPtr<IMap> map;
    CIdentityHashMap::New(101, (IMap**)&map);
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
    map->KeySet((ISet**)&s);
    s->GetIterator((IIterator**)&it);
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
    it->Next((IInterface**)&outface);
    AutoPtr<IObject> obj = IObject::Probe(outface);
    AutoPtr<IInterface> outface2;
    list->Get(0, (IInterface**)&outface2);
    assert((obj->Equals(outface2, &isflag), isflag));
    assert(1 == (map->GetSize(&sizelen1), sizelen1));
    map->KeySet((ISet**)&s);
    s->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
    obj = IObject::Probe(outface);
    list->Get(0, (IInterface**)&outface2);
    assert((obj->Equals(outface2, &isflag), isflag));

    AutoPtr<IMap> map2;
    CIdentityHashMap::New(101, (IMap**)&map2);
    AutoPtr<IInteger32> int4;
    CInteger32::New(4, (IInteger32**)&int4);
    AutoPtr<ICharSequence> sq4;
    CString::New(String("4"), (ICharSequence**)&sq4);
    map2->Put(int1, sq1, (IInterface**)&outface);
    map2->Put(int4, sq4, (IInterface**)&outface);
    AutoPtr<IIterator> it2;
    map2->KeySet((ISet**)&s);
    s->GetIterator((IIterator**)&it2);
    AutoPtr<IInterface> remove3;
    it2->Next((IInterface**)&remove3);
    AutoPtr<IInteger32> next;
    if ((IInteger32::Probe(remove3)->GetValue(&sizelen1), sizelen1) == 1)
        CInteger32::New(4, (IInteger32**)&next);
    else
        CInteger32::New(1, (IInteger32**)&next);
    it2->HasNext(&isflag);
    it2->Remove();
    it2->Next((IInterface**)&outface);
    assert((Object::Equals(outface, next, &isflag), isflag));
    assert(1 == (map2->GetSize(&sizelen1), sizelen1));
    map2->KeySet((ISet**)&s);
    s->GetIterator((IIterator**)&it);
    it->Next((IInterface**)&outface);
    assert(Object::Equals(outface, next));
    return 0;
}

int CTest::test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_putLjava_lang_ObjectLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.IdentityHashMap.put(java.lang.Object, java.lang.Object)
    AutoPtr<ICharSequence> KEY;
    CString::New(String("KEY"), (ICharSequence**)&KEY);
    AutoPtr<ICharSequence> VALUE;
    CString::New(String("VALUE"), (ICharSequence**)&VALUE);
    AutoPtr<IInterface> outface;
    hm->Put(KEY, VALUE, (IInterface**)&outface);
    String str;
    hm->Get(KEY, (IInterface**)&outface);
    ICharSequence::Probe(outface)->ToString(&str);
    assert(String("VALUE") == str);

    AutoPtr<IMap> m;
    CIdentityHashMap::New((IMap**)&m);
    AutoPtr<IInteger16> s0;
    CInteger16::New(0, (IInteger16**)&s0);
    AutoPtr<ICharSequence> sqshort;
    CString::New(String("short"), (ICharSequence**)&sqshort);
    m->Put(s0, sqshort, (IInterface**)&outface);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    m->Put(NULL, test, (IInterface**)&outface);
    AutoPtr<IInteger32> i0;
    CInteger32::New(0, (IInteger32**)&i0);
    AutoPtr<ICharSequence> sqint;
    CString::New(String("int"), (ICharSequence**)&sqint);
    m->Put(i0, sqint, (IInterface**)&outface);
    m->Get(s0, (IInterface**)&outface);
    ICharSequence::Probe(outface)->ToString(&str);
    assert(String("short") == str);
    m->Get(i0, (IInterface**)&outface);
    ICharSequence::Probe(outface)->ToString(&str);
    assert(String("int") == str);
    return 0;
}

int CTest::test_putAllLjava_util_Map(int argc, char* argv[])
{
    PEL("CTest::test_putAllLjava_util_Map")
    // Test for method void java.util.IdentityHashMap.putAll(java.util.Map)
    AutoPtr<IMap> hm2;
    CIdentityHashMap::New((IMap**)&hm2);
    AutoPtr<IInterface> outface;
    hm2->PutAll(hm);
    Boolean isflag = FALSE;
    for (int i = 0; i < 1000; i++) {
        hm2->Get((*objArray2)[i], (IInterface**)&outface);
        AutoPtr<IInteger32> midint;
        CInteger32::New(i, (IInteger32**)&midint);
        assert((IInteger32::Probe(outface)->Equals(midint, &isflag), isflag));
    }

    CIdentityHashMap::New((IMap**)&hm2);
    // Map mockMap = new MockMap();
    // hm2.putAll(mockMap);
    // assertEquals("Size should be 0", 0, hm2.size());

    // try {
    //     hm2.putAll(null);
    //     fail("NullPointerException expected");
    // } catch (NullPointerException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_removeLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_removeLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.IdentityHashMap.remove(java.lang.Object)
    Int32 size = 0;
    hm->GetSize(&size);
    AutoPtr<IInterface> x;
    Boolean isflag = FALSE;
    hm->Remove((*objArray2)[9], (IInterface**)&x);
    AutoPtr<IInteger32> int9;
    CInteger32::New(9, (IInteger32**)&int9);
    assert((IInteger32::Probe(x)->Equals(int9, &isflag), isflag));
    AutoPtr<IInterface> outface;
    assert((hm->Get((*objArray2)[9], (IInterface**)&outface), outface) == NULL);
    Int32 sizelen1 = 0;
    assert((hm->GetSize(&sizelen1), sizelen1) == (size - 1));
    AutoPtr<ICharSequence> LCLCLC;
    CString::New(String("LCLCLC"), (ICharSequence**)&LCLCLC);
    assert((hm->Remove(LCLCLC, (IInterface**)&outface), outface) == NULL);

    AutoPtr<IMap> m;
    CIdentityHashMap::New((IMap**)&m);
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    m->Put(NULL, test, (IInterface**)&outface);
    assert((m->Remove((*objArray)[0], (IInterface**)&outface), outface) == NULL);
    String str;
    m->Remove(NULL, (IInterface**)&outface);
    ICharSequence::Probe(outface)->ToString(&str);
    assert(String("test") == str);
    return 0;
}

int CTest::test_size(int argc, char* argv[])
{
    PEL("CTest::test_size")
    // Test for method int java.util.IdentityHashMap.size()
    Int32 sizelen1 = 0;
    hm->GetSize(&sizelen1);
    PFL_EX("sizelen1: %d, objArray: %d", sizelen1, objArray->GetLength())

    assert((objArray->GetLength() + 2) == sizelen1);
    return 0;
}

int CTest::test_equalsLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_equalsLjava_lang_Object")
    AutoPtr<IMap> mapOne;
    CIdentityHashMap::New((IMap**)&mapOne);
    AutoPtr<IMap> mapTwo;
    CIdentityHashMap::New((IMap**)&mapTwo);
    AutoPtr<IMap> mapThree;
    CIdentityHashMap::New((IMap**)&mapThree);
    AutoPtr<IMap> mapFour;
    CIdentityHashMap::New((IMap**)&mapFour);

    String one = String("one");
    String alsoOne = String(one); // use the new operator to ensure a
    // new reference is constructed
    String two = String("two");
    String alsoTwo = String(two); // use the new operator to ensure a
    // new reference is constructed

    AutoPtr<IInterface> outface;
    AutoPtr<ICharSequence> sqone;
    CString::New(one, (ICharSequence**)&sqone);
    AutoPtr<ICharSequence> sqtwo;
    CString::New(two, (ICharSequence**)&sqtwo);
    mapOne->Put(sqone, sqtwo, (IInterface**)&outface);
    mapFour->Put(sqone, sqtwo, (IInterface**)&outface);

    // these two are not equal to the above two
    AutoPtr<ICharSequence> sqalsoOne;
    CString::New(alsoOne, (ICharSequence**)&sqalsoOne);
    AutoPtr<ICharSequence> sqalsoTwo;
    CString::New(alsoTwo, (ICharSequence**)&sqalsoTwo);
    mapTwo->Put(sqalsoOne, sqtwo, (IInterface**)&outface);
    mapThree->Put(sqone, sqalsoTwo, (IInterface**)&outface);

    Boolean isflag = FALSE;
    assert((mapOne->Equals(mapFour, &isflag), isflag));
    assert(!(mapOne->Equals(mapTwo, &isflag), isflag));
    assert(!(mapOne->Equals(mapThree, &isflag), isflag));
    assert(!(mapTwo->Equals(mapThree, &isflag), isflag));

    AutoPtr<IMap> hashMapTwo;
    CHashMap::New((IMap**)&hashMapTwo);
    AutoPtr<IMap> hashMapThree;
    CHashMap::New((IMap**)&hashMapThree);
    hashMapTwo->Put(sqalsoOne, sqtwo, (IInterface**)&outface);
    hashMapThree->Put(sqone, sqalsoTwo, (IInterface**)&outface);

    assert(!(mapOne->Equals(hashMapTwo, &isflag), isflag));
    assert(!(mapOne->Equals(hashMapThree, &isflag), isflag));
    return 0;
}

int CTest::test_values(int argc, char* argv[])
{
    PEL("CTest::test_values")
    // Test for method java.util.Collection
    // java.util.IdentityHashMap.values()
    AutoPtr<ICollection> c;
    hm->Values((ICollection**)&c);
    Int32 sizelen1 = 0;
    Int32 sizelen2 = 0;
    assert((c->GetSize(&sizelen1), sizelen1) == (hm->GetSize(&sizelen2), sizelen2));
    Boolean isflag = FALSE;
    for (int i = 0; i < objArray->GetLength(); i++)
        assert((c->Contains((*objArray)[i], &isflag), isflag));

    AutoPtr<IMap> myIdentityHashMap;
    CIdentityHashMap::New((IMap**)&myIdentityHashMap);
    AutoPtr<IInterface> outface;
    for (int i = 0; i < 100; i++)
        myIdentityHashMap->Put((*objArray2)[i], (*objArray)[i], (IInterface**)&outface);
    AutoPtr<ICollection> values;
    myIdentityHashMap->Values((ICollection**)&values);
    values->Remove((*objArray)[0], &isflag);
    assert(!(myIdentityHashMap->ContainsValue((*objArray2)[0], &isflag), isflag));
    return 0;
}

int CTest::test_Serialization(int argc, char* argv[])
{
    PEL("CTest::test_Serialization")
    AutoPtr<IMap> map;
    CIdentityHashMap::New((IMap**)&map);
    AutoPtr<ICharSequence> world;
    CString::New(String("world"), (ICharSequence**)&world);
    AutoPtr<ICharSequence> sqID;
    CString::New(ID, (ICharSequence**)&sqID);
    AutoPtr<IInterface> outface;
    map->Put(sqID, world, (IInterface**)&outface);
    // BEGIN android-added
    // Regression test for null key in serialized IdentityHashMap (1178549)
    // Together with this change the IdentityHashMap.golden.ser resource
    // was replaced by a version that contains a map with a null key.
    AutoPtr<ICharSequence> sqnull;
    CString::New(String("null"), (ICharSequence**)&sqnull);
    map->Put(NULL, sqnull, (IInterface**)&outface);
    // END android-added
    // SerializationTest.verifySelf(map, comparator);
    // SerializationTest.verifyGolden(this, map, comparator);
    return 0;
}

int CTest::test_rehash(int argc, char* argv[])
{
    PEL("CTest::test_rehash")
    return 0;
}

void CTest::setUp()
{
    PEL("CTest::setUp")
    objArray = ArrayOf<IInterface*>::Alloc(hmSize);
    objArray2 = ArrayOf<IInterface*>::Alloc(hmSize);
    for (int i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<IInteger32> midint;
        CInteger32::New(i, (IInteger32**)&midint);
        objArray->Set(i, midint);
        // android-changed: the containsKey test requires unique strings.
        String str;
        midint->ToString(&str);
        AutoPtr<ICharSequence> midsq;
        CString::New(str, (ICharSequence**)&midsq);
        objArray2->Set(i, midsq);
    }

    CIdentityHashMap::New((IMap**)&hm);
    AutoPtr<IInterface> outface;
    for (int i = 0; i < objArray->GetLength(); i++) {
        hm->Put((*objArray2)[i], (*objArray)[i], (IInterface**)&outface);
    }
    AutoPtr<ICharSequence> test;
    CString::New(String("test"), (ICharSequence**)&test);
    Int32 sizelen1 = 0;
    Int32 sizelen2 = 0;
    Int32 sizelen3 = 0;
    hm->GetSize(&sizelen1);
    ECode ec = hm->Put(test, NULL, (IInterface**)&outface);
    hm->GetSize(&sizelen2);
    ec = hm->Put(NULL, test, (IInterface**)&outface);
    hm->GetSize(&sizelen3);
PFL_EX("sizelen1 : %d, sizelen2 : %d, sizelen3: %d", sizelen1, sizelen2, sizelen3)
}

void CTest::tearDown()
{
    PEL("CTest::tearDown")
    objArray = NULL;
    objArray2 = NULL;
    hm = NULL;
}
