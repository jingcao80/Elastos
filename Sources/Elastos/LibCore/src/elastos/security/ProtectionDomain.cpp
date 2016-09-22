
#include "ProtectionDomain.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(ProtectionDomain, Object, IProtectionDomain)
ProtectionDomain::ProtectionDomain(
    /* [in] */ ICodeSource* cs,
    /* [in] */ IPermissionCollection* permissions)
{}

ProtectionDomain::ProtectionDomain(
    /* [in] */ ICodeSource* cs,
    /* [in] */ IPermissionCollection* permissions,
    /* [in] */ IClassLoader* cl,
    /* [in] */ ArrayOf<IPrincipal*>* principals)
{}

ECode ProtectionDomain::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader);
    *loader = NULL;
    return NOERROR;
}

ECode ProtectionDomain::GetCodeSource(
    /* [out] */ ICodeSource** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ProtectionDomain::GetPermissions(
    /* [out] */ IPermissionCollection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ProtectionDomain::GetPrincipals(
    /* [out] */ ArrayOf<IPrincipal*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ProtectionDomain::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
