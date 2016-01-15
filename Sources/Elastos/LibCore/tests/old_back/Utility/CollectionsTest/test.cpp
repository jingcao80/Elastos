
#include "test.h"

using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IMap;
using Elastos::Utility::ICollection;

//====================================================================
// CTest::
//====================================================================

CTest::CTest()
{
}

CTest::~CTest()
{
}

// test 1
int CTest::testEmptyEnumeration(int argc, char* argv[])
{
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    AutoPtr<IEnumeration> e;
    cls->EmptyEnumeration((IEnumeration**)&e);
//    assertFalse(e instanceof Serializable);
    Boolean res = FALSE;
    e->HasMoreElements(&res);
    assert(res == FALSE);

    AutoPtr<IInterface> ele;
    ECode ec = e->NextElement((IInterface**)&ele);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
    // try {
    //     e.nextElement();
    //     fail();
    // } catch (NoSuchElementException expected) {
    // }
    return 0;
}

// test 2
int CTest::testEmptyIterator(int argc, char* argv[])
{
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);

    AutoPtr<IIterator> it;
    cls->EmptyIterator((IIterator**)&it);
    test_EmptyIterator(it);

    AutoPtr<IList> l;
    cls->EmptyList((IList**)&l);
    AutoPtr<IIterator> it2;
    l->GetIterator((IIterator**)&it2);
    test_EmptyIterator(it2);

    AutoPtr<ISet> s;
    cls->EmptySet((ISet**)&s);
    AutoPtr<IIterator> it3;
    s->GetIterator((IIterator**)&it3);
    test_EmptyIterator(it3);

    AutoPtr<IMap> m;
    cls->EmptyMap((IMap**)&m);
    AutoPtr<ISet> keyS;
    m->KeySet((ISet**)&keyS);
    AutoPtr<IIterator> it4;
    keyS->GetIterator((IIterator**)&it4);
    test_EmptyIterator(it4);

    AutoPtr<ISet> entryS;
    m->EntrySet((ISet**)&entryS);
    AutoPtr<IIterator> it5;
    entryS->GetIterator((IIterator**)&it5);
    test_EmptyIterator(it5);

    AutoPtr<ICollection> val;
    m->Values((ICollection**)&val);
    AutoPtr<IIterator> it6;
    val->GetIterator((IIterator**)&it6);
    test_EmptyIterator(it6);
    return 0;
}

void CTest::test_EmptyIterator(IIterator* i)
{
//    assertFalse(i instanceof Serializable);
    Boolean b = FALSE;
    i->HasNext(&b);
    assert(b == FALSE);
    AutoPtr<IInterface> obj;
    ECode ec = i->Next((IInterface**)&obj);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
    // try {
    //     i.next();
    //     fail();
    // } catch (NoSuchElementException expected) {
    // }
    ec = i->Remove();
    assert(ec == E_ILLEGAL_STATE_EXCEPTION);
    // try {
    //     i.remove();
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
}

// test 3
int CTest::testEmptyListIterator(int argc, char* argv[])
{
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);

    AutoPtr<IListIterator> ilt1;
    cls->EmptyListIterator((IListIterator**)&ilt1);
    test_EmptyListIterator(ilt1);

    AutoPtr<IList> l;
    cls->EmptyList((IList**)&l);
    AutoPtr<IListIterator> ilt2;
    l->GetListIterator((IListIterator**)&ilt2);
    test_EmptyListIterator(ilt2);

    AutoPtr<IListIterator> ilt3;
    l->GetListIteratorEx(0, (IListIterator**)&ilt3);
    test_EmptyListIterator(ilt3);
    return 0;
}

void CTest::test_EmptyListIterator(IListIterator* i)
{
//    assertFalse(i instanceof Serializable);
    Boolean b = FALSE;
    i->HasNext(&b);
    assert(b == FALSE);
    i->HasPrevious(&b);
    assert(b == FALSE);
    Int32 index;
    i->NextIndex(&index);
    assert(0 == index);

    AutoPtr<IInterface> obj;
    ECode ec = i->Next((IInterface**)&obj);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
    // try {
    //     i.next();
    //     fail();
    // } catch (NoSuchElementException expected) {
    // }
    i->PreviousIndex(&index);
    assert(-1 == index);
    AutoPtr<IInterface> obj2;
    ec = i->Previous((IInterface**)&obj2);
    assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
    // try {
    //     i.previous();
    //     fail();
    // } catch (NoSuchElementException expected) {
    // }
    ec = i->Add(NULL);
    assert(ec != NOERROR);
    // try {
    //     i.add(null);
    //     fail();
    // } catch (UnsupportedOperationException expected) {
    // }
    ec = i->Remove();
    assert(ec == E_ILLEGAL_STATE_EXCEPTION);
    // try {
    //     i.remove();
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
}