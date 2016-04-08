#include "test.h"

#define TEST(a, x) a.test_##x

CTest::CTest(){}

CTest::~CTest(){}

int CTest::test_caseInsensitiveButCasePreserving(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> h;
    CRawHeaders::New((IRawHeaders**)&h);
    String str0("Content-Type");
    String str1("text/plain");
    h->Add(str0, str1);
    // Case-insensitive:
    String strOut;
    h->Get(str0, &strOut);
    Boolean flag = strOut.Equals(str1);
    assert(flag == TRUE);

    String str2("Content-type");
    h->Get(str2, &strOut);
    flag = strOut.Equals(str1);
    assert(flag == TRUE);

    String str3("content-type");
    h->Get(str3, &strOut);
    flag = strOut.Equals(str1);
    assert(flag == TRUE);

    String str4("CONTENT-TYPE");
    h->Get(str4, &strOut);
    flag = strOut.Equals(str1);
    assert(flag == TRUE);

    // ...but case-preserving:
    AutoPtr<IMap> innerMap;
    h->ToMultimap((IMap**)&innerMap);
    AutoPtr<ISet> keyset;
    innerMap->KeySet((ISet**)&keyset);
    AutoPtr<ArrayOf<IInterface*> > array;
    keyset->ToArray((ArrayOf<IInterface*>**)&array);
    AutoPtr<ICharSequence> cs = (ICharSequence*)ICharSequence::Probe((*array)[0]);
    String str5(String("Content-Type"));
    cs->ToString(&strOut);
    flag = strOut.Equals(str5);
    assert(flag == TRUE);

    // We differ from the RI in that the Map we return is also case-insensitive.
    // Our behavior seems more consistent. (And code that works on the RI will work on Android.)
    // AutoPtr<ICharSequence> cs1;
    // CString::New(String("Content-Type"), (ICharSequence**)&cs1);
    // AutoPtr<IInterface> value;
    // innerMap->Get(cs1, (IInterface**)&value);
    // assertEquals(Arrays.asList("text/plain"), h.toMultimap().get("Content-Type"));
    // assertEquals(Arrays.asList("text/plain"), h.toMultimap().get("Content-type")); // RI fails this.
}

// The copy constructor used to be broken for headers with multiple values.
// http://code.google.com/p/android/issues/detail?id=6722
int CTest::test_copyConstructor(int argc, char* argv[])
{
   AutoPtr<IRawHeaders> h1;
   CRawHeaders::New((IRawHeaders**)&h1);
   String str0("key");
   String str1("value1");
   String str2("value2");
   h1->Add(str0, str1);
   h1->Add(str0, str2);
   AutoPtr<IMap> objMap;
   h1->ToMultimap((IMap**)&objMap);
   AutoPtr<IRawHeaders> h2;
   AutoPtr<IRawHeadersHelper> rawHeadersHelper;
   CRawHeadersHelper::AcquireSingleton((IRawHeadersHelper**)&rawHeadersHelper);

   rawHeadersHelper->FromMultimap(objMap, (IRawHeaders**)&h2);
   Int32 length;
   h2->Length(&length);
   assert(2 == length);

   String strOut;
   h2->GetFieldName(0, &strOut);
   Boolean flag = strOut.Equals(str0);
   assert(flag == TRUE);

   h2->GetValue(0, &strOut);
   flag = strOut.Equals(str1);
   assert(flag == TRUE);

   h2->GetFieldName(1, &strOut);
   flag = strOut.Equals(str0);
   assert(flag == TRUE);

   h2->GetValue(1, &strOut);
   flag = strOut.Equals(str2);
   assert(flag == TRUE);
}

