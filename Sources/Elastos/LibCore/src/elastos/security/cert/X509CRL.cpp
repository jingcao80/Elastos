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

#include "X509CRL.h"
#include "CCertificateFactoryHelper.h"
#include "CByteArrayInputStream.h"
#include "Arrays.h"

using Elastos::IO::IInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL_2(X509CRL, CRL, IX509CRL, IX509Extension)

ECode X509CRL::Equals(
    /* [in] */ IInterface *other,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    if (IX509CRL::Probe(other) == (IX509CRL*)this) {
        *result = TRUE;
        return NOERROR;
    }
    if (!IX509CRL::Probe(other)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > encode, oEncode;
    ECode ec = GetEncoded((ArrayOf<Byte>**)&encode);
    if (FAILED(ec)) {
        *result = FALSE;
        return NOERROR;
    }
    ec = IX509CRL::Probe(other)->GetEncoded((ArrayOf<Byte>**)&oEncode);
    if (FAILED(ec)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = Arrays::Equals(encode, oEncode);
    return NOERROR;
}

ECode X509CRL::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 res = 0;
    AutoPtr<ArrayOf<Byte> > array;
    ECode ec = GetEncoded((ArrayOf<Byte>**)&array);
    if (FAILED(ec)) {
        *hashCode = 0;
        return NOERROR;
    }
    for (Int32 i = 0; i < array->GetLength(); i++) {
        res += (*array)[i] & 0xFF;
    }
    *hashCode = res;
    return NOERROR;
}

ECode X509CRL::GetIssuerX500Principal(
    /* [out] */ IX500Principal **issuer)
{
    VALIDATE_NOT_NULL(issuer)
    // TODO if there is no X.509 certificate provider installed
    // should we try to access Harmony X509CRLImpl via classForName?
    AutoPtr<ICertificateFactoryHelper> help;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&help);
    AutoPtr<ICertificateFactory> factory;
    help->GetInstance(String("X.509"), (ICertificateFactory**)&factory);
    AutoPtr<ArrayOf<Byte> > encode;
    GetEncoded((ArrayOf<Byte>**)&encode);
    AutoPtr<IInputStream> is;
    CByteArrayInputStream::New(encode, (IInputStream**)&is);
    AutoPtr<ICRL> crl;
    factory->GenerateCRL(is, (ICRL**)&crl);
    return IX509CRL::Probe(crl)->GetIssuerX500Principal(issuer);
}

ECode X509CRL::GetRevokedCertificateEx(
    /* [in] */ IX509Certificate *certificate,
    /* [out] */ IX509CRLEntry **crlEntry)
{
    if (certificate == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IBigInteger> number;
    certificate->GetSerialNumber((IBigInteger**)&number);
    return GetRevokedCertificate(number, crlEntry);
}

X509CRL::X509CRL()
    : CRL(String("X.509"))
{}

}
}
}

