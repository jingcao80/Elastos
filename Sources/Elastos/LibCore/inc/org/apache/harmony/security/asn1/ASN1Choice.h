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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CHOICE_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CHOICE_H__

#include "ASN1Type.h"

using Elastos::Core::IArrayOf;
using Elastos::Utility::INavigableMap;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1Choice
    : public ASN1Type
    , public IASN1Choice
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<IASN1Type*>* asn1Type);

    virtual CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [out] */ Boolean* checkTag);

    using ASN1Type::Decode;

    virtual CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    virtual CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI GetIndex(
        /* [in] */ IInterface* object,
        /* [out] */ Int32* index) = 0;

    virtual CARAPI GetObjectToEncode(
        /* [in] */ IInterface* object,
        /* [out] */ IInterface** obj) = 0;

    virtual CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI GetType(
        /* [out, callee] */ ArrayOf<IASN1Type*>** type);

private:
    CARAPI AddIdentifier(
        /* [in] */ INavigableMap* map,
        /* [in] */ Int32 identifier,
        /* [in] */ Int32 index);

public:
    AutoPtr<ArrayOf<IASN1Type*> > mType;

private:
    /**
     * identifiers table: [2][number of distinct identifiers]
     * identifiers[0]: stores identifiers (includes nested choices)
     * identifiers[1]: stores identifiers' indexes in array of types
     */
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Int32> > > > mIdentifiers;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __CASN1CHOICE_H__
