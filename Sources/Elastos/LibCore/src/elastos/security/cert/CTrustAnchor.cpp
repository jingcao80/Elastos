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

#include "CTrustAnchor.h"
#include "StringBuilder.h"
#include "CX500Principal.h"

using Elastos::Security::IPrincipal;
using Elastosx::Security::Auth::X500::CX500Principal;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CTrustAnchor)
CAR_INTERFACE_IMPL(CTrustAnchor, Object, ITrustAnchor)
ECode CTrustAnchor::GetNameConstraints(
    /* [out, callee] */ ArrayOf<Byte> **nameConstraints)
{
    VALIDATE_NOT_NULL(nameConstraints)
    if (!mNameConstraints->GetLength()) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(mNameConstraints->GetLength());
    ret->Copy(0, mNameConstraints, 0, mNameConstraints->GetLength());
    *nameConstraints = ret;
    REFCOUNT_ADD(*nameConstraints)
    return NOERROR;
}

ECode CTrustAnchor::GetTrustedCert(
    /* [out] */ IX509Certificate **cert)
{
    VALIDATE_NOT_NULL(cert)
    *cert = mTrustedCert;
    REFCOUNT_ADD(*cert)
    return NOERROR;
}

ECode CTrustAnchor::GetCA(
    /* [out] */ IX500Principal **ca)
{
    VALIDATE_NOT_NULL(ca)
    *ca = mCaPrincipal;
    REFCOUNT_ADD(*ca)
    return NOERROR;
}

ECode CTrustAnchor::GetCAName(
    /* [out] */ String *caName)
{
    VALIDATE_NOT_NULL(caName)
    *caName = mCaName;
    return NOERROR;
}

ECode CTrustAnchor::GetCAPublicKey(
    /* [out] */ IPublicKey **key)
{
    VALIDATE_NOT_NULL(key)
    *key = mCaPublicKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CTrustAnchor::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("TrustAnchor: [\n");
    if (mTrustedCert) {
        sb.Append("Trusted CA certificate: ");
        sb.Append(mTrustedCert);
        sb.Append("\n");
    }
    if (mCaPrincipal) {
        sb.Append("Trusted CA Name: ");
        sb.Append(mCaPrincipal);
        sb.Append("\n");
    }
    if (mCaPublicKey) {
        sb.Append("Trusted CA Public Key: ");
        sb.Append(mCaPublicKey);
        sb.Append("\n");
    }
    // FIXME if needed:
    if (mNameConstraints->GetLength()) {
        sb.Append("Name Constraints:\n");
        //Apache...Todo late
        //sb.append(Array.toString(nameConstraints, "    "));
    }
    sb.Append("\n]");
    return sb.ToString(str);
}

ECode CTrustAnchor::constructor(
    /* [in] */ IX509Certificate *trustedCert,
    /* [in] */ ArrayOf<Byte> *nameConstraints)
{
    if (trustedCert == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mTrustedCert = trustedCert;
    // copy nameConstraints if not null
    if (nameConstraints->GetLength()) {
        mNameConstraints = ArrayOf<Byte>::Alloc(nameConstraints->GetLength());
        mNameConstraints->Copy(0, nameConstraints, 0, mNameConstraints->GetLength());
        ProcessNameConstraints();
    }
    else {
        mNameConstraints = NULL;
    }
    mCaName = String();
    mCaPrincipal = NULL;
    mCaPublicKey = NULL;
    return NOERROR;
}

ECode CTrustAnchor::constructor(
    /* [in] */ const String& caName,
    /* [in] */ IPublicKey *caPublicKey,
    /* [in] */ ArrayOf<Byte> *nameConstraints)
{
    if (caName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCaName = caName;
    if (caPublicKey == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCaPublicKey = caPublicKey;
    // copy nameConstraints if not null
    if (nameConstraints->GetLength()) {
        mNameConstraints = ArrayOf<Byte>::Alloc(nameConstraints->GetLength());
        mNameConstraints->Copy(0, nameConstraints, 0, mNameConstraints->GetLength());
        ProcessNameConstraints();
    }
    else {
        mNameConstraints = NULL;
    }
    mTrustedCert = NULL;

    // X500Principal checks caName validity
    if (caName.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IX500Principal> principal;
    CX500Principal::New(mCaName, (IX500Principal**)&principal);
    mCaPrincipal = principal;
    return NOERROR;
}

ECode CTrustAnchor::constructor(
    /* [in] */ IX500Principal *caPrincipal,
    /* [in] */ IPublicKey *caPublicKey,
    /* [in] */ ArrayOf<Byte> *nameConstraints)
{
    if (caPrincipal == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCaPrincipal = caPrincipal;
    if (caPublicKey == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCaPublicKey = caPublicKey;
    // copy nameConstraints if not null
    if (nameConstraints->GetLength()) {
        mNameConstraints = ArrayOf<Byte>::Alloc(nameConstraints->GetLength());
        mNameConstraints->Copy(0, nameConstraints, 0, mNameConstraints->GetLength());
        ProcessNameConstraints();
    }
    else {
        mNameConstraints = NULL;
    }
    mTrustedCert = NULL;
    return IPrincipal::Probe(caPrincipal)->GetName(&mCaName);
}

ECode CTrustAnchor::ProcessNameConstraints()
{
    //Apache...Todo
    /*
    try {
        // decode and check nameConstraints
        NameConstraints.ASN1.decode(nameConstraints);
    } catch (IOException e) {
        throw new IllegalArgumentException(e.getMessage());
    }
    */
    return E_NOT_IMPLEMENTED;
}

}
}
}

