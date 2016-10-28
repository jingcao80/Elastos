
#include "_Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include "_Org.Conscrypt.h"
#include "elastos/droid/net/SSLSessionCache.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::ISSLSessionCache;
using Elastos::Droid::Utility::ILog;

using Elastos::IO::IFile;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::ISSLContext;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionContext;

using Org::Conscrypt::IClientSessionContext;
using Org::Conscrypt::IFileClientSessionCache;
using Org::Conscrypt::ISSLClientSessionCache;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(SSLSessionCache, Object, ISSLSessionCache)

const String SSLSessionCache::TAG("SSLSessionCache");

ECode SSLSessionCache::Install(
    /* [in] */ ISSLSessionCache* cache,
    /* [in] */ ISSLContext* context)
{
    AutoPtr<ISSLSessionContext> clientContext;
    context->GetClientSessionContext((ISSLSessionContext**)&clientContext);
    if (IClientSessionContext::Probe(clientContext) != NULL) {
        // TODO: Waiting for IClientSessionContext
        assert(0);
        // IClientSessionContext::Probe(clientContext)->SetPersistentCache(
        //         cache == NULL ? NULL : ((SSLSessionCache*)cache)->mSessionCache);
    } else {
        String s;
        IObject::Probe(context)->ToString(&s);
        Logger::E("Incompatible SSLContext: %s", s.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SSLSessionCache::constructor(
    /* [in] */ IInterface* cache)
{
    mSessionCache = ISSLClientSessionCache::Probe(cache);
    return NOERROR;
}

ECode SSLSessionCache::constructor(
    /* [in] */ IFile* dir)
{
    // TODO: Waiting for FileClientSessionCache
    assert(0);
    return NOERROR;
    // AutoPtr<IFileClientSessionCacheHelper> helper;
    // CFileClientSessionCacheHelper::AcquireSingleton((IFileClientSessionCacheHelper**)&helper);
    // return helper->UsingDirectory(dir, (ISSLClientSessionCache**)&mSessionCache);
}

ECode SSLSessionCache::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IFile> dir;
    context->GetDir(String("sslcache"), IContext::MODE_PRIVATE, (IFile**)&dir);
    AutoPtr<ISSLClientSessionCache> cache = NULL;
    // TODO: Waiting for FileClientSessionCache
    assert(0);
    // // try {
    // ECode ec = CFileClientSessionCache::UsingDirectory(dir, (ISSLClientSessionCache**)&cache);
    // // } catch (IOException e) {
    // if (FAILED(ec)) {
    //     if (ec == (ECode)E_IO_EXCEPTION) {
    //         String s;
    //         IObject::Probe(dir)->ToString(&s);
    //         Logger::W(TAG, "Unable to create SSL session cache in %s, %d", s.string(), ec);
    //     }
    //     else return ec;
    // }
    // }
    mSessionCache = cache;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
