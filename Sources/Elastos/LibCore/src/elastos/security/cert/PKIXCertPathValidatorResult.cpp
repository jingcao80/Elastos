//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "PKIXCertPathValidatorResult.h"
#include "StringBuilder.h"
#include "CPKIXCertPathValidatorResult.h"
#include "CPKIXCertPathBuilderResult.h"

using Elastos::Core::EIID_ICloneable;
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
    sb.Append(": [\n Trust Anchor: ");
    String strAnchor;
    mTrustAnchor->ToString(&strAnchor);
    sb.Append(strAnchor);
    sb.Append("\n Policy Tree: ");
    if (!mPolicyTree) {
        sb.Append("no valid policy tree\n");
    }
    else {
        String strPolicyTree = Object::ToString(mPolicyTree);
        sb.Append(strPolicyTree);
    }
    sb.Append("\n Subject Public Key: ");
    String subject = Object::ToString(mSubjectPublicKey);
    sb.Append(subject);
    sb.Append("\n]");
    return sb.ToString(str);
}

} // end Cert
} // end Security
} // end Elastos