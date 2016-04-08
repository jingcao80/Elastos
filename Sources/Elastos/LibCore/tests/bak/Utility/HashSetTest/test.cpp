
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
    // Test for method java.util.HashSet()
    AutoPtr<ISet> hs2;
    CHashSet::New((ISet**)&hs2);
    Int32 sizlen = 0;
    assert(0 == (hs2->GetSize(&sizlen), sizlen));
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    // Test for method java.util.HashSet(int)
    AutoPtr<ISet> hs2;
    CHashSet::New(5, (ISet**)&hs2);
    Int32 sizlen = 0;
    assert(0 == (hs2->GetSize(&sizlen), sizlen));
    // try {
    ECode ec = CHashSet::New(-1, (ISet**)&hs2);
    // } catch (IllegalArgumentException e) {
        // return;
    // }
    if (ec != NOERROR) {
        printf("Failed to throw IllegalArgumentException for capacity < 0\n");
    }
    return 0;
}

int CTest::test_ConstructorIF(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorIF")
    // Test for method java.util.HashSet(int, float)
    AutoPtr<ISet> hs2;
    CHashSet::New(5, (Float) 0.5, (ISet**)&hs2);
    Int32 sizlen = 0;
    assert(0 == (hs2->GetSize(&sizlen), sizlen));
    // try {
    ECode ec = CHashSet::New(0, 0, (ISet**)&hs2);
    // } catch (IllegalArgumentException e) {
        // return;
    // }
    if (ec != NOERROR) {
        printf("Failed to throw IllegalArgumentException for initial load factor <= 0\n");
    }
    return 0;
}

int CTest::test_ConstructorLjava_util_Collection(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_util_Collection")
    // Test for method java.util.HashSet(java.util.Collection)
    AutoPtr<ISet> hs2;
    CHashSet::New(hs, (ISet**)&hs2);
    Boolean isflag = FALSE;
    for (int counter = 0; counter < objArray->GetLength(); counter++)
        assert((hs->Contains((*objArray)[counter], &isflag), isflag));
    Int32 sizlen = 0;
    hs2->GetSize(&sizlen);
    PFL_EX("sizlen: %d , objArraylen : %d", sizlen, objArray->GetLength())
    assert(sizlen == objArray->GetLength() + 1);

    // try {
    ECode ec = CHashSet::New(NULL, (ISet**)&hs2);
    if (ec != NOERROR) {
        printf("NullPointerException expected\n");
    }
    // } catch (NullPointerException e) {
        //expected
    // }
    return 0;
}

int CTest::test_addLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_addLjava_lang_Object")
    // Test for method boolean java.util.HashSet.add(java.lang.Object)
    int size = 0;
    hs->GetSize(&size);
    AutoPtr<IInteger32> int8;
    CInteger32::New(8, (IInteger32**)&int8);
    Boolean isflag = FALSE;
    hs->Add(int8, &isflag);
    Int32 sizelen = 0;
    assert((hs->GetSize(&sizelen), sizelen) == size);
    AutoPtr<IInteger32> int_9;
    CInteger32::New(-9, (IInteger32**)&int_9);
    hs->Add(int_9, &isflag);
    assert((hs->GetSize(&sizelen), sizelen) == size + 1);
    assert((hs->Contains(int_9, &isflag), isflag));
    return 0;
}

int CTest::test_clear(int argc, char* argv[])
{
    PEL("CTest::test_clear")
    // Test for method void java.util.HashSet.clear()
    AutoPtr<ISet> orgSet;
    AutoPtr<ICloneable> outclone = (ICloneable*) hs->Probe(EIID_ICloneable);
    outclone->Clone((IInterface**)&orgSet);
    hs->Clear();
    AutoPtr<IIterator> i;
    orgSet->GetIterator((IIterator**)&i);
    Int32 sizelen = 0;
    assert( 0 == (hs->GetSize(&sizelen), sizelen));
    Boolean isflag = FALSE;
    while ((i->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        i->Next((IInterface**)&outface);
        assert(!(hs->Contains(outface, &isflag)));
    }
    return 0;
}

int CTest::test_clone(int argc, char* argv[])
{
    PEL("CTest::test_clone")
    // Test for method java.lang.Object java.util.HashSet.clone()
    AutoPtr<ISet> hs2;
    AutoPtr<ICloneable> outclone = (ICloneable*) hs->Probe(EIID_ICloneable);
PFL
    outclone->Clone((IInterface**)&hs2);
PFL
    assert(hs != hs2);
PFL
    Boolean isflag = FALSE;
PFL
    assert((hs->Equals(hs2, &isflag), isflag));
PFL
    return 0;
}

int CTest::test_containsLjava_lang_Object(int argc, char* argv[])
{
    PEL("CTest::test_containsLjava_lang_Object")
    // Test for method boolean java.util.HashSet.contains(java.lang.Object)
    Boolean isflag = FALSE;
    assert((hs->Contains((*objArray)[90], &isflag), isflag));
    AutoPtr<IInteger32> int10000;
    CInteger32::New(1000000, (IInteger32**)&int10000);

    assert(!(hs->Contains(int10000, &isflag), isflag));

    AutoPtr<ISet> s;
    CHashSet::New((ISet**)&s);
    s->Add(NULL, &isflag);
    assert((s->Contains(NULL, &isflag), isflag));
    return 0;
}

int CTest::test_isEmpty(int argc, char* argv[])
{
    PEL("CTest::test_isEmpty")
    // Test for method boolean java.util.HashSet.isEmpty()
    Boolean isflag = FALSE;
    AutoPtr<ISet> hs2;
    CHashSet::New((ISet**)&hs2);
    assert((hs2->IsEmpty(&isflag), isflag));
    assert(!(hs->IsEmpty(&isflag), isflag));
    return 0;
}

int CTest::test_iterator(int argc, char* argv[])
{
    PEL("CTest::test_iterator")
    // Test for method java.util.Iterator java.util.HashSet.iterator()
    AutoPtr<IIterator> i;
    hs->GetIterator((IIterator**)&i);
    int x = 0;
    Boolean isflag = FALSE;
    while ((i->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        i->Next((IInterface**)&outface);
        assert((hs->Contains(outface, &isflag), isflag));
        ++x;
    }
    Int32 sizelen = 0;
    assert((hs->GetSize(&sizelen), sizelen) == x);

    AutoPtr<ISet> s;
    CHashSet::New((ISet**)&s);
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
    // Test for method boolean java.util.HashSet.remove(java.lang.Object)
    int size = 0;
    hs->GetSize(&size);
    AutoPtr<IInteger32> int98;
    CInteger32::New(98, (IInteger32**)&int98);
    Boolean isflag = FALSE;
    hs->Remove(int98, &isflag);
    assert(!(hs->Contains(int98, &isflag), isflag));
    Int32 sizelen = 0;
    assert((hs->GetSize(&sizelen), sizelen) == size - 1);

    AutoPtr<ISet> s;
    CHashSet::New((ISet**)&s);
    s->Add(NULL, &isflag);
    assert((s->Remove(NULL, &isflag), isflag));
    CInteger32::New(-98, (IInteger32**)&int98);
    assert(!(hs->Remove(int98, &isflag), isflag));
    return 0;
}

int CTest::test_size(int argc, char* argv[])
{
    PEL("CTest::test_size")
    // Test for method int java.util.HashSet.size()
    Int32 sizelen = 0;
    assert((hs->GetSize(&sizelen), sizelen) == (objArray->GetLength() + 1));
    hs->Clear();
    assert(0 == (hs->GetSize(&sizelen), sizelen));
    return 0;
}

int CTest::test_Serialization(int argc, char* argv[])
{
    PEL("CTest::test_Serialization")
    AutoPtr<ISet> hs;
    CHashSet::New((ISet**)&hs);
    AutoPtr<ICharSequence> hello;
    CString::New(String("hello"), (ICharSequence**)&hello);
    AutoPtr<ICharSequence> world;
    CString::New(String("world"), (ICharSequence**)&world);
    Boolean isflag = FALSE;
    hs->Add(hello, &isflag);
    hs->Add(world, &isflag);
    // SerializationTest.verifySelf(hs, comparator);
    // SerializationTest.verifyGolden(this, hs, comparator);
    return 0;
}

void CTest::setUp()
{
    objArray = ArrayOf<IInterface*>::Alloc(1000);
    for (int i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<IInteger32> intface;
        CInteger32::New(i, (IInteger32**)&intface);
        objArray->Set(i, intface);
    }

    Boolean isflag = FALSE;
    ECode ec = CHashSet::New((ISet**)&hs);
    for (int i = 0; i < objArray->GetLength(); i++) {
        hs->Add((*objArray)[i], &isflag);
    }

    hs->Add(NULL, &isflag);
}

void CTest::tearDown()
{
    hs = NULL;
    objArray = NULL;
}
