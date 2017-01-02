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

#include "org/apache/harmony/security/x509/CGeneralNames.h"
#include "org/apache/harmony/security/x509/CGeneralName.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/utility/CArrayList.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CGeneralNames::MyASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);
    AutoPtr<IGeneralNames> names;
    CGeneralNames::New(IList::Probe(con), array, (IGeneralNames**)&names);
    *object = TO_IINTERFACE(names);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CGeneralNames::MyASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    CGeneralNames* gns = (CGeneralNames*)IGeneralNames::Probe(object);
    *values = ICollection::Probe(gns->mGeneralNames);
    REFCOUNT_ADD(*values);
    return NOERROR;
}

AutoPtr<IASN1Type> CGeneralNames::initASN1()
{
    AutoPtr<ASN1SequenceOf> tmp = new ASN1SequenceOf();
    tmp->constructor(IASN1Type::Probe(CGeneralName::ASN1));
    return tmp;
}

INIT_PROI_7 AutoPtr<IASN1Type> CGeneralNames::ASN1 = initASN1();

CAR_OBJECT_IMPL(CGeneralNames)

CAR_INTERFACE_IMPL(CGeneralNames, Object, IGeneralNames)

ECode CGeneralNames::GetNames(
    /* [out] */ IList** ppNames)
{
    VALIDATE_NOT_NULL(ppNames);

    Int32 size;
    if ((mGeneralNames == NULL) || ((mGeneralNames->GetSize(&size), size) == 0)) {
        *ppNames = NULL;
        return NOERROR;
    }
    return CArrayList::New(ICollection::Probe(mGeneralNames), ppNames);
}

ECode CGeneralNames::GetPairsList(
    /* [out] */ ICollection** ppPairslist)
{
    VALIDATE_NOT_NULL(ppPairslist);

    AutoPtr<ICollection> result;
    CArrayList::New((ICollection**)&result);
    if (mGeneralNames == NULL) {
        *ppPairslist = result;
        REFCOUNT_ADD(*ppPairslist);
        return NOERROR;
    }

    Int32 size;
    mGeneralNames->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mGeneralNames->Get(i, (IInterface**)&obj);
        AutoPtr<IGeneralName> generalName = IGeneralName::Probe(obj);

        /*
         * If we have an error decoding one of the GeneralNames, we'll just
         * omit it from the final list.
         */
        AutoPtr<IList> genNameList;
        //try {
        ECode ec = generalName->GetAsList((IList**)&genNameList);
        //} catch (IllegalArgumentException ignored) {
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            continue;
        }
        //}

        result->Add(TO_IINTERFACE(genNameList));
    }
    *ppPairslist = result;
    REFCOUNT_ADD(*ppPairslist);
    return NOERROR;
}

ECode CGeneralNames::AddName(
    /* [in] */ IGeneralName* name)
{
    mEncoding = NULL;
    if (mGeneralNames == NULL) {
        CArrayList::New((IList**)&mGeneralNames);
    }
    mGeneralNames->Add(TO_IINTERFACE(name));
    return NOERROR;
}

ECode CGeneralNames::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CGeneralNames::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    if (mGeneralNames == NULL) {
        return NOERROR;
    }

    Int32 size;
    mGeneralNames->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mGeneralNames->Get(i, (IInterface**)&obj);
        AutoPtr<IGeneralName> generalName = IGeneralName::Probe(obj);

        sb->Append(prefix);
        sb->Append(generalName);
        sb->Append('\n');

    }
    return NOERROR;
}

ECode CGeneralNames::constructor()
{
    return CArrayList::New((IList**)&mGeneralNames);
}

ECode CGeneralNames::constructor(
    /* [in] */ IList* generalNames)
{
    mGeneralNames = generalNames;
    return NOERROR;
}

ECode CGeneralNames::constructor(
    /* [in] */ IList* generalNames,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mGeneralNames = generalNames;
    mEncoding = encoding;
    return NOERROR;
}

ECode CGeneralNames::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CGeneralNames::SetASN1(
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