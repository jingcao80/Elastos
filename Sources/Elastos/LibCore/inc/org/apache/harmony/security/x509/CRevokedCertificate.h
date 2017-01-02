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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CREVOKEDCERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CREVOKEDCERTIFICATE_H__

#include "_Org_Apache_Harmony_Security_X509_CRevokedCertificate.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CRevokedCertificate)
    , public Object
    , public IRevokedCertificate
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCrlEntryExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetUserCertificate(
        /* [out] */ IBigInteger** ppCertificate);

    CARAPI GetRevocationDate(
        /* [out] */ IDate** ppDate);

    CARAPI GetIssuer(
        /* [out] */ IX500Principal** ppIssuer);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface* pRc,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pHashCode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ IBigInteger* pUserCertificate,
        /* [in] */ IDate* pRevocationDate,
        /* [in] */ IExtensions* pCrlEntryExtensions);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CREVOKEDCERTIFICATE_H__
