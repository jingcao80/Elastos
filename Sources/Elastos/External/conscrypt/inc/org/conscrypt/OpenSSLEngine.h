
#ifndef __ORG_CONSCRYPT_OPENSSLENGINE_H__
#define __ORG_CONSCRYPT_OPENSSLENGINE_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::Security::IPrivateKey;
using Elastosx::Crypto::ISecretKey;

namespace Org {
namespace Conscrypt {

class OpenSSLEngine
    : public IOpenSSLEngine
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLEngine();

    ~OpenSSLEngine();

    CARAPI constructor(
        /* [in] */ Int64 engineCtx);

    static CARAPI GetInstance(
        /* [in] */ const String& engine,
        /* [out] */ IOpenSSLEngine** result);

    CARAPI GetPrivateKeyById(
        /* [in] */ const String& id,
        /* [out] */ IPrivateKey** result);

    CARAPI GetSecretKeyById(
        /* [in] */ const String& id,
        /* [in] */ const String& algorithm,
        /* [out] */ ISecretKey** result);

    CARAPI GetEngineContext(
        /* [out] */ Int64* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    static Boolean Init();

private:
    static Object mLoadingLock;

    /** The ENGINE's native handle. */
    Int64 mCtx;

    static Boolean sIsInited;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLENGINE_H__
