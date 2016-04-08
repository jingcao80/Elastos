
#include "test.h"

CTest::CTest()
{
    CString::New(String("A"), (ICharSequence**)&csA);
    CString::New(String("a"), (ICharSequence**)&csa);
    CString::New(String("B"), (ICharSequence**)&csB);
    CString::New(String("b"), (ICharSequence**)&csb);
    CString::New(String("C"), (ICharSequence**)&csC);
    CString::New(String("c"), (ICharSequence**)&csc);
    CString::New(String("D"), (ICharSequence**)&csD);
    CString::New(String("d"), (ICharSequence**)&csd);
    CString::New(String("x"), (ICharSequence**)&csx);
    CString::New(String("y"), (ICharSequence**)&csy);
    CString::New(String("z"), (ICharSequence**)&csz);
}

CTest::~CTest()
{}

int CTest::testEntrySetSetValue(int argc, char* argv[])
{
    PEL("CTest::testEntrySetSetValue")
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
PFL_EX("map: %p", map.Get())
    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
PFL
    map->Put(csB, csb, (IInterface**)&outface);
PFL
    map->Put(csC, csc, (IInterface**)&outface);
PFL_EX("outface:%p", outface.Get())
    AutoPtr< ArrayOf<IEntry*> > outarr;
    map->EntrySet((ArrayOf<IEntry*>**)&outarr);
PFL
    AutoPtr<IEntry> entryA = (*outarr)[0];
    outface = NULL;
PFL
    entryA->SetValue(csx, (IInterface**)&outface);
PFL_EX("entryA: %p, outface:%p", entryA.Get(), outface.Get())
    String str;
    AutoPtr<ICharSequence> res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
PFL
    res->ToString(&str);
    assert(String("a") == str);

PFL
    outface = NULL;
    entryA->GetValue((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("x") == str);
PFL
    outface = NULL;
    map->Get(csA, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("x") == str);

    AutoPtr<IEntry> entryB = (*outarr)[1];
    outface = NULL;
    entryB->SetValue(csy, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("b") == str);

    AutoPtr<IEntry> entryC = (*outarr)[2];
    outface = NULL;
    entryC->SetValue(csz, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("c") == str);

    outface = NULL;
    entryB->GetValue((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("y") == str);

    outface = NULL;
    map->Get(csB, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("y") == str);

    outface = NULL;
    entryC->GetValue((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("z") == str);

    outface = NULL;
    map->Get(csC, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("z") == str);
    return 0;
}

int CTest::testSubMapEntrySetSetValue(int argc, char* argv[])
{
    PEL("CTest::testSubMapEntrySetSetValue")
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
    map->Put(csB, csb, (IInterface**)&outface);
    map->Put(csC, csc, (IInterface**)&outface);
    map->Put(csD, csd, (IInterface**)&outface);

PFL
    AutoPtr<INavigableMap> subMap;
    map->SubMapEx(csA, TRUE, csC, TRUE, (INavigableMap**)&subMap);
PFL
    AutoPtr< ArrayOf<IEntry*> > outarr;
    subMap->EntrySet((ArrayOf<IEntry*>**)&outarr);

    AutoPtr<IEntry> entryA = (*outarr)[0];
    outface = NULL;
    entryA->SetValue(csx, (IInterface**)&outface);
    String str;
    AutoPtr<ICharSequence> res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("a") == str);
PFL
    outface = NULL;
    entryA->GetValue((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("x") == str);
PFL
    outface = NULL;
    subMap->Get(csA, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("x") == str);
PFL
    outface = NULL;
    map->Get(csA, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("x") == str);

    AutoPtr<IEntry> entryB = (*outarr)[1];
    outface = NULL;
    entryB->SetValue(csy, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("b") == str);
PFL
    AutoPtr<IEntry> entryC = (*outarr)[2];
    outface = NULL;
    entryB->SetValue(csz, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("c") == str);
PFL
    outface = NULL;
    entryB->GetValue((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("y") == str);
PFL
    outface = NULL;
    subMap->Get(csB, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("y") == str);

    outface = NULL;
    map->Get(csB, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("y") == str);
PFL
    outface = NULL;
    entryC->GetValue((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("z") == str);

    outface = NULL;
    subMap->Get(csC, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("z") == str);
PFL
    outface = NULL;
    map->Get(csC, (IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("z") == str);
PFL
    return 0;
}

int CTest::testExceptionsOnSetValue(int argc, char* argv[])
{
    PEL("CTest::testExceptionsOnSetValue")
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);

    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
    map->Put(csB, csb, (IInterface**)&outface);
    map->Put(csC, csc, (IInterface**)&outface);

    assertAllEntryMethodsReturnImmutableEntries(map);
    return 0;
}

int CTest::testExceptionsOnSubMapSetValue(int argc, char* argv[])
{
    PEL("CTest::testExceptionsOnSubMapSetValue")
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);

    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
    map->Put(csB, csb, (IInterface**)&outface);
    map->Put(csC, csc, (IInterface**)&outface);
    map->Put(csD, csd, (IInterface**)&outface);

    AutoPtr<INavigableMap> outsubmap;
    map->SubMapEx(csA, TRUE, csC, TRUE, (INavigableMap**)&outsubmap);
    assertAllEntryMethodsReturnImmutableEntries(outsubmap);
    return 0;
}

int CTest::testConcurrentModificationDetection(int argc, char* argv[])
{
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);

    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
    map->Put(csB, csb, (IInterface**)&outface);
    map->Put(csC, csc, (IInterface**)&outface);

    AutoPtr< ArrayOf<IEntry*> > outarr;
    map->EntrySet((ArrayOf<IEntry*>**)&outarr);
    // Iterator<Entry<String,String>> iterator = map.entrySet().iterator();
    // iterator.next();
    // iterator.next();
    // iterator.remove();
    map->Remove(csB, (IInterface**)&outface);

    map->Put(csD, csd, (IInterface**)&outface);
    // try {
    // iterator.next();
    // fail();
    // } catch (ConcurrentModificationException e) {
    // }
    return 0;
}

int CTest::testIteratorRemoves(int argc, char* argv[])
{
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);

    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
    map->Put(csB, csb, (IInterface**)&outface);
    map->Put(csC, csc, (IInterface**)&outface);

    AutoPtr< ArrayOf<IEntry*> > outarr;
    map->EntrySet((ArrayOf<IEntry*>**)&outarr);

    AutoPtr<IEntry> entryA = (*outarr)[0];
    outface = NULL;
    String str;
    entryA->GetKey((IInterface**)&outface);
    AutoPtr<ICharSequence> res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("A") == str);

    AutoPtr<IEntry> entryB = (*outarr)[1];
    outface = NULL;
    entryB->GetKey((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("B") == str);

    // iterator.remove();
    map->Remove(csB, (IInterface**)&outface);//todo

    AutoPtr<IEntry> entryC = (*outarr)[2];
    outface = NULL;
    entryC->GetKey((IInterface**)&outface);
    res = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (res == NULL) {
        printf("res == NULL  error ~~~~~~\n");
        return -1;
    }
    res->ToString(&str);
    assert(String("C") == str);

    // iterator.remove();
    // assertFalse(iterator.hasNext());
    return 0;
}

int CTest::testEntrySetUsesComparatorOnly(int argc, char* argv[])
{
    // Map<String,String> map = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);

    AutoPtr<IInterface> outface;
    AutoPtr<ICharSequence> csABC;
    CString::New(String("ABC"), (ICharSequence**)&csABC);
    map->Put(csABC, csa, (IInterface**)&outface);

    // assertTrue(map.entrySet().contains(new SimpleEntry<String, String>("abc", "a")));
    // assertTrue(map.entrySet().remove(new SimpleEntry<String, String>("abc", "a")));
    // assertEquals(0, map.size());
    return 0;
}

int CTest::testMapConstructorPassingSortedMap(int argc, char* argv[])
{
    // Map<String,String> source = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);

    AutoPtr<IMap> source = (IMap*) map->Probe(EIID_IMap);

    AutoPtr<INavigableMap> copy;
    CTreeMap::New(source, (INavigableMap**)&copy);

    AutoPtr<IComparator> outcom;
    copy->Comparator((IComparator**)&outcom);
    assert(NULL == outcom);
    return 0;
}

int CTest::testNullsWithNaturalOrder(int argc, char* argv[])
{
    AutoPtr<IMap> copyFrom;
    CHashMap::New((IMap**)&copyFrom);
    AutoPtr<IInterface> outface;
    copyFrom->Put(NULL, csb, (IInterface**)&outface);
    // try {
    AutoPtr<INavigableMap> map;
    ECode ec = CTreeMap::New(copyFrom, (INavigableMap**)&map);
    // fail(); // the RI doesn't throw if null is the only key
    if (ec != NOERROR) {
        printf("expected error 1 ~~~~~~~~~~~~\n");
    }
    // } catch (NullPointerException expected) {
    // }

    map = NULL;
    CTreeMap::New((INavigableMap**)&map);
    // try {
    ec = map->Put(NULL, csb, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf(" expected error ~~~~~~~~~\n");
    }
    // fail();
    // } catch (NullPointerException e) {
    // }

    // try {
    AutoPtr<INavigableMap> desmap;
    map->DescendingMap((INavigableMap**)&desmap);
    ec = desmap->Put(NULL, csb, (IInterface**)&outface);
    // fail();
    if (ec != NOERROR) {
        printf("expected error ~~~~~~~~\n");
    }
    // } catch (NullPointerException e) {
    // }

    // try {
    AutoPtr<ISortedMap> sortmap;
    map->SubMap(csa, csz, (ISortedMap**)&sortmap);
    ec = sortmap->Put(NULL, csb, (IInterface**)&outface);
    // fail();
    if (ec != NOERROR) {
        printf("expected error ~~~~~~~~~~~~\n");
    }
    // } catch (NullPointerException e) {
    // }
    return 0;
}

int CTest::testClassCastExceptions(int argc, char* argv[])
{
    AutoPtr<IMap> map;
    AutoPtr<INavigableMap> namap;
    CTreeMap::New((INavigableMap**)&namap);
    map = (IMap*) namap->Probe(EIID_IMap);
    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
    // try {
    AutoPtr<IInteger32> Integer5;
    CInteger32::New(5, (IInteger32**)&Integer5);
    ECode ec = map->Get(Integer5, (IInterface**)&outface);
    // fail();
    if (ec != NOERROR) {
        printf("expected error ~~~~~~~~~~~~\n");
    }
    // } catch (ClassCastException e) {
    // }
    // try {
    Boolean isflag = FALSE;
    ec = map->ContainsKey(Integer5, &isflag);
    // fail();
    if (ec != NOERROR || !isflag) {
        printf("expected error ~~~~~~~~~~~~\n");
    }
    // } catch (ClassCastException e) {
    // }
    // try {
    ec = map->Remove(Integer5, (IInterface**)&outface);
    // fail();
    if (ec != NOERROR) {
        printf("expected error ~~~~~~~~~~~~\n");
    }
    // } catch (ClassCastException e) {
    // }
    return 0;
}

int CTest::testClassCastExceptionsOnBounds(int argc, char* argv[])
{
    AutoPtr<IMap> map;
    AutoPtr<INavigableMap> namap;
    CTreeMap::New((INavigableMap**)&namap);
    AutoPtr<ISortedMap> sortmap;
    namap->SubMap(csa, csz, (ISortedMap**)&sortmap);
    map = (IMap*) sortmap->Probe(EIID_IMap);
    // try {
    AutoPtr<IInterface> outface;
    AutoPtr<IInteger32> Integer5;
    CInteger32::New(5, (IInteger32**)&Integer5);
    ECode ec = map->Get(Integer5, (IInterface**)&outface);
    // fail();
    if (ec != NOERROR) {
        printf("expected error ~~~~~~~~~~~~\n");
    }
    // } catch (ClassCastException e) {
    // }
    // try {
    Boolean isflag = FALSE;
    ec = map->ContainsKey(Integer5, &isflag);
    // fail();
    if (ec != NOERROR || !isflag) {
        printf("expected error ~~~~~~~~~~~~\n");
    }
    // } catch (ClassCastException e) {
    // }
    // try {
    ec = map->Remove(Integer5, (IInterface**)&outface);
    // fail();
    if (ec != NOERROR) {
        printf("expected error ~~~~~~~~~~~~\n");
    }
    // } catch (ClassCastException e) {
    // }
    return 0;
}

int CTest::testClone(int argc, char* argv[])
{
    // TreeMap<String, String> map = new TreeMap<String, String>() {
    //     @Override public String toString() {
    //         return "x:" + super.toString();
    //     }
    // };
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csA, csa, (IInterface**)&outface);
    map->Put(csB, csb, (IInterface**)&outface);

    // @SuppressWarnings("unchecked")
    AutoPtr<IInterface> clone;
    AutoPtr<ICloneable> mapclone = (ICloneable*) map->Probe(EIID_ICloneable);
    if (mapclone == NULL) {
        printf("clone == NULL error~~~~~~~~\n");
        return -1;
    }
    mapclone->Clone((IInterface**)&clone);
    // assertEquals(map.getClass(), clone.getClass());
    // assertEquals("x:{A=a, B=b}", map.toString());
    return 0;
}

int CTest::testEmptyMapSerialization(int argc, char* argv[])
{
    String s = String("aced0005737200116a6176612e7574696c2e547265654d61700cc1f63e2d256a")
            + String("e60300014c000a636f6d70617261746f727400164c6a6176612f7574696c2f436")
            + String("f6d70617261746f723b78707077040000000078");
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    // new SerializationTester<TreeMap<String, String>>(map, s).test();
    return 0;
}

int CTest::testSerializationWithComparator(int argc, char* argv[])
{
    String s = String("aced0005737200116a6176612e7574696c2e547265654d61700cc1f63e2d256a")
            + String("e60300014c000a636f6d70617261746f727400164c6a6176612f7574696c2f436")
            + String("f6d70617261746f723b78707372002a6a6176612e6c616e672e537472696e6724")
            + String("43617365496e73656e736974697665436f6d70617261746f7277035c7d5c50e5c")
            + String("e02000078707704000000027400016171007e00057400016271007e000678");
    // TreeMap<String, String> map = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csa, csa, (IInterface**)&outface);
    map->Put(csb, csb, (IInterface**)&outface);
    // new SerializationTester<NavigableMap<String, String>>(map, s) {
    //     @Override protected void verify(NavigableMap<String, String> deserialized) {
    //         assertEquals(0, deserialized.comparator().compare("X", "x"));
    //     }
    // }.test();
    return 0;
}

int CTest::testSubMapSerialization(int argc, char* argv[])
{
    String s = String("aced0005737200216a6176612e7574696c2e547265654d617024417363656e646")
            + String("96e675375624d61700cab946d1f0fab1c020000787200216a6176612e7574696c2")
            + String("e547265654d6170244e6176696761626c655375624d6170e2d0a70e64210e08020")
            + String("0075a000966726f6d53746172745a000b6869496e636c75736976655a000b6c6f4")
            + String("96e636c75736976655a0005746f456e644c000268697400124c6a6176612f6c616")
            + String("e672f4f626a6563743b4c00026c6f71007e00024c00016d7400134c6a6176612f7")
            + String("574696c2f547265654d61703b7870000001007400016374000161737200116a617")
            + String("6612e7574696c2e547265654d61700cc1f63e2d256ae60300014c000a636f6d706")
            + String("17261746f727400164c6a6176612f7574696c2f436f6d70617261746f723b78707")
            + String("372002a6a6176612e6c616e672e537472696e672443617365496e73656e7369746")
            + String("97665436f6d70617261746f7277035c7d5c50e5ce0200007870770400000004710")
            + String("07e000671007e00067400016271007e000c71007e000571007e000574000164710")
            + String("07e000d78");
    // TreeMap<String, String> map = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csa, csa, (IInterface**)&outface);
    map->Put(csb, csb, (IInterface**)&outface);
    map->Put(csc, csc, (IInterface**)&outface);
    map->Put(csd, csd, (IInterface**)&outface);
    AutoPtr<ISortedMap> subMap;
    map->SubMap(csa, csc, (ISortedMap**)&subMap);
    // new SerializationTester<SortedMap<String, String>>(subMap, s) {
    //     @Override protected void verify(SortedMap<String, String> deserialized) {
    //         try {
    //             deserialized.put("e", "e");
    //             fail();
    //         } catch (IllegalArgumentException expected) {
    //         }
    //     }
    // }.test();
    return 0;
}

int CTest::testNavigableSubMapSerialization(int argc, char* argv[])
{
    String s = String("aced0005737200216a6176612e7574696c2e547265654d617024417363656e646")
            + String("96e675375624d61700cab946d1f0fab1c020000787200216a6176612e7574696c2")
            + String("e547265654d6170244e6176696761626c655375624d6170e2d0a70e64210e08020")
            + String("0075a000966726f6d53746172745a000b6869496e636c75736976655a000b6c6f4")
            + String("96e636c75736976655a0005746f456e644c000268697400124c6a6176612f6c616")
            + String("e672f4f626a6563743b4c00026c6f71007e00024c00016d7400134c6a6176612f7")
            + String("574696c2f547265654d61703b7870000100007400016374000161737200116a617")
            + String("6612e7574696c2e547265654d61700cc1f63e2d256ae60300014c000a636f6d706")
            + String("17261746f727400164c6a6176612f7574696c2f436f6d70617261746f723b78707")
            + String("372002a6a6176612e6c616e672e537472696e672443617365496e73656e7369746")
            + String("97665436f6d70617261746f7277035c7d5c50e5ce0200007870770400000004710")
            + String("07e000671007e00067400016271007e000c71007e000571007e000574000164710")
            + String("07e000d78");
    // TreeMap<String, String> map = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csa, csa, (IInterface**)&outface);
    map->Put(csb, csb, (IInterface**)&outface);
    map->Put(csc, csc, (IInterface**)&outface);
    map->Put(csd, csd, (IInterface**)&outface);
    AutoPtr<INavigableMap> subMap;
    map->SubMapEx(csa, FALSE, csc,TRUE, (INavigableMap**)&subMap);
    // new SerializationTester<SortedMap<String, String>>(subMap, s) {
    //     @Override protected void verify(SortedMap<String, String> deserialized) {
    //         try {
    //             deserialized.put("e", "e");
    //             fail();
    //         } catch (IllegalArgumentException expected) {
    //         }
    //     }
    // }.test();
    return 0;
}

int CTest::testDescendingMapSerialization(int argc, char* argv[])
{
    String s = String("aced0005737200226a6176612e7574696c2e547265654d61702444657363656e6")
            + String("4696e675375624d61700cab946d1f0f9d0c0200014c001172657665727365436f6")
            + String("d70617261746f727400164c6a6176612f7574696c2f436f6d70617261746f723b7")
            + String("87200216a6176612e7574696c2e547265654d6170244e6176696761626c6553756")
            + String("24d6170e2d0a70e64210e080200075a000966726f6d53746172745a000b6869496")
            + String("e636c75736976655a000b6c6f496e636c75736976655a0005746f456e644c00026")
            + String("8697400124c6a6176612f6c616e672f4f626a6563743b4c00026c6f71007e00034")
            + String("c00016d7400134c6a6176612f7574696c2f547265654d61703b787001010101707")
            + String("0737200116a6176612e7574696c2e547265654d61700cc1f63e2d256ae60300014")
            + String("c000a636f6d70617261746f7271007e000178707372002a6a6176612e6c616e672")
            + String("e537472696e672443617365496e73656e736974697665436f6d70617261746f727")
            + String("7035c7d5c50e5ce02000078707704000000027400016171007e000a74000162710")
            + String("07e000b78737200286a6176612e7574696c2e436f6c6c656374696f6e732452657")
            + String("665727365436f6d70617261746f7232000003fa6c354d510200014c0003636d707")
            + String("1007e0001787071007e0009");
    // TreeMap<String, String> map = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csa, csa, (IInterface**)&outface);
    map->Put(csb, csb, (IInterface**)&outface);
    AutoPtr<INavigableMap> descendingMap;
    map->DescendingMap((INavigableMap**)&descendingMap);
    // new SerializationTester<NavigableMap<String, String>>(descendingMap, s) {
    //     @Override protected void verify(NavigableMap<String, String> deserialized) {
    //         assertEquals("b", deserialized.navigableKeySet().first());
    //     }
    // }.test();
    return 0;
}

int CTest::testJava5SerializationWithComparator(int argc, char* argv[])
{
    String s = String("aced0005737200116a6176612e7574696c2e547265654d61700cc1f63e2d256a")
            + String("e60300014c000a636f6d70617261746f727400164c6a6176612f7574696c2f436")
            + String("f6d70617261746f723b78707372002a6a6176612e6c616e672e537472696e6724")
            + String("43617365496e73656e736974697665436f6d70617261746f7277035c7d5c50e5c")
            + String("e02000078707704000000027400016171007e00057400016271007e000678");
    // TreeMap<String, String> map = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csa, csa, (IInterface**)&outface);
    map->Put(csb, csb, (IInterface**)&outface);
    // new SerializationTester<TreeMap<String, String>>(map, s) {
    //     @Override protected void verify(TreeMap<String, String> deserialized) {
    //         assertEquals(0, deserialized.comparator().compare("X", "x"));
    //     }
    // }.test();
    return 0;
}

int CTest::testJava5SubMapSerialization(int argc, char* argv[])
{
    String s = String("aced0005737200186a6176612e7574696c2e547265654d6170245375624d6170")
            + String("a5818343a213c27f0200055a000966726f6d53746172745a0005746f456e644c0")
            + String("00766726f6d4b65797400124c6a6176612f6c616e672f4f626a6563743b4c0006")
            + String("7468697324307400134c6a6176612f7574696c2f547265654d61703b4c0005746")
            + String("f4b657971007e00017870000074000161737200116a6176612e7574696c2e5472")
            + String("65654d61700cc1f63e2d256ae60300014c000a636f6d70617261746f727400164")
            + String("c6a6176612f7574696c2f436f6d70617261746f723b78707372002a6a6176612e")
            + String("6c616e672e537472696e672443617365496e73656e736974697665436f6d70617")
            + String("261746f7277035c7d5c50e5ce020000787077040000000471007e000471007e00")
            + String("047400016271007e000a7400016371007e000b7400016471007e000c7871007e0")
            + String("00b");
    // TreeMap<String, String> map = new TreeMap<String, String>(String.CASE_INSENSITIVE_ORDER);
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    AutoPtr<IInterface> outface;
    map->Put(csa, csa, (IInterface**)&outface);
    map->Put(csb, csb, (IInterface**)&outface);
    map->Put(csc, csc, (IInterface**)&outface);
    map->Put(csd, csd, (IInterface**)&outface);
    AutoPtr<ISortedMap> subMap;
    map->SubMap(csa, csc, (ISortedMap**)&subMap);
    // new SerializationTester<SortedMap<String, String>>(subMap, s) {
    //     @Override protected void verify(SortedMap<String, String> deserialized) {
    //         try {
    //             deserialized.put("e", "e");
    //             fail();
    //         } catch (IllegalArgumentException expected) {
    //         }
    //     }
    // }.test();
    return 0;
}

void CTest::assertAllEntryMethodsReturnImmutableEntries(INavigableMap* map)
{
    PEL("CTest::assertAllEntryMethodsReturnImmutableEntries")
    AutoPtr<IEntry> outent;
PFL
    assertImmutable((map->CeilingEntry(csB, (IEntry**)&outent), outent));
PFL
    assertImmutable((map->FirstEntry((IEntry**)&outent), outent));
PFL
    assertImmutable((map->FloorEntry(csD, (IEntry**)&outent), outent));
PFL
    // assertImmutable((map->HigherEntry(csA, (IEntry**)&outent), outent));
PFL
    assertImmutable((map->LastEntry((IEntry**)&outent), outent));
PFL
    assertImmutable((map->LowerEntry(csC, (IEntry**)&outent), outent));
PFL
    assertImmutable((map->PollFirstEntry((IEntry**)&outent), outent));
PFL
    assertImmutable((map->PollLastEntry((IEntry**)&outent), outent));
}

void CTest::assertImmutable(IEntry* entry)
{
    PEL("CTest::assertImmutable")
    String initialValue;
    AutoPtr<IInterface> outface;
    entry->GetValue((IInterface**)&outface);
    AutoPtr<ICharSequence> squen = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (squen == NULL) {
        printf("squen == NULL ~~~~~~~\n");
        return;
    }
    squen->ToString(&initialValue);
    // try {
    AutoPtr<IInterface> setface;
    ECode ec = entry->SetValue(csx, (IInterface**)&setface);
    if (ec != NOERROR) {
        printf("error expected ~~~~~\n");
    }
    // fail();
    // } catch (UnsupportedOperationException e) {
    // }
    String str;
    entry->GetValue((IInterface**)&outface);
    squen = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    if (squen == NULL) {
        printf("squen == NULL ~~~~~~~\n");
        return;
    }
    squen->ToString(&str);
PFL_EX("initialValue: %s, str: %s", initialValue.string(), str.string())
    assert(initialValue == str);
}

