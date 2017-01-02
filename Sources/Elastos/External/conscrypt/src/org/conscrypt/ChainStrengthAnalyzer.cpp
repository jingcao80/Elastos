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

#include "org/conscrypt/ChainStrengthAnalyzer.h"

using Elastos::Math::IBigInteger;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Interfaces::IRSAKey;
using Elastos::Security::Interfaces::IRSAPublicKey;
using Elastos::Security::IPublicKey;

namespace Org {
namespace Conscrypt {

const Int32 ChainStrengthAnalyzer::MIN_MODULUS;

AutoPtr<ArrayOf<String> > ChainStrengthAnalyzer::InitOID_BLACKLIST()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, String("1.2.840.113549.1.1.4"));
    return array;
}

AutoPtr<ArrayOf<String> > ChainStrengthAnalyzer::OID_BLACKLIST = InitOID_BLACKLIST();

ECode ChainStrengthAnalyzer::Check(
    /* [in] */  ArrayOf<IX509Certificate*>* chain)
{
    for (Int32 i = 0; i < chain->GetLength(); i++) {
        AutoPtr<IX509Certificate> cert = (*chain)[i];
        FAIL_RETURN(CheckCert(cert));
    }
    return NOERROR;
}

ECode ChainStrengthAnalyzer::CheckCert(
    /* [in] */ IX509Certificate* cert)
{
    FAIL_RETURN(CheckModulusLength(cert));
    FAIL_RETURN(CheckNotMD5(cert));
    return NOERROR;
}

ECode ChainStrengthAnalyzer::CheckModulusLength(
    /* [in] */ IX509Certificate* cert)
{
    AutoPtr<IPublicKey> pubkey;
    ICertificate::Probe(cert)->GetPublicKey((IPublicKey**)&pubkey);
    AutoPtr<IRSAPublicKey> key = IRSAPublicKey::Probe(pubkey);
    if (key != NULL) {
        AutoPtr<IBigInteger> bi;
        IRSAKey::Probe(key)->GetModulus((IBigInteger**)&bi);
        Int32 modulusLength;
        bi->BitLength(&modulusLength);
        if(!(modulusLength >= MIN_MODULUS)) {
            // throw new CertificateException("Modulus is < 1024 bits");
            return E_CERTIFICATE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode ChainStrengthAnalyzer::CheckNotMD5(
    /* [in] */ IX509Certificate* cert)
{
    String oid;
    cert->GetSigAlgOID(&oid);
    for (Int32 i = 0; i < OID_BLACKLIST->GetLength(); i++) {
        String blacklisted = (*OID_BLACKLIST)[i];
        if (oid.Equals(blacklisted)) {
            // throw new CertificateException("Signature uses an insecure hash function");
            return E_CERTIFICATE_EXCEPTION;
        }
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
