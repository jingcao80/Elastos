
#include "test.h"

CTest::CTest()
{
    mabcde = ArrayOf<IInterface*>::Alloc(5);
    autoarr = ArrayOf<IInterface*>::Alloc(5);
    CString::New(String("a"), (ICharSequence**)&sqa);
    CString::New(String("b"), (ICharSequence**)&sqb);
    CString::New(String("c"), (ICharSequence**)&sqc);
    CString::New(String("d"), (ICharSequence**)&sqd);
    CString::New(String("e"), (ICharSequence**)&sqe);
    CString::New(String("f"), (ICharSequence**)&sqf);
    CString::New(String("g"), (ICharSequence**)&sqg);
    CString::New(String("h"), (ICharSequence**)&sqh);
    CString::New(String("i"), (ICharSequence**)&sqi);

    CString::New(String("A"), (ICharSequence**)&sqA);
    CString::New(String("B"), (ICharSequence**)&sqB);
    CString::New(String("C"), (ICharSequence**)&sqC);
    CString::New(String("D"), (ICharSequence**)&sqD);
    CString::New(String("E"), (ICharSequence**)&sqE);

    mabcde->Set(0, sqa);
    mabcde->Set(1, sqb);
    mabcde->Set(2, sqc);
    mabcde->Set(3, sqd);
    mabcde->Set(4, sqe);
}

CTest::~CTest()
{

}

int CTest::testIteratorAndNonStructuralChanges(int argc, char* argv[])
{
    PEL("CTest::testIteratorAndNonStructuralChanges")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    Boolean isflag = FALSE;
    list->AddAll(outlist, &isflag);
    AutoPtr<IIterator> abcde;
    list->GetIterator((IIterator**)&abcde);
    AutoPtr<IInterface> outface;
    ECode ec = abcde->Next((IInterface**)&outface);
    PFL_EX("abcde: %p, ec: %p", abcde.Get(), ec)
    AutoPtr<ICharSequence> minsq = ICharSequence::Probe(outface);
    PFL_EX("minsq: %p, outface: %p", minsq.Get(), outface.Get())
    assert(Object::Equals(sqa, outface));
    list->Set(1, sqB, (IInterface**)&outface);

    abcde->Next((IInterface**)&outface);
    assert(Object::Equals(sqb, outface));

    abcde->Next((IInterface**)&outface);
    assert(Object::Equals(sqc, outface));

    abcde->Next((IInterface**)&outface);
    assert(Object::Equals(sqd, outface));

    abcde->Next((IInterface**)&outface);
    assert(Object::Equals(sqe, outface));
    return 0;
}

int CTest::testSubListAndNonStructuralChanges(int argc, char* argv[])
{
    PEL("CTest::testSubListAndNonStructuralChanges")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> bcd;
    list->SubList(1, 4, (IList**)&bcd);
    AutoPtr<IInterface> outface;
    list->Set(2, sqC, (IInterface**)&outface);
    // try {
    ECode ec = bcd->Get(1, (IInterface**)&outface);
    if (ec != NOERROR) {
        /* code */
        printf("fail~~~~~~~~~~~~~~~~~~~~~\n");
    }
    // } catch (ConcurrentModificationException expected) {
    // }
    return 0;
}

int CTest::testSubListAndStructuralChanges(int argc, char* argv[])
{
    PEL("CTest::testSubListAndStructuralChanges")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> bcd;
    list->SubList(1, 4, (IList**)&bcd);
    list->Clear();
    // try {
    ECode ec = bcd->Get(1, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("expected fails~~~~~~~~~~~~~~~\n");
    }
    // } catch (ConcurrentModificationException expected) {
    // }
    return 0;
}

int CTest::testSubListAndSizePreservingStructuralChanges(int argc, char* argv[])
{
    PEL("CTest::testSubListAndSizePreservingStructuralChanges")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> bcd;
    list->SubList(1, 4, (IList**)&bcd);
    list->Clear();
    autoarr->Set(0, sqA);
    autoarr->Set(1, sqB);
    autoarr->Set(2, sqC);
    autoarr->Set(3, sqD);
    autoarr->Set(4, sqE);
    Arrays::AsList(autoarr, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    // try {
    ECode ec = bcd->Get(1, (IInterface**)&outface);
    if (ec != NOERROR) {
        /* code */
        printf("fail~~~~~~~~~~~~~~\n");
    }
    // } catch (ConcurrentModificationException expected) {
    // }
    return 0;
}

int CTest::testRemoveAll(int argc, char* argv[])
{
    PEL("CTest::testRemoveAll")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);

    AutoPtr<IList> rmlist;
    Arrays::AsList(NULL, (IList**)&rmlist);
    list->RemoveAll(rmlist, &isflag);
    assert((outlist->Equals(list, &isflag), isflag));

    autoarr = ArrayOf<IInterface*>::Alloc(1);
    autoarr->Set(0, sqe);
    Arrays::AsList(autoarr, (IList**)&outlist);
    list->RemoveAll(outlist, &isflag);

    autoarr = ArrayOf<IInterface*>::Alloc(4);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqc);
    autoarr->Set(3, sqd);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));

    autoarr = ArrayOf<IInterface*>::Alloc(2);
    autoarr->Set(0, sqb);
    autoarr->Set(1, sqc);
    Arrays::AsList(autoarr, (IList**)&outlist);
    list->RemoveAll(outlist, &isflag);

    autoarr->Set(0, sqa);
    autoarr->Set(1, sqd);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));
    return 0;
}

int CTest::testSubListClear(int argc, char* argv[])
{
    PEL("CTest::testSubListClear")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> bcd;
    list->SubList(1, 4, (IList**)&bcd);
    bcd->Clear();

    autoarr = ArrayOf<IInterface*>::Alloc(2);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqe);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));

    autoarr = ArrayOf<IInterface*>::Alloc(3);
    autoarr->Set(0, sqB);
    autoarr->Set(1, sqC);
    autoarr->Set(2, sqD);
    Arrays::AsList(autoarr, (IList**)&outlist);
    bcd->AddAll(outlist, &isflag);

    autoarr = ArrayOf<IInterface*>::Alloc(5);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqB);
    autoarr->Set(2, sqC);
    autoarr->Set(3, sqD);
    autoarr->Set(4, sqe);

    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));
    return 0;
}

int CTest::testSubListClearWhenEmpty(int argc, char* argv[])
{
    PEL("CTest::testSubListClearWhenEmpty")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    AutoPtr<IList> outlist;
    list->SubList(0, 0, (IList**)&outlist);
    ECode ec = outlist->Clear();
    if (ec != NOERROR) {
        printf("expected fails~~~~~~~~~~~~expected\n");
    }
    // new CopyOnWriteArrayList<String>().subList(0, 0).clear(); // the RI fails here
    return 0;
}

int CTest::testSubListIteratorGetsSnapshot(int argc, char* argv[])
{
    PEL("CTest::testSubListIteratorGetsSnapshot")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    list->SubList(1, 4, (IList**)&outlist);
    AutoPtr<IIterator> bcd;
    outlist->GetIterator((IIterator**)&bcd);
    list->Clear();
    bcd->Next((IInterface**)&outface);
    assert(Object::Equals(sqb, outface));
    bcd->Next((IInterface**)&outface);
    assert(Object::Equals(sqc, outface));
    bcd->Next((IInterface**)&outface);
    assert(Object::Equals(sqd, outface));
    bcd->HasNext(&isflag);
    assert(!isflag);
    return 0;
}

int CTest::testSubListRemoveByValue(int argc, char* argv[])
{
    PEL("CTest::testSubListRemoveByValue")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> bcd;
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    list->SubList(1, 4, (IList**)&bcd);

    bcd->Remove(sqc, &isflag); // the RI fails here
    autoarr = ArrayOf<IInterface*>::Alloc(2);
    autoarr->Set(0, sqb);
    autoarr->Set(1, sqd);
    AutoPtr<IList> bdlist;
    Arrays::AsList(autoarr, (IList**)&bdlist);
    assert((bdlist->Equals(bcd, &isflag), isflag));

    autoarr = ArrayOf<IInterface*>::Alloc(4);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqd);
    autoarr->Set(3, sqe);
    Arrays::AsList(autoarr, (IList**)&bcd);
    assert((bcd->Equals(list, &isflag), isflag));
    return 0;
}

int CTest::testSubListRemoveByIndex(int argc, char* argv[])
{
    PEL("CTest::testSubListRemoveByIndex")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    AutoPtr<IList> bcd;
    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    list->SubList(1, 4, (IList**)&bcd);
    bcd->RemoveEx(1, (IInterface**)&outface);
    autoarr = ArrayOf<IInterface*>::Alloc(2);
    autoarr->Set(0, sqb);
    autoarr->Set(1, sqd);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(bcd, &isflag), isflag));

    autoarr = ArrayOf<IInterface*>::Alloc(4);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqd);
    autoarr->Set(3, sqe);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));
    return 0;
}

int CTest::testSubListRetainAll(int argc, char* argv[])
{
    PEL("CTest::testSubListRetainAll")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    autoarr = ArrayOf<IInterface*>::Alloc(9);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqc);
    autoarr->Set(3, sqd);
    autoarr->Set(4, sqe);
    autoarr->Set(5, sqf);
    autoarr->Set(6, sqg);
    autoarr->Set(7, sqh);
    autoarr->Set(8, sqi);

    AutoPtr<IList> outlist;
    Arrays::AsList(autoarr, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> def;
    list->SubList(3, 6, (IList**)&def);

    autoarr = ArrayOf<IInterface*>::Alloc(3);
    autoarr->Set(0, sqc);
    autoarr->Set(1, sqe);
    autoarr->Set(2, sqh);
    Arrays::AsList(autoarr, (IList**)&outlist);
    def->RetainAll(outlist, &isflag); // the RI fails here

    autoarr = ArrayOf<IInterface*>::Alloc(7);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqc);
    autoarr->Set(3, sqe);
    autoarr->Set(4, sqg);
    autoarr->Set(5, sqh);
    autoarr->Set(6, sqi);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));

    autoarr = ArrayOf<IInterface*>::Alloc(1);
    autoarr->Set(0, sqe);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(def, &isflag), isflag));
    return 0;
}

int CTest::testSubListRemoveAll(int argc, char* argv[])
{
    PEL("CTest::testSubListRemoveAll")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    autoarr = ArrayOf<IInterface*>::Alloc(9);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqc);
    autoarr->Set(3, sqd);
    autoarr->Set(4, sqe);
    autoarr->Set(5, sqf);
    autoarr->Set(6, sqg);
    autoarr->Set(7, sqh);
    autoarr->Set(8, sqi);

    AutoPtr<IList> outlist;
    Arrays::AsList(autoarr, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> def;
    list->SubList(3, 6, (IList**)&def);

    mabcde = ArrayOf<IInterface*>::Alloc(3);
    mabcde->Set(0, sqc);
    mabcde->Set(1, sqe);
    mabcde->Set(2, sqh);
    AutoPtr<IList> abcdlist;
    Arrays::AsList(mabcde, (IList**)&abcdlist);

    def->RemoveAll(abcdlist, &isflag);  // the RI fails here

    autoarr = ArrayOf<IInterface*>::Alloc(8);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqc);
    autoarr->Set(3, sqd);
    autoarr->Set(4, sqf);
    autoarr->Set(5, sqg);
    autoarr->Set(6, sqh);
    autoarr->Set(7, sqi);

    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));
    mabcde = ArrayOf<IInterface*>::Alloc(2);
    mabcde->Set(0, sqd);
    mabcde->Set(1, sqf);
    Arrays::AsList(mabcde, (IList**)&abcdlist);
    assert((abcdlist->Equals(def, &isflag), isflag));
    return 0;
}

int CTest::testAtomicAdds(int argc, char* argv[])
{
    PEL("CTest::testAtomicAdds")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);
    testAddAllIsAtomic(list);
    return 0;
}

int CTest::testSubListAtomicAdds(int argc, char* argv[])
{
    PEL("CTest::testSubListAtomicAdds")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    AutoPtr<IList> outlist;
    list->SubList(0, 0, (IList**)&outlist);
    testAddAllIsAtomic(outlist);
    return 0;
}

int CTest::testSubListAddIsAtEnd(int argc, char* argv[])
{
    PEL("CTest::testSubListAddIsAtEnd")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> bcd;
    list->SubList(1, 4, (IList**)&bcd);
    bcd->Add(sqf, &isflag);

    autoarr = ArrayOf<IInterface*>::Alloc(6);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqc);
    autoarr->Set(3, sqd);
    autoarr->Set(4, sqf);
    autoarr->Set(5, sqe);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));
    autoarr = ArrayOf<IInterface*>::Alloc(4);
    autoarr->Set(0, sqb);
    autoarr->Set(1, sqc);
    autoarr->Set(2, sqd);
    autoarr->Set(3, sqf);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(bcd, &isflag), isflag));
    return 0;
}

int CTest::testSubListAddAll(int argc, char* argv[])
{
    PEL("CTest::testSubListAddAll")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IList> bcd;
    list->SubList(1, 4, (IList**)&bcd);

    autoarr = ArrayOf<IInterface*>::Alloc(4);
    autoarr->Set(0, sqf);
    autoarr->Set(1, sqg);
    autoarr->Set(2, sqh);
    autoarr->Set(3, sqi);
    Arrays::AsList(autoarr, (IList**)&outlist);
    bcd->AddAllEx(1, outlist, &isflag);

    autoarr = ArrayOf<IInterface*>::Alloc(9);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqf);
    autoarr->Set(3, sqg);
    autoarr->Set(4, sqh);
    autoarr->Set(5, sqi);
    autoarr->Set(6, sqc);
    autoarr->Set(7, sqd);
    autoarr->Set(8, sqe);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(list, &isflag), isflag));

    autoarr = ArrayOf<IInterface*>::Alloc(7);
    autoarr->Set(0, sqb);
    autoarr->Set(1, sqf);
    autoarr->Set(2, sqg);
    autoarr->Set(3, sqh);
    autoarr->Set(4, sqi);
    autoarr->Set(5, sqc);
    autoarr->Set(6, sqd);
    Arrays::AsList(autoarr, (IList**)&outlist);
    assert((outlist->Equals(bcd, &isflag), isflag));
    return 0;
}

int CTest::testListIterator(int argc, char* argv[])
{
    PEL("CTest::testListIterator")
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&list);

    AutoPtr<IList> outlist;
    Arrays::AsList(mabcde, (IList**)&outlist);
    list->AddAll(outlist, &isflag);
    AutoPtr<IListIterator> i;
    list->GetListIteratorEx(5, (IListIterator**)&i);
    list->Clear();

    Int32 index = 0;
    i->NextIndex(&index);
    assert(5 == index);

    i->PreviousIndex(&index);
    assert(4 == index);
    i->Previous((IInterface**)&outface);
    assert(Object::Equals(sqe, outface));
    i->NextIndex(&index);
    assert(4 == index);
    i->PreviousIndex(&index);
    assert(3 == index);
    i->HasNext(&isflag);
    assert(isflag);
    i->HasPrevious(&isflag);
    assert(isflag);
    i->Previous((IInterface**)&outface);
    assert(Object::Equals(sqd, outface));
    i->NextIndex(&index);
    assert(3 == index);

    i->PreviousIndex(&index);
    assert(2 == index);
    i->HasNext(&isflag);
    assert(isflag);
    i->HasPrevious(&isflag);
    assert(isflag);
    i->Previous((IInterface**)&outface);
    assert(Object::Equals(sqc, outface));
    i->NextIndex(&index);
    assert(2 == index);
    i->PreviousIndex(&index);
    assert(1 == index);
    i->HasNext(&isflag);
    assert(isflag);
    i->HasPrevious(&isflag);
    assert(isflag);
    i->Previous((IInterface**)&outface);
    assert(Object::Equals(sqb, outface));
    i->NextIndex(&index);
    assert(1 == index);
    i->PreviousIndex(&index);
    assert(0 == index);
    i->HasNext(&isflag);
    assert(isflag);
    i->HasPrevious(&isflag);
    assert(isflag);
    i->Previous((IInterface**)&outface);
    assert(Object::Equals(sqa, outface));
    i->NextIndex(&index);
    assert(0 == index);
    i->PreviousIndex(&index);
    assert(-1 == index);
    i->HasNext(&isflag);
    assert(isflag);
    i->HasPrevious(&isflag);
    assert(!isflag);
    // try {
    ECode ec = i->Previous((IInterface**)&outface);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~~~~\n");
    }
    // } catch (NoSuchElementException expected) {
    // }
    return 0;
}

int CTest::testSerialize(int argc, char* argv[])
{
    PEL("CTest::testSerialize")
    String s = String("aced0005737200296a6176612e7574696c2e636f6e63757272656e742e436f70")
             + String("794f6e577269746541727261794c697374785d9fd546ab90c3030000787077040")
             + String("0000005740001617400016274000163707400016578");

    AutoPtr<IList> contents;
    autoarr = ArrayOf<IInterface*>::Alloc(5);
    autoarr->Set(0, sqa);
    autoarr->Set(1, sqb);
    autoarr->Set(2, sqc);
    autoarr->Set(3, NULL);
    autoarr->Set(4, sqe);
    Arrays::AsList(autoarr, (IList**)&contents);
    AutoPtr<ICopyOnWriteArrayList> list;
    CCopyOnWriteArrayList::New(contents, (ICopyOnWriteArrayList**)&list);

    // new SerializationTester<CopyOnWriteArrayList<String>>(list, s).test();
    return 0;
}

int CTest::testDoesNotRetainToArray(int argc, char* argv[])
{

    PEL("CTest::testDoesNotRetainToArray")
    AutoPtr< ArrayOf<IInterface*> > strings = ArrayOf<IInterface*>::Alloc(3);
    strings->Set(0, sqa);
    strings->Set(1, sqb);
    strings->Set(2, sqc);
    AutoPtr<IList> asList;
    Arrays::AsList(strings, (IList**)&asList);
    // assertEquals(String[].class, asList.toArray().getClass());
    // CopyOnWriteArrayList<Object> objects = new CopyOnWriteArrayList<Object>(asList);
    // objects.add(Boolean.TRUE);
    return 0;
}

void CTest::testAddAllIsAtomic(IList* list)
{
    PEL("CTest::testAddAllIsAtomic")
    AutoPtr<ICountDownLatch> done;
    CCountDownLatch::New(1, (ICountDownLatch**)&done);

    // ExecutorService executor = Executors.newSingleThreadExecutor();
    // Future<?> future = executor.submit(new Runnable() {
    //     @Override public void run() {
    //         while (done.getCount() > 0) {
    //             int listSize = list.size();
    //             assertEquals("addAll() not atomic; size=" + listSize, 0, listSize % 1000);
    //             Thread.yield();
    //         }
    //     }
    // });
    // executor.shutdown();


    AutoPtr<IList> toAdd;
    autoarr = ArrayOf<IInterface*>::Alloc(1000);
    Arrays::AsList(autoarr, (IList**)&toAdd);
    for (int i = 0; i < 100; i++) {
        list->AddAll(toAdd, &isflag);
        list->Clear();
        // Thread.yield();
    }

    done->CountDown();
    // future.get(); // this will throw the above exception
}
