
#include "test.h"

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
    // Test for method java.util.Hashtable()
    // new Support_MapTest2(new Hashtable()).runTest();

    AutoPtr<IDictionary> h;
    CHashTable::New((IDictionary**)&h);
    Int32 sizelen = 0;
    assert(0 == (h->GetSize(&sizelen), sizelen));
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    // Test for method java.util.Hashtable(int)
    AutoPtr<IDictionary> h;
    CHashTable::New(9, (IDictionary**)&h);
    Int32 sizelen = 0;
    assert(0 == (h->GetSize(&sizelen), sizelen));

    AutoPtr<IDictionary> empty;
    CHashTable::New(0, (IDictionary**)&empty);
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
    AutoPtr<ICharSequence> outchar;
    empty->Get(something, (IInterface**)&outface);
    outchar = ICharSequence::Probe(outface);
    outchar->ToString(&str);
    assert(str == "here");

    // try {
    ECode ec = CHashTable::New(-1, (IDictionary**)&h);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_ConstructorIF(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorIF")
    // Test for method java.util.Hashtable(int, float)
    AutoPtr<IDictionary> h;
    CHashTable::New(10, 0.5f, (IDictionary**)&h);
    Int32 sizelen = 0;
    assert(0 == (h->GetSize(&sizelen), sizelen));

    AutoPtr<IDictionary> empty;
    CHashTable::New(0, 0.75f, (IDictionary**)&empty);
    AutoPtr<ICharSequence> nothing;
    CString::New(String("nothing"), (ICharSequence**)&nothing);
    AutoPtr<IInterface> outface;
    assert((empty->Get(nothing, (IInterface**)&outface), outface) == NULL);

    AutoPtr<ICharSequence> something;
    CString::New(String("something"), (ICharSequence**)&something);
    AutoPtr<ICharSequence> here;
    CString::New(String("here"), (ICharSequence**)&here);
    empty->Put(something, here, (IInterface**)&outface);

    empty->Get(something, (IInterface**)&outface);
    here = ICharSequence::Probe(outface);
    String str;
    here->ToString(&str);
    assert(str == "here");

    // try {
    ECode ec = CHashTable::New(-1, 0.75f, (IDictionary**)&h);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }

    // try {
    ec = CHashTable::New(0, -0.75f, (IDictionary**)&h);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_ConstructorLjava_util_Map(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_util_Map")
    // Test for method java.util.Hashtable(java.util.Map)
    AutoPtr<IMap> map;
    AutoPtr<INavigableMap> navmap;
    CTreeMap::New((INavigableMap**)&navmap);
    map = IMap::Probe(navmap);
    AutoPtr<ICharSequence> firstVal;
    CString::New(String("Gabba"), (ICharSequence**)&firstVal);
    AutoPtr<IInteger32> secondVal;
    CInteger32::New(5, (IInteger32**)&secondVal);
    AutoPtr<ICharSequence> Gah;
    CString::New(String("Gah"), (ICharSequence**)&Gah);
    AutoPtr<IInterface> outface;
    ECode ec = map->Put(Gah, firstVal, (IInterface**)&outface);
    AutoPtr<ICharSequence> Ooga;
    CString::New(String("Ooga"), (ICharSequence**)&Ooga);
    ec = map->Put(Ooga, secondVal, (IInterface**)&outface);
    AutoPtr<IDictionary> ht;
    CHashTable::New(map, (IDictionary**)&ht);
    ht->Get(Gah, (IInterface**)&outface);
    assert(outface == firstVal);
    ht->Get(Ooga, (IInterface**)&outface);
    assert(outface == secondVal);

    // try {
    ec = CHashTable::New(NULL, (IDictionary**)&ht);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_ConversionConstructorNullValue(int argc, char* argv[])
{
    PEL("CTest::test_ConversionConstructorNullValue")
    // Map<String, Void> map = Collections.singletonMap("Dog", null);
    // try {
    //     new Hashtable<String, Void>(map);
    //     fail("NullPointerException expected");
    // } catch (NullPointerException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_clear(int argc, char* argv[])
{
    PEL("CTest::test_clear")
    // Test for method void java.util.Hashtable.clear()
    AutoPtr<IDictionary> h = hashtableClone(htfull);
    assert(0 && "TODO");
    // h->Clear();
    Int32 sizelen = 0;
    assert(0 == (h->GetSize(&sizelen), sizelen));
    AutoPtr<IEnumeration> el;
    h->Elements((IEnumeration**)&el);
    AutoPtr<IEnumeration> keys;
    h->Keys((IEnumeration**)&keys);
    Boolean isflag = FALSE;
    assert(!(el->HasMoreElements(&isflag), isflag)
            && !(keys->HasMoreElements(&isflag), isflag));
    return 0;
}

int CTest::test_clone(int argc, char* argv[])
{
    PEL("CTest::test_clone")
    // Test for method java.lang.Object java.util.Hashtable.clone()
    AutoPtr<IDictionary> h;
    AutoPtr<ICloneable> resclone = ICloneable::Probe(htfull);
    resclone->Clone((IInterface**)&h);
    Int32 hsize = 0;
    Int32 htsize = 0;
    assert((h->GetSize(&hsize), hsize) == (htfull->GetSize(&htsize), htsize));

    AutoPtr<IEnumeration> org;
    htfull->Keys((IEnumeration**)&org);
    AutoPtr<IEnumeration> cpy;
    h->Keys((IEnumeration**)&cpy);

    String okey, ckey;
    Boolean isflag = FALSE;
    while ((org->HasMoreElements(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        AutoPtr<ICharSequence> outcharo;
        AutoPtr<ICharSequence> outcharc;
        org->NextElement((IInterface**)&outface);
        outcharo = ICharSequence::Probe(outface);
        outcharo->ToString(&okey);
        cpy->NextElement((IInterface**)&outface);
        outcharc = ICharSequence::Probe(outface);
        outcharc->ToString(&ckey);
        assert(okey.Equals(ckey));

        AutoPtr<ICharSequence> getcharo;
        AutoPtr<ICharSequence> getcharc;
        htfull->Get(outcharo, (IInterface**)&outface);
        getcharo = ICharSequence::Probe(outface);
        getcharo->ToString(&okey);
        h->Get(outcharc, (IInterface**)&outface);
        getcharc = ICharSequence::Probe(outface);
        getcharc->ToString(&ckey);
        assert(okey.Equals(ckey));
    }
    assert(!(cpy->HasMoreElements(&isflag), isflag));
    return 0;
}

int CTest::test_containsLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsLjava_lang_Object")
    // Test for method boolean
    // java.util.Hashtable.contains(java.lang.Object)
    AutoPtr<ICharSequence> val7;
    CString::New(String("Val 7"), (ICharSequence**)&val7);
    Boolean isflag = FALSE;
    assert(0 && "TODO");
    AutoPtr<IMap> ht10map = IMap::Probe(ht10);
    assert(ht10map->ContainsValue(val7, &isflag));
    AutoPtr<ICharSequence> ZZZZZZZZZZZZZZZZ;
    CString::New(String("ZZZZZZZZZZZZZZZZ"), (ICharSequence**)&ZZZZZZZZZZZZZZZZ);
    assert(!(ht10map->ContainsValue(ZZZZZZZZZZZZZZZZ, &isflag), isflag));

    // try {
    ECode ec = ht10map->ContainsValue(NULL, &isflag);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
        //expected
    // }
    return 0;
}

int CTest::test_containsKeyLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsKeyLjava_lang_Object")
    // Test for method boolean
    // java.util.Hashtable.containsKey(java.lang.Object)
    Boolean isflag = FALSE;
    AutoPtr<ICharSequence> FKey4;
    CString::New(String("FKey 4"), (ICharSequence**)&FKey4);
    assert(0 && "TODO");
    AutoPtr<IMap> htfullmap = IMap::Probe(htfull);
    assert((htfullmap->ContainsKey(FKey4, &isflag), isflag));
    AutoPtr<ICharSequence> FKey99;
    CString::New(String("FKey 99"), (ICharSequence**)&FKey99);
    assert(!(htfullmap->ContainsKey(FKey99, &isflag), isflag));

    // try {
    ECode ec = htfullmap->ContainsKey(NULL, &isflag);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
        //expected
    // }
    return 0;
}

int CTest::test_containsValueLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsValueLjava_lang_Object")
    // Test for method boolean
    // java.util.Hashtable.containsValue(java.lang.Object)
    AutoPtr<IEnumeration> e;
    elmVector->Elements((IEnumeration**)&e);
    Boolean isflag = FALSE;
    assert(0 && "TODO");
    AutoPtr<IMap> ht10map = IMap::Probe(ht10);
    while ((e->HasMoreElements(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        e->NextElement((IInterface**)&outface);
        assert((ht10map->ContainsValue(outface, &isflag), isflag));
    }
    AutoPtr<ICharSequence> nullchar;
    CString::New(String("NULL"), (ICharSequence**)&nullchar);
    assert(!(ht10map->ContainsValue(nullchar, &isflag), isflag));

    // try {
    ECode ec = ht10map->ContainsValue(NULL, &isflag);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException ee) {
        //expected
    // }
    return 0;
}

int CTest::test_elements(int argc, char* argv[])
{
    PEL("CTest::test_elements")
    // Test for method java.util.Enumeration java.util.Hashtable.elements()
    AutoPtr<IEnumeration> elms;
PFL
    ht10->Elements((IEnumeration**)&elms);
PFL_EX("elms: %p", elms.Get())
    int i = 0;
    Boolean isflag = FALSE;
    while ((elms->HasMoreElements(&isflag), isflag)) {
        AutoPtr<ICharSequence> outchar;
        elms->NextElement((IInterface**)&outchar);
        assert((elmVector->Contains(outchar, &isflag), isflag));
        ++i;
    }
    Int32 sizelen = 0;
    assert(10 == (ht10->GetSize(&sizelen), sizelen));
    return 0;
}

int CTest::test_entrySet(int argc, char* argv[])
{
    PEL("CTest::test_entrySet")
//     // Test for method java.util.Set java.util.Hashtable.entrySet()
//     Set s = ht10.entrySet();
//     Set s2 = new HashSet();
//     Iterator i = s.iterator();
//     while (i.hasNext())
//         s2.add(((Map.Entry) i.next()).getValue());
//     Enumeration e = elmVector.elements();
//     while (e.hasMoreElements())
//         assertTrue("Returned incorrect entry set", s2.contains(e
//                 .nextElement()));
// // BEGIN android-removed
// // implementation dependent
// //        assertEquals("Not synchronized",
// //                "java.util.Collections$SynchronizedSet", s.getClass().getName());
// // END android-removed

//     boolean exception = false;
//     try {
//         ((Map.Entry) ht10.entrySet().iterator().next()).setValue(null);
//     } catch (NullPointerException e1) {
//         exception = true;
//     }
//     assertTrue(
//             "Should not be able to assign null to a Hashtable entrySet() Map.Entry",
//             exception);
    return 0;
}

int CTest::test_equalsLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_equalsLjava_lang_Object")
    // Test for method boolean java.util.Hashtable.equals(java.lang.Object)
    AutoPtr<IDictionary> h = hashtableClone(ht10);
    assert(0 && "TODO");
    AutoPtr<IMap> hmap = IMap::Probe(h);
    Boolean isflag = FALSE;
    assert((hmap->Equals(ht10, &isflag), isflag));
    assert(!(IMap::Probe(ht10)->Equals(htfull, &isflag), isflag));
    return 0;
}

int CTest::test_getLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_getLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.Hashtable.get(java.lang.Object)
    AutoPtr<IDictionary> h = hashtableClone(htfull);
    AutoPtr<ICharSequence> FKey2;
    CString::New(String("FKey 2"), (ICharSequence**)&FKey2);
    AutoPtr<IInterface> outface;
    h->Get(FKey2, (IInterface**)&outface);
    String str;
    FKey2 = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    FKey2->ToString(&str);
    assert(String("FVal 2") == str);
    return 0;
}

int CTest::test_hashCode(int argc, char* argv[])
{
    PEL("CTest::test_hashCode")
    // Test for method int java.util.Hashtable.hashCode()
    // Set entrySet = ht10.entrySet();
    // Iterator iterator = entrySet.iterator();
    // int expectedHash;
    // for (expectedHash = 0; iterator.hasNext(); expectedHash += iterator
    //         .next().hashCode())
    //     ;
    // assertTrue("Incorrect hashCode returned.  Wanted: " + expectedHash
    //         + " got: " + ht10.hashCode(), expectedHash == ht10.hashCode());
    return 0;
}

int CTest::test_isEmpty(int argc, char* argv[])
{
    PEL("CTest::test_isEmpty")
    // Test for method boolean java.util.Hashtable.isEmpty()
    Boolean isflag = FALSE;
    assert(!(ht10->IsEmpty(&isflag), isflag));

    AutoPtr<IDictionary> newh;
    CHashTable::New((IDictionary**)&newh);
    assert((newh->IsEmpty(&isflag), isflag));

    AutoPtr<IDictionary> ht;
    CHashTable::New((IDictionary**)&ht);
    AutoPtr<ICharSequence> int0;
    CString::New(String("0"), (ICharSequence**)&int0);
    AutoPtr<ICharSequence> strnull;
    CString::New(String(""), (ICharSequence**)&strnull);
    AutoPtr<IInterface> outface;
    ht->Put(int0, strnull, (IInterface**)&outface);
    // Thread t1 = new Thread() {
    //     public void run() {
    //         while (!ht.isEmpty())
    //             ;
    //         ht.put("final", "");
    //     }
    // };
    // t1.start();
    for (int i = 1; i < 10000; i++) {
        // synchronized (ht) {
        AutoPtr<ICharSequence> seq;
        CString::New(StringUtils::Int32ToString(i - 1), (ICharSequence**)&seq);
        ht->Remove(seq, (IInterface**)&outface);
        AutoPtr<ICharSequence> seq2;
        CString::New(StringUtils::Int32ToString(i), (ICharSequence**)&seq2);
        ht->Put(seq2, strnull, (IInterface**)&outface);
        // }
        int size = 0;
        Int32 sizelen = 0;
        if ((size = (ht->GetSize(&sizelen), sizelen)) != 1) {
            String result = String("Size is not 1: ") + StringUtils::Int32ToString(size) + String(" ");
            // terminate the thread
            assert(0 && "TODO");
            IMap::Probe(ht)->Clear();
            printf("%s  expected~~~~~~~~~\n", result.string());
        }
    }
    // terminate the thread
    assert(0 && "TODO");
    IMap::Probe(ht)->Clear();
    return 0;
}

int CTest::test_keys(int argc, char* argv[])
{
    PEL("CTest::test_keys")
    // Test for method java.util.Enumeration java.util.Hashtable.keys()

    AutoPtr<IEnumeration> keys;
    ht10->Keys((IEnumeration**)&keys);
    int i = 0;
    Boolean isflag = FALSE;
    while ((keys->HasMoreElements(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        keys->NextElement((IInterface**)&outface);
PFL_EX("i: %d, outface:%p", i, outface.Get())
        ECode ec = keyVector->Contains(outface, &isflag);
PFL_EX("ec : %p, isflag: %d", ec, isflag)
        assert(isflag);
        ++i;
    }

    assert(10 == (ht10->GetSize(&i), i));
    return 0;
}

int CTest::test_keys_subtest0(int argc, char* argv[])
{
    PEL("CTest::test_keys_subtest0")
    // this is the reference implementation behavior
    AutoPtr<IDictionary> ht;
    CHashTable::New(3, (IDictionary**)&ht);
    AutoPtr<ICharSequence> initial;
    CString::New(String("initial"), (ICharSequence**)&initial);
    AutoPtr<ICharSequence> strnull;
    CString::New(String(""), (ICharSequence**)&strnull);
    AutoPtr<IInterface> outface;
    ht->Put(initial, strnull, (IInterface**)&outface);
PFL
    AutoPtr<IEnumeration> en;
    ht->Keys((IEnumeration**)&en);
    Boolean isflag = FALSE;
    en->HasMoreElements(&isflag);
PFL
    ht->Remove(initial, (IInterface**)&outface);
PFL
    Boolean exception = false;
    // try {
    AutoPtr<IInterface> result;
PFL
    en->NextElement((IInterface**)&result);
PFL
    initial = ICharSequence::Probe(result);
    String str;
    initial->ToString(&str);
    assert(String("initial").Equals(str));
    // } catch (NoSuchElementException e) {
    //     exception = true;
    // }
    // assertTrue("unexpected NoSuchElementException", !exception);
    return 0;
}

int CTest::test_keySet(int argc, char* argv[])
{
    PEL("CTest::test_keySet")
    // Test for method java.util.Set java.util.Hashtable.keySet()
    AutoPtr<ISet> s;
    assert(0 && "TODO");
    IMap::Probe(ht10)->KeySet((ISet**)&s);
    AutoPtr<IEnumeration> e;
    keyVector->Elements((IEnumeration**)&e);
    Boolean isflag = FALSE;
    AutoPtr<IInterface> outface;
    while ((e->HasMoreElements(&isflag), isflag)) {
        assert((s->Contains((e->NextElement((IInterface**)&outface), outface), &isflag), isflag));
    }


// BEGIN android-removed
// implementation dependent
//        assertEquals("Not synchronized",
//                "java.util.Collections$SynchronizedSet", s.getClass().getName());
// END android-removed

    AutoPtr<IDictionary> h101;
    CHashTable::New(101, (IDictionary**)&h101);
    assert(0 && "TODO");
    AutoPtr<IMap> map = IMap::Probe(h101);
    AutoPtr<ICharSequence> seq1;
    CString::New(String("1"), (ICharSequence**)&seq1);
    AutoPtr<IInteger32> int1;
    CInteger32::New(1, (IInteger32**)&int1);
    map->Put(int1, seq1, (IInterface**)&outface);
    AutoPtr<IInteger32> int102;
    CInteger32::New(102, (IInteger32**)&int102);
    AutoPtr<ICharSequence> seq102;
    CString::New(String("102"), (ICharSequence**)&seq102);
    map->Put(int102, seq102, (IInterface**)&outface);
    AutoPtr<IInteger32> int203;
    CInteger32::New(203, (IInteger32**)&int203);
    AutoPtr<ICharSequence> seq203;
    CString::New(String("203"), (ICharSequence**)&int203);
    map->Put(int203, seq203, (IInterface**)&outface);
    map->KeySet((ISet**)&s);
    AutoPtr<IIterator> it;
    s->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> remove1;
    it->Next((IInterface**)&remove1);
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
    AutoPtr<IInterface> outface4;
    it->Next((IInterface**)&outface4);
    AutoPtr<IInteger32> outint = IInteger32::Probe(outface4);
    assert((outint->Equals((list->Get(0, (IInterface**)&outface), IInteger32::Probe(outface)), &isflag), isflag));
    Int32 sizelen = 0;
    assert(1 == (map->GetSize(&sizelen), sizelen));
    map->KeySet((ISet**)&s);
    s->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> outface1;
    it->Next((IInterface**)&outface1);
    AutoPtr<IInteger32> intface1 = IInteger32::Probe(outface1);
    AutoPtr<IInterface> outface2;
    assert((intface1->Equals((list->Get(0, (IInterface**)&outface2), IInteger32::Probe(outface2)), &isflag), isflag));

    AutoPtr<IDictionary> midh;
    CHashTable::New(101, (IDictionary**)&midh);
    AutoPtr<IMap> map2 = IMap::Probe(midh);
    map2->Put(int1, seq1, (IInterface**)&outface);
    AutoPtr<ICharSequence> seq4;
    CString::New(String("4"), (ICharSequence**)&seq4);
    AutoPtr<IInteger32> int4;
    CInteger32::New(4, (IInteger32**)&int4);
    map2->Put(int4, seq4, (IInterface**)&outface);
    // Iterator it2 = map2.keySet().iterator();
    // Integer remove3 = (Integer) it2.next();
    // Integer next;
    // if (remove3.intValue() == 1)
    //     next = new Integer(4);
    // else
    //     next = new Integer(1);
    // it2.hasNext();
    // it2.remove();
    // assertTrue("Wrong result 2", it2.next().equals(next));
    // assertEquals("Wrong size 2", 1, map2.size());
    // assertTrue("Wrong contents 2", map2.keySet().iterator().next().equals(
    //         next));
    return 0;
}

int CTest::test_keySet_subtest0(int argc, char* argv[])
{
    PEL("CTest::test_keySet_subtest0")
    // Set s1 = ht10.keySet();
    // assertTrue("should contain key", s1.remove("Key 0"));
    // assertTrue("should not contain key", !s1.remove("Key 0"));

    // final int iterations = 10000;
    // final Hashtable ht = new Hashtable();
    // Thread t1 = new Thread() {
    //     public void run() {
    //         for (int i = 0; i < iterations; i++) {
    //             ht.put(String.valueOf(i), "");
    //             ht.remove(String.valueOf(i));
    //         }
    //     }
    // };
    // t1.start();
    // Set set = ht.keySet();
    // for (int i = 0; i < iterations; i++) {
    //     Iterator it = set.iterator();
    //     try {
    //         it.next();
    //         it.remove();
    //         int size;
    //         // ensure removing with the iterator doesn't corrupt the
    //         // Hashtable
    //         if ((size = ht.size()) < 0) {
    //             fail("invalid size: " + size);
    //         }
    //     } catch (NoSuchElementException e) {
    //     } catch (ConcurrentModificationException e) {
    //     }
    // }
    return 0;
}

int CTest::test_putLjava_lang_ObjectLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_putLjava_lang_ObjectLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.Hashtable.put(java.lang.Object, java.lang.Object)
    AutoPtr<IDictionary> h = hashtableClone(ht100);
    AutoPtr<IInteger32> key;
    CInteger32::New(100, (IInteger32**)&key);
    AutoPtr<ICharSequence> value100;
    CString::New(String("Value 100"), (ICharSequence**)&value100);
    AutoPtr<IInterface> outface;
    h->Put(value100, key, (IInterface**)&outface);
    Int32 sizelen = 0;
    Boolean isflag = FALSE;
    assert((h->GetSize(&sizelen), sizelen) == 1 && (IMap::Probe(h)->ContainsValue(key, &isflag), isflag));

    // Put into "full" table
    h = hashtableClone(htfull);
    h->Put(value100, key, (IInterface**)&outface);
    assert((h->GetSize(&sizelen), sizelen) == 8 && (IMap::Probe(h)->ContainsValue(key, &isflag), isflag));

    // try {
    ECode ec = h->Put(NULL, key, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
    //     //expected
    // }

    // try {
    ec = h->Put(value100, NULL, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_putAllLjava_util_Map(int argc, char* argv[])
{
    PEL("CTest::test_putAllLjava_util_Map")
    // Test for method void java.util.Hashtable.putAll(java.util.Map)
    AutoPtr<IDictionary> h;
    CHashTable::New((IDictionary**)&h);
    assert(0 && "TODO");
    // ((CHashTable*)h.Get())->PutAll(ht10);
    AutoPtr<IEnumeration> e;
    keyVector->Elements((IEnumeration**)&e);
    Boolean isflag = FALSE;
    AutoPtr<IInterface> outface;
    while ((e->HasMoreElements(&isflag), isflag)) {
        AutoPtr<IInterface> x;
PFL
        e->NextElement((IInterface**)&x);
PFL_EX("x: %p",  x.Get())
        h->Get(x, (IInterface**)&outface);
PFL_EX("outface: %p", outface.Get())
        AutoPtr<ICharSequence> res = ICharSequence::Probe(outface);
        String str1;
PFL_EX("res: %p", res.Get())
        res->ToString(&str1);
        ht10->Get(x, (IInterface**)&outface);
PFL
        AutoPtr<ICharSequence> res2 = ICharSequence::Probe(outface);
        String str2;
        res2->ToString(&str2);
        assert(str1.Equals(str2));
    }

    // try {
    ECode ec = IMap::Probe(h)->PutAll(NULL);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException ee) {
        //expected
    // }
    return 0;
}

int CTest::test_removeLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_removeLjava_lang_Object")
    // Test for method java.lang.Object
    // java.util.Hashtable.remove(java.lang.Object)
    AutoPtr<IDictionary> h = hashtableClone(htfull);
    AutoPtr<IInterface> k;
    AutoPtr<ICharSequence> FKey0;
    CString::New(String("FKey 0"), (ICharSequence**)&FKey0);
    h->Remove(FKey0, (IInterface**)&k);
    Boolean isflag = FALSE;
    assert(!(IMap::Probe(h)->ContainsKey(FKey0, &isflag), isflag) || k == NULL);
    h->Remove(FKey0, (IInterface**)&k);
    assert(k == NULL);

    // try {
    ECode ec = h->Remove(NULL, (IInterface**)&k);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
        //expected
    // }
    return 0;
}

int CTest::test_size(int argc, char* argv[])
{
    PEL("CTest::test_size")
    // // Test for method int java.util.Hashtable.size()
    // assertTrue("Returned invalid size", ht10.size() == 10
    //         && (ht100.size() == 0));

    // final Hashtable ht = new Hashtable();
    // ht.put("0", "");
    // Thread t1 = new Thread() {
    //     public void run() {
    //         while (ht.size() > 0)
    //             ;
    //         ht.put("final", "");
    //     }
    // };
    // t1.start();
    // for (int i = 1; i < 10000; i++) {
    //     synchronized (ht) {
    //         ht.remove(String.valueOf(i - 1));
    //         ht.put(String.valueOf(i), "");
    //     }
    //     int size;
    //     if ((size = ht.size()) != 1) {
    //         String result = "Size is not 1: " + size + " " + ht;
    //         // terminate the thread
    //         ht.clear();
    //         fail(result);
    //     }
    // }
    // // terminate the thread
    // ht.clear();
    return 0;
}

int CTest::test_toString(int argc, char* argv[])
{
    PEL("CTest::test_toString")
    // Test for method java.lang.String java.util.Hashtable.toString()
    AutoPtr<IDictionary> h;
    CHashTable::New((IDictionary**)&h);
    // assertEquals("Incorrect toString for Empty table",
    //         "{}", h.toString());
    AutoPtr<ICharSequence> one;
    CString::New(String("one"), (ICharSequence**)&one);
    AutoPtr<ICharSequence> int1;
    CString::New(String("1"), (ICharSequence**)&int1);
    AutoPtr<IInterface> outface;
    h->Put(one, int1, (IInterface**)&outface);
    AutoPtr<ICharSequence> two;
    CString::New(String("two"), (ICharSequence**)&two);
    h->Put(two, h, (IInterface**)&outface);
    AutoPtr<ICharSequence> sq3;
    CString::New(String("3"), (ICharSequence**)&sq3);
    h->Put(h, sq3, (IInterface**)&outface);
    h->Put(h, h, (IInterface**)&outface);
    // String result = h.toString();
    // assertTrue("should contain self ref", result.indexOf("(this") > -1);
    return 0;
}

int CTest::test_values(int argc, char* argv[])
{
    PEL("CTest::test_values")
    // Test for method java.util.Collection java.util.Hashtable.values()
    AutoPtr<ICollection> c;
    assert(0 && "TODO");
    IMap::Probe(ht10)->Values((ICollection**)&c);
    AutoPtr<IEnumeration> e;
    elmVector->Elements((IEnumeration**)&e);
    Boolean isflag = FALSE;
    while ((e->HasMoreElements(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        e->NextElement((IInterface**)&outface);
        assert((c->Contains(outface, &isflag), isflag));
    }

// BEGIN android-removed
// implementation dependent
//        assertEquals("Not synchronized",
//                "java.util.Collections$SynchronizedCollection", c.getClass().getName());
// END android-removed

    AutoPtr<IDictionary> myHashtable;
    CHashTable::New((IDictionary**)&myHashtable);
    for (int i = 0; i < 100; i++) {
        AutoPtr<IInteger32> intmid;
        CInteger32::New(i, (IInteger32**)&intmid);
        AutoPtr<IInterface> outface;
        myHashtable->Put(intmid, intmid, (IInterface**)&outface);
    }
    AutoPtr<ICollection> values;
    IMap::Probe(myHashtable)->Values((ICollection**)&values);
    // new Support_UnmodifiableCollectionTest(
    //         "Test Returned Collection From Hashtable.values()", values)
    //         .runTest();
    AutoPtr<IInteger32> int0;
    CInteger32::New(0, (IInteger32**)&int0);
    values->Remove(int0, &isflag);
    assert(!(IMap::Probe(myHashtable)->ContainsValue(int0, &isflag), isflag));
    return 0;
}


int CTest::test_entrySet_remove(int argc, char* argv[])
{
    PEL("CTest::test_entrySet_remove")
    AutoPtr<IDictionary> hashtable;
    CHashTable::New((IDictionary**)&hashtable);
    AutoPtr<ICharSequence> sqmy;
    CString::New(String("my.nonexistent.prop"), (ICharSequence**)&sqmy);
    AutoPtr<ICharSequence> AAA;
    CString::New(String("AAA"), (ICharSequence**)&AAA);
    AutoPtr<IInterface> outface;
    hashtable->Put(sqmy, AAA, (IInterface**)&outface);
    AutoPtr<ICharSequence> sqpar;
    CString::New(String("parse.error"), (ICharSequence**)&sqpar);
    AutoPtr<ICharSequence> BBB;
    CString::New(String("BBB"), (ICharSequence**)&BBB);
    hashtable->Put(sqpar, BBB, (IInterface**)&outface);
    AutoPtr<IIterator> iterator;
    AutoPtr<ISet> outset;
    assert(0 && "TODO");
    IMap::Probe(hashtable)->EntrySet((ISet**)&outset);
    outset->GetIterator((IIterator**)&iterator);
    Boolean isflag = FALSE;
    while(iterator->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        iterator->Next((IInterface**)&outface);
        AutoPtr<IEntry> entry = IEntry::Probe(outface);
        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        String str;
        ICharSequence::Probe(value)->ToString(&str);
        if(str.Equals("AAA")) {
           iterator->Remove();
        }
    }
    assert(!(IMap::Probe(hashtable)->ContainsKey(sqmy, &isflag), isflag));
    return 0;
}

int CTest::test_rehash(int argc, char* argv[])
{
    PEL("CTest::test_rehash")
    // Mock_Hashtable mht = new Mock_Hashtable(5);

    // assertFalse(mht.isRehashed());
    // for(int i = 0; i < 10; i++) {
    //     mht.put(i, "New value");
    // }
    // assertTrue(mht.isRehashed());
    return 0;
}

AutoPtr<IDictionary> CTest::hashtableClone(AutoPtr<IDictionary> s)
{
    PEL("CTest::hashtableClone")
    AutoPtr<IInterface> outface;
    ICloneable::Probe(s)->Clone((IInterface**)&outface);
    AutoPtr<IDictionary> outdic = IDictionary::Probe(outface);
    return outdic;
}

void CTest::setUp()
{
    CHashTable::New(10, (IDictionary**)&ht10);
    CHashTable::New(100, (IDictionary**)&ht100);
    CHashTable::New(10, (IDictionary**)&htfull);
    CVector::New(10, (IVector**)&keyVector);
    CVector::New(10, (IVector**)&elmVector);

    for (int i = 0; i < 10; i++) {
        AutoPtr<ICharSequence> vali;
        AutoPtr<ICharSequence> keyi;
        CString::New(String("Key") + StringUtils::Int32ToString(i), (ICharSequence**)&keyi);
        CString::New(String("Val") + StringUtils::Int32ToString(i), (ICharSequence**)&vali);
        AutoPtr<IInterface> outface;
        ht10->Put(keyi, vali, (IInterface**)&outface);
        keyVector->AddElement(keyi);
        elmVector->AddElement(vali);
    }

    for (int i = 0; i < 7; i++) {
        AutoPtr<ICharSequence> vali;
        AutoPtr<ICharSequence> keyi;
        CString::New(String("FKey") + StringUtils::Int32ToString(i), (ICharSequence**)&keyi);
        CString::New(String("FVal") + StringUtils::Int32ToString(i), (ICharSequence**)&vali);
        AutoPtr<IInterface> outface;
        htfull->Put(keyi, vali, (IInterface**)&outface);
    }
}

void CTest::tearDown()
{
    ht10 = NULL;
    ht100 = NULL;
    htfull = NULL;
    keyVector = NULL;
    elmVector = NULL;
}
