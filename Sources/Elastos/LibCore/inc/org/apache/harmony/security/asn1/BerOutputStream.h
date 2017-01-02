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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_BEROUTPUTSTREAM_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_BEROUTPUTSTREAM_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "coredef.h"
#include "core/Object.h"

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class BerOutputStream
    : public Object
    , public IBerOutputStream
{
public:
    BerOutputStream();

    CAR_INTERFACE_DECL()

    CARAPI EncodeTag(
        /* [in] */ Int32 tag);

    CARAPI EncodeANY();

    CARAPI EncodeBitString();

    CARAPI EncodeBoolean();

    CARAPI EncodeChoice(
        /* [in] */ IASN1Choice* choice);

    CARAPI EncodeExplicit(
        /* [in] */ IASN1Explicit* xplicit);

    CARAPI EncodeGeneralizedTime();

    CARAPI EncodeUTCTime();

    CARAPI EncodeInteger();

    CARAPI EncodeOctetString();

    CARAPI EncodeOID();

    CARAPI EncodeSequence(
        /* [in] */ IASN1Sequence* sequence);

    CARAPI EncodeSequenceOf(
        /* [in] */ IASN1SequenceOf* sequenceOf);

    CARAPI EncodeSet(
        /* [in] */ IASN1Set* set);

    CARAPI EncodeSetOf(
        /* [in] */ IASN1SetOf* setOf);

    CARAPI EncodeString();

    CARAPI GetChoiceLength(
        /* [in] */ IASN1Choice* choice);

    CARAPI GetExplicitLength(
        /* [in] */ IASN1Explicit* sequence);

    CARAPI GetSequenceLength(
        /* [in] */ IASN1Sequence* sequence);

    CARAPI GetSequenceOfLength(
        /* [in] */ IASN1SequenceOf* sequence);

    CARAPI GetSetLength(
        /* [in] */ IASN1Set* set);

    CARAPI GetSetOfLength(
        /* [in] */ IASN1SetOf* setOf);

//    CARAPI SetEncoded(
//        /* [in] */ ArrayOf<Byte>* encoded);
//
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI SetLength(
        /* [in] */ Int32 length);

    CARAPI GetLength(
        /* [out] */ Int32* length);

    CARAPI SetContent(
        /* [in] */ IInterface* content);

    CARAPI GetContent(
        /* [out] */ IInterface** content);

public:
    /** Encoded byte array */
    AutoPtr<ArrayOf<Byte> > mEncoded;

    /** Current encoded length */
    Int32 mLength;

    /** Current encoded content */
    AutoPtr<IInterface> mContent;

protected:
    /** current offset */
    Int32 mOffset;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_BEROUTPUTSTREAM_H__
