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

#ifndef __ORG_CONSCRYPT_OPENSSLX509CRL_H__
#define __ORG_CONSCRYPT_OPENSSLX509CRL_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IDate;
using Elastos::Security::IPublicKey;
using Elastos::Security::IPrincipal;
using Elastos::Security::Cert::IX509CRLEntry;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificate;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Conscrypt {

class OpenSSLX509CRL
    : public Object // public X509CRL
    , public IOpenSSLX509CRL
{
public:
    static CARAPI FromX509DerInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IOpenSSLX509CRL** result);

    static CARAPI FromPkcs7DerInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IList** result);

    static CARAPI FromX509PemInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IOpenSSLX509CRL** result);

    static CARAPI FromPkcs7PemInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IList** result);

    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI GetExtensionValue(
        /* [in] */ String oid,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI Verify(
        /* [in] */ IPublicKey* key);

    CARAPI Verify(
        /* [in] */ IPublicKey* key,
        /* [in] */ String sigProvider);

    CARAPI GetVersion(
        /* [out] */ Int32* result);

    CARAPI GetIssuerDN(
        /* [out] */ IPrincipal** result);

    CARAPI GetIssuerX500Principal(
        /* [out] */ IX500Principal** result);

    CARAPI GetThisUpdate(
        /* [out] */ IDate** result);

    CARAPI GetNextUpdate(
        /* [out] */ IDate** result);

    CARAPI GetRevokedCertificate(
        /* [in] */ IBigInteger* serialNumber,
        /* [out] */ IX509CRLEntry** result);

    CARAPI GetRevokedCertificate(
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ IX509CRLEntry** result);

    CARAPI GetRevokedCertificates(
        /* [out] */ ISet** result);

    CARAPI GetTBSCertList(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetSignature(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetSigAlgName(
        /* [out] */ String* result);

    CARAPI GetSigAlgOID(
        /* [out] */ String* result);

    CARAPI GetSigAlgParams(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI IsRevoked(
        /* [in] */ ICertificate* cert,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Finalize();

private:
    OpenSSLX509CRL(
        /* [in] */ Int64 ctx);

    void VerifyOpenSSL(
        /* [in] */ IOpenSSLKey* pkey);

    void VerifyInternal(
        /* [in] */ IPublicKey* key,
        /* [in] */ String sigProvider);

private:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509CRL_H__