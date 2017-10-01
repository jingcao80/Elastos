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

#ifndef __ELASTOS_SECURITY_CERT_PKIXPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_PKIXPARAMETERS_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

using Elastos::Utility::ISet;
using Elastos::Utility::IList;
using Elastos::Utility::IDate;
using Elastos::Security::IKeyStore;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Security {
namespace Cert {

class PKIXParameters
    : public Object
    , public IPKIXParameters
    , public ICertPathParameters
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    PKIXParameters();

    virtual ~PKIXParameters();

    virtual CARAPI GetTrustAnchors(
        /* [out] */ ISet **trustAnchors);

    virtual CARAPI SetTrustAnchors(
        /* [in] */ ISet *trustAnchors);

    virtual CARAPI IsAnyPolicyInhibited(
        /* [out] */ Boolean *anyPolicyInhibited);

    virtual CARAPI SetAnyPolicyInhibited(
        /* [in] */ Boolean anyPolicyInhibited);

    virtual CARAPI GetCertPathCheckers(
        /* [out] */ IList **certPathCheckers);

    virtual CARAPI SetCertPathCheckers(
        /* [in] */ IList *certPathCheckers);

    virtual CARAPI AddCertPathChecker(
        /* [in] */ IPKIXCertPathChecker *checker);

    virtual CARAPI GetCertStores(
        /* [out] */ IList **certStores);

    virtual CARAPI SetCertStores(
        /* [in] */ IList *certStores);

    virtual CARAPI AddCertStore(
        /* [in] */ ICertStore *store);

    virtual CARAPI GetDate(
        /* [out] */ IDate **date);

    virtual CARAPI SetDate(
        /* [in] */ IDate *date);

    virtual CARAPI IsExplicitPolicyRequired(
        /* [out] */ Boolean *explicitPolicyRequired);

    virtual CARAPI SetExplicitPolicyRequired(
        /* [in] */ Boolean explicitPolicyRequired);

    virtual CARAPI GetInitialPolicies(
        /* [out] */ ISet **initialPolicies);

    virtual CARAPI SetInitialPolicies(
        /* [in] */ ISet *initialPolicies);

    virtual CARAPI IsPolicyMappingInhibited(
        /* [out] */ Boolean *policyMappingInhibited);

    virtual CARAPI SetPolicyMappingInhibited(
        /* [in] */ Boolean policyMappingInhibited);

    virtual CARAPI GetPolicyQualifiersRejected(
        /* [out] */ Boolean * policyQualifiersRejected);

    virtual CARAPI SetPolicyQualifiersRejected(
        /* [in] */ Boolean policyQualifiersRejected);

    virtual CARAPI IsRevocationEnabled(
        /* [out] */ Boolean *revocationEnabled);

    virtual CARAPI SetRevocationEnabled(
        /* [in] */ Boolean revocationEnabled);

    virtual CARAPI GetSigProvider(
        /* [out] */ String *sigProvider);

    virtual CARAPI SetSigProvider(
        /* [in] */ const String& sigProvider);

    virtual CARAPI GetTargetCertConstraints(
        /* [out] */ ICertSelector **targetCertConstraints);

    virtual CARAPI SetTargetCertConstraints(
        /* [in] */ ICertSelector *targetCertConstraints);

    virtual CARAPI Clone(
        /* [out] */ IInterface **obj);

    virtual CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ ISet *trustAnchors);

    CARAPI constructor(
        /* [in] */ IKeyStore *keyStore);

private:
    /**
     * Checks that {@code trustAnchors} contains only {@code
     * TrustAnchor} instances.
     *
     * @throws InvalidAlgorithmParameterException if trustAnchors set is empty.
     */
    CARAPI CheckTrustAnchors(
        /* [in] */ ISet *trustAnchors);

private:
    // Set of trust anchors - most trusted CAs
    AutoPtr<ISet> mTrustAnchors;
    // Set of acceptable initial policy identifiers (OID strings)
    AutoPtr<ISet> mInitialPolicies;
    // List of cert stores that used to find certificates and CRLs
    AutoPtr<IList> mCertStores;
    // Time for which the validity of the certification
    // patch should be determined
    AutoPtr<IDate> mDate;
    // List of certification patch checkers (PKIXCertPathChecker)
    AutoPtr<IList> mCertPathCheckers;
    // Preferred signature provider name
    String mSigProvider;
    // Required constraints on the target certificate
    AutoPtr<ICertSelector> mTargetCertConstraints;
    // Indicates whether cert revocation is enabled or not
    Boolean mRevocationEnabled;
    // Indicates whether explicit policy required or not
    Boolean mExplicitPolicyRequired;
    // Indicates whether policy mapping inhibited or not
    Boolean mPolicyMappingInhibited;
    // Indicates whether any policy inhibited or not
    Boolean mAnyPolicyInhibited;
    // Indicates whether certificates that include policy
    // qualifiers in a certificate policies extension that
    // is marked critical must be rejected or not
    Boolean mPolicyQualifiersRejected;
};

}
}
}

#endif // __CPKIXPARAMETERS_H__
