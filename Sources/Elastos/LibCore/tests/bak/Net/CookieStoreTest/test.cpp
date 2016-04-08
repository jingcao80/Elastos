
#include "test.h"

CTest::CTest()
{
    setUp();
}

CTest::~CTest()
{
    tearDown();
}

int CTest::test_add_LURI_LHttpCookie(int argc, char* argv[])
{
    PEL("CTest::test_add_LURI_LHttpCookie")
    AutoPtr<IURI> uri;
    CURI::New(String("http://harmony.test.unit.org"), (IURI**)&uri);
    AutoPtr<IHttpCookie> cookie;
PFL
    CHttpCookie::New(String("name1"), String("value1"), (IHttpCookie**)&cookie);
PFL_EX("cookieStore: %p", cookieStore.Get())
    cookie->SetDiscard(true);
    // try {
    ECode ec = cookieStore->Add(NULL, cookie);
PFL
    if (ec != NOERROR) {
        printf("1 should throw NullPointerException\n");
    }
    // } catch (NullPointerException e) {
    //     // expected
    // }

    // try {
    ec = cookieStore->Add(uri, NULL);
    if (ec != NOERROR) {
        printf("2 should throw NullPointerException\n");
    }
    // } catch (NullPointerException e) {
    //     // expected
    // }

    // try {
    ec = cookieStore->Add(NULL, NULL);
    if (ec != NOERROR) {
        printf("3 should throw NullPointerException\n");
    }
    // } catch (NullPointerException e) {
    //     // expected
    // }

    cookieStore->Add(uri, cookie);
    AutoPtr<IList> list;
    cookieStore->Get(uri, (IList**)&list);
    Int32 sizelen = 0;
    list->GetSize(&sizelen);
    assert(1 == sizelen);
    Boolean isflag = FALSE;
    list->Contains(cookie, &isflag);
    assert(isflag);

    AutoPtr<IHttpCookie> cookie2;
    CHttpCookie::New(String("  NaME1   "), String("  TESTVALUE1   "), (IHttpCookie**)&cookie2);
    cookieStore->Add(uri, cookie2);
    cookieStore->Get(uri, (IList**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);
    AutoPtr<IInterface> outface;
    list->Get(0, (IInterface**)&outface);
    String outstr;
    IHttpCookie::Probe(outface)->GetValue(&outstr);
    assert(String("  TESTVALUE1   ") == outstr);
    list->Contains(cookie2, &isflag);
    assert(isflag);

    // domain and path attributes works
    AutoPtr<IHttpCookie> anotherCookie;
    CHttpCookie::New(String("name1"), String("value1"), (IHttpCookie**)&anotherCookie);
    anotherCookie->SetDomain(String("domain"));
    anotherCookie->SetPath(String("Path"));
    cookieStore->Add(uri, anotherCookie);
    cookieStore->Get(uri, (IList**)&list);
    list->GetSize(&sizelen);
PFL_EX("sizelen: %d == 2", sizelen)
    // assert(2 == sizelen);
    list->Get(0, (IInterface**)&outface);
    IHttpCookie::Probe(outface)->GetDomainAttr(&outstr);
PFL_EX("outstr: %s", outstr.string())
    assert(outstr.IsNull());
    list->Get(1, (IInterface**)&outface);
    IHttpCookie::Probe(outface)->GetDomainAttr(&outstr);
    assert(String("domain") == outstr);
    IHttpCookie::Probe(outface)->GetPath(&outstr);
    assert(String("Path") == outstr);

    AutoPtr<IURI> uri2;
    CURI::New(String("http://.test.unit.org"), (IURI**)&uri2);
    AutoPtr<IHttpCookie> cookie3;
    CHttpCookie::New(String("NaME2"), String("VALUE2"), (IHttpCookie**)&cookie3);
    cookieStore->Add(uri2, cookie3);
    cookieStore->Get(uri2, (IList**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);
    list->Get(0, (IInterface**)&outface);
    IHttpCookie::Probe(outface)->GetValue(&outstr);
    assert(String("VALUE2") == outstr);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(3 == sizelen);

    // expired cookie won't be selected.
    AutoPtr<IHttpCookie> cookie4;
    CHttpCookie::New(String("cookie4"), String("value4"), (IHttpCookie**)&cookie4);
    cookie4->SetMaxAge(-2);
    cookie4->HasExpired(&isflag);
    assert(isflag);
    cookieStore->Add(uri2, cookie4);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(3 == sizelen);
    cookieStore->Remove(uri2, cookie4, &isflag);
    assert(!isflag);

    cookie4->SetMaxAge(3000);
    cookie4->SetDomain(String("domain"));
    cookie4->SetPath(String("path"));
    cookieStore->Add(uri2, cookie4);
    cookieStore->Get(uri2, (IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);

    cookieStore->Add(uri, cookie4);
    cookieStore->Get(uri, (IList**)&list);
    list->GetSize(&sizelen);
    assert(3 == sizelen);
    cookieStore->Get(uri2, (IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(4 == sizelen);

    AutoPtr<IURI> baduri;
    CURI::New(String("bad_url"), (IURI**)&baduri);
    AutoPtr<IHttpCookie> cookie6;
    CHttpCookie::New(String("cookie5"), String("value5"), (IHttpCookie**)&cookie6);
    cookieStore->Add(baduri, cookie6);
    cookieStore->Get(baduri, (IList**)&list);
    list->Contains(cookie6, &isflag);
    assert(isflag);
    return 0;
}

int CTest::test_get_LURI(int argc, char* argv[])
{
    PEL("CTest::test_get_LURI")
    // try {
    AutoPtr<IList> list;
    ECode ec = cookieStore->Get(NULL, (IList**)&list);
    if (ec != NOERROR) {
        printf("should throw NullPointerException   ~~~~~\n");
    }
    // } catch (NullPointerException e) {
    //     // expected
    // }

    AutoPtr<IURI> uri1;
    CURI::New(String("http://get.uri1.test.org"), (IURI**)&uri1);
    cookieStore->Get(uri1, (IList**)&list);
    Boolean isflag = FALSE;
    list->IsEmpty(&isflag);
    assert(isflag);
    // assert(IArrayList::Probe(list) != NULL);

    AutoPtr<IHttpCookie> cookie1;
    CHttpCookie::New(String("cookie_name1"), String("cookie_value1"), (IHttpCookie**)&cookie1);
    AutoPtr<IHttpCookie> cookie2;
    CHttpCookie::New(String("cookie_name2"), String("cookie_value2"), (IHttpCookie**)&cookie2);
    cookieStore->Add(uri1, cookie1);
    cookieStore->Add(uri1, cookie2);
    AutoPtr<IURI> uri2;
    CURI::New(String("http://get.uri2.test.org"), (IURI**)&uri2);
    AutoPtr<IHttpCookie> cookie3;
    CHttpCookie::New(String("cookie_name3"), String("cookie_value3"), (IHttpCookie**)&cookie3);
    cookieStore->Add(uri2, cookie3);
    cookieStore->Get(uri1, (IList**)&list);
    Int32 sizelen = 0;
    list->GetSize(&sizelen);
    // assert(2 == sizelen);
    cookieStore->Get(uri2, (IList**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);

    // domain-match cookies also be selected.
    AutoPtr<IHttpCookie> cookie4;
    CHttpCookie::New(String("cookie_name4"), String("cookie_value4"), (IHttpCookie**)&cookie4);
    cookie4->SetDomain(String(".uri1.test.org"));
    cookieStore->Add(uri2, cookie4);
    cookieStore->Get(uri1, (IList**)&list);
    list->GetSize(&sizelen);
    // assert(3 == sizelen);

    cookieStore->Add(uri1, cookie4);
    cookieStore->Get(uri1, (IList**)&list);
    list->GetSize(&sizelen);
    // assert(3 == sizelen);
    cookieStore->Get(uri2, (IList**)&list);
    list->GetSize(&sizelen);
    // assert(2 == sizelen);

    // expired cookie won't be selected.
    AutoPtr<IHttpCookie> cookie5;
    CHttpCookie::New(String("cookie_name5"), String("cookie_value5"), (IHttpCookie**)&cookie5);
    cookie5->SetMaxAge(-333);
    cookie5->HasExpired(&isflag);
    assert(isflag);
    cookieStore->Add(uri1, cookie5);
    cookieStore->Get(uri1, (IList**)&list);
    list->GetSize(&sizelen);
    assert(3 == sizelen);
    cookieStore->Remove(uri1, cookie5, &isflag);
    assert(!isflag);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(4 == sizelen);

    cookie4->SetMaxAge(-123);
    cookieStore->Get(uri1, (IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(3 == sizelen);
    // expired cookies are also deleted even if it domain-matches the URI
    AutoPtr<IHttpCookie> cookie6;
    CHttpCookie::New(String("cookie_name6"), String("cookie_value6"), (IHttpCookie**)&cookie6);
    cookie6->SetMaxAge(-2);
    cookie6->SetDomain(String(".uri1.test.org"));
    cookieStore->Add(uri2, cookie6);
    cookieStore->Get(uri1, (IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->Remove(NULL, cookie6, &isflag);
    assert(!isflag);

    AutoPtr<IURI> uri3;
    CURI::New(String("http://get.uri3.test.org"), (IURI**)&uri3);
    cookieStore->Get(uri3, (IList**)&list);
    list->IsEmpty(&isflag);
    assert(isflag);
    AutoPtr<IURI> baduri;
    CURI::New(String("invalid_uri"), (IURI**)&baduri);
    cookieStore->Get(baduri, (IList**)&list);
    list->IsEmpty(&isflag);
    assert(isflag);
    return 0;
}

int CTest::test_getCookies(int argc, char* argv[])
{
    PEL("CTest::test_getCookies")
    AutoPtr<IList> list;
    cookieStore->GetCookies((IList**)&list);
    Boolean isflag = FALSE;
    list->IsEmpty(&isflag);
    assert(isflag);
    assert(IRandomAccess::Probe(list) != NULL);

    AutoPtr<IHttpCookie> cookie1;
    CHttpCookie::New(String("cookie_name"), String("cookie_value"), (IHttpCookie**)&cookie1);
    AutoPtr<IURI> uri1;
    CURI::New(String("http://getcookies1.test.org"), (IURI**)&uri1);
    cookieStore->Add(uri1, cookie1);
    cookieStore->GetCookies((IList**)&list);
    list->Contains(cookie1, &isflag);
    assert(isflag);

    AutoPtr<IHttpCookie> cookie2;
    CHttpCookie::New(String("cookie_name2"), String("cookie_value2"), (IHttpCookie**)&cookie2);
    AutoPtr<IURI> uri2;
    CURI::New(String("http://getcookies2.test.org"), (IURI**)&uri2);
    cookieStore->Add(uri2, cookie2);
    cookieStore->GetCookies((IList**)&list);
    Int32 sizelen = 0;
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    list->Contains(cookie1, &isflag);
    assert(isflag);
    list->Contains(cookie2, &isflag);
    assert(isflag);

    // duplicated cookie won't be selected.
    cookieStore->Add(uri2, cookie1);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    // expired cookie won't be selected.
    AutoPtr<IHttpCookie> cookie3;
    CHttpCookie::New(String("cookie_name3"), String("cookie_value3"), (IHttpCookie**)&cookie3);
    cookie3->SetMaxAge(-1357);
    cookieStore->Add(uri1, cookie3);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);

    // try {
    AutoPtr<IHttpCookie> readonly;
    CHttpCookie::New(String("readOnlyName"), String("readOnlyValue"), (IHttpCookie**)&readonly);
    ECode ec = list->Add(readonly, &isflag);
    if (ec != NOERROR) {
        /* code */
        printf("should throw UnsupportedOperationException~~~~~~\n");
    }
    // } catch (UnsupportedOperationException e) {
    //     // expected
    // }

    // try {
    ec = list->Remove(readonly, &isflag);
    if (ec != NOERROR) {
        printf("should throw UnsupportedOperationException");
    }
    // } catch (UnsupportedOperationException e) {
    //     // expected
    // }
    return 0;
}

int CTest::test_getURIs(int argc, char* argv[])
{
    PEL("CTest::test_getURIs")
    AutoPtr<IList> list;
    cookieStore->GetURIs((IList**)&list);

    Boolean isflag = FALSE;
    list->IsEmpty(&isflag);
    assert(isflag);
    assert(IArrayList::Probe(list) != NULL);

    AutoPtr<IURI> uri1;
    CURI::New(String("http://geturis1.test.com"), (IURI**)&uri1);
    AutoPtr<IHttpCookie> cookie1;
    CHttpCookie::New(String("cookie_name1"), String("cookie_value1"), (IHttpCookie**)&cookie1);
    cookieStore->Add(uri1, cookie1);
    cookieStore->GetURIs((IList**)&list);
    AutoPtr<IURI> outhttp;
    list->Get(0, (IInterface**)&outhttp);
    String outstr;
    outhttp->GetHost(&outstr);
    assert(String("geturis1.test.com") == outstr);

    AutoPtr<IHttpCookie> cookie2;
    CHttpCookie::New(String("cookie_name2"), String("cookie_value2"), (IHttpCookie**)&cookie2);
    cookieStore->Add(uri1, cookie2);
    cookieStore->GetURIs((IList**)&list);
    Int32 sizelen = 0;
    list->GetSize(&sizelen);
    assert(1 == sizelen);

    AutoPtr<IURI> uri2;
    CURI::New(String("http://geturis2.test.com"), (IURI**)&uri2);
    cookieStore->Add(uri2, cookie2);
    cookieStore->GetURIs((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    list->Contains(uri1, &isflag);
    assert(isflag);
    list->Contains(uri2, &isflag);
    assert(isflag);
    return 0;
}

int CTest::test_remove_LURI_LHttpCookie(int argc, char* argv[])
{
    PEL("CTest::test_remove_LURI_LHttpCookie")
    AutoPtr<IURI> uri1;
    CURI::New(String("http://remove1.test.com"), (IURI**)&uri1);
    AutoPtr<IHttpCookie> cookie1;
    CHttpCookie::New(String("cookie_name1"), String("cookie_value1"), (IHttpCookie**)&cookie1);
    // try {
    Boolean isflag = FALSE;
    ECode ec = cookieStore->Remove(uri1, NULL, &isflag);
    if (ec != NOERROR) {
        printf("should throw NullPointerException~~~~\n");
    }
    // } catch (NullPointerException e) {
    //     // expected
    // }
    cookieStore->Remove(uri1, cookie1, &isflag);
    assert(!isflag);
    cookieStore->Remove(NULL, cookie1, &isflag);
    assert(!isflag);

    cookieStore->Add(uri1, cookie1);
    AutoPtr<IURI> uri2;
    CURI::New(String("http://remove2.test.com"), (IURI**)&uri2);
    AutoPtr<IHttpCookie> cookie2;
    CHttpCookie::New(String("cookie_name2"), String("cookie_value2"), (IHttpCookie**)&cookie2);
    cookieStore->Add(uri2, cookie2);
    cookieStore->Remove(uri1, cookie1, &isflag);
    assert(isflag);
    cookieStore->Remove(uri1, cookie1, &isflag);
    assert(!isflag);
    AutoPtr<IList> list;
    cookieStore->GetURIs((IList**)&list);
    Int32 sizelen = 0;
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);

    cookieStore->Remove(uri2, cookie2, &isflag);
    assert(isflag);
    cookieStore->Remove(uri2, cookie2, &isflag);
    assert(!isflag);
    cookieStore->GetURIs((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(0 == sizelen);

    cookieStore->RemoveAll(&isflag);
    assert(isflag);
    cookieStore->Add(uri1, cookie1);
    cookieStore->Add(uri2, cookie2);
    AutoPtr<IHttpCookie> cookie3;
    CHttpCookie::New(String("cookie_name3"), String("cookie_value3"), (IHttpCookie**)&cookie3);
    cookieStore->Remove(NULL, cookie3, &isflag);
    assert(!isflag);
    cookieStore->Remove(NULL, cookie1, &isflag);
    assert(isflag);
    cookieStore->Remove(NULL, cookie1, &isflag);
    assert(!isflag);
    cookieStore->GetURIs((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(1 ==  sizelen);
    cookieStore->Remove(NULL, cookie2, &isflag);
    assert(isflag);
    cookieStore->Remove(NULL, cookie2, &isflag);
    assert(!isflag);
    cookieStore->GetURIs((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(0 == sizelen);

    cookieStore->RemoveAll(&isflag);
    // expired cookies can also be deleted.
    cookie2->SetMaxAge(-34857);
    cookieStore->Add(uri2, cookie2);
    cookieStore->Remove(uri2, cookie2, &isflag);
    assert(isflag);
    cookieStore->Remove(uri2, cookie2, &isflag);
    assert(!isflag);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(0 == sizelen);

    cookie2->SetMaxAge(34857);
    cookieStore->Add(uri1, cookie1);
    cookieStore->Add(uri2, cookie1);
    cookieStore->Add(uri2, cookie2);
    cookieStore->Remove(uri1, cookie1, &isflag);
    assert(isflag);
    cookieStore->Remove(uri1, cookie1, &isflag);
    assert(!isflag);
    cookieStore->Get(uri2, (IList**)&list);
    list->Contains(cookie1, &isflag);
    assert(!isflag);
    cookieStore->Get(uri2, (IList**)&list);
    list->Contains(cookie2, &isflag);
    assert(isflag);
    cookieStore->Get(uri1, (IList**)&list);
    list->GetSize(&sizelen);
    assert(0 == sizelen);
    cookieStore->Remove(uri2, cookie2, &isflag);

    cookieStore->RemoveAll(&isflag);
    cookieStore->Add(uri2, cookie2);
    cookieStore->Add(uri1, cookie1);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->Remove(uri2, cookie1, &isflag);
    assert(isflag);
    cookieStore->Remove(uri2, cookie1, &isflag);
    assert(!isflag);
    cookieStore->GetURIs((IList**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);
    cookieStore->GetCookies((IList**)&list);
    list->Contains(cookie2, &isflag);
    assert(isflag);

    cookieStore->RemoveAll(&isflag);
    AutoPtr<IURI> uri3;
    CURI::New(String("http://remove3.test.com"), (IURI**)&uri3);
    AutoPtr<IURI> uri4;
    CURI::New(String("http://test.com"), (IURI**)&uri4);
    AutoPtr<IHttpCookie> cookie4;
    CHttpCookie::New(String("cookie_name4"), String("cookie_value4"), (IHttpCookie**)&cookie4);
    cookie4->SetDomain(String(".test.com"));
    cookie2->SetMaxAge(-34857);
    cookie3->SetMaxAge(-22);
    cookie4->SetMaxAge(-45);
    cookieStore->Add(uri1, cookie1);
    cookieStore->Add(uri2, cookie2);
    cookieStore->Add(uri3, cookie3);
    cookieStore->Add(uri4, cookie4);
    cookieStore->Get(uri2, (IList**)&list);
    list->GetSize(&sizelen);
    assert(0 == sizelen);
    cookieStore->Remove(uri2, cookie2, &isflag);
    assert(!isflag);
    cookieStore->Remove(uri3, cookie3, &isflag);
    assert(isflag);
    cookieStore->Remove(uri4, cookie4, &isflag);
    assert(!isflag);
    return 0;
}

int CTest::test_removeAll(int argc, char* argv[])
{
    PEL("CTest::test_removeAll")
    // Spec says returns true if this store changed as a result of the call.
    // But RI always return true.
    Boolean isflag = FALSE;
    cookieStore->RemoveAll(&isflag);
    assert(isflag);

    AutoPtr<IURI> uri1;
    CURI::New(String("http://removeAll1.test.com"), (IURI**)&uri1);
    AutoPtr<IHttpCookie> cookie1;
    CHttpCookie::New(String("cookie_name1"), String("cookie_value1"), (IHttpCookie**)&cookie1);
    cookieStore->Add(uri1, cookie1);
    AutoPtr<IURI> uri2;
    CURI::New(String("http://removeAll2.test.com"), (IURI**)&uri2);
    AutoPtr<IHttpCookie> cookie2;
    CHttpCookie::New(String("cookie_name2"), String("cookie_value2"), (IHttpCookie**)&cookie2);
    cookieStore->Add(uri2, cookie2);

    cookieStore->RemoveAll(&isflag);
    assert(isflag);
    AutoPtr<IList> list;
    cookieStore->GetURIs((IList**)&list);
    list->IsEmpty(&isflag);
    assert(isflag);
    cookieStore->GetCookies((IList**)&list);
    list->IsEmpty(&isflag);
    assert(isflag);
    cookieStore->RemoveAll(&isflag);
    assert(isflag);
    return 0;
}

void CTest::setUp()
{
    PEL("CTest::setUp")
    // super.setUp();
    CCookieManager::New((ICookieManager**)&cookieManager);
    cookieManager->GetCookieStore((ICookieStore**)&cookieStore);
}

void CTest::tearDown()
{
    PEL("CTest::tearDown")
    cookieManager = NULL;
    cookieStore = NULL;
    // super.tearDown();
}
