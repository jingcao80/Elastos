
#ifndef __ELASTOS_SECURITY_CKEYSTOREPASSWORDPROTECTION_H__
#define __ELASTOS_SECURITY_CKEYSTOREPASSWORDPROTECTION_H__

#include "_Elastos_Security_CKeyStorePasswordProtection.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Security {

CarClass(CKeyStorePasswordProtection)
    , public Object
    , public IKeyStorePasswordProtection
    , public IKeyStoreProtectionParameter
    , public IDestroyable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyStorePasswordProtection();

    CARAPI GetPassword(
        /* [out, callee] */ ArrayOf<Char32> **pwd);

    CARAPI Destroy();

    CARAPI IsDestroyed(
        /* [out] */ Boolean *isDestroyed);

    CARAPI constructor(
        /* [in] */ ArrayOf<Char32> *password);

private:
    // Store password
    AutoPtr<ArrayOf<Char32> > mPassword;

    Boolean mIsDestroyed;

    Object mLock;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYSTOREPASSWORDPROTECTION_H__
