
#include "test.h"

CTest::CTest()
{
}

CTest::~CTest()
{
}

int CTest::testEmptySetSerialization(int argc, char* argv[])
{
    PEL("CTest::testEmptySetSerialization")

    String s = String("aced0005737200116a6176612e7574696c2e54726565536574dd98509395ed87")
             + String("5b03000078707077040000000078");
    AutoPtr<INavigableSet> set;
    CTreeSet::New((INavigableSet**)&set);
    // new SerializationTester<TreeSet<String>>(set, s).test();
    return 0;
}

int CTest::testSerializationWithComparator(int argc, char* argv[])
{
    PEL("CTest::testSerializationWithComparator")

    String s = String("aced0005737200116a6176612e7574696c2e54726565536574dd98509395ed87")
             + String("5b03000078707372002a6a6176612e6c616e672e537472696e672443617365496")
             + String("e73656e736974697665436f6d70617261746f7277035c7d5c50e5ce0200007870")
             + String("770400000002740001617400016278");
    AutoPtr<INavigableSet> set;
    CTreeSet::New(/*String.CASE_INSENSITIVE_ORDER*/(INavigableSet**)&set);
    AutoPtr<ICharSequence> sqa;
    CString::New(String("a"), (ICharSequence**)&sqa);
    AutoPtr<ICharSequence> sqb;
    CString::New(String("b"), (ICharSequence**)&sqb);
    Boolean isflag = FALSE;
    set->Add(sqa, &isflag);
    set->Add(sqb, &isflag);
    // new SerializationTester<NavigableSet<String>>(set, s) {
    //     @Override protected void verify(NavigableSet<String> deserialized) {
    //         assertEquals(0, deserialized.comparator().compare("X", "x"));
    //     }
    // }.test();
    return 0;
}

int CTest::testSubSetSerialization(int argc, char* argv[])
{
    PEL("CTest::testSubSetSerialization")

    String s = String("aced0005737200116a6176612e7574696c2e54726565536574dd98509395ed87")
             + String("5b03000078707372002a6a6176612e6c616e672e537472696e672443617365496")
             + String("e73656e736974697665436f6d70617261746f7277035c7d5c50e5ce0200007870")
             + String("770400000002740001617400016278");
    AutoPtr<INavigableSet> set;
    CTreeSet::New(/*String.CASE_INSENSITIVE_ORDER*/(INavigableSet**)&set);
    AutoPtr<ICharSequence> sqa;
    CString::New(String("a"), (ICharSequence**)&sqa);
    AutoPtr<ICharSequence> sqb;
    CString::New(String("b"), (ICharSequence**)&sqb);
    AutoPtr<ICharSequence> sqc;
    CString::New(String("c"), (ICharSequence**)&sqc);
    AutoPtr<ICharSequence> sqd;
    CString::New(String("d"), (ICharSequence**)&sqd);
    Boolean isflag = FALSE;
    set->Add(sqa, &isflag);
    set->Add(sqb, &isflag);
    set->Add(sqc, &isflag);
    set->Add(sqd, &isflag);
    // final SortedSet<String> subSet = set.subSet("a", "c");
    // new SerializationTester<SortedSet<String>>(subSet, s) {
    //     @Override protected void verify(SortedSet<String> deserialized) {
    //         assertBounded(deserialized, deserialized == subSet);
    //     }
    // }.test();
    return 0;
}

int CTest::testNavigableSubSetSerialization(int argc, char* argv[])
{
    PEL("CTest::testNavigableSubSetSerialization")

    String s = String("aced0005737200116a6176612e7574696c2e54726565536574dd98509395ed87")
             + String("5b03000078707372002a6a6176612e6c616e672e537472696e672443617365496")
             + String("e73656e736974697665436f6d70617261746f7277035c7d5c50e5ce0200007870")
             + String("770400000002740001627400016378");
    AutoPtr<INavigableSet> set;
    CTreeSet::New(/*String.CASE_INSENSITIVE_ORDER*/(INavigableSet**)&set);
    AutoPtr<ICharSequence> sqa;
    CString::New(String("a"), (ICharSequence**)&sqa);
    AutoPtr<ICharSequence> sqb;
    CString::New(String("b"), (ICharSequence**)&sqb);
    AutoPtr<ICharSequence> sqc;
    CString::New(String("c"), (ICharSequence**)&sqc);
    AutoPtr<ICharSequence> sqd;
    CString::New(String("d"), (ICharSequence**)&sqd);
    Boolean isflag = FALSE;
    set->Add(sqa, &isflag);
    set->Add(sqb, &isflag);
    set->Add(sqc, &isflag);
    set->Add(sqd, &isflag);
    AutoPtr<INavigableSet> res;
    set->SubSetEx(sqa, FALSE, sqc, TRUE, (INavigableSet**)&res);
    AutoPtr<ISortedSet> subSet = ISortedSet::Probe(res);
    // new SerializationTester<SortedSet<String>>(subSet, s) {
    //     @Override protected void verify(SortedSet<String> deserialized) {
    //         assertBounded(deserialized, deserialized == subSet);
    //     }
    // }.test();
    return 0;
}

int CTest::testDescendingSetSerialization(int argc, char* argv[])
{
    PEL("CTest::testDescendingSetSerialization")

    String s = String("aced0005737200116a6176612e7574696c2e54726565536574dd98509395ed87")
             + String("5b0300007870737200276a6176612e7574696c2e436f6c6c656374696f6e73245")
             + String("2657665727365436f6d70617261746f7264048af0534e4ad00200007870770400")
             + String("000002740001627400016178");
    AutoPtr<INavigableSet> set;
    CTreeSet::New((INavigableSet**)&set);
    AutoPtr<ICharSequence> sqa;
    CString::New(String("a"), (ICharSequence**)&sqa);
    AutoPtr<ICharSequence> sqb;
    CString::New(String("b"), (ICharSequence**)&sqb);
    Boolean isflag = FALSE;
    set->Add(sqa, &isflag);
    set->Add(sqb, &isflag);
    AutoPtr<INavigableSet> descendingSet;
    set->DescendingSet((INavigableSet**)&descendingSet);
    // new SerializationTester<NavigableSet<String>>(descendingSet, s) {
    //     @Override protected void verify(NavigableSet<String> deserialized) {
    //         assertEquals("b", deserialized.first());
    //     }
    // }.test();
    return 0;
}

int CTest::testJava5SerializationWithComparator(int argc, char* argv[])
{
    PEL("CTest::testJava5SerializationWithComparator")

    String s = String("aced0005737200116a6176612e7574696c2e54726565536574dd98509395ed87")
             + String("5b03000078707372002a6a6176612e6c616e672e537472696e672443617365496")
             + String("e73656e736974697665436f6d70617261746f7277035c7d5c50e5ce0200007870")
             + String("770400000002740001617400016278");
    AutoPtr<INavigableSet> set;
    CTreeSet::New((INavigableSet**)&set);
    AutoPtr<ICharSequence> sqa;
    CString::New(String("a"), (ICharSequence**)&sqa);
    AutoPtr<ICharSequence> sqb;
    CString::New(String("b"), (ICharSequence**)&sqb);
    Boolean isflag = FALSE;
    set->Add(sqa, &isflag);
    set->Add(sqb, &isflag);
    // new SerializationTester<TreeSet<String>>(set, s) {
    //     @Override protected void verify(TreeSet<String> deserialized) {
    //         assertEquals(0, deserialized.comparator().compare("X", "x"));
    //     }
    // }.test();
    return 0;
}

void CTest::assertBounded(ISortedSet* deserialized, Boolean bounded)
{
    PEL("CTest::assertBounded")

    Boolean isflag = FALSE;
    AutoPtr<ICharSequence> sqe;
    CString::New(String("e"), (ICharSequence**)&sqe);
    if (bounded) {
        // try {
        ECode ec = deserialized->Add(sqe, &isflag);
        if (ec != NOERROR || !isflag) {
            printf("fail~~~~~~~~~~~~~~~~");
        }
        // } catch (IllegalArgumentException expected) {
        // }
    }
    else {
        assert((deserialized->Add(sqe, &isflag), isflag));
        assert((deserialized->Remove(sqe, &isflag), isflag));
    }
}
