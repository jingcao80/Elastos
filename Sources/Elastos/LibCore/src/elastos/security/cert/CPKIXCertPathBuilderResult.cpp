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

#include "CPKIXCertPathBuilderResult.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPKIXCertPathBuilderResult)
CAR_INTERFACE_IMPL_2(CPKIXCertPathBuilderResult, PKIXCertPathValidatorResult, \
        IPKIXCertPathBuilderResult,ICertPathBuilderResult)
ECode CPKIXCertPathBuilderResult::ToString(
    /* [out] */ String *str)
{
    FAIL_RETURN(PKIXCertPathValidatorResult::ToString(str))
    StringBuilder sb(*str);
    sb.Append("\n Certification Path: ");
    String certPath;
    IObject::Probe(mCertPath)->ToString(&certPath);
    sb.Append(certPath);
    sb.Append("\n]");
    return sb.ToString(str);
}

ECode CPKIXCertPathBuilderResult::GetCertPath(
    /* [out] */ ICertPath **certPath)
{
    VALIDATE_NOT_NULL(certPath)
    *certPath = mCertPath;
    REFCOUNT_ADD(*certPath)
    return NOERROR;
}

ECode CPKIXCertPathBuilderResult::constructor(
    /* [in] */ ICertPath *certPath,
    /* [in] */ ITrustAnchor *trustAnchor,
    /* [in] */ IPolicyNode *policyTree,
    /* [in] */ IPublicKey *subjectPublicKey)
{
    PKIXCertPathValidatorResult::constructor(trustAnchor, policyTree, subjectPublicKey);
    if (!certPath) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCertPath = certPath;
    return NOERROR;
}

}
}
}
