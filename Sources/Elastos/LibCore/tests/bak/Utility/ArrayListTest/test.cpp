
#include "test.h"

CTest::CTest(){
    setUp();
    CString::New(String("yoink"), (ICharSequence**)&youink);
    CString::New(String("kazoo"), (ICharSequence**)&kazoo);
}

CTest::~CTest(){
    tearDown();
}

int CTest::test_Constructor(int argc, char* argv[])
{
    PEL("CTest::test_Constructor")
    // Test for method java.util.ArrayList()
    // new Support_ListTest("", alist).runTest();

    AutoPtr<IArrayList> subList;
    CArrayList::New((IArrayList**)&subList);
    for (int i = -50; i < 150; i++) {
        AutoPtr<IInteger32> intface;
        CInteger32::New(i, (IInteger32**)&intface);
        Boolean isflag =  FALSE;
        subList->Add(intface, &isflag);
    }
    // new Support_ListTest("", subList.subList(50, 150)).runTest();
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    // Test for method java.util.ArrayList(int)
    AutoPtr<IArrayList> al;
    CArrayList::New(5, (IArrayList**)&al);
    Int32 sizelen = 0;
    al->GetSize(&sizelen);
    // "Incorrect arrayList created"
    assert(0 == sizelen);

    // try {
    ECode ec = CArrayList::New(-10, (IArrayList**)&al);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_ConstructorLjava_util_Collection(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_util_Collection")
    // Test for method java.util.ArrayList(java.util.Collection)
    AutoPtr<IArrayList> al; // = new ArrayList(Arrays.asList(objArray));
    CArrayList::New(alist, (IArrayList**)&al);
    Int32 sizelen = 0;
    assert((al->GetSize(&sizelen), sizelen) == objArray->GetLength());
    for (int counter = 0; counter < objArray->GetLength(); counter++) {
        AutoPtr<IInterface> outface;
        assert((al->Get(counter, (IInterface**)&outface), outface) == (*objArray)[counter]);
    }
    // try {
    ECode ec = CArrayList::New(NULL, (IArrayList**)&al);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
        //expected
    // }
    return 0;
}

int CTest::test_addILjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_addILjava_lang_Object")
    // Test for method void java.util.ArrayList.add(int, java.lang.Object)
    AutoPtr<IInterface> o;
    Boolean isflag = FALSE;
    ECode ec = alist->AddEx(50, o);

    for (int i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<IInterface> outface;
        alist->Get(i, (IInterface**)&outface);
    }

    AutoPtr<IInterface> outface;
    assert((alist->Get(50, (IInterface**)&outface), outface) == o);
    assert((alist->Get(51, (IInterface**)&outface), outface) == (*objArray)[50]
          && ((alist->Get(52, (IInterface**)&outface), outface) == (*objArray)[51]));
    AutoPtr<IInterface> oldItem;
    alist->Get(25, (IInterface**)&oldItem);
    alist->AddEx(25, NULL);
    assert((alist->Get(25, (IInterface**)&outface), outface) == NULL);
    assert((alist->Get(26, (IInterface**)&outface), outface) == oldItem);

    // try {
    ec = alist->AddEx(-1, NULL);
    if (ec != NOERROR) {
        printf("IndexOutOfBoundsException expected\n");
    }
    // } catch (IndexOutOfBoundsException e) {
        //expected
    // }

    // try {
    Int32 sizelen = 0;
    ec = alist->AddEx((alist->GetSize(&sizelen), sizelen) + 1, NULL);
    if (ec != NOERROR) {
        printf("IndexOutOfBoundsException expected\n");
    }
    // } catch (IndexOutOfBoundsException e) {
        //expected
    // }
    return 0;
}

int CTest::test_addLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_addLjava_lang_Object")
    // Test for method boolean java.util.ArrayList.add(java.lang.Object)
    AutoPtr<IInterface> o; // = new Object();
    Boolean isflag = FALSE;
    alist->Add(o, &isflag);
    Int32 sizelen = 0;
    AutoPtr<IInterface> outface;
    assert((alist->Get((alist->GetSize(&sizelen), sizelen) - 1, (IInterface**)&outface), outface) == o);
    alist->Add(NULL, &isflag);
    assert((alist->Get((alist->GetSize(&sizelen), sizelen) - 1, (IInterface**)&outface), outface) == NULL);
    return 0;
}

int CTest::test_addAllILjava_util_Collection(int argc, char* argv[])
{
    PEL("CTest::test_addAllILjava_util_Collection")
    // Test for method boolean java.util.ArrayList.addAll(int,
    // java.util.Collection)
    Boolean isflag = FALSE;
    alist->AddAllEx(50, alist, &isflag);
    Int32 sizelen = 0;
    assert(200 == (alist->GetSize(&sizelen), sizelen));
    AutoPtr<IInterface> outface;
    for (int i = 0; i < 50; i++){
        assert((alist->Get(i, (IInterface**)&outface), outface) == (*objArray)[i]);
    }
    for (int i = 0; i >= 50 && (i < 150); i++) {
        assert((alist->Get(i, (IInterface**)&outface), outface) == (*objArray)[i - 50]);
    }
    for (int i = 0; i >= 150 && (i < 200); i++) {
        assert((alist->Get(i, (IInterface**)&outface), outface) == (*objArray)[i - 100]);
    }
    AutoPtr<IArrayList> listWithNulls;
    CArrayList::New((IArrayList**)&listWithNulls);
    listWithNulls->Add(NULL, &isflag);
    listWithNulls->Add(NULL, &isflag);
    listWithNulls->Add(youink, &isflag);
    listWithNulls->Add(kazoo, &isflag);
    listWithNulls->Add(NULL, &isflag);
    alist->AddAllEx(100, listWithNulls, &isflag);
    assert((alist->GetSize(&sizelen), sizelen) == 205);
    assert((alist->Get(100, (IInterface**)&outface), outface) == NULL);
    assert((alist->Get(101, (IInterface**)&outface), outface) == NULL);
    alist->Get(102, (IInterface**)&outface);
    AutoPtr<ICharSequence> outcs = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    String str;
    outcs->ToString(&str);
    assert(String("yoink") == str);

    alist->Get(103, (IInterface**)&outface);
    outcs = (ICharSequence*) outface->Probe(EIID_ICharSequence);
    outcs->ToString(&str);
    assert(String("kazoo") == str);

    assert((alist->Get(104, (IInterface**)&outface), outface) == NULL);
    alist->AddAllEx(205, listWithNulls, &isflag);
    assert((alist->GetSize(&sizelen), sizelen) == 210);

    // try {
    ECode ec = alist->AddAllEx(-1, listWithNulls, &isflag);
    if (ec != NOERROR) {
        printf("IndexOutOfBoundsException expected\n");
    }
    // } catch (IndexOutOfBoundsException e) {
    //     //expected
    // }

    // try {
    ec = alist->AddAllEx((alist->GetSize(&sizelen), sizelen) + 1, listWithNulls, &isflag);
    if (ec != NOERROR) {
        printf("IndexOutOfBoundsException expected\n");
    }
    // } catch (IndexOutOfBoundsException e) {
    //     //expected
    // }

    // try {
    ec = alist->AddAll(0, NULL);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_addAllLjava_util_Collection(int argc, char* argv[])
{
    PEL("CTest::test_addAllLjava_util_Collection")
    // Test for method boolean
    // java.util.ArrayList.addAll(java.util.Collection)
    AutoPtr<IArrayList> l;
    CArrayList::New((IArrayList**)&l);
    Boolean isflag = FALSE;
    l->AddAll(alist, &isflag);
    Int32 sizelen = 0;
    AutoPtr<IInterface> outface;
    AutoPtr<IInterface> outface2;
    for (int i = 0; i < (alist->GetSize(&sizelen), sizelen); i++) {
        l->Get(i, (IInterface**)&outface);
        alist->Get(i, (IInterface**)&outface2);
        assert(outface == outface2);
    }

    alist->AddAll(alist, &isflag);
    assert(200 == (alist->GetSize(&sizelen), sizelen));
    for (int i = 0; i < 100; i++) {
        alist->Get(i, (IInterface**)&outface);
        l->Get(i, (IInterface**)&outface2);
        assert(outface == outface2);
        alist->Get(i + 100, (IInterface**)&outface);
        assert(outface == outface2);
    }

//     AutoPtr<ISet> setWithNulls;
//     CHashSet::New((ISet**)&setWithNulls);
//     setWithNulls->Add(NULL, &isflag);
//     setWithNulls->Add(NULL, &isflag);
//     setWithNulls->Add(youink, &isflag);
//     setWithNulls->Add(kazoo, &isflag);
//     setWithNulls->Add(NULL, &isflag);
//     alist->AddAllEx(100, setWithNulls, &isflag);
//     AutoPtr<IIterator> i;
//     setWithNulls->GetIterator((IIterator**)&i);
//     alist->Get(100, (IInterface**)&outface);
//     i->Next((IInterface**)&outface2);
//     assert(outface == outface2);
//     alist->Get(101, (IInterface**)&outface);
//     i->Next((IInterface**)&outface2);
//     assert(outface == outface2);
//     alist->Get(102, (IInterface**)&outface);
//     i->Next((IInterface**)&outface2);
//     assert(outface == outface2);

    // Regression test for Harmony-3481
    AutoPtr<IArrayList> originalList;
    CArrayList::New(12, (IArrayList**)&originalList);
    for (int j = 0; j < 12; j++) {
        AutoPtr<IInteger32> intface;
        CInteger32::New(j, (IInteger32**)&intface);
        originalList->Add(intface, &isflag);
    }

    AutoPtr<IInteger32> intface;
    originalList->RemoveEx(0, (IInterface**)&outface);
    originalList->RemoveEx(0, (IInterface**)&outface);

    AutoPtr<IArrayList> additionalList;
    CArrayList::New(11, (IArrayList**)&additionalList);
    for (int j = 0; j < 11; j++) {
        CInteger32::New(j, (IInteger32**)&intface);
        additionalList->Add(intface, &isflag);
    }
    assert((originalList->AddAll(additionalList, &isflag), isflag));
    assert(21 == (originalList->GetSize(&sizelen), sizelen));

    // try {
    ECode ec = alist->AddAll(NULL, &isflag);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
        //expected
    // }
    return 0;
}

int CTest::test_clear(int argc, char* argv[])
{
    PEL("CTest::test_clear")
    // Test for method void java.util.ArrayList.clear()
    alist->Clear();
    Int32 sizelen = 0;
    assert(0 == (alist->GetSize(&sizelen), sizelen));
    Boolean isflag = FALSE;
    alist->Add(NULL, &isflag);
    alist->Add(NULL, &isflag);
    alist->Add(NULL, &isflag);
    AutoPtr<ICharSequence> bam;
    CString::New(String("bam"), (ICharSequence**)&bam);
    alist->Add(bam, &isflag);
    alist->Clear();
    assert(0 == (alist->GetSize(&sizelen), sizelen));
    /*
     * for (int i = 0; i < alist.size(); i++) assertNull("Failed to clear
     * list", alist.get(i));
     */
    return 0;
}

int CTest::test_clone(int argc, char* argv[])
{
    PEL("CTest::test_clone")
    // Test for method java.lang.Object java.util.ArrayList.clone()
    AutoPtr<IArrayList> x;
    AutoPtr<ICloneable> listclone = (ICloneable*) alist->Probe(EIID_ICloneable);
    listclone->Clone((IInterface**)&x);
    Boolean isflag = FALSE;
    assert((x->Equals(alist, &isflag), isflag));
    Int32 sizelen = 0;
    AutoPtr<IInterface> outface;
    AutoPtr<IInterface> outface2;
    for (int i = 0; i < (alist->GetSize(&sizelen), sizelen); i++) {
        alist->Get(i, (IInterface**)&outface);
        x->Get(i, (IInterface**)&outface2);
        assert(outface == outface2);
    }

    alist->Add(NULL, &isflag);
    alist->AddEx(25, NULL);
    listclone->Clone((IInterface**)&x);
    assert((x->Equals(alist, &isflag), isflag));
    for (int i = 0; i < (alist->GetSize(&sizelen), sizelen); i++) {
        alist->Get(i, (IInterface**)&outface);
        x->Get(i, (IInterface**)&outface2);
        assert(outface == outface2);
    }
    return 0;
}

int CTest::test_containsLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsLjava_lang_Object")
    // Test for method boolean
    // java.util.ArrayList.contains(java.lang.Object)
    Boolean isflag = FALSE;
    assert((alist->Contains((*objArray)[99], &isflag), isflag));
    AutoPtr<IInteger32> intface;
    CInteger32::New(8, (IInteger32**)&intface);
    assert((alist->Contains(intface, &isflag), isflag));
    AutoPtr<IInterface> outface;
    assert((alist->Contains(outface, &isflag), !isflag));
    assert((alist->Contains(NULL, &isflag), !isflag));
    alist->Add(NULL, &isflag);
    assert((alist->Contains(NULL, &isflag), isflag));
    return 0;
}

int CTest::test_ensureCapacityI(int argc, char* argv[])
{
    PEL("CTest::test_ensureCapacityI")
    // Test for method void java.util.ArrayList.ensureCapacity(int)
    // TODO : There is no good way to test this as it only really impacts on
    // the private implementation.

    AutoPtr<IInteger32> testObject;
    CInteger32::New(10001, (IInteger32**)&testObject);
    int capacity = 20;
    AutoPtr<IArrayList> al;
    CArrayList::New(capacity, (IArrayList**)&al);
    int i = 0;
    for (i = 0; i < capacity / 2; i++) {
        AutoPtr<IInteger32> testObject1;
        CInteger32::New(1000 + i, (IInteger32**)&testObject1);
        al->AddEx(i, testObject1);
    }
    al->AddEx(i, testObject);
    int location = 0;
    al->IndexOf(testObject, &location);
    // try {
    al->EnsureCapacity(capacity);
    Int32 location2 = 0;
    assert(location == (al->IndexOf(testObject, &location2), location2));
    Boolean isflag = FALSE;
    AutoPtr<IInterface> outface;
    ECode ec = al->RemoveEx(0, (IInterface**)&outface);
PFL_EX("ec: %p, outface: %p", ec, outface.Get())
    al->EnsureCapacity(capacity);
PFL_EX("location: %d, location2: %d", location, (al->IndexOf(testObject, &location2), location2))
    assert(--location == (al->IndexOf(testObject, &location2), location2));
    al->EnsureCapacity(capacity + 2);
    assert(location == (al->IndexOf(testObject, &location2), location2));
    // } catch (Exception e) {
    //     fail("Exception during test : " + e.getMessage());
    // }
    return 0;
}

int CTest::test_getI(int argc, char* argv[])
{
    PEL("CTest::test_getI")
    // Test for method java.lang.Object java.util.ArrayList.get(int)
    AutoPtr<IInterface> outface;
    assert((alist->Get(22, (IInterface**)&outface), outface) == (*objArray)[22]);
    // try {
    ECode ec = alist->Get(8765, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("Failed to throw expected exception for index > size\n");
    }
    // } catch (IndexOutOfBoundsException e) {
    // }
    return 0;
}

int CTest::test_indexOfLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_indexOfLjava_lang_Object")
    // Test for method int java.util.ArrayList.indexOf(java.lang.Object)
    Int32 location = 0;
    AutoPtr<IInterface> outface;
    assert(87 == (alist->IndexOf((*objArray)[87], &location), location));
    assert(-1 == (alist->IndexOf(outface, &location), location));
    alist->AddEx(25, NULL);
    alist->AddEx(50, NULL);
    assert((alist->IndexOf(NULL, &location), location) == 25);
    return 0;
}

int CTest::test_isEmpty(int argc, char* argv[])
{
    PEL("CTest::test_isEmpty")
    // Test for method boolean java.util.ArrayList.isEmpty()
    AutoPtr<IArrayList> al;
    CArrayList::New((IArrayList**)&al);
    Boolean isflag = FALSE;
    assert((al->IsEmpty(&isflag), isflag));
    assert(!(alist->IsEmpty(&isflag), isflag));
    return 0;
}

int CTest::test_lastIndexOfLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_lastIndexOfLjava_lang_Object")
    // Test for method int java.util.ArrayList.lastIndexOf(java.lang.Object)
    AutoPtr<IInteger32> intface;
    CInteger32::New(99, (IInteger32**)&intface);
    Boolean isflag = FALSE;
    alist->Add(intface, &isflag);
    Int32 location = 0;
    assert(100 == (alist->LastIndexOf((*objArray)[99], &location), location));
    AutoPtr<IInterface> outface;
    assert(-1 == (alist->LastIndexOf(outface, &location), location));
    alist->AddEx(25, NULL);
    alist->AddEx(50, NULL);
    assert((alist->LastIndexOf(NULL, &location), location) == 50);
    return 0;
}

int CTest::test_removeI(int argc, char* argv[])
{
    PEL("CTest::test_removeI")
    // Test for method java.lang.Object java.util.ArrayList.remove(int)
    Boolean isflag = FALSE;
    AutoPtr<IInterface> outface;
    alist->RemoveEx(10, (IInterface**)&outface);
    Int32 location = 0;
    assert(-1 == (alist->IndexOf((*objArray)[10], &location), location));
    // try {
    ECode ec = alist->RemoveEx(999, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("Failed to throw exception when index out of range\n");
    }
    // } catch (IndexOutOfBoundsException e) {
    // }

    AutoPtr<IArrayList> myList;
    AutoPtr<ICloneable> listclone = (ICloneable*) alist->Probe(EIID_ICloneable);
    listclone->Clone((IInterface**)&myList);
    alist->AddEx(25, NULL);
    alist->AddEx(50, NULL);

PFL
    alist->RemoveEx(50, (IInterface**)&outface);
    alist->RemoveEx(25, (IInterface**)&outface);
    assert((alist->Equals(myList, &isflag), isflag));

PFL
    // List list = new ArrayList(Arrays.asList(new String[] { "a", "b", "c",
    //         "d", "e", "f", "g" }));
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<ICharSequence> csa;
    CString::New(String("a"), (ICharSequence**)&csa);
    AutoPtr<ICharSequence> csb;
    CString::New(String("b"), (ICharSequence**)&csb);
    AutoPtr<ICharSequence> csc;
    CString::New(String("c"), (ICharSequence**)&csc);
    AutoPtr<ICharSequence> csd;
    CString::New(String("d"), (ICharSequence**)&csd);
    AutoPtr<ICharSequence> cse;
    CString::New(String("e"), (ICharSequence**)&cse);
    AutoPtr<ICharSequence> csf;
    CString::New(String("f"), (ICharSequence**)&csf);
    AutoPtr<ICharSequence> csg;
    CString::New(String("g"), (ICharSequence**)&csg);
    list->Add(csa, &isflag);
    list->Add(csb, &isflag);
    list->Add(csc, &isflag);
    list->Add(csd, &isflag);
    list->Add(cse, &isflag);
    list->Add(csf, &isflag);
    list->Add(csg, &isflag);

PFL
    assert((list->RemoveEx(0, (IInterface**)&outface), outface) == csa);
    assert((list->RemoveEx(4, (IInterface**)&outface), outface) == csf);
    AutoPtr< ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(5);
    list->ToArray((ArrayOf<IInterface*>**)&result);
    // assertTrue("Removed wrong element 3", Arrays.equals(result,
    //         new String[] { "b", "c", "d", "e", "g" }));

PFL
    AutoPtr<IArrayList> l;
    CArrayList::New(0, (IArrayList**)&l);
    outface = NULL;
    l->Add(outface, &isflag);
    l->Add(outface, &isflag);
    l->RemoveEx(0, (IInterface**)&outface);
    l->RemoveEx(0, (IInterface**)&outface);
PFL
    // try {
    ec = l->RemoveEx(-1, (IInterface**)&outface);
PFL
    if (ec != NOERROR) {
        printf("-1 should cause exception\n");
    }
    // } catch (IndexOutOfBoundsException e) {
    // }
    // try {
    ec = l->RemoveEx(0, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("0 should case exception\n");
    }
    // } catch (IndexOutOfBoundsException e) {
    // }
    return 0;
}

int CTest::test_setILjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_setILjava_lang_Object")
    // Test for method java.lang.Object java.util.ArrayList.set(int,
    // java.lang.Object)
    AutoPtr<IInterface> obj;
    AutoPtr<IInterface> outface;
    alist->Set(65, obj, (IInterface**)&outface);

PFL
    assert((alist->Get(65, (IInterface**)&outface), outface) == obj);
    alist->Set(50, NULL, (IInterface**)&outface);
    assert((alist->Get(50, (IInterface**)&outface), outface) == NULL);
    Int32 sizelen = 0;
    assert((alist->GetSize(&sizelen), sizelen) == 100);

PFL
    // try {
    ECode ec = alist->Set(-1, NULL, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("IndexOutOfBoundsException expected\n");
    }
    // } catch (IndexOutOfBoundsException e) {
        //expected
    // }

    // try {
    ec = alist->Set((alist->GetSize(&sizelen), sizelen) + 1, NULL, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("IndexOutOfBoundsException expected\n");
    }
    // } catch (IndexOutOfBoundsException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_size(int argc, char* argv[])
{
    PEL("CTest::test_size")
    // Test for method int java.util.ArrayList.size()
    Int32 sizelen = 0;
    assert(100 == (alist->GetSize(&sizelen), sizelen));
    AutoPtr<IArrayList> al;
    CArrayList::New((IArrayList**)&al);
    assert(0 == (al->GetSize(&sizelen), sizelen));
    return 0;
}

int CTest::test_toArray(int argc, char* argv[])
{
    PEL("CTest::test_toArray")
    // Test for method java.lang.Object [] java.util.ArrayList.toArray()
    AutoPtr<IInterface> outface;
    alist->Set(25, NULL, (IInterface**)&outface);
    alist->Set(75, NULL, (IInterface**)&outface);
    AutoPtr< ArrayOf<IInterface*> > obj;
    alist->ToArray((ArrayOf<IInterface*>**)&obj);
    assert(objArray->GetLength() == obj->GetLength());

    for (int i = 0; i < obj->GetLength(); i++) {
        if ((i == 25) || (i == 75)) {
            assert((*obj)[i] == NULL);
        }
        else {
            assert((*obj)[i] == (*objArray)[i]);
        }
    }
    return 0;
}

int CTest::test_toArrayLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_toArrayLjava_lang_Object")
    // Test for method java.lang.Object []
    // java.util.ArrayList.toArray(java.lang.Object [])
    AutoPtr<IInterface> outface;
    alist->Set(25, NULL, (IInterface**)&outface);
    alist->Set(75, NULL, (IInterface**)&outface);
    AutoPtr< ArrayOf<IInterface*> > argArray = ArrayOf<IInterface*>::Alloc(100);
    AutoPtr< ArrayOf<IInterface*> > retArray;
    alist->ToArrayEx(argArray, (ArrayOf<IInterface*>**)&retArray);
    assert(retArray == argArray);
    argArray = ArrayOf<IInterface*>::Alloc(1000);
    alist->ToArrayEx(argArray, (ArrayOf<IInterface*>**)&retArray);
    Int32 sizelen = 0;
    assert((*argArray)[(alist->GetSize(&sizelen), sizelen)] == NULL);
    for (int i = 0; i < 100; i++) {
        if ((i == 25) || (i == 75))
            assert((*retArray)[i] == NULL);
        else
            assert((*retArray)[i] == (*objArray)[i]);
    }

    AutoPtr< ArrayOf<IInterface*> > strArray = ArrayOf<IInterface*>::Alloc(100);
    // try {
    ECode ec = alist->ToArrayEx(strArray, (ArrayOf<IInterface*>**)&retArray);
    if (ec != NOERROR) {
        printf("ArrayStoreException expected\n");
    }

    // } catch (ArrayStoreException e) {
        //expected
    // }
    return 0;
}

int CTest::test_trimToSize_01(int argc, char* argv[])
{
    PEL("CTest::test_trimToSize_01")
    // Test for method void java.util.ArrayList.trimToSize()
    Boolean isflag = FALSE;
    AutoPtr<IInterface> outface;
    for (int i = 99; i > 24; i--) {
        alist->RemoveEx(i, (IInterface**)&outface);
    }
    alist->TrimToSize();
    Int32 sizelen  = 0;
    assert(25 == (alist->GetSize(&sizelen), sizelen));
    for (int i = 0; i < (alist->GetSize(&sizelen), sizelen); i++) {
        assert((alist->Get(i, (IInterface**)&outface), outface) == (*objArray)[i]);
    }
    AutoPtr<IVector> v;
    CVector::New((IVector**)&v);
    AutoPtr<ICharSequence> csa;
    CString::New(String("a"), (ICharSequence**)&csa);
    v->Add(csa, &isflag);

    AutoPtr<ICharSequence> csb;
    CString::New(String("b"), (ICharSequence**)&csb);
    v->Add(csb, &isflag);
    AutoPtr<IArrayList> al;
    CArrayList::New(v, (IArrayList**)&al);
    AutoPtr<IIterator> it;
    al->GetIterator((IIterator**)&it);
    al->Remove(0, &isflag);
    al->TrimToSize();
    // try {
    ECode ec = it->Next((IInterface**)&outface);
    if (ec != NOERROR) {
        printf("should throw a ConcurrentModificationException\n");
    }
    // } catch (ConcurrentModificationException ioobe) {
    //     // expected
    // }
    return 0;
}

int CTest::test_trimToSize_02(int argc, char* argv[])
{
    PEL("CTest::test_trimToSize_02")
    // ArrayList list = new ArrayList(Arrays.asList(new String[] { "a", "b", "c",
    //         "d", "e", "f", "g" }));
    Boolean isflag = FALSE;
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<ICharSequence> csa;
    CString::New(String("a"), (ICharSequence**)&csa);
    AutoPtr<ICharSequence> csb;
    CString::New(String("b"), (ICharSequence**)&csb);
    AutoPtr<ICharSequence> csc;
    CString::New(String("c"), (ICharSequence**)&csc);
    AutoPtr<ICharSequence> csd;
    CString::New(String("d"), (ICharSequence**)&csd);
    AutoPtr<ICharSequence> cse;
    CString::New(String("e"), (ICharSequence**)&cse);
    AutoPtr<ICharSequence> csf;
    CString::New(String("f"), (ICharSequence**)&csf);
    AutoPtr<ICharSequence> csg;
    CString::New(String("g"), (ICharSequence**)&csg);
    list->Add(csa, &isflag);
    list->Add(csb, &isflag);
    list->Add(csc, &isflag);
    list->Add(csd, &isflag);
    list->Add(cse, &isflag);
    list->Add(csf, &isflag);
    list->Add(csg, &isflag);

    list->Remove(csa, &isflag);
    list->Remove(csf, &isflag);
    list->TrimToSize();
    return 0;
}

int CTest::test_addAll(int argc, char* argv[])
{
    PEL("CTest::test_addAll")
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<ICharSequence> one;
    CString::New(String("one"), (ICharSequence**)&one);
    AutoPtr<ICharSequence> two;
    CString::New(String("two"), (ICharSequence**)&two);
    Boolean isflag = FALSE;
    list->Add(one, &isflag);
    list->Add(two, &isflag);
    Int32 sizelen = 0;
    assert(2 == (list->GetSize(&sizelen), sizelen));

    AutoPtr<IInterface> outface;
    list->RemoveEx(0, (IInterface**)&outface);
    assert(1 == (list->GetSize(&sizelen), sizelen));

    AutoPtr<IArrayList> collection;
    CArrayList::New((IArrayList**)&collection);
    AutoPtr<ICharSequence> cs1;
    CString::New(String("1"), (ICharSequence**)&cs1);
    AutoPtr<ICharSequence> cs2;
    CString::New(String("2"), (ICharSequence**)&cs2);
    AutoPtr<ICharSequence> cs3;
    CString::New(String("3"), (ICharSequence**)&cs3);
    collection->Add(cs1, &isflag);
    collection->Add(cs2, &isflag);
    collection->Add(cs3, &isflag);
    assert(3 == (collection->GetSize(&sizelen), sizelen));

    list->AddAllEx(0, collection, &isflag);
    assert(4 == (list->GetSize(&sizelen), sizelen));

    list->RemoveEx(0, (IInterface**)&outface);
    list->RemoveEx(0, (IInterface**)&outface);
    assert(2 == (list->GetSize(&sizelen), sizelen));

    AutoPtr<ICharSequence> cs4;
    CString::New(String("4"), (ICharSequence**)&cs4);
    AutoPtr<ICharSequence> cs5;
    CString::New(String("5"), (ICharSequence**)&cs5);
    AutoPtr<ICharSequence> cs6;
    CString::New(String("6"), (ICharSequence**)&cs6);
    AutoPtr<ICharSequence> cs7;
    CString::New(String("7"), (ICharSequence**)&cs7);
    AutoPtr<ICharSequence> cs8;
    CString::New(String("8"), (ICharSequence**)&cs8);
    AutoPtr<ICharSequence> cs9;
    CString::New(String("9"), (ICharSequence**)&cs9);
    AutoPtr<ICharSequence> cs10;
    CString::New(String("10"), (ICharSequence**)&cs10);
    AutoPtr<ICharSequence> cs11;
    CString::New(String("11"), (ICharSequence**)&cs11);
    AutoPtr<ICharSequence> cs12;
    CString::New(String("12"), (ICharSequence**)&cs12);

    collection->Add(cs4, &isflag);
    collection->Add(cs5, &isflag);
    collection->Add(cs6, &isflag);
    collection->Add(cs7, &isflag);
    collection->Add(cs8, &isflag);
    collection->Add(cs9, &isflag);
    collection->Add(cs10, &isflag);
    collection->Add(cs11, &isflag);
    collection->Add(cs12, &isflag);

    assert(12 == (collection->GetSize(&sizelen), sizelen));

    list->AddAllEx(0, collection, &isflag);
    assert(14 == (list->GetSize(&sizelen), sizelen));
    return 0;
}

int CTest::test_removeLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_removeLjava_lang_Object")
    // List list = new ArrayList(Arrays.asList(new String[] { "a", "b", "c",
    //         "d", "e", "f", "g" }));
    Boolean isflag = FALSE;
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<ICharSequence> csa;
    CString::New(String("a"), (ICharSequence**)&csa);
    AutoPtr<ICharSequence> csb;
    CString::New(String("b"), (ICharSequence**)&csb);
    AutoPtr<ICharSequence> csc;
    CString::New(String("c"), (ICharSequence**)&csc);
    AutoPtr<ICharSequence> csd;
    CString::New(String("d"), (ICharSequence**)&csd);
    AutoPtr<ICharSequence> cse;
    CString::New(String("e"), (ICharSequence**)&cse);
    AutoPtr<ICharSequence> csf;
    CString::New(String("f"), (ICharSequence**)&csf);
    AutoPtr<ICharSequence> csg;
    CString::New(String("g"), (ICharSequence**)&csg);
    list->Add(csa, &isflag);
    list->Add(csb, &isflag);
    list->Add(csc, &isflag);
    list->Add(csd, &isflag);
    list->Add(cse, &isflag);
    list->Add(csf, &isflag);
    list->Add(csg, &isflag);

    assert((list->Remove(csa, &isflag), isflag));
    assert((list->Remove(csf, &isflag), isflag));
    // String[] result = new String[5];
    AutoPtr< ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(5);
    AutoPtr< ArrayOf<IInterface*> > outarr;
    list->ToArrayEx(result, (ArrayOf<IInterface*>**)&outarr);
    String str[5] = {String("b"), String("c"), String("d"), String("e"), String("g") };
    for (int i = 0; i < 5; ++i) {
        AutoPtr<ICharSequence> res = (ICharSequence*) (*outarr)[i]->Probe(EIID_ICharSequence);
        if (res == NULL) {
            assert(0);
        }
        String outstr;
        res->ToString(&outstr);
        assert( outstr == str[i]);
    }
    // assertTrue("Removed wrong element 3", Arrays.equals(result, new String[] { "b", "c", "d", "e", "g" }));
    return 0;
}

int CTest::test_removeRangeII(int argc, char* argv[])
{
    // Mock_ArrayList* mal = new Mock_ArrayList();
    // Boolean isflag = FALSE;
    // AutoPtr<ICharSequence> csa;
    // CString::New(String("a"), (ICharSequence**)&csa);
    // AutoPtr<ICharSequence> csb;
    // CString::New(String("b"), (ICharSequence**)&csb);
    // AutoPtr<ICharSequence> csc;
    // CString::New(String("c"), (ICharSequence**)&csc);
    // AutoPtr<ICharSequence> csd;
    // CString::New(String("d"), (ICharSequence**)&csd);
    // AutoPtr<ICharSequence> cse;
    // CString::New(String("e"), (ICharSequence**)&cse);
    // AutoPtr<ICharSequence> csf;
    // CString::New(String("f"), (ICharSequence**)&csf);
    // AutoPtr<ICharSequence> csg;
    // CString::New(String("g"), (ICharSequence**)&csg);
    // mal->Add(csa, &isflag);
    // mal->Add(csb, &isflag);
    // mal->Add(csc, &isflag);
    // mal->Add(csd, &isflag);
    // mal->Add(cse, &isflag);
    // mal->Add(csf, &isflag);
    // mal->Add(csg, &isflag);
    // mal.add("a");
    // mal.add("b");
    // mal.add("c");
    // mal.add("d");
    // mal.add("e");
    // mal.add("f");
    // mal.add("g");
    // mal.add("h");

    // mal.removeRange(2, 4);

    // String[] result = new String[6];
    // mal.toArray(result);
    // assertTrue("Removed wrong element 3", Arrays.equals(result,
    //         new String[] { "a", "b", "e", "f", "g", "h"}));
    return 0;
}


void CTest::setUp()
{
    // super.setUp();

    objArray = ArrayOf<IInterface*>::Alloc(100);
    for (int i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<IInteger32> intface;
        CInteger32::New(i, (IInteger32**)&intface);
        objArray->Set(i, intface);
    }

    CArrayList::New((IArrayList**)&alist);
    for (int i = 0; i < objArray->GetLength(); i++) {
        Boolean isflag = FALSE;
        alist->Add((*objArray)[i], &isflag);
    }
}

void CTest::tearDown()
{
    objArray = NULL;
    alist = NULL;

    // super.tearDown();
}
