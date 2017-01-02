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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CDEROUTPUTSTREAM_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CDEROUTPUTSTREAM_H__

#include "_Org_Apache_Harmony_Security_Asn1_CDerOutputStream.h"
#include "BerOutputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CDerOutputStream)
    , public BerOutputStream
    , public IDerOutputStream
{
public:
    CDerOutputStream();

    CARAPI constructor(
        /* [in] */ IASN1Type* asn1,
        /* [in] */ IInterface* object);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI EncodeChoice(
        /* [in] */ IASN1Choice* choice);

    CARAPI EncodeExplicit(
        /* [in] */ IASN1Explicit* xplicit);

    CARAPI EncodeSequence(
        /* [in] */ IASN1Sequence* sequence);

    CARAPI EncodeSequenceOf(
        /* [in] */ IASN1SequenceOf* sequenceOf);

    CARAPI EncodeSetOf(
        /* [in] */ IASN1SetOf* setOf);

    CARAPI GetChoiceLength(
        /* [in] */ IASN1Choice* choice);

    CARAPI GetExplicitLength(
        /* [in] */ IASN1Explicit* sequence);

    CARAPI GetSequenceLength(
        /* [in] */ IASN1Sequence* sequence);

    CARAPI GetSequenceOfLength(
        /* [in] */ IASN1SequenceOf* sequence);

    CARAPI GetSetOfLength(
        /* [in] */ IASN1SetOf* setOf);

private:
    CARAPI EncodeValueCollection(
        /* [in] */ IASN1ValueCollection* collection);

    CARAPI_(void) Push(
        /* [in] */ ArrayOf<Int32>* lengths,
        /* [in] */ ArrayOf<IInterface*>* values);

    CARAPI_(void) GetValueOfLength(
        /* [in] */ IASN1ValueCollection* collection);

private:
    static const Int32 sInitSize;
    Int32 mIndex;
    AutoPtr< ArrayOf< AutoPtr< ArrayOf<Int32> > > > mLen;
    AutoPtr< ArrayOf< AutoPtr< ArrayOf<IInterface*> > > > mVal;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CDEROUTPUTSTREAM_H__
