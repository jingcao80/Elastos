
#ifndef __ORG_CONSCRYPT_CCLIENTSESSIONCONTEXT_H__
#define __ORG_CONSCRYPT_CCLIENTSESSIONCONTEXT_H__

#include "_Org_Conscrypt_CClientSessionContext.h"
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "org/conscrypt/AbstractSessionContext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;
using Elastosx::Net::Ssl::ISSLSession;

namespace Org {
namespace Conscrypt {

/**
 * Caches client sessions. Indexes by host and port. Users are typically
 * looking to reuse any session for a given host and port.
 */
CarClass(CClientSessionContext)
    , public IClientSessionContext
    , public AbstractSessionContext
{
public:
    class HostAndPort
        : public IClientSessionContextHostAndPort
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        HostAndPort(
            /* [in] */ const String& host,
            /* [in] */ Int32 port);

        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

    public:
        String mHost;
        Int32 mPort;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CClientSessionContext();

    CARAPI constructor();

    CARAPI Size(
        /* [out] */  Int32* result);

    CARAPI SetPersistentCache(
        /* [in] */  ISSLClientSessionCache* persistentCache);

    /**
     * Finds a cached session for the given host name and port.
     *
     * @param host of server
     * @param port of server
     * @return cached session or null if none found
     */
    CARAPI GetSession(
        /* [in] */  const String& host,
        /* [in] */  Int32 port,
        /* [out] */  ISSLSession** result);

    CARAPI PutSession(
        /* [in] */ ISSLSession* session);

protected:
    CARAPI_(void) SessionRemoved(
        /* [in] */ ISSLSession* session);

public:
    AutoPtr<IMap> mSessionsByHostAndPort; // = new HashMap<HostAndPort, SSLSession>();

private:
    AutoPtr<ISSLClientSessionCache> mPersistentCache;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CCLIENTSESSIONCONTEXT_H__
