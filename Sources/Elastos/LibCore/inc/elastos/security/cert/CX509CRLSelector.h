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

#ifndef __ELASTOS_SECURITY_CERT_CX509CRLSELECTOR_H__
#define __ELASTOS_SECURITY_CERT_CX509CRLSELECTOR_H__

#include "_Elastos_Security_Cert_CX509CRLSelector.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "core/Object.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IDate;
using Elastos::Utility::ICollection;
using Elastos::Math::IBigInteger;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CX509CRLSelector)
    , public Object
    , public IX509CRLSelector
    , public ICRLSelector
    , public ICloneable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CX509CRLSelector();

    CARAPI SetIssuers(
        /* [in] */ ICollection *issuers);

    CARAPI SetIssuerNames(
        /* [in] */ ICollection *names);

    CARAPI AddIssuer(
        /* [in] */ IX500Principal *issuer);

    CARAPI AddIssuerName(
        /* [in] */ const String& issuerName);

    CARAPI AddIssuerNameEx(
        /* [in] */ ArrayOf<Byte> *iss_name);

    CARAPI SetMinCRLNumber(
        /* [in] */ IBigInteger *minCRL);

    CARAPI SetMaxCRLNumber(
        /* [in] */ IBigInteger *maxCRL);

    CARAPI SetDateAndTime(
        /* [in] */ IDate *dateAndTime);

    CARAPI SetCertificateChecking(
        /* [in] */ IX509Certificate *cert);

    CARAPI GetIssuers(
        /* [out] */ ICollection **issuers);

    CARAPI GetIssuerNames(
        /* [out] */ ICollection **issuer);

    CARAPI GetMinCRL(
        /* [out] */ IBigInteger **minCrl);

    CARAPI GetMaxCRL(
        /* [out] */ IBigInteger **maxCrl);

    CARAPI GetDateAndTime(
        /* [out] */ IDate **dt);

    CARAPI GetCertificateChecking(
        /* [out] */ IX509Certificate **checking);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor();

    CARAPI Match(
        /* [in] */ ICRL *crl,
        /* [out] */ Boolean *result);

    CARAPI Clone(
    /* [out] */ IInterface **obj);

private:
    // issuerNames criterion:
    // contains X.500 distinguished names in CANONICAL format
    AutoPtr<IArrayList> mIssuerNames;
    // contains X500Principal objects corresponding to the names
    // from issuerNames collection (above)
    AutoPtr<IArrayList> mIssuerPrincipals;
    // minCRLNumber criterion
    AutoPtr<IBigInteger> mMinCRL;
    // maxCRLNumber criterion
    AutoPtr<IBigInteger> mMaxCRL;
    // dateAndTime criterion
    Int64 mDateAndTime;
    // the certificate being checked
    AutoPtr<IX509Certificate> mCertificateChecking;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CX509CRLSELECTOR_H__
