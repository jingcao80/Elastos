#include "test.h"

CTest::CTest()
{
    CBreakIteratorHelper::AcquireSingleton((IBreakIteratorHelper**)&breakhelper);
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochelper);
    lochelper->GetUS((ILocale**)&locUS);
    breakhelper->GetCharacterInstanceEx(locUS,(IBreakIterator**)&iterator);
}

CTest::~CTest()
{

}

int CTest::testGetAvailableLocales(int argc, char* argv[])
{
    AutoPtr<ArrayOf<ILocale*> > locales;
    breakhelper->GetAvailableLocales((ArrayOf<ILocale*> **)&locales);
    assert(locales != NULL); // "Array available locales is null"
    assert((locales != NULL && locales->GetLength() != 0));// "Array available locales is 0-length"
    Boolean found = FALSE;
    for (int i = 0 ;i < locales->GetLength() ; i++) {
        Boolean flagloc(FALSE);
        (*locales)[i]->Equals(locUS,&flagloc);
        if (flagloc) {
            // expected
            found = TRUE;
        }
    }
    assert(found);
    return 0;
}

int CTest::testGetWordInstanceLocale(int argc, char* argv[])
{
    AutoPtr<IBreakIterator> it1;
    AutoPtr<ILocale> locCANADA;
    lochelper->GetCANADA_FRENCH((ILocale**)&locCANADA);
    breakhelper->GetWordInstanceEx(locCANADA,(IBreakIterator**)&it1);
    AutoPtr<IBreakIterator> wordit;
    breakhelper->GetWordInstance((IBreakIterator**)&wordit);
    assert(it1 != wordit); // "Incorrect BreakIterator"

    AutoPtr<ILocale> newloc;
    CLocale::New(String("bad locale"), (ILocale**)&newloc);
    AutoPtr<IBreakIterator> it2;
    breakhelper->GetWordInstanceEx(newloc,(IBreakIterator**)&it2);
    assert( it2 != wordit); // "Incorrect BreakIterator"
    return 0;
}

void assertExpectedWordBoundaries(IBreakIterator * it, String s) {
    int expectedPos = 0;
    int pos = 0;
    it->First(&pos);
    assert(expectedPos == pos);
    while (pos != IBreakIterator::DONE) {
        expectedPos += 5; // The five characters until the end of this word.
        it->Next(&pos);
        assert(expectedPos == pos);

        expectedPos += 1; // The space before the start of the next word...
        if (expectedPos > s.GetLength()) {
            expectedPos = IBreakIterator::DONE; // ...unless we're done.
        }
        it->Next(&pos);
        assert(expectedPos == pos);
    }
}

int CTest::testWordBoundaries(int argc, char* argv[])
{
    StringBuilder sb;
    for (int i = 0; i < 1024; ++i) {
        if (i > 0) {
            sb.AppendChar(' ');
        }
        sb.AppendString(String("12345"));
    }
    String s = sb.ToString();

    AutoPtr<IBreakIterator> it;
    breakhelper->GetWordInstanceEx(locUS,(IBreakIterator**)&it);
    it->SetText(s);
    // Check we're not leaking global references. 2048 would bust the VM's hard-coded limit.
    for (int i = 0; i < 2048; ++i) {
        it->SetText(s);
    }

    AutoPtr<IBreakIterator> clone;
    it->Clone((IInterface**)&clone);
    assertExpectedWordBoundaries(it, s);
    assertExpectedWordBoundaries(clone, s);
    return 0;
}

int CTest::testIsBoundary(int argc, char* argv[])
{
    AutoPtr<IBreakIterator> it;
    breakhelper->GetCharacterInstanceEx(locUS, (IBreakIterator**)&it);
    it->SetText(String("hello"));

    Boolean flagbound(FALSE);
    it->IsBoundary(-1,&flagbound);
    if (!flagbound)
    {
        printf("IsBoundary should failed !!!\n");
    }
    // fail();
    it->IsBoundary(0,&flagbound);
    assert(flagbound);

    it->IsBoundary(1,&flagbound);
    assert(flagbound);

    it->IsBoundary(4,&flagbound);
    assert(flagbound);

    it->IsBoundary(5,&flagbound);
    assert(flagbound);

    it->IsBoundary(6,&flagbound);
    if (!flagbound)
    {
        printf("IsBoundary should failed !!!\n");
    }

    return 0;
}

int CTest::testFollowing(int argc, char* argv[])
{
    AutoPtr<IBreakIterator> it;
    breakhelper->GetCharacterInstanceEx(locUS, (IBreakIterator**)&it);
    ECode ec = it->SetText(String("hello"));

    Int32 flagfollow(0);
    it->Following(-1,&flagfollow);
    if (!flagfollow)
    {
        printf("IsBoundary should failed !!!\n");
    }
    it->Following(0,&flagfollow);
    assert(1 == flagfollow);

    it->Following(1,&flagfollow);
    assert(2 == flagfollow);

    it->Following(4,&flagfollow);
    assert(5 == flagfollow);

    it->Following(5,&flagfollow);
    assert(IBreakIterator::DONE == flagfollow);

    it->Following(6,&flagfollow);
    if (!flagfollow)
    {
        printf("IsBoundary should failed !!!\n");
    }
    return 0;
}

int CTest::testPreceding(int argc, char* argv[])
{
    AutoPtr<IBreakIterator> it;
    breakhelper->GetCharacterInstanceEx(locUS,(IBreakIterator**)&it);
    it->SetText(String("hello"));

    Int32 flagpreced(0);
    it->Preceding(-1,&flagpreced);
    if (!flagpreced)
    {
        printf("IsBoundary should failed !!!\n");
    }
    it->Preceding(0,&flagpreced);
    assert(IBreakIterator::DONE == flagpreced);

    it->Preceding(1,&flagpreced);
    assert(0 == flagpreced);

    it->Preceding(5,&flagpreced);
    assert(4 == flagpreced);

    it->Preceding(6,&flagpreced);
    if (!flagpreced)
    {
        printf("IsBoundary should failed !!!\n");
    }
    return 0;
}
