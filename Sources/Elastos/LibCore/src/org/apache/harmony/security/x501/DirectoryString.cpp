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

#include "DirectoryString.h"
#include "ASN1StringType.h"

using Org::Apache::Harmony::Security::Asn1::ASN1StringType;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

DirectoryString::ASN1ChoiceCls::ASN1ChoiceCls()
{}

ECode DirectoryString::ASN1ChoiceCls::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* asn1Type)
{
    return ASN1Choice::constructor(asn1Type);
}

ECode DirectoryString::ASN1ChoiceCls::GetIndex(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = 1; // always code as ASN1 printableString
    //return 4; // always code as ASN1 utf8String
    return NOERROR;
}

ECode DirectoryString::ASN1ChoiceCls::GetObjectToEncode(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = /*(String)*/ object;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IASN1Choice> DirectoryString::Init()
{
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(5);
    array->Set(0, IASN1Type::Probe(ASN1StringType::TELETEXSTRING));
    array->Set(1, IASN1Type::Probe(ASN1StringType::PRINTABLESTRING));
    array->Set(2, IASN1Type::Probe(ASN1StringType::UNIVERSALSTRING));
    array->Set(3, IASN1Type::Probe(ASN1StringType::UTF8STRING));
    array->Set(4, IASN1Type::Probe(ASN1StringType::BMPSTRING));
    AutoPtr<ASN1ChoiceCls> ac = new ASN1ChoiceCls();
    ac->constructor(array);
    return ac;
}

INIT_PROI_4 AutoPtr<IASN1Choice> DirectoryString::ASN1 = Init();

}
}
}
}
}
