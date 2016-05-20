    
#include "test.h"

CTest::CTest()
{
    PEL("CTest::CTest")
}

CTest::~CTest()
{
}

int CTest::test_Put_Get_LURI_LMap_exception(int argc, char* argv[])
{
    PEL("CTest::test_Put_Get_LURI_LMap_exception")
    // get
    CURI::New(String(""), (IURI**)&mIuri);
    checkValidParams4Get(mIuri, NULL);
    CURI::New(String("http://www.test.com"), (IURI**)&mIuri);
    checkValidParams4Get(mIuri, NULL);
    checkValidParams4Get(NULL, NULL);
    CHashMap::New((IMap**)&mMap);
    checkValidParams4Get(NULL, mMap);

    // put
    CURI::New(String(""), (IURI**)&mIuri);
    checkValidParams4Put(mIuri, NULL);
    CURI::New(String("http://www.test.com"), (IURI**)&mIuri);
    checkValidParams4Put(mIuri, NULL);
    checkValidParams4Put(NULL, NULL);
    checkValidParams4Put(NULL, mMap);
    return 0;
}

int CTest::test_Put_Get_LURI_LMap(int argc, char* argv[])
{
    PEL("CTest::test_Put_Get_LURI_LMap")
    // cookie-key | (content, URI)...
    String cookies[6][5] = {
            { String("Set-cookie"),
              String("Set-cookie:PREF=test;path=/;domain=.b.c;"),
              String("http://a.b.c/"),
              String("Set-cookie:PREF1=test2;path=/;domain=.beg.com;"),
              String("http://a.b.c/")},

            { String("Set-cookie2"),
              String("Set-cookie2:NAME1=VALUE1;path=/te;domain=.b.c;"),
              String("http://a.b.c/test")},

            { String("Set-cookie"),
              String("Set-cookie2:NAME=VALUE;path=/;domain=.beg.com;"),
              String("http://a.beg.com/test"),
              String("Set-cookie2:NAME1=VALUE1;path=/;domain=.beg.com;"),
              String("http://a.beg.com/test")},

            { String("Set-cookie2"),
              String("Set-cookie3:NAME=VALUE;path=/;domain=.test.org;"),
              String("http://a.test.org/test")},

            { String(NULL),
              String("Set-cookie3:NAME=VALUE;path=/te;domain=.test.org;"),
              String("http://a.test.org/test")},

            { String("Set-cookie2"),
              String("lala"),
              String("http://a.test.org/test")}
    };

    // requires path of cookie is the prefix of uri
    // domain of cookie must match that of uri
    String incooke[2][5] = {cookies[0], cookies[1] };
    AutoPtr<IMap> responseHeaders = addCookie((String**)incooke, 2);
    AutoPtr<ICookieManager> manager = store((String**)incooke, 2, responseHeaders, NULL);

    AutoPtr<IMap> dummyMap;
    CHashMap::New((IMap**)&dummyMap);
    AutoPtr<IMap> map;
    CURI::New(String("http://a.b.c/"), (IURI**)&mIuri);
    manager->Get(mIuri, dummyMap, (IMap**)&map);

    Int32 sizelen = 0;
    map->GetSize(&sizelen);
    assert(1 == sizelen);
    AutoPtr<IList> list;
    AutoPtr<ICharSequence> sq;
    CString::New(String("Cookie"), (ICharSequence**)&sq);
    map->Get(sq, (IInterface**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);

    // requires path of cookie is the prefix of uri
    CURI::New(String("http://a.b.c/te"), (IURI**)&mIuri);
    manager->Get(mIuri, dummyMap, (IMap**)&map);
    map->Get(sq, (IInterface**)&list);
    list->GetSize(&sizelen);
    assert(2 == sizelen);

    // If all cookies are of version 1, then $version=1 will be added
    // ,no matter the value cookie-key
    String incooke1[1][5] = {cookies[2]};
    responseHeaders = addCookie((String**)incooke1, 1);
    manager = store((String**)incooke1, 1, responseHeaders, NULL);
    CURI::New(String("http://a.beg.com/test"), (IURI**)&mIuri);
    manager->Get(mIuri, dummyMap, (IMap**)&map);
    map->Get(sq, (IInterface**)&list);
    AutoPtr<ICharSequence> outsq;
    list->Get(0, (IInterface**)&outsq);
    String outstr;
    outsq->ToString(&outstr);
    assert(String("$Version=\"1\"") == outstr);
    list->GetSize(&sizelen);
    assert(3 == sizelen);

    // cookie-key will not have effect on determining cookie version
    String incooke2[1][5] = {cookies[3]};
    responseHeaders = addCookie((String**)incooke2, 1);
    manager = store((String**)incooke2, 1, responseHeaders, NULL);
    CURI::New(String("http://a.test.org/"), (IURI**)&mIuri);
    manager->Get(mIuri, responseHeaders, (IMap**)&map);
    map->Get(sq, (IInterface**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);
    list->Get(0, (IInterface**)&outsq);
    outsq->ToString(&outstr);
    assert(String("Set-cookie3:NAME=VALUE") == outstr);

    // When key is null, no cookie can be stored/retrieved, even if policy =
    // ACCEPT_ALL
    String incooke3[1][5] = {cookies[4]};
    responseHeaders = addCookie((String**)incooke3, 1);
    AutoPtr<ICookiePolicyHelper> cphelp;
    CCookiePolicyHelper::AcquireSingleton((ICookiePolicyHelper**)&cphelp);
    AutoPtr<ICookiePolicy> outcookie;
    cphelp->GetCookiePolicy(CookiePolicyKind_ACCEPT_ALL, (ICookiePolicy**)&outcookie);
    manager = store((String**)incooke3, 1, responseHeaders, outcookie);
    manager->Get(mIuri, responseHeaders, (IMap**)&map);
    map->Get(sq, (IInterface**)&list);
    list->GetSize(&sizelen);
    assert(1 == sizelen);

    // All cookies will be rejected if policy == ACCEPT_NONE
    cphelp->GetCookiePolicy(CookiePolicyKind_ACCEPT_NONE, (ICookiePolicy**)&outcookie);
    responseHeaders = addCookie((String**)incooke2, 1);
    manager = store((String**)incooke2, 1, responseHeaders, outcookie);
    manager->Get(mIuri, responseHeaders, (IMap**)&map);
    map->Get(sq, (IInterface**)&list);
    list->GetSize(&sizelen);
    assert(0 == sizelen);

    String incooke4[1][5] = {cookies[5]};
    responseHeaders = addCookie((String**)incooke4, 1);
    cphelp->GetCookiePolicy(CookiePolicyKind_ACCEPT_ALL, (ICookiePolicy**)&outcookie);
    manager = store((String**)incooke4, 1,  responseHeaders, outcookie);
    map->Get(sq, (IInterface**)&list);
    list->GetSize(&sizelen);
    assert(0 == sizelen);

    // try {
    AutoPtr<IInterface> outface;
    ECode ec = map->Put(NULL, NULL, (IInterface**)&outface);
    if (ec != NOERROR) {
        printf("Should throw UnsupportedOperationException~~~~\n");
    }
    // } catch (UnsupportedOperationException e) {
    //     // expected
    // }
    return 0;
}

int CTest::test_CookieManager(int argc, char* argv[])
{
    PEL("CTest::test_CookieManager")
    AutoPtr<ICookieManager> cookieManager;
    CCookieManager::New((ICookieManager**)&cookieManager);
    assert(cookieManager != NULL);
    AutoPtr<ICookieStore> outcookie;
    cookieManager->GetCookieStore((ICookieStore**)&outcookie);
    assert(outcookie != NULL);
    return 0;
}

int CTest::testCookieManager_LCookieStore_LCookiePolicy(int argc, char* argv[])
{
    PEL("CTest::testCookieManager_LCookieStore_LCookiePolicy")
    AutoPtr<ICookieStore> store = new DummyStore();
    AutoPtr<ICookieManager> cookieManager;
    AutoPtr<ICookiePolicyHelper> cphelp;
    CCookiePolicyHelper::AcquireSingleton((ICookiePolicyHelper**)&cphelp);
    AutoPtr<ICookiePolicy> acceptcp;
    cphelp->GetCookiePolicy(CookiePolicyKind_ACCEPT_ALL, (ICookiePolicy**)&acceptcp);
    CCookieManager::New(store, acceptcp, (ICookieManager**)&cookieManager);
    AutoPtr<ICookieStore> out2;
    cookieManager->GetCookieStore((ICookieStore**)&out2);
    String outstr;
    ((DummyStore*)out2.Get())->GetName(&outstr);
    assert(Object::Equals(store, out2));
    return 0;
}

int CTest::test_SetCookiePolicy_LCookiePolicy(int argc, char* argv[])
{
    PEL("CTest::test_SetCookiePolicy_LCookiePolicy")
    // Policy = ACCEPT_NONE
    AutoPtr<ICookieManager> manager;
    CCookieManager::New((ICookieManager**)&manager);
    AutoPtr<ICookiePolicyHelper> cphelp;
    CCookiePolicyHelper::AcquireSingleton((ICookiePolicyHelper**)&cphelp);
    AutoPtr<ICookiePolicy> acceptcp;
    cphelp->GetCookiePolicy(CookiePolicyKind_ACCEPT_NONE, (ICookiePolicy**)&acceptcp);
    manager->SetCookiePolicy(acceptcp);
    AutoPtr<IMap> responseHeaders;
    CTreeMap::New((INavigableMap**)&responseHeaders);
    AutoPtr<IURI> uri;
    CURI::New(String("http://a.b.c"), (IURI**)&uri);
    manager->Put(uri, responseHeaders);
    AutoPtr<IMap> map;
    AutoPtr<IMap> outhm;
    CHashMap::New((IMap**)&outhm);
    manager->Get(uri, outhm, (IMap**)&map);

    Int32 maplen = 0;
    map->GetSize(&maplen);
    assert(1 == maplen);
    AutoPtr<IInterface> outface;
    AutoPtr<ICharSequence> Cookie;
    CString::New(String("Cookie"), (ICharSequence**)&Cookie);
    map->Get(Cookie, (IInterface**)&outface);
    // assertTrue(map.get("Cookie").isEmpty());
    AutoPtr< ArrayOf<IInterface*> > outarr;
    AutoPtr<ISet> outset;
    map->KeySet((ISet**)&outset);
    outset->ToArray((ArrayOf<IInterface*>**)&outarr);
    AutoPtr<IInterface> key = (*outarr)[0];
    assert(key != NULL);
    String outstr;
    ICharSequence::Probe(key)->ToString(&outstr);
    assert(String("Cookie") == outstr);

    // Policy = ACCEPT_ALL
    cphelp->GetCookiePolicy(CookiePolicyKind_ACCEPT_ALL, (ICookiePolicy**)&acceptcp);
    manager->SetCookiePolicy(acceptcp);
    CTreeMap::New((INavigableMap**)&responseHeaders);
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<ICharSequence> test;
    CString::New(String("test=null"), (ICharSequence**)&test);
    Boolean isflag = FALSE;
    list->Add(test, &isflag);
    AutoPtr<ICharSequence> setcookie;
    CString::New(String("Set-cookie"), (ICharSequence**)&setcookie);
    responseHeaders->Put(setcookie, list, (IInterface**)&outface);
    AutoPtr<IURI> newuri;
    CURI::New(String("http://b.c.d"), (IURI**)&newuri);
    manager->Put(newuri, responseHeaders);
    AutoPtr<IMap> newhash;
    CHashMap::New((IMap**)&newhash);
    manager->Get(uri, newhash, (IMap**)&map);
    map->GetSize(&maplen);
    assert(1 == maplen);
    return 0;
}

int CTest::test_GetCookieStore(int argc, char* argv[])
{
    PEL("CTest::test_GetCookieStore")
    AutoPtr<ICookieManager> cookieManager;
    CCookieManager::New((ICookieManager**)&cookieManager);
    AutoPtr<ICookieStore> store;
    cookieManager->GetCookieStore((ICookieStore**)&store);
    assert(store != NULL);
    return 0;
}

void CTest::checkValidParams4Get(IURI* uri, IMap* map)
{
    PEL("CTest::checkValidParams4Get")
    AutoPtr<ICookieManager> manager;
    CCookieManager::New((ICookieManager**)&manager);
    // try {
    AutoPtr<IMap> outmap;
    ECode ec = manager->Get(uri, map, (IMap**)&outmap);
    if (ec != NOERROR) {
        printf("Should throw IllegalArgumentException~~~~\n");
    }
    // } catch (IllegalArgumentException e) {
    //     // expected
    // }
}

void CTest::checkValidParams4Put(IURI* uri, IMap* map)
{
    PEL("CTest::checkValidParams4Put")
    AutoPtr<ICookieManager> manager;
    CCookieManager::New((ICookieManager**)&manager);
    // try {
    ECode ec = manager->Put(uri, map);
    if (ec != NOERROR) {
        printf("Should throw IllegalArgumentException~~~~\n");
    }
    // } catch (IllegalArgumentException e) {
    //     // expected
    // }
}

AutoPtr<IMap> CTest::addCookie(String** cookies, int length)
{
    PEL("CTest::addCookie")
    AutoPtr<IMap> responseHeaders;
    CTreeMap::New((INavigableMap**)&responseHeaders);
    for (int i = 0; i < length; i++) {
        AutoPtr<IList> fields;
        CArrayList::New((IArrayList**)&fields);
        for (int j = 1; j < 5; j += 2) {
            AutoPtr<ICharSequence> sq;
            CString::New(cookies[i][j], (ICharSequence**)&sq);
            Boolean isflag = FALSE;
            fields->Add(sq, &isflag);
        }
        AutoPtr<IInterface> outface;
        AutoPtr<ICharSequence> sq;
        CString::New(cookies[i][0], (ICharSequence**)&sq);
        responseHeaders->Put(sq, fields, (IInterface**)&outface);
    }
    return responseHeaders;
}

AutoPtr<ICookieManager> CTest::store(String** cookies, int length, IMap* responseHeaders, ICookiePolicy* policy)
{
    PEL("CTest::store")
    AutoPtr<ICookieManager> manager;
    CCookieManager::New(NULL, policy, (ICookieManager**)&manager);
    // Put all cookies into manager
    for (int i = 0; i < length; i++) {
        for (int j = 2; j < 5; j += 2) {
            AutoPtr<IURI> uri;
            CURI::New(cookies[i][j], (IURI**)&uri);
            manager->Put(uri, responseHeaders);
        }
    }
    return manager;
}

CAR_INTERFACE_IMPL(CTest::DummyStore, ICookieStore)

ECode CTest::DummyStore::Add(
    /* [in] */ IURI* uri,
    /* [in] */ IHttpCookie* cookie)
{
    // expected
    return NOERROR;
}

ECode CTest::DummyStore::Get(
    /* [in] */ IURI* uri,
    /* [out] */ IObjectContainer** httpCookies)
{
    // return null;
    return NOERROR;
}

ECode CTest::DummyStore::GetCookies(
    /* [out] */ IObjectContainer** httpCookies)
{
    // return null;
    return NOERROR;
}

ECode CTest::DummyStore::GetURIs(
    /* [out] */ IObjectContainer** URIs)
{
    // return null;
    return NOERROR;
}

ECode CTest::DummyStore::Remove(
    /* [in] */ IURI* uri,
    /* [in] */ IHttpCookie* cookie,
    /* [out] */ Boolean* succeeded)
{
    // return false;
    return NOERROR;
}

ECode CTest::DummyStore::RemoveAll(
    /* [out] */ Boolean* succeeded)
{
    // return false;
    return NOERROR;
}

ECode CTest::DummyStore::GetName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = "A dummy store";
    return NOERROR;
}
