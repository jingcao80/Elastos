#include "elastos/droid/keystore/security/KeyStoreParameter.h"
#include "elastos/droid/keystore/security/KeyStore.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

//import android.content.Context;
//
//import java.security.KeyPairGenerator;
//import java.security.KeyStore.ProtectionParameter;
using Elastos::Security::EIID_IKeyStoreProtectionParameter;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(KeyStoreParameter::Builder, Object, IKeyStoreParameterBuilder);

ECode KeyStoreParameter::Builder::constructor(
    /* [in] */ IContext* context)
{
    if (context == NULL) {
        //throw new NullPointerException("context == null");
        Logger::E("KeyStoreParameter::Builder", "Builder() context == null");
        assert(0);
    }
    // Context is currently not used, but will be in the future.
    return NOERROR;
}

ECode KeyStoreParameter::Builder::SetEncryptionRequired(
    /* [in] */ Boolean required)
{
    if (required) {
        mFlags |= KeyStore::FLAG_ENCRYPTED;
    } else {
        mFlags &= ~KeyStore::FLAG_ENCRYPTED;
    }
    return NOERROR;
}

ECode KeyStoreParameter::Builder::Build(
    /* [out] */ IKeyStoreParameter** para)
{
    VALIDATE_NOT_NULL(para);
    AutoPtr<KeyStoreParameter> ksp = new KeyStoreParameter();
    ksp->constructor(mFlags);
    *para = ksp;
    REFCOUNT_ADD(*para);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(KeyStoreParameter, Object, IKeyStoreParameter, IKeyStoreProtectionParameter);

ECode KeyStoreParameter::constructor(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode KeyStoreParameter::GetFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFlags;
    return NOERROR;
}

ECode KeyStoreParameter::IsEncryptionRequired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & KeyStore::FLAG_ENCRYPTED) != 0;
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security
