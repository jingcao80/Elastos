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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_BERINPUTSTREAM_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_BERINPUTSTREAM_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.IO.h"
#include "coredef.h"
#include "core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class BerInputStream
    : public Object
    , public IBerInputStream
{
public:
    BerInputStream();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 expectedLength);

    CARAPI constructor(
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ Int32 initialSize);

    CAR_INTERFACE_DECL()

    CARAPI Reset(
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI Next(
        /* [out] */ Int32* next);

    static CARAPI GetLength(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [out] */ Int32* length);

    CARAPI ReadBitString();

    CARAPI ReadEnumerated();

    CARAPI ReadBoolean();

    CARAPI ReadGeneralizedTime();

    CARAPI ReadUTCTime();

    CARAPI ReadInteger();

    CARAPI ReadOctetString();

    CARAPI ReadOID();

    CARAPI ReadSequence(
        /* [in] */ IASN1Sequence* sequence);

    CARAPI ReadSequenceOf(
        /* [in] */ IASN1SequenceOf* sequenceOf);

    CARAPI ReadSet(
        /* [in] */ IASN1Set* set);

    CARAPI ReadSetOf(
        /* [in] */ IASN1SetOf* setOf);

    CARAPI ReadString(
        /* [in] */ IASN1StringType* type);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI GetBuffer(
        /* [out, callee] */ ArrayOf<Byte>** buffer);

    CARAPI GetLength(
        /* [out] */ Int32* length);

    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    CARAPI GetEndOffset(
        /* [out] */ Int32* endOffset);

    CARAPI GetTagOffset(
        /* [out] */ Int32* tagOffset);

    CARAPI SetVerify();

    CARAPI ReadContent();

    CARAPI CompactBuffer();

    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* entry);

    CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** entry);

//    CARAPI SetTag(
//        /* [in] */ Int32 tag);
//
//    CARAPI GetTag(
//        /* [out] */ Int32* tag);
//
//    CARAPI SetLength(
//        /* [in] */ Int32 length);
//
//    CARAPI SetContent(
//        /* [in] */ IInterface* content);
//
    CARAPI GetContent(
        /* [out] */ IInterface** content);

//    CARAPI GetTagOffSet(
//        /* [out] */ Int32* tagOffset);
//
//    CARAPI SetTagOffSet(
//        /* [in] */ Int32 tagOffset);
//
    CARAPI GetContentOffset(
        /* [out] */ Int32* contentOffset);

//    CARAPI SetContentOffset(
//        /* [in] */ Int32 contentOffset);
//
//    CARAPI SetChoiceIndex(
//        /* [in] */ Int32 choiceIndex);
//
//    CARAPI GetChoiceIndex(
//        /* [out] */ Int32* choiceIndex);
//
//    CARAPI SetTimes(
//        /* [in] */ ArrayOf<Int32>* times);
//
//    CARAPI GetTimes(
//        /* [out, callee] */ ArrayOf<Int32>** times);
//
//
//    CARAPI SetOidElement(
//        /* [in] */ Int32 oidElement);
//
//    CARAPI GetOidElement(
//        /* [out] */ Int32* oidElement);

    CARAPI GetVerify(
        /* [out] */ Boolean* isVerify);

protected:
    virtual CARAPI Read(
        /* [out] */ Int32* ret);

private:
//    CARAPI InnerConstructor(
//        /* [in] */ ArrayOf<Byte>* encoded,
//        /* [in] */ Int32 offset,
//        /* [in] */ Int32 expectedLength);
//
//    CARAPI InnerConstructorEx(
//        /* [in] */ IInputStream* is,
//        /* [in] */ Int32 initialSize);

    CARAPI StrToInt(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [out] */ Int32* ret);

    CARAPI Expected(
        /* [in] */ const String& what);

    CARAPI DecodeValueCollection(
        /* [in] */ IASN1ValueCollection* collection);

public:
    /** Current decoded tag */
    Int32 mTag;

    /** Current decoded content */
    AutoPtr<IInterface> mContent;

    /** The last choice index */
    Int32 mChoiceIndex;

    /** Keeps last decoded: year, month, day, hour, minute, second, millisecond */
    AutoPtr< ArrayOf<Int32> > mTimes;

    Int32 mOidElement;

    AutoPtr< ArrayOf<Byte> > mBuffer;

    /**
    * The position in the buffer.
    * Next read must place data into the buffer from this offset
     */
    Int32 mOffset;

    /** Indicates indefinite length of the current type */
    static const Int32 INDEFINIT_LENGTH;

    /** Current decoded length */
    Int32 mLength;

    /** Current decoded tag offset */
    Int32 mTagOffset;

    /** Current decoded content offset */
    Int32 mContentOffset;

    /**
     * Indicates verify or store mode.
     *
     * In store mode a decoded content is stored in a newly allocated
     * appropriate object. The <code>content</code> variable holds
     * a reference to the last created object.
     *
     * In verify mode a decoded content is not stored.
     */
    // FIXME it is used only for one case
    // decoding PCKS#8 Private Key Info notation
    // remove this option because it does decoding more complex
    Boolean mIsVerify;

    /**
     * Indicates defined or indefined reading mode for associated InputStream.
     *
     * This mode is defined by reading a length
     * for a first ASN.1 type from InputStream.
     */
    Boolean mIsIndefinedLength;

private:
    AutoPtr<IInputStream> mIn;

    /**
    * The buffer increment size.
    * Must be reasonable big to reallocate memory not to often.
    * Primary is used for decoding indefinite length encoding
     */
    static const Int32 BUF_INCREASE_SIZE;

    AutoPtr< ArrayOf< AutoPtr< ArrayOf<IInterface*> > > > mPool;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __CBERINPUTSTREAM_H__
