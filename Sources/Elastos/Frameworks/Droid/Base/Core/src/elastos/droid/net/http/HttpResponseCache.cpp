
#include "elastos/droid/net/http/HttpResponseCache.h"
#include "elastos/droid/net/http/CHttpResponseCache.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/Headers.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/ReturnOutValue.h"

using Elastos::Droid::Content::IContext;

using Elastos::IO::EIID_ICloseable;
using Elastos::IO::ICloseable;
using Elastos::Net::CResponseCacheHelper;
using Elastos::Net::IHttpURLConnection;
using Elastos::Net::IResponseCache;
using Elastos::Net::IResponseCacheHelper;
using Elastos::Net::IURI;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastosx::Net::Ssl::IHttpsURLConnection;

using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Impl::Client::IDefaultHttpClient;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL_2(HttpResponseCache, ResponseCache, IHttpResponseCache, ICloseable)

ECode HttpResponseCache::constructor(
    /* [in] */ Com::Squareup::Okhttp::IHttpResponseCache* delegate)
{
    mDelegate = delegate;
    return NOERROR;
}

ECode HttpResponseCache::GetInstalled(
    /* [out] */ IHttpResponseCache** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IResponseCache> installed;
    ResponseCache::GetDefault((IResponseCache**)&installed);
    if (Com::Squareup::Okhttp::IHttpResponseCache::Probe(installed) != NULL) {
        AutoPtr<IHttpResponseCache> rev = new CHttpResponseCache();
        ((HttpResponseCache*)rev.Get())->constructor(Com::Squareup::Okhttp::IHttpResponseCache::Probe(installed));
        FUNC_RETURN(rev)
    }
    return NOERROR;
}

ECode HttpResponseCache::Install(
    /* [in] */ IFile* directory,
    /* [in] */ Int64 maxSize,
    /* [out] */ IHttpResponseCache** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IResponseCacheHelper> helper;
    CResponseCacheHelper::AcquireSingleton((IResponseCacheHelper**)&helper);
    AutoPtr<IResponseCache> installed;
    helper->GetDefault((IResponseCache**)&installed);
    if (Com::Squareup::Okhttp::IHttpResponseCache::Probe(installed) != NULL) {
        AutoPtr<Com::Squareup::Okhttp::IHttpResponseCache> installedCache =
                Com::Squareup::Okhttp::IHttpResponseCache::Probe(installed);
        // don't close and reopen if an equivalent cache is already installed
        Boolean isEquals;
        // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
        assert(0);
        // Ptr(installedCache)->Func(installedCache->GetDirectory)->Equals(directory, &isEquals);
        // if (isEquals
        //         && Ptr(installedCache)->Func(installedCache->GetMaxSize) == maxSize
        //         && !Ptr(installedCache)->Func(installedCache->IsClosed)) {
        //     return CHttpResponseCache::New(installedCache, result);
        // } else {
        //     // The HttpResponseCache that owns this object is about to be replaced.
        //     installedCache->Close();
        // }
    }
    AutoPtr<Com::Squareup::Okhttp::IHttpResponseCache> responseCache;
    // TODO: Waiting for Com::Squareup::Okhttp::CHttpResponseCache
    assert(0);
    // Com::Squareup::Okhttp::CHttpResponseCache::New(directory, maxSize, (Com::Squareup::Okhttp::IHttpResponseCache**)&responseCache);
    helper->SetDefault(IResponseCache::Probe(responseCache));
    AutoPtr<IHttpResponseCache> rev = new CHttpResponseCache();
    ((HttpResponseCache*)rev.Get())->constructor(responseCache);
    FUNC_RETURN(rev)
}

ECode HttpResponseCache::Get(
    /* [in] */ IURI* uri,
    /* [in] */ const String& requestMethod,
    /* [in] */ IMap* requestHeaders,
    /* [out] */ ICacheResponse** result)
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    // return mDelegate->Get(uri, requestMethod, requestHeaders, result);
    return NOERROR;
}

ECode HttpResponseCache::Put(
    /* [in] */ IURI* uri,
    /* [in] */ IURLConnection* urlConnection,
    /* [out] */ ICacheRequest** result)
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    // return mDelegate->Gut(uri, urlConnection, result);
    return NOERROR;
}

ECode HttpResponseCache::Size(
    /* [out] */ Int64* result)
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    // return mDelegate->GetSize(result);
    return NOERROR;
}

ECode HttpResponseCache::MaxSize(
    /* [out] */ Int64* result)
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    // return mDelegate->GetMaxSize(result);
    return NOERROR;
}

ECode HttpResponseCache::Flush()
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    //     // try {
    // ECode ec = mDelegate->Flush();
    //     // } catch (IOException ignored) {
    // if (FAILED(ec)) {
    //     if (ec != E_IO_EXCEPTION) return ec;
    // }
    //     // }
    return NOERROR;
}

ECode HttpResponseCache::GetNetworkCount(
    /* [out] */ Int32* result)
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    return NOERROR;
    // return mDelegate->GetNetworkCount(result);
}

ECode HttpResponseCache::GetHitCount(
    /* [out] */ Int32* result)
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    return NOERROR;
    // return mDelegate->GetHitCount(result);
}

ECode HttpResponseCache::GetRequestCount(
    /* [out] */ Int32* result)
{
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    return NOERROR;
    // return mDelegate->GetRequestCount(result);
}

ECode HttpResponseCache::Close()
{
    AutoPtr<IResponseCacheHelper> helper;
    CResponseCacheHelper::AcquireSingleton((IResponseCacheHelper**)&helper);
    Boolean isEquals;
    IObject::Probe(mDelegate)->Equals(Ptr(helper)->Func(helper->GetDefault), &isEquals);
    if (isEquals) {
        helper->SetDefault(NULL);
    }
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    // mDelegate->Close();
    return NOERROR;
}

ECode HttpResponseCache::Delete()
{
    AutoPtr<IResponseCacheHelper> helper;
    CResponseCacheHelper::AcquireSingleton((IResponseCacheHelper**)&helper);
    Boolean isEquals;
    IObject::Probe(mDelegate)->Equals(Ptr(helper)->Func(helper->GetDefault), &isEquals);
    if (isEquals) {
        helper->SetDefault(NULL);
    }
    // TODO: Waiting for Com::Squareup::Okhttp::IHttpResponseCache
    assert(0);
    // mDelegate->Delete();
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
