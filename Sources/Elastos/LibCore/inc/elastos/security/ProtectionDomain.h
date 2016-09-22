
#ifndef __ELASTOS_SECURITY_PROTECTIONDOMAIN_H__
#define __ELASTOS_SECURITY_PROTECTIONDOMAIN_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Security {

/**
 * Legacy security code; do not use.
 */
class ProtectionDomain
    : public Object
    , public IProtectionDomain
{
public:
    CAR_INTERFACE_DECL()

    ProtectionDomain(
        /* [in] */ ICodeSource* cs,
        /* [in] */ IPermissionCollection* permissions);

    ProtectionDomain(
        /* [in] */ ICodeSource* cs,
        /* [in] */ IPermissionCollection* permissions,
        /* [in] */ IClassLoader* cl,
        /* [in] */ ArrayOf<IPrincipal*>* principals);

    CARAPI GetClassLoader(
        /* [out] */ IClassLoader** loader);

    CARAPI GetCodeSource(
        /* [out] */ ICodeSource** result);

    CARAPI GetPermissions(
        /* [out] */ IPermissionCollection** result);

    CARAPI GetPrincipals(
        /* [out] */ ArrayOf<IPrincipal*>** result);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_PROTECTIONDOMAIN_H__
