
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/net/PskKeyManager.h"

using Elastosx::Net::Ssl::EIID_IKeyManager;
using Org::Conscrypt::EIID_IPSKKeyManager;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_3(PskKeyManager, Object, IKeyManager, IPSKKeyManager, IPskKeyManager)

ECode PskKeyManager::ChooseServerKeyIdentityHint(
    /* [in] */ ISocket* socket,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    return NOERROR;
}

ECode PskKeyManager::ChooseServerKeyIdentityHint(
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    return NOERROR;
}

ECode PskKeyManager::ChooseClientKeyIdentity(
    /* [in] */ const String& identityHint,
    /* [in] */ ISocket* socket,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("");

    return NOERROR;
}

ECode PskKeyManager::ChooseClientKeyIdentity(
    /* [in] */ const String& identityHint,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("");

    return NOERROR;
}

ECode PskKeyManager::GetKey(
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [in] */ ISocket* socket,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    return NOERROR;
}

ECode PskKeyManager::GetKey(
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
