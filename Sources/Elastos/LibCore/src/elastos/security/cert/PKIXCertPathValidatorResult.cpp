
#include "PKIXCertPathValidatorResult.h"
#include "StringBuilder.h"
#include "CPKIXCertPathValidatorResult.h"
#include "CPKIXCertPathBuilderResult.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL_3(PKIXCertPathValidatorResult, Object, IPKIXCertPathValidatorResult,ICertPathValidatorResult,ICloneable)
ECode PKIXCertPathValidatorResult::constructor(
    /* [in] */ ITrustAnchor *trustAnchor,
    /* [in] */ IPolicyNode *policyTree,
    /* [in] */ IPublicKey *subjectPublicKey)
{
    mTrustAnchor = trustAnchor;
    mPolicyTree = policyTree;
    mSubjectPublicKey = subjectPublicKey;
    if (!mTrustAnchor) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!mSubjectPublicKey) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode PKIXCertPathValidatorResult::GetPolicyTree(
    /* [out] */ IPolicyNode **node)
{
    VALIDATE_NOT_NULL(node)
    *node = mPolicyTree;
    REFCOUNT_ADD(*node)
    return NOERROR;
}

ECode PKIXCertPathValidatorResult::GetPublicKey(
    /* [out] */ IPublicKey **key)
{
    VALIDATE_NOT_NULL(key)
    *key = mSubjectPublicKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode PKIXCertPathValidatorResult::GetTrustAnchor(
    /* [out] */ ITrustAnchor **anchor)
{
    VALIDATE_NOT_NULL(anchor)
    *anchor = mTrustAnchor;
    REFCOUNT_ADD(*anchor)
    return NOERROR;
}

ECode PKIXCertPathValidatorResult::Clone(
        /* [out] */ IInterface **obj)
{
    VALIDATE_NOT_NULL(obj)
    if (this->Probe(EIID_IPKIXCertPathValidatorResult)) {
        AutoPtr<IPKIXCertPathValidatorResult> result;
        FAIL_RETURN(CPKIXCertPathValidatorResult::New(NULL, NULL, NULL, (IPKIXCertPathValidatorResult**)&result))
        *obj = result.Get();
        REFCOUNT_ADD(*obj)
        return NOERROR;
    }
    else if (this->Probe(EIID_IPKIXCertPathBuilderResult)) {
        AutoPtr<IPKIXCertPathBuilderResult> result;
        FAIL_RETURN(CPKIXCertPathBuilderResult::New(
            NULL, NULL, NULL, NULL, (IPKIXCertPathBuilderResult**)&result))
        *obj = result.Get();
        REFCOUNT_ADD(*obj)
        return NOERROR;
    }
    return E_RUNTIME_EXCEPTION;
}

ECode PKIXCertPathValidatorResult::ToString(
    /* [out] */ String *str)
{
    VALIDATE_NOT_NULL(str)

    *str = Object::ToString(this);
    StringBuilder sb(*str);
    sb.AppendCStr(": [\n Trust Anchor: ");
    String strAnchor;
    mTrustAnchor->ToString(&strAnchor);
    sb.AppendString(strAnchor);
    sb.AppendCStr("\n Policy Tree: ");
    if (!mPolicyTree) {
        sb.AppendCStr("no valid policy tree\n");
    }
    else {
        String strPolicyTree = Object::ToString(mPolicyTree);
        sb.AppendString(strPolicyTree);
    }
    sb.AppendCStr("\n Subject Public Key: ");
    String subject = Object::ToString(mSubjectPublicKey);
    sb.AppendString(subject);
    sb.AppendCStr("\n]");
    return sb.ToString(str);
}

} // end Cert
} // end Security
} // end Elastos