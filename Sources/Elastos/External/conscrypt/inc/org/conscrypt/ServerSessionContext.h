
#ifndef __ORG_CONSCRYPT_SERVERSESSIONCONTEXT_H__
#define __ORG_CONSCRYPT_SERVERSESSIONCONTEXT_H__

#include "_Org.Conscrypt.h"
#include "AbstractSessionContext.h"

using Elastosx::Net::Ssl::ISSLSession;

namespace Org {
namespace Conscrypt {

/**
 * Caches server sessions. Indexes by session ID. Users typically look up
 * sessions using the ID provided by an SSL client.
 */
class ServerSessionContext
    : public AbstractSessionContext
    , public IServerSessionContext
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI SetPersistentCache(
        /* [in] */ ISSLServerSessionCache* persistentCache);

    CARAPI SessionRemoved(
        /* [in] */ ISSLSession* session);

    CARAPI GetSession(
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [out] */ ISSLSession** result);

    CARAPI PutSession(
        /* [in] */ ISSLSession* session);

private:
    AutoPtr<ISSLServerSessionCache> mPersistentCache;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_SERVERSESSIONCONTEXT_H__