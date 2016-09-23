
#ifndef __ORG_CONSCRYPT_CDEFAULTSSLCONTEXTIMPL_H__
#define __ORG_CONSCRYPT_CDEFAULTSSLCONTEXTIMPL_H__

#include "_Org_Conscrypt_CDefaultSSLContextImpl.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include "org/conscrypt/OpenSSLContextImpl.h"

using Elastos::Security::ISecureRandom;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ITrustManager;

namespace Org {
namespace Conscrypt {

/**
 * Support class for this package.
 */
CarClass(CDefaultSSLContextImpl)
    , public IDefaultSSLContextImpl
    , public OpenSSLContextImpl
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * DefaultSSLContextImpl delegates the work to the super class
     * since there is no way to put a synchronized around both the
     * call to super and the rest of this constructor to guarantee
     * that we don't have races in creating the state shared between
     * all default SSLContexts.
     */
    CARAPI constructor();

    CARAPI GetKeyManagers(
        /* [out, callee] */ ArrayOf<IKeyManager*>** result);

    CARAPI GetTrustManagers(
        /* [out, callee] */ ArrayOf<ITrustManager*>** result);

    // @Override
    virtual CARAPI EngineInit(
        /* [in] */ ArrayOf<IKeyManager*>* km,
        /* [in] */ ArrayOf<ITrustManager*>* tm,
        /* [in] */ ISecureRandom* sr);

private:
    /**
     * Accessed by SSLContextImpl(DefaultSSLContextImpl) holding the
     * DefaultSSLContextImpl.class monitor
     */
    static AutoPtr<ArrayOf<IKeyManager*> > KEY_MANAGERS;

    /**
     * Accessed by SSLContextImpl(DefaultSSLContextImpl) holding the
     * DefaultSSLContextImpl.class monitor
     */
    static AutoPtr<ArrayOf<ITrustManager*> > TRUST_MANAGERS;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CDEFAULTSSLCONTEXTIMPL_H__
