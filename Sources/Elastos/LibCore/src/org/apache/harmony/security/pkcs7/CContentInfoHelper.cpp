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

#include "coredef.h"
#include "CContentInfoHelper.h"
#include "ContentInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CAR_INTERFACE_IMPL(CContentInfoHelper, Singleton, IContentInfoHelper)

CAR_SINGLETON_IMPL(CContentInfoHelper)

ECode CContentInfoHelper::GetData(
    /* [out, callee] */ ArrayOf<Int32>** data)
{
    VALIDATE_NOT_NULL(data)
    *data = ContentInfo::DATA;
    REFCOUNT_ADD(*data)
    return NOERROR;
}

ECode CContentInfoHelper::GetSignedData(
    /* [out, callee] */ ArrayOf<Int32>** signedData)
{
    VALIDATE_NOT_NULL(signedData)
    *signedData = ContentInfo::SIGNED_DATA;
    REFCOUNT_ADD(*signedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetEnvelopedData(
    /* [out, callee] */ ArrayOf<Int32>** envelopedData)
{
    VALIDATE_NOT_NULL(envelopedData)
    *envelopedData = ContentInfo::ENVELOPED_DATA;
    REFCOUNT_ADD(*envelopedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetSignedAndEnvelopedData(
    /* [out, callee] */ ArrayOf<Int32>** signedAndEnvelopedData)
{
    VALIDATE_NOT_NULL(signedAndEnvelopedData)
    *signedAndEnvelopedData = ContentInfo::SIGNED_AND_ENVELOPED_DATA;
    REFCOUNT_ADD(*signedAndEnvelopedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetDigestedData(
    /* [out, callee] */ ArrayOf<Int32>** digestedData)
{
    VALIDATE_NOT_NULL(digestedData)
    *digestedData = ContentInfo::DIGESTED_DATA;
    REFCOUNT_ADD(*digestedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetEncryptedData(
    /* [out, callee] */ ArrayOf<Int32>** encryptedData)
{
    VALIDATE_NOT_NULL(encryptedData)
    *encryptedData = ContentInfo::ENCRYPTED_DATA;
    REFCOUNT_ADD(*encryptedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** asn1)
{
    VALIDATE_NOT_NULL(asn1)
    *asn1 = ContentInfo::ASN1;
    REFCOUNT_ADD(*asn1)
    return NOERROR;
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
