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

#ifndef __ELASTOS_SECURITY_CERT_X509CRL_H__
#define __ELASTOS_SECURITY_CERT_X509CRL_H__

#include "CRL.h"

using Elastos::Security::IPublicKey;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Utility::IDate;
using Elastos::Math::IBigInteger;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Security {
namespace Cert {

class X509CRL
    : public CRL
    , public IX509CRL
    , public IX509Extension
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Equals(
        /* [in] */ IInterface *other,
        /* [out] */ Boolean *result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI GetIssuerX500Principal(
        /* [out] */ IX500Principal **issuer);

    CARAPI GetRevokedCertificateEx(
        /* [in] */ IX509Certificate *certificate,
        /* [out] */ IX509CRLEntry **crlEntry);

protected:
    X509CRL();
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_X509CRL_H__
