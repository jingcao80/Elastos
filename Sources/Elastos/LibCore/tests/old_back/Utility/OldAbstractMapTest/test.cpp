#include "test.h"
#include "cmdef.h"

CTest::CTest() {
}

CTest::~CTest() {
}

int CTest::test_Constructor(int argc, char* argv[])
{
    // AMT amt = new AMT();
    // assertNotNull(amt);
}

int CTest::test_equalsLjava_lang_Object(int argc, char* argv[])
{
    // AbstractMap<String, String> amt1 = new AMT();
    // AbstractMap<String, String> amt2 = new AMT();
    // assertTrue("assert 0", amt1.equals(amt2));
    // assertTrue("assert 1", amt1.equals(amt1));
    // assertTrue("assert 2", amt2.equals(amt1));
    // amt1.put("1", "one");
    // assertFalse("assert 3", amt1.equals(amt2));
    // amt1.put("2", "two");
    // amt1.put("3", "three");

    // amt2.put("1", "one");
    // amt2.put("2", "two");
    // amt2.put("3", "three");
    // assertTrue("assert 4", amt1.equals(amt2));
    // assertFalse("assert 5", amt1.equals(this));
}
int CTest::test_hashCode(int argc, char* argv[])
{
    // AMT amt1 = new AMT();
    // AMT amt2 = new AMT();
    // amt1.put("1", "one");

    // assertNotSame(amt1.hashCode(), amt2.hashCode());
}
int CTest::test_isEmpty(int argc, char* argv[])
{
    // AMT amt = new AMT();
    // assertTrue(amt.isEmpty());
    // amt.put("1", "one");
    // assertFalse(amt.isEmpty());
}
int CTest::test_put(int argc, char* argv[])
{
    // AMT amt = new AMT();
    // assertEquals(0, amt.size());
    // amt.put("1", "one");
    // assertEquals(1, amt.size());
    // amt.put("2", "two");
    // assertEquals(2, amt.size());
    // amt.put("3", "three");
    // assertEquals(3, amt.size());
}
int CTest::test_size(int argc, char* argv[])
{
    // AMT amt = new AMT();
    // assertEquals(0, amt.size());
    // amt.put("1", "one");
    // assertEquals(1, amt.size());
    // amt.put("2", "two");
    // assertEquals(2, amt.size());
    // amt.put("3", "three");
    // assertEquals(3, amt.size());
}
int CTest::test_toString(int argc, char* argv[])
{
    // AMT amt = new AMT();
    // assertEquals("{}", amt.toString());
    // amt.put("1", "one");
    // assertEquals("{1=one}", amt.toString());
    // amt.put("2", "two");
    // assertEquals("{1=one, 2=two}", amt.toString());
    // amt.put("3", "three");
    // assertEquals("{1=one, 2=two, 3=three}", amt.toString());
}

PInterface CTest::AMT::Probe(
    /* [in] */ REIID riid)
{
    return NULL;
}

CARAPI CTest::AMT::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    // AutoPtr<>
    // String result = remove(key);
    // entries.add(new AbstractMap.SimpleEntry<String, String>(key, value));
    // return result;
}

CARAPI CTest::AMT::EntrySet(
    /* [out] */ ISet** entries)
{
    // return new AbstractSet<Entry<String, String>>() {
    //     @Override public Iterator<Entry<String, String>> iterator() {
    //         return entries.iterator();
    //     }
    //     @Override public int size() {
    //         return entries.size();
    //     }
    // };
}
