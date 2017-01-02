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

#include "org/apache/harmony/security/x509/CX509PublicKey.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;
using Elastos::Security::EIID_IPublicKey;
using Elastos::Security::EIID_IKey;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CX509PublicKey)
CAR_INTERFACE_IMPL_4(CX509PublicKey, Object, IX509PublicKey, IPublicKey, IKey, ISerializable)

ECode CX509PublicKey::GetAlgorithm(
    /* [out] */ String* pAlgorithm)
{
    VALIDATE_NOT_NULL(pAlgorithm);
    *pAlgorithm = mAlgorithm;
    return NOERROR;
}

ECode CX509PublicKey::GetFormat(
    /* [out] */ String* pFormat)
{
    VALIDATE_NOT_NULL(pFormat);
    *pFormat = String("X.509");
    return NOERROR;
}

ECode CX509PublicKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);
    *ppEncoded = mEncoded;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CX509PublicKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder buf("algorithm = ");
    buf.Append(mAlgorithm);
    buf.Append(String(", params unparsed, unparsed keybits = \n"));
    // TODO: implement compatible toString method()
    // buf.append(Arrays.toString(keyBytes));
    return buf.ToString(result);
}

ECode CX509PublicKey::constructor(
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ ArrayOf<Byte>* keyBytes)
{
    mAlgorithm = algorithm;
    mEncoded = encoded;
    mKeyBytes = keyBytes;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org