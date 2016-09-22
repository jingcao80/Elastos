
#include "CUnresolvedPermission.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CUnresolvedPermission)
CAR_INTERFACE_IMPL(CUnresolvedPermission, Permission, IUnresolvedPermission)
ECode CUnresolvedPermission::constructor(
    /* [in] */ const String& type,
    /* [in] */ const String& name,
    /* [in] */ const String& actions,
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*>* certs)
{
    return Permission::constructor(String(""));
}

ECode CUnresolvedPermission::GetUnresolvedName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CUnresolvedPermission::GetUnresolvedActions(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CUnresolvedPermission::GetUnresolvedType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CUnresolvedPermission::GetUnresolvedCerts(
    /* [out] */ ArrayOf<Elastos::Security::Cert::ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

// @Override
ECode CUnresolvedPermission::GetActions(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

// @Override
ECode CUnresolvedPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

}
}
