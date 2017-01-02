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

#include "CX509CRLEntryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CAR_OBJECT_IMPL(CX509CRLEntryImpl)

ECode CX509CRLEntryImpl::GetCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    if (mExtensions == NULL) {
        return NOERROR;
    }
    return mExtensions->GetCriticalExtensions(oids);
}

ECode CX509CRLEntryImpl::GetExtensionValue(
    /* [in] */ const String& oid,
    /* [out, callee] */ ArrayOf<Byte>** extensionValue)
{
    if (mExtensions == NULL) {
            return NOERROR;
    }
    AutoPtr<IX509Extension> ext;
    mExtensions->GetExtensionByOID(oid, (IX509Extension**)&ext);
    if (ext == NULL) {
        return NOERROR;
    }
    else {
        return ext->GetRawExtnValue(extensionValue);
    }
}

ECode CX509CRLEntryImpl::GetNonCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    VALIDATE_NOT_NULL(oids)
    if (mExtensions == NULL) {
        return NOERROR;
    }
    return mExtensions->GetNonCriticalExtensions(oids);
}

ECode CX509CRLEntryImpl::HasUnsupportedCriticalExtension(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mExtensions == NULL) {
        *result = FALSE;
    }
    return mExtensions->HasUnsupportedCritical(result);
}

ECode CX509CRLEntryImpl::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    return X509CRLEntry::Equals(other, result);
}

ECode CX509CRLEntryImpl::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return X509CRLEntry::GetHashCode(hashCode);
}

ECode CX509CRLEntryImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mEncoding == NULL) {
        mRcert->GetEncoded((ArrayOf<Byte>**)&mEncoding);
    }

    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mEncoding->GetLength());
    result->Copy(0, mEncoding, 0, mEncoding->GetLength());
    *encoded = result;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode CX509CRLEntryImpl::GetSerialNumber(
    /* [out] */ IBigInteger** number)
{
    return mRcert->GetUserCertificate(number);
}

ECode CX509CRLEntryImpl::GetCertificateIssuer(
    /* [out] */ IX500Principal** issuer)
{
    VALIDATE_NOT_NULL(issuer)
    *issuer = mIssuer;
    REFCOUNT_ADD(*issuer)
    return NOERROR;
}

ECode CX509CRLEntryImpl::GetRevocationDate(
    /* [out] */ IDate** revocationDate)
{
    return mRcert->GetRevocationDate(revocationDate);
}

ECode CX509CRLEntryImpl::HasExtensions(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size;
    mExtensions->GetSize(&size);
    *result = mExtensions != NULL && size != 0;
    return NOERROR;
}

ECode CX509CRLEntryImpl::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String tmp;
    mRcert->ToString(&tmp);
    *str = String("X509CRLEntryImpl: ") + tmp;
    return NOERROR;
}

ECode CX509CRLEntryImpl::constructor(
    /* [in] */ IRevokedCertificate* rcert,
    /* [in] */ IX500Principal* issuer)
{
    mRcert = rcert;
    rcert->GetCrlEntryExtensions((IExtensions**)&mExtensions);
    mIssuer = issuer;
}

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

