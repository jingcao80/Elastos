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

#include "CKeyStoreTrustedCertificateEntry.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStoreTrustedCertificateEntry)
CAR_INTERFACE_IMPL_2(CKeyStoreTrustedCertificateEntry, Object, IKeyStoreTrustedCertificateEntry, IKeyStoreEntry)
ECode CKeyStoreTrustedCertificateEntry::GetTrustedCertificate(
    /* [out] */ Elastos::Security::Cert::ICertificate **cert)
{
    VALIDATE_NOT_NULL(cert)
    *cert = mTrustCertificate;
    REFCOUNT_ADD(*cert)
    return NOERROR;
}

ECode CKeyStoreTrustedCertificateEntry::ToString(
    /* [out] */ String *str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("Trusted certificate entry:\n");
    String cert;
    IObject::Probe(mTrustCertificate)->ToString(&cert);
    (*str) += cert;
    return NOERROR;
}

ECode CKeyStoreTrustedCertificateEntry::constructor(
    /* [in] */ Elastos::Security::Cert::ICertificate *trustCertificate)
{
    if (trustCertificate == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mTrustCertificate = trustCertificate;
    return NOERROR;
}

}
}
