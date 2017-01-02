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

#include "org/apache/harmony/security/x509/CInfoAccessSyntax.h"
#include "org/apache/harmony/security/x509/CAccessDescription.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::IASN1Any;
using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CInfoAccessSyntax::MyASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);
    AutoPtr<IInfoAccessSyntax> tmp;
    return CInfoAccessSyntax::New(IList::Probe(con), array, (IInfoAccessSyntax**)&tmp);
}

ECode CInfoAccessSyntax::MyASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    CInfoAccessSyntax* ias = (CInfoAccessSyntax*)IInfoAccessSyntax::Probe(object);
    *values = ICollection::Probe(ias->mAccessDescriptions);
    REFCOUNT_ADD(*values);
    return NOERROR;
}

AutoPtr<IASN1Type> CInfoAccessSyntax::initASN1()
{
    AutoPtr<ASN1SequenceOf> tmp = new MyASN1SequenceOf();
    tmp->constructor(IASN1Type::Probe(CAccessDescription::ASN1));
    return IASN1Type::Probe(tmp);
}

AutoPtr<IASN1Type> CInfoAccessSyntax::ASN1 = initASN1();

CAR_OBJECT_IMPL(CInfoAccessSyntax)

CAR_INTERFACE_IMPL(CInfoAccessSyntax, ExtensionValue, IInfoAccessSyntax)

ECode CInfoAccessSyntax::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CInfoAccessSyntax::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("AccessDescriptions:\n"));

    Boolean res;
    if (mAccessDescriptions == NULL || (mAccessDescriptions->IsEmpty(&res), res)) {
        sb->Append(String("NULL\n"));
    }
    else {
        Int32 size;
        mAccessDescriptions->GetSize(&size);
        mAccessDescriptions->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> accessDescription;
            mAccessDescriptions->Get(i, (IInterface**)&accessDescription);
            String str;
            IObject::Probe(accessDescription)->ToString(&str);
            sb->Append(str);
        }
    }
    return NOERROR;
}

ECode CInfoAccessSyntax::ToString(
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    StringBuilder res;
    res += String("\n---- InfoAccessSyntax:");
    if (mAccessDescriptions != NULL) {
        Int32 size;
        mAccessDescriptions->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> accessDescription;
            mAccessDescriptions->Get(i, (IInterface**)&accessDescription);
            res += '\n';
            res += accessDescription;
        }
    }
    res += "\n---- InfoAccessSyntax END\n";
    return res.ToString(pStr);
}

ECode CInfoAccessSyntax::constructor(
    /* [in] */ IList* accessDescriptions,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    Boolean res;
    if (accessDescriptions == NULL || (accessDescriptions->IsEmpty(&res), res)) {
        //throw new IOException("AccessDescriptions list is null or empty");
        return E_IO_EXCEPTION;
    }
    mAccessDescriptions = accessDescriptions;
    mEncoding = encoding;
    return NOERROR;
}

ECode CInfoAccessSyntax::Decode(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ IInfoAccessSyntax** ppSyntax)
{
    VALIDATE_NOT_NULL(ppSyntax);

    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);
    *ppSyntax = IInfoAccessSyntax::Probe(obj);
    REFCOUNT_ADD(*ppSyntax);
    return NOERROR;
}

ECode CInfoAccessSyntax::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CInfoAccessSyntax::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org