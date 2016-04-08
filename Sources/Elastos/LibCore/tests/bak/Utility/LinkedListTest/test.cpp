#include "test.h"
#include "cmdef.h"
#include "elstring.h"

CTest::CTest() {
    setUp();
}

CTest::~CTest() {
    tearDown();
}

void CTest::setUp() {
    objArray = ArrayOf<IInterface*>::Alloc(100);
    AutoPtr<IInteger32> spI;
    for (Int32 i = 0; i < objArray->GetLength(); ++i) {
        CInteger32::New(i, (IInteger32**)&spI);
        objArray->Set(i, spI);
    }

    CLinkedList::New((ILinkedList**)&ll);
    Boolean flag = FALSE;
    for (Int32 i = 0; i < objArray->GetLength(); ++i) {
        ll->Add((*objArray)[i], &flag);
    }
}

void CTest::tearDown() {
    objArray = NULL;
    ll = NULL;
}

int CTest::test_addILjava_lang_Object(int argc, char* argv[]) {
    // Test for method void java.util.LinkedList.add(int, java.lang.Object)
    Boolean flag = FALSE;
    AutoPtr<ICharSequence> o;
    CString::New(String("Test"), (ICharSequence**)&o);
    ll->AddEx(50, o);
    AutoPtr<IInterface> lsObj;
    ll->Get(50, (IInterface**)&lsObj);
    if (NULL != lsObj) {
        String str;
        AutoPtr<ICharSequence> pCs;
        pCs = (ICharSequence*)lsObj->Probe(EIID_ICharSequence);
        pCs->ToString(&str);
        assert(str.Equals(String("Test")));
    }
    ll->Get(51, (IInterface**)&lsObj);
    assert(lsObj == (*objArray)[50]);
    ll->Get(52, (IInterface**)&lsObj);
    assert(lsObj == (*objArray)[51]);
    ll->AddEx(50, NULL);
    ll->Get(50, (IInterface**)&lsObj);
    assert(lsObj == NULL);
}

int CTest::test_addLjava_lang_Object(int argc, char* argv[]) {
    // Test for method boolean java.util.LinkedList.add(java.lang.Object)
    AutoPtr<IInterface> o;
    Boolean flag = FALSE;
    ll->Add(o, &flag);
    AutoPtr<IInterface> obj;
    ll->GetLast((IInterface**)&obj);
    assert(o == obj);

    ll->Add(NULL, &flag);
    Int32 size;
    ll->GetSize(&size);
    AutoPtr<IInterface> sobj;
    ll->Get(size -1, (IInterface**)&sobj);
    assert(sobj.Get() == NULL);
}

int CTest::test_addAllILjava_util_Collection(int argc, char* argv[]) {
    // Test for method boolean java.util.LinkedList.addAll(int,
    // java.util.Collection)
    Boolean flag = FALSE;
    AutoPtr<IInterface> llclone;
    ICloneable::Probe(ll)->Clone((IInterface**)&llclone);
    ll->AddAllEx(50, ICollection::Probe(llclone), &flag);
    Int32 size;
    ll->GetSize(&size);
    assert(size == 200);

    AutoPtr<IInterface> lsObj;
    for (Int32 i = 0; i < 50; ++i) {
        ll->Get(i, (IInterface**)&lsObj);
        flag = Object::Equals(lsObj, (*objArray)[i]);
        assert(flag == Object::Equals(lsObj, (*objArray)[i]));
    }

    for (Int32 i = 0; i >= 50 && (i < 150); ++i) {
        ll->Get(i, (IInterface**)&lsObj);
        assert(Object::Equals(lsObj, (*objArray)[i - 50]));
    }

    for (Int32 i = 0; i >= 150 && i < 200; i++) {
        ll->Get(i, (IInterface**)&lsObj);
        assert(Object::Equals(lsObj, (*objArray)[i - 100]));
    }

    AutoPtr<ILinkedList> myList;
    CLinkedList::New((ILinkedList**)&myList);
    myList->Add(NULL, &flag);

    AutoPtr<ICharSequence> cs1;
    CString::New(String("Blah"), (ICharSequence**)&cs1);
    myList->Add(cs1, &flag);
    myList->Add(NULL, &flag);
    AutoPtr<ICharSequence> cs2;
    CString::New(String("Booga"), (ICharSequence**)&cs2);
    myList->Add(cs2, &flag);
    myList->Add(NULL, &flag);
    ll->AddAllEx(50, myList, &flag);

    ll->Get(50, (IInterface**)&lsObj);
    assert(lsObj == NULL);

    ll->Get(51, (IInterface**)&lsObj);
    assert(Object::Equals(lsObj, cs1));

    ll->Get(52, (IInterface**)&lsObj);
    assert(lsObj == NULL);

    ll->Get(53, (IInterface**)&lsObj);
    assert(Object::Equals(lsObj, cs2));

    ll->Get(54, (IInterface**)&lsObj);
    assert(lsObj == NULL);

    ECode ec = ll->AddAllEx(-1, (ICollection*)NULL, &flag);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ll->GetSize(&size);
    ec = ll->AddAllEx(size + 1, (ICollection*)NULL, &flag);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = ll->AddAllEx(0, (ICollection*)NULL, &flag);
    assert(ec == E_NULL_POINTER_EXCEPTION);
}

int CTest::test_addAllILjava_util_Collection_2(int argc, char* argv[]) {
    AutoPtr<ILinkedList> obj;
    CLinkedList::New((ILinkedList**)&obj);
    Boolean flag = TRUE;
    ECode ec = obj->AddAllEx(-1, (ILinkedList*)NULL, &flag);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

int CTest::test_addAllLjava_util_Collection(int argc, char* argv[]) {
    // Test for method boolean
    // java.util.LinkedList.addAll(java.util.Collection)
    AutoPtr<IArrayList> _l;
    CArrayList::New((IArrayList**)&_l);
    AutoPtr<IList> l = IList::Probe(_l);
    AutoPtr<IInterface> llclone;
    ICloneable::Probe(ll)->Clone((IInterface**)&llclone);
    Boolean flag = FALSE;
    l->AddAll(ICollection::Probe(llclone), &flag);
    Int32 size;
    ll->GetSize(&size);

    AutoPtr<IInterface> obj1;
    AutoPtr<IInterface> obj2;
    for (Int32 i = 0; i < size; ++i) {
        ll->Get(i, (IInterface**)&obj1);
        l->Get(i, (IInterface**)&obj2);
        assert(Object::Equals(obj1, obj2));
    }

    ll->AddAll(ICollection::Probe(llclone), &flag);
    ll->GetSize(&size);
    assert(size == 200);

    for (int i = 0; i < 100; ++i) {
        ll->Get(i, (IInterface**)&obj1);
        l->Get(i, (IInterface**)&obj2);
        assert(Object::Equals(obj1, obj2));

        ll->Get(i + 100, (IInterface**)&obj1);
        assert(Object::Equals(obj1, obj2));
    }

    AutoPtr<ILinkedList> _myList;
    CLinkedList::New((ILinkedList**)&_myList);
    AutoPtr<IList> myList = IList::Probe(_myList);
    myList->Add((IInterface*)NULL, &flag);
    AutoPtr<ICharSequence> blah;
    CString::New(String("Blah"), (ICharSequence**)&blah);
    myList->Add(blah, &flag);
    myList->Add((IInterface*)NULL, &flag);
    AutoPtr<ICharSequence> booga;
    CString::New(String("Booga"), (ICharSequence**)&booga);
    myList->Add(booga, &flag);
    myList->Add((IInterface*)NULL, &flag);
    ll->AddAll(ICollection::Probe(myList), &flag);
    ll->Get(200, (IInterface**)&obj1);
    assert(obj1 == NULL);

    ll->Get(201, (IInterface**)&obj2);
    assert(Object::Equals(blah, obj2));

    AutoPtr<IInterface> obj3;
    IList::Probe(ll)->Get(202, (IInterface**)&obj3);
    assert(obj3 == NULL);

    AutoPtr<IInterface> obj4;
    ll->Get(203, (IInterface**)&obj4);
    flag = Object::Equals(booga, obj4);
    assert(Object::Equals(booga, obj4));

    AutoPtr<IInterface> obj5;
    ll->Get(204, (IInterface**)&obj5);
    assert(obj5 == NULL);

    ECode ec = ll->AddAll((ICollection*)NULL, &flag);
    assert(ec == E_NULL_POINTER_EXCEPTION);
}

int CTest::test_addFirstLjava_lang_Object(int argc, char* argv[])
{
    // Test for method void java.util.LinkedList.addFirst(java.lang.Object)
    AutoPtr<IInterface> o;
    ll->AddFirst(o);
    AutoPtr<IInterface> first;
    ll->GetFirst((IInterface**)&first);
    Boolean flag = FALSE;
    flag = first == o;
    assert(flag);
    ll->AddFirst(NULL);
    AutoPtr<IInterface> first2;
    ll->GetFirst((IInterface**)&first2);
    assert(first2.Get() == NULL);
}

int CTest::test_addLastLjava_lang_Object(int argc, char* argv[])
{
    // Test for method void java.util.LinkedList.addLast(java.lang.Object)
    AutoPtr<IInterface> o;
    ll->AddLast(o);
    AutoPtr<IInterface> last;
    ll->GetLast((IInterface**)&last);
    Boolean flag = FALSE;
    flag = last == o;
    assert(flag);
    ll->AddLast(NULL);
    AutoPtr<IInterface> last2;
    ll->GetLast((IInterface**)&last2);
    assert(last2.Get() == NULL);
}

int CTest::test_clear(int argc, char* argv[])
{
    // Test for method void java.util.LinkedList.clear()
    ll->Clear();
    Int32 size;
    ll->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> lsObj;
        ll->Get(i, (IInterface**)&lsObj);
        assert(lsObj.Get() == NULL);
    }
}

int CTest::test_clone(int argc, char* argv[])
{
    // Test for method java.lang.Object java.util.LinkedList.clone()
    AutoPtr<IInterface> x;
    ICloneable::Probe(ll)->Clone((IInterface**)&x);
    Boolean flag = TRUE;
    assert(Object::Equals(x, ll));

    Int32 size;
    ll->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> llObj;
        ll->Get(i, (IInterface**)&llObj);
        AutoPtr<IInterface> xObj;
        IList::Probe(x)->Get(i, (IInterface**)&xObj);
        assert(Object::Equals(llObj, xObj));
    }
    ll->AddFirst((IInterface*)NULL);
    ICloneable::Probe(ll)->Clone((IInterface**)&x);
    assert(Object::Equals(ll, x));
}

int CTest::test_containsLjava_lang_Object(int argc, char* argv[]) {
    // Test for method boolean
    // java.util.LinkedList.contains(java.lang.Object)
    Boolean flag = FALSE;
    ll->Contains((*objArray)[99], &flag);
    assert(flag);
    AutoPtr<IInteger32> o1;
    CInteger32::New(8, (IInteger32**)&o1);
    ll->Contains(o1, &flag);
    assert(flag);

    AutoPtr<IInterface> obj;
    ll->Contains(obj, &flag);
    assert(!flag);

    ll->AddEx(25, NULL);
    ll->Contains(NULL, &flag);
    assert(flag);
}

int CTest::test_getI(int argc, char* argv[]) {
    // Test for method java.lang.Object java.util.LinkedList.get(int)
    AutoPtr<IInterface> llObj;
    ll->Get(22, (IInterface**)&llObj);
    Boolean flag = FALSE;
    flag = llObj == (*objArray)[22];
    assert(flag);

    ECode ec = ll->Get(8765, (IInterface**)&llObj);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

int CTest::test_getFirst(int argc, char* argv[]) {
    // Test for method java.lang.Object java.util.LinkedList.getFirst()
    AutoPtr<IInterface> llObj;
    ll->GetFirst((IInterface**)&llObj);
    Boolean flag = llObj == (*objArray)[0];
    assert(flag);

    ll->Clear();
    ECode ec = ll->GetFirst((IInterface**)&llObj);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
}

int CTest::test_getLast(int argc, char* argv[]) {
    // Test for method java.lang.Object java.util.LinkedList.getLast()
    AutoPtr<IInterface> llObj;
    ll->GetLast((IInterface**)&llObj);
    Int32 length = objArray->GetLength();
    Boolean flag = llObj == (*objArray)[length - 1];
    assert(flag);

    ll->Clear();
    ECode ec = ll->GetLast((IInterface**)&llObj);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
}

int CTest::test_indexOfLjava_lang_Object(int argc, char* argv[]) {
    AutoPtr<IInterface> obj = (*objArray)[87];
    Int32 index;
    ll->IndexOf(obj, &index);
    assert(index == 87);

    AutoPtr<IInterface> o;
    ll->IndexOf(o, &index);
    assert(index == -1);

    ll->AddEx(20, NULL);
    ll->AddEx(24, NULL);
    ll->IndexOf(NULL, &index);
    assert(index == 20);
}

int CTest::test_lastIndexOfLjava_lang_Object(int argc, char* argv[]) {
    // Test for method int
    // java.util.LinkedList.lastIndexOf(java.lang.Object)
    Boolean flag = FALSE;
    AutoPtr<IInteger32> o1;
    CInteger32::New(99, (IInteger32**)&o1);
    ll->Add(o1, &flag);
    Int32 size;
    ll->GetSize(&size);
    Int32 index;
    ll->LastIndexOf((*objArray)[99], &index);
    assert(index == 100);
    AutoPtr<IInterface> o;
    ll->LastIndexOf(o, &index);
    assert(index == -1);
    ll->AddEx(20, NULL);
    ll->AddEx(24, NULL);
    ll->LastIndexOf(NULL, &index);
    assert(index == 24);
}

int CTest::test_listIteratorI(int argc, char* argv[]) {
    // Test for method java.util.ListIterator
    // java.util.LinkedList.listIterator(int)
    AutoPtr<IListIterator> i1;
    ll->GetListIterator((IListIterator**)&i1);
    AutoPtr<IListIterator> i2;
    ll->GetListIteratorEx(0,(IListIterator**)&i2);

    AutoPtr<IInterface> elm;
    Int32 n = 0;
    Boolean flag = FALSE;
    while((i2->HasNext(&flag), flag)) {
        Int32 length = objArray->GetLength();
        if (n == 0 || n == length - 1) {
            if (n == 0) {
                i2->HasPrevious(&flag);
                assert(!flag);
            }
            if (n == length) {
                i2->HasNext(&flag);
                assert(!flag);
            }
        }
        i2->Next((IInterface**)&elm);
        flag = elm == (*objArray)[n];
        assert(flag);
        Int32 index;
        if (n > 0 && n <= length - 1) {
            i2->NextIndex(&index);
            flag = index == (n + 1);
            assert(flag);

            i2->PreviousIndex(&index);
            flag = index == n ;
            assert(flag);
        }
        i1->Next((IInterface**)&elm);
        flag = elm == (*objArray)[n];
        assert(flag);
        ++n;
    }

    Int32 llSize;
    ll->GetSize(&llSize);
    Int32 inSize = llSize / 2;
    ll->GetListIteratorEx(inSize, (IListIterator**)&i2);
    i2->Next((IInterface**)&elm);
    AutoPtr<IInteger32> is;
    is = (IInteger32*)elm->Probe(EIID_IInteger32);
    Int32 value;
    is->GetValue(&value);
    flag = value == inSize;
    assert(flag);

    AutoPtr<ILinkedList> myList;
    CLinkedList::New((ILinkedList**)&myList);
    flag = TRUE;
    myList->Add(NULL, &flag);
    AutoPtr<ICharSequence> o1;
    CString::New(String("Blah"), (ICharSequence**)&o1);
    myList->Add(o1, &flag);
    myList->Add(NULL, &flag);
    AutoPtr<ICharSequence> o2;
    CString::New(String("Booga"), (ICharSequence**)&o2);
    myList->Add(o2, &flag);
    myList->Add(NULL, &flag);
    AutoPtr<IListIterator> li;
    myList->GetListIterator((IListIterator**)&li);
    li->HasPrevious(&flag);
    assert(!flag);

    AutoPtr<IInterface> obj;
    li->Next((IInterface**)&obj);
    assert(obj == NULL);

    li->HasPrevious(&flag);
    assert(flag);

    AutoPtr<IInterface> spObj;
    li->Previous((IInterface**)&spObj);
    assert(spObj == NULL);

    AutoPtr<IInterface> sObj;
    li->Next((IInterface**)&sObj);
    assert(sObj == NULL);

    AutoPtr<IInterface> smObj;
    li->Next((IInterface**)&smObj);
    flag = smObj == o1;

    li->Next((IInterface**)&obj);
    assert(obj == NULL);

    li->Next((IInterface**)&obj);
    AutoPtr<ICharSequence> o4;
    CString::New(String("Booga"), (ICharSequence**)&o4);
    assert(obj == o2);

    li->HasNext(&flag);
    assert(flag);

    li->Next((IInterface**)&obj);
    assert(obj == NULL);

    li->HasNext(&flag);
    assert(!flag);

    AutoPtr<IListIterator> itlist;
    ECode ec = ll->GetListIteratorEx(-1, (IListIterator**)&itlist);
    Int32 size;
    ll->GetSize(&size);
    ec = ll->GetListIteratorEx(size + 1, (IListIterator**)&itlist);
}

int CTest::test_removeI(int argc, char* argv[]) {
    // Test for method java.lang.Object java.util.LinkedList.remove(int)
    AutoPtr<IInterface> obj;
    ll->RemoveEx(10, (IInterface**)&obj);
    Int32 index;
    ll->IndexOf((*objArray)[10], &index);
    assert(index == -1);

    ECode ec = ll->RemoveEx(999, (IInterface**)&obj);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    ll->AddEx(20, NULL);
    ll->RemoveEx(20, (IInterface**)&obj);
    ll->Get(20, (IInterface**)&obj);
    assert(obj != NULL);
}

int CTest::test_removeLjava_lang_Object(int argc, char* argv[]) {
    // Test for method boolean java.util.LinkedList.remove(java.lang.Object)
    Boolean flag = FALSE;
    ll->Remove((*objArray)[87], &flag);
    assert(flag);

    AutoPtr<IInterface> o;
    flag = TRUE;
    ll->Remove(o, &flag);
    assert(!flag);

    Int32 index;
    ll->IndexOf((*objArray)[87], &index);
    assert(index == -1);

    ll->Add(NULL, &flag);
    ll->Remove(NULL, &flag);
    ll->Contains(NULL, &flag);
    assert(!flag);
}

int CTest::test_removeFirst(int argc, char* argv[]) {
    // Test for method java.lang.Object java.util.LinkedList.removeFirst()
    AutoPtr<IInterface> obj;
    ll->RemoveFirst((IInterface**)&obj);
    ll->GetFirst((IInterface**)&obj);
    Boolean flag = obj == (*objArray)[0];
    assert(!flag);

    ll->Clear();
    ECode ec = ll->RemoveFirst((IInterface**)&obj);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
}

int CTest::test_removeLast(int argc, char* argv[]) {
    // Test for method java.lang.Object java.util.LinkedList.removeLast()
    AutoPtr<IInterface> obj;
    ll->RemoveLast((IInterface**)&obj);
    ll->GetLast((IInterface**)&obj);
    Int32 length;
    length = objArray->GetLength();
    Boolean flag = obj == (*objArray)[length - 1];
    assert(!flag);

    ll->Clear();
    ECode ec = ll->RemoveLast((IInterface**)&obj);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
 }

int CTest::test_setILjava_lang_Object(int argc, char* argv[]) {
    // Test for method java.lang.Object java.util.LinkedList.set(int,
    // java.lang.Object)
    AutoPtr<IInterface> obj;
    AutoPtr<IInterface> outObj;
    ll->Set(65, obj, (IInterface**)&outObj);
    AutoPtr<IInterface> obj1;
    ll->Get(65, (IInterface**)&obj1);
    Boolean flag = FALSE;
    flag = obj == obj1;
    assert(flag);

    AutoPtr<IInterface> obj2;
    ECode ec = ll->Set(-1, obj2, (IInterface**)&outObj);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    Int32 size;
    ll->GetSize(&size);
    ec = ll->Set(size + 1, obj2, (IInterface**)&outObj);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

int CTest::test_size(int argc, char* argv[]) {
    // Test for method int java.util.LinkedList.size()
    Int32 size;
    ll->GetSize(&size);
    Int32 length = objArray->GetLength();
    assert(size == length);

    AutoPtr<IInterface> o;
    ll->RemoveFirst((IInterface**)&o);
    ll->GetSize(&size);
    length = objArray->GetLength();
    assert(size == length - 1);
}

int CTest::test_toArray(int argc, char* argv[]) {
    // Test for method java.lang.Object [] java.util.LinkedList.toArray()
    Boolean flag = FALSE;
    ll->Add(NULL, &flag);
    AutoPtr<ArrayOf<IInterface*> > obj;
    ll->ToArray((ArrayOf<IInterface*>**)&obj);
    Int32 objLength = obj->GetLength();
    Int32 objArrayLength = objArray->GetLength();
    assert(objLength == objArrayLength + 1);

    flag = FALSE;
    for (Int32 i = 0; i < objLength - 1; ++i) {
        flag = (*obj)[i] == (*objArray)[i];
        assert(flag);
    }
    flag = FALSE;
    assert((*obj)[objLength - 1] == NULL);
}

int CTest::test_toArrayLjava_lang_Object(int argc, char* argv[]) {
    // Test for method java.lang.Object []
    // java.util.LinkedList.toArray(java.lang.Object []) IInteger32
/*    AutoPtr<ArrayOf<IInteger32*> > argArray = ArrayOf<IInteger32*>::Alloc(100);
    ArrayOf<IInterface*> retArray;
    ll->ToArrayEx(argArray, (ArrayOf<IInterface*>**)&retArray);
    Boolean flag = FALSE;
    // assert(retArray->Equals(argArray));

    AutoPtr<IList> list;
    Arrays::AsList(retArray, (IList**)&list);
    AutoPtr<ILinkedList> linkedList;
    CLinkedList::New(ICollection::Probe(list), (ILinkedList**)&linkedList);
    AutoPtr<IList> retList = IList::Probe(linkedList);
    AutoPtr<IIterator> li;
    ll->GetIterator((IIterator**)&li);
    AutoPtr<IIterator> ri;
    retList->GetIterator((IIterator**)&ri);
    while((li->HasNext(&flag), flag)) {
        AutoPtr<IInterface> obj1;
        li->Next((IInterface**)&obj1);
        AutoPtr<IInterface> obj2;
        ri->Next((IInterface**)&obj2);
        assert(Object::Equals(obj1, obj2));
    }

    argArray = ArrayOf<Int32>::Alloc(1000);
    ll->ToArrayEx(argArray, (ArrayOf<IInterface*>**)&retArray);
    Int32 size;
    ll->GetSize(&size);
    assert((*argArray)[size] == NULL);

    for (int i = 0; i < size; ++i) {
        assert(Object::Equals((*retArray)[i], (*argArray)[i]));
    }

    ll->AddEx(50, NULL);
    argArray = ArrayOf<Int32>::Alloc(101);
    ll->ToArrayEx(argArray, (ArrayOf<IInterface*>**)&retArray);
    assert(retArray->Equals(argArray));

    ll->ToArrayEx(argArray, (ArrayOf<IInterface*>**)&retArray);
    assert(retArray->Equals(argArray));
    iArrays->AsList(retArray, (IList**)&list);
    CLinkedList::New(ICollection::Probe(list), (ILinkedList**)&linkedList);
    AutoPtr<IList> retList = IList::Probe(linkedList);
    ll->GetIterator((IIterator**)&li);
    retList->GetIterator((IIterator**)&ri);
    while((li->HasNext(&flag), flag)) {
        AutoPtr<IInterface> obj1;
        li->Next((IInterface**)&obj1);
        AutoPtr<IInterface> obj2;
        ri->Next((IInterface**)&obj2);
        assert(Object::Equals(obj1, obj2));
    }

    ECode ec = ll->ToArrayEx(NULL, (ArrayOf<IInterface*>**)&retArray);
    assert(ec == E_NULL_POINTER_EXCEPTION);

    AutoPtr<ILinkedList> lls;
    CLinkedList::New((ILinkedList**)&lls);
    AutoPtr<ICharSequence> first;
    CString::New(String("First"), (ICharSequence**)&first);
    lls->Add(first, &flag);
    AutoPtr<ICharSequence> second;
    CString::New(String("Second"), (ICharSequence**)&second);
    lls->Add(second, &flag);

    ec = lls->ToArrayEx(argArray, (ArrayOf<IInterface*>**)&retArray);*/
    return 0 ;
}

int CTest::test_offer(int argc, char* argv[]) {
    Int32 origSize;
    ll->GetSize(&origSize);
    Boolean flag = FALSE;
    ll->Offer((*objArray)[0], &flag);
    assert(flag);

    Int32 index;
    ll->LastIndexOf((*objArray)[0], &index);
    assert(index == origSize);
}

int CTest::test_poll(int argc, char* argv[]) {
    Int32 length =  objArray->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> o;
        ll->Poll((IInterface**)&o);
        assert(o == (*objArray)[i]);
    }
    Int32 size;
    ll->GetSize(&size);
    assert(size == 0);
    AutoPtr<IInterface> obj;
    ll->Poll((IInterface**)&obj);
    assert(obj == NULL);
}

int CTest::test_remove(int argc, char* argv[]) {
    Int32 length =  objArray->GetLength();
    AutoPtr<IInterface> obj;
    Boolean flag = FALSE;
    for (Int32 i = 0; i < length; ++i) {
        ll->RemoveEx2((IInterface**)&obj);
        flag = obj == (*objArray)[i];
        assert(flag);
    }
    ll->GetSize(&length);
    assert(length == 0);

    ECode ec = ll->RemoveEx2((IInterface**)&obj);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
}

int CTest::test_element(int argc, char* argv[]) {
    AutoPtr<IInterface> obj;
    ll->GetElement((IInterface**)&obj);
    Boolean flag = FALSE;
    flag = obj == (*objArray)[0];
    assert(flag);

    Int32 size;
    ll->GetSize(&size);
    Int32 length = objArray->GetLength();
    assert(size == length);

    AutoPtr<ILinkedList> list;
    CLinkedList::New((ILinkedList**)&list);
    AutoPtr<IInterface> o;
    ECode ec = list->RemoveEx2((IInterface**)&o);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
}

int CTest::test_peek(int argc, char* argv[]) {
    AutoPtr<IInterface> o;
    ll->Peek((IInterface**)&o);
    Boolean flag = FALSE;
    if (o == NULL) {
        printf("o == NULL, line = %d", __LINE__);
    }
    flag = o == (*objArray)[0];
    assert(flag);

    ll->Peek((IInterface**)&o);
    assert(o == (*objArray)[0]);

    ll->Clear();
    ll->Peek((IInterface**)&o);
    assert(o == NULL);
}

// int CTest::test_ConstructorLjava_util_Collection(int argc, char* argv[]) {
//     // Test for method java.util.LinkedList(java.util.Collection)
//     printf("Entering test_ConstructorLjava_util_Collection\n");
//     AutoPtr<ILinkedList> lsList;
//     CLinkedList::New(ll, (ILinkedList**)&lsList);
//     printf("After CLinkedList::New(,)\n");
//     Boolean flag = FALSE;
//     ll->Equals(lsList, &flag);
//     assert(flag);

//     Boolean flag1 = TRUE;
//     ECode ec = CLinkedList::New(NULL, (ILinkedList**)&ll);
//     if (FAILED(ec)) {
//         assert(flag1);
//     }
//     return 0;
// }
























