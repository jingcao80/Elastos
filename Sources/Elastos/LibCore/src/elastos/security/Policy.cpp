
#include "Policy.h"
#include "AllPermissionCollection.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(Policy, Object, IPolicy)
AutoPtr<PermissionCollection> Policy::UNSUPPORTED_EMPTY_COLLECTION = new AllPermissionCollection();
Policy::~Policy()
{}

Policy::Policy()
{}

ECode Policy::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IPolicyParameters* params,
    /* [out] */ IPolicy** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IPolicyParameters* params,
    /* [in] */ const String& provider,
    /* [out] */ IPolicy** result) /*throws NoSuchProviderException, NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IPolicyParameters* params,
    /* [in] */ IProvider* provider,
    /* [out] */ IPolicy** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetParameters(
    /* [out] */ IPolicyParameters** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetProvider(
    /* [out] */ IProvider** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode Policy::GetPermissions(
    /* [in] */ ICodeSource* cs,
    /* [out] */ IPermissionCollection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::Refresh()
{
    return NOERROR;
}

ECode Policy::GetPermissions(
    /* [in] */ IProtectionDomain* domain,
    /* [out] */ IPermissionCollection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::Implies(
    /* [in] */ IProtectionDomain* domain,
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

AutoPtr<IPolicy> Policy::GetPolicy()
{
    return NULL;
}

ECode Policy::SetPolicy(
    /* [in] */ IPolicy* policy)
{
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
