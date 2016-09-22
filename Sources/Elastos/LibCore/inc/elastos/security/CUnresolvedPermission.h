
#ifndef __ELASTOS_SECURITY_CUNRESOLVEDPERMISSION_H__
#define __ELASTOS_SECURITY_CUNRESOLVEDPERMISSION_H__

#include "_Elastos_Security_CUnresolvedPermission.h"
#include "Permission.h"

namespace Elastos {
namespace Security {

/**
 * Legacy security code; do not use.
 */
CarClass(CUnresolvedPermission)
    , public Permission
    , public IUnresolvedPermission
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& type,
        /* [in] */ const String& name,
        /* [in] */ const String& actions,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*>* certs);

    CARAPI GetUnresolvedName(
        /* [out] */ String* result);

    CARAPI GetUnresolvedActions(
        /* [out] */ String* result);

    CARAPI GetUnresolvedType(
        /* [out] */ String* result);

    CARAPI GetUnresolvedCerts(
        /* [out] */ ArrayOf<Elastos::Security::Cert::ICertificate*>** result);

    // @Override
    CARAPI GetActions(
        /* [out] */ String* result);

    // @Override
    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

}
}

#endif // __ELASTOS_SECURITY_CUNRESOLVEDPERMISSION_H__
