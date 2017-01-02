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

#include "org/apache/harmony/security/x509/CAlternativeName.h"
#include "org/apache/harmony/security/x509/CGeneralNames.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CAlternativeName)

CAR_INTERFACE_IMPL(CAlternativeName, ExtensionValue, IAlternativeName)

ECode CAlternativeName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        CGeneralNames::ASN1->Encode(mAlternativeNames, (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CAlternativeName::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append((mWhich) ? String("Subject") : String("Issuer"));
    sb->Append(String(" Alternative Names [\n"));
    mAlternativeNames->DumpValue(sb, prefix + String("  "));
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CAlternativeName::constructor(
    /* [in] */ Boolean which,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);
    mWhich = which;

    AutoPtr<IInterface> obj;
    CGeneralNames::ASN1->Decode(encoding, (IInterface**)&obj);
    mAlternativeNames = IGeneralNames::Probe(obj);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org