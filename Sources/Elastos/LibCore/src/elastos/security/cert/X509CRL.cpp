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

// {62B8CDC7-7E0C-5371-6409-7D49314BBC7A}
extern "C" const InterfaceID EIID_X509CRL =
        { 0x62b8cdc7, 0x7e0c, 0x5371, { 0x64, 0x09, 0x7d, 0x49, 0x31, 0x4b, 0xbc, 0x7a } };

CAR_INTERFACE_IMPL(X509CRL, CRL, IX509Extension)

ECode X509CRL::Equals(
    /* [in] */ IInterface *other,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    if (reinterpret_cast<X509CRL*>(other->Probe(EIID_X509CRL)) == this) {
        *result = TRUE;
        return NOERROR;
    }
    if (other->Probe(EIID_X509CRL)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > encode, oEncode;
    ECode ec;
    FAIL_GOTO(ec = GetEncoded((ArrayOf<Byte>**)&encode), ERROR_PROCESS)
    FAIL_GOTO(ec = reinterpret_cast<X509CRL*>(other->Probe(EIID_X509CRL))->GetEncoded((ArrayOf<Byte>**)&oEncode), ERROR_PROCESS)
    *result = Arrays::Equals(encode, oEncode);
    return NOERROR;
ERROR_PROCESS:
    if ((unsigned Int32)ec == E_CRL_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
    return ec;
}

ECode X509CRL::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 res = 0;
    AutoPtr<ArrayOf<Byte> > array;
    ECode ec;
    FAIL_GOTO(ec = GetEncoded((ArrayOf<Byte>**)&array), ERROR_PROCESS)
    for (Int32 i = 0; i < array->GetLength(); i++) {
        res += (*array)[i] & 0xFF;
    }
    *hashCode = res;
    return NOERROR;
ERROR_PROCESS:
    if ((unsigned Int32)ec == E_CRL_EXCEPTION) {
        *hashCode = 0;
        return NOERROR;
    }
    return ec;
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
    return reinterpret_cast<X509CRL*>(crl->Probe(EIID_X509CRL))->GetIssuerX500Principal(issuer);
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

