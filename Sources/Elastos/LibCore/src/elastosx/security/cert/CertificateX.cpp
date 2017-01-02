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

#include "CertificateX.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(Certificate, Object, ICertificate)

ECode Certificate::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    *isEqual = FALSE;
    VALIDATE_NOT_NULL(obj)

    if (obj->Probe(EIID_ICertificate) == this->Probe(EIID_ICertificate)) {
        *isEqual = TRUE;
        return NOERROR;
    }
    if (NULL == obj->Probe(EIID_ICertificate)) {
        return NOERROR;
    }
    AutoPtr<ICertificate> object = (ICertificate*) obj->Probe(EIID_ICertificate);
    AutoPtr<ArrayOf<Byte> > encode, oEncode;
    ECode ec;
    FAIL_GOTO(ec = GetEncoded((ArrayOf<Byte>**)&encode), ERROR_PROCESS)
    FAIL_GOTO(ec =((ICertificate*) obj->Probe(EIID_ICertificate))->GetEncoded((ArrayOf<Byte>**)&oEncode), ERROR_PROCESS)
    *isEqual = Arrays::Equals(encode, oEncode);
    return NOERROR;
ERROR_PROCESS:
    if ((ECode)E_CERTIFICATE_ENCODING_EXCEPTION == ec) {
        *isEqual = FALSE;
        return NOERROR;
    }
    return ec;
}

ECode Certificate::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 0;

    Int32 res = 0;
    AutoPtr<ArrayOf<Byte> > encode;
    ECode ec;
    FAIL_GOTO(ec = GetEncoded((ArrayOf<Byte>**)&encode), ERROR_PROCESS)
    for (Int32 i = 0; i < encode->GetLength(); ++i) {
        res += (*encode)[i];
    }
ERROR_PROCESS:
    if ((ECode)E_CERTIFICATE_ENCODING_EXCEPTION == ec) {
        ;
    }
    else {
        FAIL_RETURN(ec)
    }
    *hashCode = res;
    return ec;
}

} // end Cert
} // end Security
} // end Elastosx