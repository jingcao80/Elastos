//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/net/http/HttpResponseCache.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/Headers.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/net/Network.h"

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
        AutoPtr<HttpResponseCache> cache = new HttpResponseCache();
        cache->constructor(Com::Squareup::Okhttp::IHttpResponseCache::Probe(installed));
        *result = cache;
        REFCOUNT_ADD(*result);
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
    AutoPtr<HttpResponseCache> rev = new HttpResponseCache();
    rev->constructor(responseCache);
    *result = rev;
    REFCOUNT_ADD(*result);
    return NOERROR;
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
    //     if (ec != (ECode)E_IO_EXCEPTION) return ec;
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
    AutoPtr<IResponseCache> defaultCache;
    helper->GetDefault((IResponseCache**)&defaultCache);
    if (Object::Equals(mDelegate, defaultCache)) {
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
    AutoPtr<IResponseCache> defaultCache;
    helper->GetDefault((IResponseCache**)&defaultCache);
    if (Object::Equals(mDelegate, defaultCache)) {
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
