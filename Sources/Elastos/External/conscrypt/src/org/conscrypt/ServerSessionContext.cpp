
#include "Elastos.CoreLibrary.Extensions.h"
#include "ServerSessionContext.h"
#include "NativeCrypto.h"

namespace Org {
namespace Conscrypt {

//=========================================
// ServerSessionContext::
//=========================================
CAR_INTERFACE_IMPL(ServerSessionContext, AbstractSessionContext, IServerSessionContext)

ECode ServerSessionContext::constructor()
{
    AbstractSessionContext::constructor(100, 0);
    // TODO make sure SSL_CTX does not automaticaly clear sessions we want it to cache
    // SSL_CTX_set_session_cache_mode(sslCtxNativePointer, SSL_SESS_CACHE_NO_AUTO_CLEAR);

    // TODO remove SSL_CTX session cache limit so we can manage it
    // SSL_CTX_sess_set_cache_size(sslCtxNativePointer, 0);

    // TODO override trimToSize and removeEldestEntry to use
    // SSL_CTX_sessions to remove from native cache

    // Set a trivial session id context. OpenSSL uses this to make
    // sure you don't reuse sessions externalized with i2d_SSL_SESSION
    // between apps. However our sessions are either in memory or
    // exported to a app's SSLServerSessionCache.
    AutoPtr<ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(1);
    (*arr)[0] = ' ';
    NativeCrypto::SSL_CTX_set_session_id_context(mSslCtxNativePointer, arr);
    return NOERROR;
}

ECode ServerSessionContext::SetPersistentCache(
    /* [in] */ ISSLServerSessionCache* persistentCache)
{
    mPersistentCache = persistentCache;
    return NOERROR;
}

ECode ServerSessionContext::SessionRemoved(
    /* [in] */ ISSLSession* session)
{
    return NOERROR;
}

ECode ServerSessionContext::GetSession(
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [out] */ ISSLSession** result)
{
    AutoPtr<ISSLSession> session;
    AbstractSessionContext::GetSession(sessionId, (ISSLSession**)&session);
    if (session != NULL) {
        *result = session;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // Check persistent cache.
    if (mPersistentCache != NULL) {
        AutoPtr<ArrayOf<Byte> > data;
        mPersistentCache->GetSessionData(sessionId, (ArrayOf<Byte>**)&data);
        if (data != NULL) {
            ToSession(data, String(NULL), -1, (ISSLSession**)&session);
            Boolean valid = FALSE;
            if (session != NULL && (session->IsValid(&valid), valid)) {
                AbstractSessionContext::PutSession(session);
                *result = session;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode ServerSessionContext::PutSession(
    /* [in] */ ISSLSession* session)
{
    AbstractSessionContext::PutSession(session);

    // TODO: In background thread.
    if (mPersistentCache != NULL) {
        AutoPtr<ArrayOf<Byte> > data;
        ToBytes(session, (ArrayOf<Byte>**)&data);
        if (data != NULL) {
            mPersistentCache->PutSessionData(session, data);
        }
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
