#ifndef __COOKIEMANAGERTEST_H__
#define __COOKIEMANAGERTEST_H__

#include "Elastos.CoreLibrary.h"
#include "cmdef.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest
{
private:
    class DummyStore
        : public ElRefBase
        , public ICookieStore
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Add(
            /* [in] */ IURI* uri,
            /* [in] */ IHttpCookie* cookie);

        CARAPI Get(
            /* [in] */ IURI* uri,
            /* [out] */ IObjectContainer** httpCookies);

        CARAPI GetCookies(
            /* [out] */ IObjectContainer** httpCookies);

        CARAPI GetURIs(
            /* [out] */ IObjectContainer** URIs);

        CARAPI Remove(
            /* [in] */ IURI* uri,
            /* [in] */ IHttpCookie* cookie,
            /* [out] */ Boolean* succeeded);

        CARAPI RemoveAll(
            /* [out] */ Boolean* succeeded);

        CARAPI GetName(
            /* [out] */ String* str);
    };

public:
    CTest();
    ~CTest();

public:
    int test_Put_Get_LURI_LMap_exception(int argc, char* argv[]);
    int test_Put_Get_LURI_LMap(int argc, char* argv[]);
    int test_CookieManager(int argc, char* argv[]);
    int testCookieManager_LCookieStore_LCookiePolicy(int argc, char* argv[]);
    int test_SetCookiePolicy_LCookiePolicy(int argc, char* argv[]);
    int test_GetCookieStore(int argc, char* argv[]);

private:
    void checkValidParams4Get(IURI* uri, IMap* map);
    void checkValidParams4Put(IURI* uri, IMap* map);
    static AutoPtr<IMap> addCookie(String** cookies, int length);
    static AutoPtr<ICookieManager> store(String** cookies, int length, IMap* responseHeaders, ICookiePolicy* policy);

private:
    AutoPtr<IURI> mIuri;
    AutoPtr<IMap> mMap;
};

#endif // __COOKIEMANAGERTEST_H__