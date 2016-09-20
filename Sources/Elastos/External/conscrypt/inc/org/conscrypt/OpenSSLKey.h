
#ifndef __ORG_CONSCRYPT_OPENSSLKEY_H__
#define __ORG_CONSCRYPT_OPENSSLKEY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastosx::Crypto::ISecretKey;

namespace Org {
namespace Conscrypt {

class OpenSSLKey
    : public Object
{
public:
    OpenSSLKey(
        /* [in] */ Int64 ctx);

    OpenSSLKey(
        /* [in] */ Int64 ctx,
        /* [in] */ IOpenSSLEngine* engine,
        /* [in] */ String alias);

    /**
     * Returns the raw pointer to the EVP_PKEY context for use in JNI calls. The
     * life cycle of this native pointer is managed by the {@code OpenSSLKey}
     * instance and must not be destroyed or freed by users of this API.
     */
    CARAPI GetPkeyContext(
        /* [out] */ Int64* result);

    CARAPI GetEngine(
        /* [out] */ IOpenSSLEngine** result);

    CARAPI IsEngineBased(
        /* [out] */ Boolean* result);

    CARAPI GetAlias(
        /* [out] */ String* result);

    static AutoPtr<IOpenSSLKey> FromPrivateKey(
        /* [in] */ IPrivateKey* key);

    static AutoPtr<IOpenSSLKey> FromPublicKey(
        /* [in] */ IPublicKey* key);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** result);

    static AutoPtr<IPublicKey> GetPublicKey(
        /* [in] */ IX509EncodedKeySpec* keySpec,
        /* [in] */ Int32 type);

    CARAPI GetPrivateKey(
        /* [out] */ IPrivateKey** result);

    static AutoPtr<IPrivateKey> GetPrivateKey(
        /* [in] */ IPKCS8EncodedKeySpec* keySpec,
        /* [in] */ Int32 type);

    CARAPI GetSecretKey(
        /* [in] */ String algorithm,
        /* [out] */ ISecretKey** result);

    CARAPI Finalize();

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

private:
    static AutoPtr<IOpenSSLKey> WrapPrivateKey(
        /* [in] */ IPrivateKey* key);

private:
    Int64 mCtx;

    AutoPtr<IOpenSSLEngine> mEngine;

    String mAlias;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLKEY_H__