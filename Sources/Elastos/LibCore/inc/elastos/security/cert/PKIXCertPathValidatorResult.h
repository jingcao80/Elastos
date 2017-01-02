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

#ifndef __ELASTOS_SECURITY_CERT_PKIXCERTPATHVALIDATORRESULT_H__
#define __ELASTOS_SECURITY_CERT_PKIXCERTPATHVALIDATORRESULT_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

using Elastos::Security::IPublicKey;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Security {
namespace Cert {

class PKIXCertPathValidatorResult
    : public Object
    , public IPKIXCertPathValidatorResult
    , public ICertPathValidatorResult
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code PKIXCertPathValidatorResult} with the specified
     * trust anchor, the valid policy tree and the subject public key.
     *
     * @param trustAnchor
     *            the trust anchor describing the certification authority (CA)
     *            that served as trust anchor for the certification path.
     * @param policyTree
     *            the valid policy tree from the validation.
     * @param subjectPublicKey
     *            the subject public key from the validation.
     */
    CARAPI constructor(
        /* [in] */ ITrustAnchor *trustAnchor,
        /* [in] */ IPolicyNode *policyTree,
        /* [in] */ IPublicKey *subjectPublicKey);

    /**
     * Returns the valid policy tree from the validation.
     *
     * @return the valid policy tree from the validation.
     */
    CARAPI GetPolicyTree(
        /* [out] */ IPolicyNode **node);

    /**
     * Returns the subject public key from the validation.
     *
     * @return the subject public key from the validation.
     */
    CARAPI GetPublicKey(
        /* [out] */ IPublicKey **key);

    /**
     * Returns the trust anchor describing the certification authority (CA) that
     * served as trust anchor for this certification path.
     *
     * @return the trust anchor.
     */
    CARAPI GetTrustAnchor(
        /* [out] */ ITrustAnchor **anchor);

    /**
     * Clones this {@code PKIXCertPathValidatorResult} instance.
     *
     * @return the cloned instance.
     */
    CARAPI Clone(
        /* [out] */ IInterface **obj);

    /**
     * Returns a string representation for this {@code
     * PKIXCertPathValidatorResult} instance.
     *
     * @return a string representation for this {@code
     *         PKIXCertPathValidatorResult} instance.
     */
    CARAPI ToString(
        /* [out] */ String *str);

private:
    // A trust anchor used during validation of certification path
    AutoPtr<ITrustAnchor> mTrustAnchor;
    // Valid policy tree resulting from PKIX
    // certification path validation algorithm
    AutoPtr<IPolicyNode> mPolicyTree;
    // Public key of the subject (target) certificate
    AutoPtr<IPublicKey> mSubjectPublicKey;
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_PKIXCERTPATHVALIDATORRESULT_H__
