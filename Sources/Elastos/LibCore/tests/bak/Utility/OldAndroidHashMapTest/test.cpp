#include "test.h"
#include "cmdef.h"

const Int32 CTest::ONE;
const Int32 CTest::TWO;
const Int32 CTest::THREE;
const Int32 CTest::FOUR;

CTest::CTest() {
}

CTest::~CTest() {
}

int CTest::testBug17184(int argc, char* argv[])
{
    PEL("CTest::testBug17184")
    AutoPtr<IMap> map1 = NULL;
    AutoPtr<IMap> map2 = NULL;
    AutoPtr<IMap> map3 = NULL;
    AutoPtr<IMap> map4 = NULL;
    ECode ec;
    ec = CHashMap::New((IMap**)&map1);
        assert(ec == NOERROR);

    ec = CHashMap::New(10, (IMap**)&map2);
        assert(ec == NOERROR);

    ec = CHashMap::New(0, (IMap**)&map2);
        assert(ec == NOERROR);

    ec = CHashMap::New(5, 0.5f, (IMap**)&map3);
        assert(ec == NOERROR);

    ec = CHashMap::New(map1, (IMap**)&map4);
        assert(ec == NOERROR);

    //abnormal
    ec = CHashMap::New(-1, (IMap**)&map2);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = CHashMap::New(10, -1.0f, (IMap**)&map3);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = CHashMap::New(0, 0.0f, (IMap**)&map3);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

return 0;
}

int CTest::testAdd(int argc, char* argv[])
{
    PEL("testAdd");
    AutoPtr<IMap> map;
    ECode ec = CHashMap::New((IMap**)&map);
    addItems(map);
    return 0;
}

int CTest::testClear(int argc, char* argv[])
{
    PEL("testClear");
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);

    addItems(map);
    map->Clear();
    Int32 mapsize = 0;
    assert(0 == (map->GetSize(&mapsize), mapsize));
    return 0;
}

int CTest::testRemove(int argc, char* argv[])
{
    PEL("testRemove");
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);

    addItems(map);
    AutoPtr<IInterface> rmface;
    AutoPtr<ICharSequence> cs1;
    CString::New(String("three"), (ICharSequence**)&cs1);
    PFL_EX("Remove: cs1:%p", cs1.Get())

    map->Remove(cs1, (IInterface**)&rmface);
    PFL_EX("rmface: %p", rmface.Get())
    assert((map->Get(cs1, (IInterface**)&rmface), rmface) == NULL);
    PFL_EX("rmface: %p", rmface.Get())

    return 0;
}

int CTest::testManipulate(int argc, char* argv[])
{
    PEL("testManipulate");

    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);

    Boolean isflag = FALSE;
    assert((map->IsEmpty(&isflag), isflag));

    Int32 sizelen = 0;
    assert(0 == (map->GetSize(&sizelen), sizelen));

    AutoPtr<IInterface> intface;
    assert((map->Get(NULL, (IInterface**)&intface), intface) == NULL);

    AutoPtr<ICharSequence> cs1;
    CString::New(String("one"), (ICharSequence**)&cs1);
    intface = NULL;
    assert((map->Get(cs1, (IInterface**)&intface), intface) == NULL);

    assert((map->ContainsKey(cs1, &isflag), !isflag));

    AutoPtr<IInteger32> integer1;
    CInteger32::New(1, (IInteger32**)&integer1);
    assert((map->ContainsValue(integer1, &isflag), !isflag));

    intface = NULL;
    assert((map->Remove(NULL, (IInterface**)&intface), intface) == NULL);

    intface = NULL;
    assert((map->Remove(cs1, (IInterface**)&intface), intface) == NULL);

    intface = NULL;
    AutoPtr<IInteger32> Integer_1;
    CInteger32::New(-1, (IInteger32**)&Integer_1);
    assert((map->Put(NULL, Integer_1, (IInterface**)&intface), intface) == NULL);

    intface = NULL;
    assert((map->Put(cs1, integer1, (IInterface**)&intface), intface) == NULL);

    AutoPtr<ICharSequence> cs2;
    CString::New(String("two"), (ICharSequence**)&cs2);
    intface = NULL;
    AutoPtr<IInteger32> Integer2;
    CInteger32::New(2, (IInteger32**)&Integer2);
    assert((map->Put(cs2, Integer2, (IInterface**)&intface), intface) == NULL);

    AutoPtr<ICharSequence> cs3;
    CString::New(String("three"), (ICharSequence**)&cs3);

    intface = NULL;
    AutoPtr<IInteger32> Integer3;
    CInteger32::New(3, (IInteger32**)&Integer3);
    assert((map->Put(cs3, Integer3, (IInterface**)&intface), intface) == NULL);

    AutoPtr<IInteger32> Integer0;
    CInteger32::New(0, (IInteger32**)&Integer0);
    intface = NULL;
    map->Put(NULL, Integer0, (IInterface**)&intface);

    Int32 value = 0;
    intface = NULL;
    map->Get(NULL, (IInterface**)&intface);
    AutoPtr<IInteger32> res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(0 == value);

    intface = NULL;
    map->Get(cs1, (IInterface**)&intface);
    res = NULL;
    res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(1 == value);

    intface = NULL;
    map->Get(cs2, (IInterface**)&intface);
    res = NULL;
    res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(2 == value);

    intface = NULL;
    map->Get(cs3, (IInterface**)&intface);
    res = NULL;
    res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(3 == value);

    assert((map->ContainsKey(NULL, &isflag), isflag));
    assert((map->ContainsKey(cs1, &isflag), isflag));
    assert((map->ContainsKey(cs2, &isflag), isflag));
    assert((map->ContainsKey(cs3, &isflag), isflag));

    assert((map->ContainsValue(Integer0, &isflag), isflag));
    assert((map->ContainsValue(integer1, &isflag), isflag));
    assert((map->ContainsValue(Integer2, &isflag), isflag));
    assert((map->ContainsValue(Integer3, &isflag), isflag));

    intface = NULL;
    map->Remove(NULL, (IInterface**)&intface);
    res = NULL;
    res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(0 == value);

    intface = NULL;
    map->Remove(cs1, (IInterface**)&intface);
    res = NULL;
    res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(1 == value);

    intface = NULL;
    map->Remove(cs2, (IInterface**)&intface);
    res = NULL;
    res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(2 == value);

    intface = NULL;
    map->Remove(cs3, (IInterface**)&intface);
    res = NULL;
    res = (IInteger32*) intface->Probe(EIID_IInteger32);
    res->GetValue(&value);
    assert(3 == value);

    assert((map->IsEmpty(&isflag), isflag));
    assert(0 == (map->GetSize(&sizelen), sizelen));
    intface = NULL;
    assert((map->Get(NULL, (IInterface**)&intface), intface) == NULL);
    assert((map->Get(cs1, (IInterface**)&intface), intface) == NULL);
    assert((map->ContainsKey(cs1, &isflag), !isflag));
    assert((map->ContainsValue(integer1, &isflag), !isflag));

    intface = NULL;
    assert((map->Remove(NULL, (IInterface**)&intface), intface) == NULL);

    intface = NULL;
    assert((map->Remove(cs1, (IInterface**)&intface), intface) == NULL);
    return 0;
}

int CTest::testKeyIterator(int argc, char* argv[])
{
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);

    Boolean slots[4] = {FALSE};

    addItems(map);

    AutoPtr< ArrayOf<PInterface> > outarr;
    AutoPtr<ISet> outset;
    map->KeySet((ISet**)&outset);
    outset->ToArray((ArrayOf<PInterface>**)&outarr);
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
        AutoPtr<ICharSequence> res = (ICharSequence*) (*outarr)[i]->Probe(EIID_ICharSequence);
        String s;
        if (res == NULL) {
            printf("res == NULL error ~~~~~~~~~\n");
        }
        res->ToString(&s);
        Int32 slot = 0;

        if (s.Equals("one")) {
            slot = 0;
        }
        else if (s.Equals("two")) {
            slot = 1;
        }
        else if (s.Equals("three")) {
            slot = 2;
        }
        else if (s.Equals("four")) {
            slot = 3;
        }
        else {
            printf("Unknown key in HashMap \n");
        }

        if (slots[slot]) {
            printf("key returned more than once \n");
        }
        else {
            slots[slot] = TRUE;
        }
    }

    assert(slots[0]);
    assert(slots[1]);
    assert(slots[2]);
    assert(slots[3]);
    return 0;
}

int CTest::testValueIterator(int argc, char* argv[])
{
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);

    Boolean slots[4] = {FALSE};

    addItems(map);

    AutoPtr<ICollection> coll;
    map->Values((ICollection**)&coll);
    AutoPtr< ArrayOf<IInterface*> > res;
    coll->ToArray((ArrayOf<IInterface*>**)&res);
    for (Int32 i = 0; i < res->GetLength(); i++) {
        AutoPtr<IInteger32> o = (IInteger32*) (*res)[i]->Probe(EIID_IInteger32);
        if (o == NULL) {
            printf("o == NULL error ~~~~~~~~~~\n");
        }
        Int32 slot = 0;
        Int32 value = 0;
        o->GetValue(&value);
        if (value == ONE) {
            slot = 0;
        }
        else if (value == TWO) {
            slot = 1;
        }
        else if (value == THREE) {
            slot = 2;
        }
        else if (value == FOUR) {
            slot = 3;
        }
        else {
            printf("Unknown value in HashMap\n");
        }

        if (slots[slot]) {
            printf("value returned more than once\n");
        }
        else {
            slots[slot] = TRUE;
        }
    }

    assert(slots[0]);
    assert(slots[1]);
    assert(slots[2]);
    assert(slots[3]);
    return 0;
}

int CTest::testEntryIterator(int argc, char* argv[])
{
    PEL("CTest::testEntryIterator")
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);

    Boolean slots[4] = {FALSE};

    addItems(map);

    AutoPtr< ArrayOf<IInterface*> > outarr;
    AutoPtr<ISet> outset;
PFL
    map->EntrySet((ISet**)&outset);
PFL_EX("outset: %p", outset.Get())
    outset->ToArray((ArrayOf<IInterface*>**)&outarr);
PFL
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
PFL
        AutoPtr<IMapEntry> o = IMapEntry::Probe((*outarr)[i]);
PFL_EX("o: %p", o.Get())
        AutoPtr<IInterface> keyface;
        AutoPtr<IInterface> valueface;
        o->GetKey((IInterface**)&keyface);
        o->GetValue((IInterface**)&valueface);
        AutoPtr<ICharSequence> res1 = (ICharSequence*) keyface->Probe(EIID_ICharSequence);
        if (res1 == NULL) {
            printf("res1 == NULL error~~~~~~~\n");
            return -1;
        }
        AutoPtr<IInteger32> res2 = (IInteger32*) valueface->Probe(EIID_IInteger32);
        if (res2 == NULL) {
            printf("res2 == NULL error~~~~~~~\n");
            return -1;
        }
        String str;
        Int32 value = 0;
        res1->ToString(&str);
        res2->GetValue(&value);
        int slot = 0;

        if ( str == "one" && value == 1) {
            slot = 0;
        }
        else if (str == "two" && value == 2) {
            slot = 1;
        }
        else if (str == "three" && value == 3) {
            slot = 2;
        }
        else if (str == "four" && value == 4) {
            slot = 3;
        }
        else {
            printf("Unknown entry in HashMap\n");
        }

        if (slots[slot]) {
            printf("entry returned more than once\n");
        } else {
            slots[slot] = true;
        }
    }

    assert(slots[0]);
    assert(slots[1]);
    assert(slots[2]);
    assert(slots[3]);
    return 0;
}

int CTest::testEquals(int argc, char* argv[])
{
    AutoPtr<IMap> map1;
    CHashMap::New((IMap**)&map1);

    AutoPtr<IMap> map2;
    CHashMap::New((IMap**)&map2);

    AutoPtr<IMap> map3;
    CHashMap::New((IMap**)&map3);

    AutoPtr<ICharSequence> cs1;
    CString::New(String("one"), (ICharSequence**)&cs1);

    AutoPtr<ICharSequence> cs2;
    CString::New(String("two"), (ICharSequence**)&cs2);

    AutoPtr<ICharSequence> cs3;
    CString::New(String("three"), (ICharSequence**)&cs3);

    AutoPtr<ICharSequence> int1;
    CString::New(String("1"), (ICharSequence**)&int1);

    AutoPtr<ICharSequence> int2;
    CString::New(String("2"), (ICharSequence**)&int2);

    AutoPtr<ICharSequence> int3;
    CString::New(String("3"), (ICharSequence**)&int3);

    AutoPtr<IInterface> outface;
    map1->Put(cs1, int1, (IInterface**)&outface);
    map1->Put(cs2, int2, (IInterface**)&outface);
    map1->Put(cs3, int3, (IInterface**)&outface);

    map2->Put(cs1, int1, (IInterface**)&outface);
    map2->Put(cs2, int2, (IInterface**)&outface);
    map2->Put(cs3, int3, (IInterface**)&outface);

    Boolean isflag = FALSE;
    assert((map1->Equals(map2, &isflag), isflag));

    map3->Put(cs1, int1, (IInterface**)&outface);
    map3->Put(cs2, int1, (IInterface**)&outface);
    map3->Put(cs3, int1, (IInterface**)&outface);

    assert((map1->Equals(map3, &isflag), !isflag));
    assert((map2->Equals(map3, &isflag), !isflag));
    return 0;
}

void CTest::addItems(IMap* map)
{
    PEL("addItems");
    AutoPtr<IInteger32> Integer1;
    CInteger32::New(ONE, (IInteger32**)&Integer1);

    AutoPtr<IInteger32> Integer2;
    CInteger32::New(TWO, (IInteger32**)&Integer2);

    AutoPtr<IInteger32> Integer3;
    CInteger32::New(THREE, (IInteger32**)&Integer3);

    AutoPtr<IInteger32> Integer4;
    CInteger32::New(FOUR, (IInteger32**)&Integer4);

    AutoPtr<ICharSequence> cs1;
    CString::New(String("one"), (ICharSequence**)&cs1);

    AutoPtr<ICharSequence> cs2;
    CString::New(String("two"), (ICharSequence**)&cs2);

    AutoPtr<ICharSequence> cs3;
    CString::New(String("three"), (ICharSequence**)&cs3);
    PFL_EX("addItems: cs3:%p", cs3.Get())

    AutoPtr<ICharSequence> cs4;
    CString::New(String("four"), (ICharSequence**)&cs4);

    AutoPtr<IInterface> outface;
    map->Put(cs1, Integer1, (IInterface**)&outface);
    map->Put(cs2, Integer2, (IInterface**)&outface);
    map->Put(cs3, Integer3, (IInterface**)&outface);
    map->Put(cs4, Integer4, (IInterface**)&outface);

    Int32 sizelen = 0;
    assert(4 == (map->GetSize(&sizelen), sizelen));

    Boolean isflag = FALSE;
    assert((Integer1->Equals((map->Get(cs1, (IInterface**)&outface), (IInteger32*) outface->Probe(EIID_IInteger32)), &isflag), isflag));
    assert((Integer2->Equals((map->Get(cs2, (IInterface**)&outface), (IInteger32*) outface->Probe(EIID_IInteger32)), &isflag), isflag));
    assert((Integer3->Equals((map->Get(cs3, (IInterface**)&outface), (IInteger32*) outface->Probe(EIID_IInteger32)), &isflag), isflag));
    assert((Integer4->Equals((map->Get(cs4, (IInterface**)&outface), (IInteger32*) outface->Probe(EIID_IInteger32)), &isflag), isflag));
}
