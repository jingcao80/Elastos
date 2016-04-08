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
    // Test for method java.util.LinkedHashSet()
    AutoPtr<ISet> hs2;
    CLinkedHashSet::New((ISet**)&hs2);
    Int32 len = 0;
    assert(0 == (hs2->GetSize(&len), len));
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    // Test for method java.util.LinkedHashSet(int)
    AutoPtr<ISet> hs2;
    CLinkedHashSet::New(5, (ISet**)&hs2);
    Int32 len = 0;
    assert(0 == (hs2->GetSize(&len), len));
    // try {
    ECode ec = CLinkedHashSet::New(-1, (ISet**)&hs2);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected~~~~~~~~~~~~~\n");
    }
    // } catch (IllegalArgumentException e) {
        //expected
    // }

    AutoPtr<ICollection> collection = NULL;

    ec = CLinkedHashSet::New(-1, (ISet**)&hs2);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = CLinkedHashSet::New(-1, 0.5f, (ISet**)&hs2);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

PFL
    ec = CLinkedHashSet::New(1, -0.5f, (ISet**)&hs2);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = CLinkedHashSet::New(1, 0.0f, (ISet**)&hs2);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

PFL
    ec = CLinkedHashSet::New(collection, (ISet**)&hs2);
        assert(ec == E_NULL_POINTER_EXCEPTION);
    return 0;
}

int CTest::test_ConstructorIF(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorIF")
    // Test for method java.util.LinkedHashSet(int, float)
    AutoPtr<ISet> hs2;
    CLinkedHashSet::New(5, (float) 0.5, (ISet**)&hs2);
    Int32 len = 0;
    assert(0 == (hs2->GetSize(&len), len));

    // try {
    ECode ec = CLinkedHashSet::New(-1, 0.5f, (ISet**)&hs2);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected~~~~~~~~\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }

    // try {
    ec = CLinkedHashSet::New(1, -0.5f, (ISet**)&hs2);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected~~~~~~~~\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }

    // try {
    ec = CLinkedHashSet::New(1, 0.0f, (ISet**)&hs2);
    if (ec != NOERROR) {
        printf("IllegalArgumentException expected~~~~~~~~\n");
    }
    // } catch (IllegalArgumentException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_ConstructorLjava_util_Collection(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_util_Collection")
    // Test for method java.util.LinkedHashSet(java.util.Collection)
    AutoPtr<ISet> hs2;
    CLinkedHashSet::New((ISet**)&hs2);
    Boolean isflag = FALSE;
    for (int i = 0; i < objArray->GetLength(); i++)
        hs2->Add((*objArray)[i], &isflag);

    for (int counter = 0; counter < objArray->GetLength(); counter++)
        assert((hs->Contains((*objArray)[counter], &isflag), isflag));

    Int32 len = 0;
    assert((hs2->GetSize(&len), len) == objArray->GetLength());

    // try {
    ECode ec = CLinkedHashSet::New(NULL, (ISet**)&hs2);
    if (ec != NOERROR) {
        printf("NullPointerException expected~~~~~~~~~~\n");
    }
    // } catch (NullPointerException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_addLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_addLjava_lang_Object")
    // Test for method boolean java.util.LinkedHashSet.add(java.lang.Object)
    int size = 0;
    hs->GetSize(&size);
    AutoPtr<IInteger32> int8;
    CInteger32::New(8, (IInteger32**)&int8);
    Boolean isflag = FALSE;
    hs->Add(int8, &isflag);
    Int32 len = 0;
    assert((hs->GetSize(&len), len) == size);
    AutoPtr<IInteger32> int_9;
    CInteger32::New(-9, (IInteger32**)&int_9);
    hs->Add(int_9, &isflag);
    assert((hs->GetSize(&len), len) == (size + 1));
    assert((hs->Contains(int_9, &isflag), isflag));
    return 0;
}

int CTest::test_clear(int argc, char* argv[])
{
    PEL("CTest::test_clear")
    // Test for method void java.util.LinkedHashSet.clear()
    AutoPtr<ISet> orgSet;
    AutoPtr<IInterface> outface;
    ICloneable::Probe(hs)->Clone((IInterface**)&outface);
    orgSet = ISet::Probe(outface);
    hs->Clear();
    AutoPtr<IIterator> i;
    orgSet->GetIterator((IIterator**)&i);
    Int32 len = 0;
    assert(0 == (hs->GetSize(&len), len));
    Boolean isflag = FALSE;
    while (i->HasNext(&isflag), isflag) {
        i->Next((IInterface**)&outface);
        assert(!(hs->Contains(outface, &isflag), isflag));
    }
    return 0;
}

int CTest::test_clone(int argc, char* argv[])
{
    PEL("CTest::test_clone")
    // Test for method java.lang.Object java.util.LinkedHashSet.clone()
    AutoPtr<ISet> hs2;
    AutoPtr<IInterface> outface;
    ICloneable::Probe(hs)->Clone((IInterface**)&outface);
    hs2 = ISet::Probe(outface);
    assert(hs != hs2);
    Boolean isflag = FALSE;
    assert((hs->Equals(hs2, &isflag), isflag));
    return 0;
}

int CTest::test_containsLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsLjava_lang_Object")
    // Test for method boolean
    // java.util.LinkedHashSet.contains(java.lang.Object)
    Boolean isflag = FALSE;
    assert((hs->Contains((*objArray)[90], &isflag), isflag));
    AutoPtr<IInteger32> int10000000;
    CInteger32::New(10000000, (IInteger32**)&int10000000);
    assert(!(hs->Contains(int10000000, &isflag), isflag));

    AutoPtr<ISet> s;
    CLinkedHashSet::New((ISet**)&s);
    s->Add(NULL, &isflag);
    assert((s->Contains(NULL, &isflag), isflag));
    return 0;
}

int CTest::test_isEmpty(int argc, char* argv[])
{
    PEL("CTest::test_isEmpty")
    // Test for method boolean java.util.LinkedHashSet.isEmpty()
    Boolean isflag = FALSE;
    AutoPtr<ISet> hs2;
    CLinkedHashSet::New((ISet**)&hs2);
    hs2->IsEmpty(&isflag);
    assert(isflag);
    assert(!(hs->IsEmpty(&isflag), isflag));
    return 0;
}

int CTest::test_iterator(int argc, char* argv[])
{
    PEL("CTest::test_iterator")
    // Test for method java.util.Iterator java.util.LinkedHashSet.iterator()
    AutoPtr<IIterator> i;
    hs->GetIterator((IIterator**)&i);
    int x = 0;
    int j = 0;
    Boolean isflag = FALSE;
    for (j = 0; i->HasNext(&isflag), isflag; j++) {
        AutoPtr<IInterface> oo;
        i->Next((IInterface**)&oo);
        if (oo != NULL) {
            AutoPtr<IInteger32> ii = IInteger32::Probe(oo);
            Int32 len = 0;
            ii->GetValue(&len);
            assert(len == j);
        }
        else {
            assert((hs->Contains(oo, &isflag), isflag));
        }
        ++x;
    }
    Int32 len = 0;
    assert((hs->GetSize(&len), len) == x);

    AutoPtr<ISet> s;
    CLinkedHashSet::New((ISet**)&s);
    s->Add(NULL, &isflag);
    s->GetIterator((IIterator**)&i);
    AutoPtr<IInterface> outface;
    i->Next((IInterface**)&outface);
    assert(outface == NULL);
    return 0;
}

int CTest::test_removeLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_removeLjava_lang_Object")
    // Test for method boolean
    // java.util.LinkedHashSet.remove(java.lang.Object)
    int size = 0;
    hs->GetSize(&size);
    AutoPtr<IInteger32> int98;
    CInteger32::New(98, (IInteger32**)&int98);
    Boolean isflag = FALSE;
    hs->Remove(int98, &isflag);
    assert(!(hs->Contains(int98, &isflag), isflag));
    Int32 len = 0;
    assert((hs->GetSize(&len), len) == (size - 1));

    AutoPtr<ISet> s;
    CLinkedHashSet::New((ISet**)&s);
    s->Add(NULL, &isflag);
    assert((s->Remove(NULL, &isflag), isflag));
    return 0;
}

int CTest::test_size(int argc, char* argv[])
{
    PEL("CTest::test_size")
    // Test for method int java.util.LinkedHashSet.size()
    Int32 len = 0;
    assert((hs->GetSize(&len), len) == (objArray->GetLength() + 1));
    hs->Clear();
    assert(0 == (hs->GetSize(&len), len));
    return 0;
}

int CTest::test_retainAllLjava_util_Collection(int argc, char* argv[])
{
    PEL("CTest::test_retainAllLjava_util_Collection")
    AutoPtr<ISet> lhs;
    CLinkedHashSet::New((ISet**)&lhs);
    AutoPtr<IVector> v;
    CVector::New((IVector**)&v);
    AutoPtr<IFloat> fl3;
    CFloat::New(3.14, (IFloat**)&fl3);
    AutoPtr<IInterface> outface;
    Boolean isflag = FALSE;
    v->Add(fl3, &isflag);
    AutoPtr<IInteger32> int1;
    CInteger32::New(1, (IInteger32**)&int1);
    lhs->Add(int1, &isflag);
    Int32 len = 0;
    assert(1 == (lhs->GetSize(&len), len));
    lhs->RetainAll(v, &isflag);
    assert(0 == (lhs->GetSize(&len), len));
    CVector::New((IVector**)&v);
    AutoPtr<IInteger32> int2;
    CInteger32::New(2, (IInteger32**)&int2);
    AutoPtr<IInteger32> int3;
    CInteger32::New(3, (IInteger32**)&int3);
    AutoPtr<IInteger32> int4;
    CInteger32::New(4, (IInteger32**)&int4);
    AutoPtr<IInteger32> int5;
    CInteger32::New(5, (IInteger32**)&int5);
    AutoPtr<IInteger32> int6;
    CInteger32::New(6, (IInteger32**)&int6);

    AutoPtr<IInteger32> int7;
    CInteger32::New(7, (IInteger32**)&int7);
    AutoPtr<IInteger32> int8;
    CInteger32::New(8, (IInteger32**)&int8);
    AutoPtr<IInteger32> int9;
    CInteger32::New(9, (IInteger32**)&int9);
    AutoPtr<IInteger32> int10;
    CInteger32::New(10, (IInteger32**)&int10);
    AutoPtr<IInteger32> int11;
    CInteger32::New(11, (IInteger32**)&int11);
    AutoPtr<IInteger32> int12;
    CInteger32::New(12, (IInteger32**)&int12);
    AutoPtr<IInteger32> int13;
    CInteger32::New(13, (IInteger32**)&int13);

    v->Add(int1, &isflag);
    v->Add(int2, &isflag);
    v->Add(int3, &isflag);
    v->Add(int4, &isflag);
    v->Add(int5, &isflag);
    v->Add(int6, &isflag);

    lhs->Add(int1, &isflag);
    lhs->Add(int6, &isflag);
    lhs->Add(int7, &isflag);
    lhs->Add(int8, &isflag);
    lhs->Add(int9, &isflag);
    lhs->Add(int10, &isflag);
    lhs->Add(int11, &isflag);
    lhs->Add(int12, &isflag);
    lhs->Add(int13, &isflag);

    assert(9 == (lhs->GetSize(&len), len));
    lhs->RetainAll(v, &isflag);
    assert(2 == (lhs->GetSize(&len), len));

    // try {
    ECode ec = lhs->RetainAll(NULL, &isflag);
    if (ec != NOERROR) {
        printf("NullPointerException expected~~~~~~\n");
    }
    // } catch (NullPointerException e) {
    //     //expected
    // }

    // lhs = new Mock_LinkedHashSet();

    // try {
    //     lhs.retainAll(v);
    //     fail("UnsupportedOperationException expected");
    // } catch (UnsupportedOperationException e) {
    //     //expected
    // }
    return 0;
}

int CTest::test_toArray(int argc, char* argv[])
{
    PEL("CTest::test_toArray")
    AutoPtr<ISet> lhs;
    CLinkedHashSet::New((ISet**)&lhs);
    AutoPtr<IInteger32> int1;
    CInteger32::New(1, (IInteger32**)&int1);
    AutoPtr<IInteger32> int6;
    CInteger32::New(6, (IInteger32**)&int6);
    AutoPtr<IInteger32> int7;
    CInteger32::New(7, (IInteger32**)&int7);
    AutoPtr<IInteger32> int8;
    CInteger32::New(8, (IInteger32**)&int8);
    AutoPtr<IInteger32> int9;
    CInteger32::New(9, (IInteger32**)&int9);
    AutoPtr<IInteger32> int10;
    CInteger32::New(10, (IInteger32**)&int10);
    AutoPtr<IInteger32> int11;
    CInteger32::New(11, (IInteger32**)&int11);
    AutoPtr<IInteger32> int12;
    CInteger32::New(12, (IInteger32**)&int12);
    AutoPtr<IInteger32> int13;
    CInteger32::New(13, (IInteger32**)&int13);

    Boolean isflag = FALSE;
    lhs->Add(int1, &isflag);
    lhs->Add(int6, &isflag);
    lhs->Add(int7, &isflag);
    lhs->Add(int8, &isflag);
    lhs->Add(int9, &isflag);
    lhs->Add(int10, &isflag);
    lhs->Add(int11, &isflag);
    lhs->Add(int12, &isflag);
    lhs->Add(int13, &isflag);

    AutoPtr< ArrayOf<IInterface*> > o;
    lhs->ToArray((ArrayOf<IInterface*>**)&o);
    for (int i = 0; i < o->GetLength(); i++) {
        assert((lhs->Contains((*o)[i], &isflag), isflag));
    }
    Int32 len = 0;
    assert((lhs->GetSize(&len), len) == o->GetLength());
    return 0;
}

int CTest::test_toArrayLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_toArrayLjava_lang_Object")
    AutoPtr<ISet> lhs;
    CLinkedHashSet::New((ISet**)&lhs);
    AutoPtr<IInteger32> int1;
    CInteger32::New(1, (IInteger32**)&int1);
    AutoPtr<IInteger32> int6;
    CInteger32::New(6, (IInteger32**)&int6);
    AutoPtr<IInteger32> int7;
    CInteger32::New(7, (IInteger32**)&int7);
    AutoPtr<IInteger32> int8;
    CInteger32::New(8, (IInteger32**)&int8);
    AutoPtr<IInteger32> int9;
    CInteger32::New(9, (IInteger32**)&int9);
    AutoPtr<IInteger32> int10;
    CInteger32::New(10, (IInteger32**)&int10);
    AutoPtr<IInteger32> int11;
    CInteger32::New(11, (IInteger32**)&int11);
    AutoPtr<IInteger32> int12;
    CInteger32::New(12, (IInteger32**)&int12);
    AutoPtr<IInteger32> int13;
    CInteger32::New(13, (IInteger32**)&int13);

    Boolean isflag = FALSE;
    lhs->Add(int1, &isflag);
    lhs->Add(int6, &isflag);
    lhs->Add(int7, &isflag);
    lhs->Add(int8, &isflag);
    lhs->Add(int9, &isflag);
    lhs->Add(int10, &isflag);
    lhs->Add(int11, &isflag);
    lhs->Add(int12, &isflag);
    lhs->Add(int13, &isflag);

    Int32 len = 0;
    lhs->GetSize(&len);
    AutoPtr< ArrayOf<IInterface*> > o1 = ArrayOf<IInterface*>::Alloc(len);
    AutoPtr< ArrayOf<IInterface*> > o2 = ArrayOf<IInterface*>::Alloc(len);
    lhs->ToArray((ArrayOf<IInterface*>**)&o1);
    for (int i = 0; i < o1->GetLength(); i++) {
        assert((lhs->Contains((*o1)[i], &isflag), isflag));
    }

    // try {
    ECode ec = lhs->ToArray(NULL);
    if (ec != NOERROR) {
        printf("NullPointerException expected~~~~~~~~~~\n");
    }
    // } catch (NullPointerException e) {
        //expected
    // }

    // try {
    ec = lhs->ToArray((ArrayOf<IInterface*>**)&o2);
    if (ec != NOERROR) {
        printf("NullPointerException expected~~~~~~~~~~\n");
    }
    // } catch (ArrayStoreException e) {
    //     //expected
    // }
    return 0;
}

void CTest::setUp()
{
    PEL("CTest::setUp")
    objArray = ArrayOf<IInterface*>::Alloc(1000);
    for (int i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<IInteger32> inti;
        CInteger32::New(i, (IInteger32**)&inti);
        objArray->Set(i, inti);
    }

    CLinkedHashSet::New((ISet**)&hs);
    Boolean isflag = FALSE;
    for (int i = 0; i < objArray->GetLength(); i++)
        hs->Add((*objArray)[i], &isflag);
    hs->Add(NULL, &isflag);
}

void CTest::tearDown()
{
    PEL("CTest::tearDown")
    objArray = NULL;
    hs = NULL;
}
