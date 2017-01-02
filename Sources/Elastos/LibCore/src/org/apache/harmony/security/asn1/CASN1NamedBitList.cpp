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

#include "CASN1NamedBitList.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

static AutoPtr<ArrayOf<Byte> > CASN1NamedBitList::sSET_MASK;
static AutoPtr<IBitString> sEmptyString = InitStatic();

CAR_OBJECT_IMPL(CASN1NamedBitList)

AutoPtr<IBitString> CASN1NamedBitList::InitStatic()
{
    sSET_MASK = ArrayOf<Byte>::Alloc(8);
    Byte mask[] = { 128, 64, 32, 16, 8, 4, 2, 1};
    for (Int32 i = 0; i < sizeof(mask)/sizeof(mask[0]); i++) {
        (*sSET_MASK)[i] = mask[i];
    }
    AutoPtr<CBitString> ret;
    AutoPtr<ArrayOf<Byte> > emptyByteArray = ArrayOf<Byte>::Alloc(0);
    CBitString::NewByFriend(emptyByteArray, 0, (CBitString**)&ret);
    return ret;
}

ECode CASN1NamedBitList::GetId(
    /* [out] */ Int32* id)
{
    return ASN1BitString::GetId(id);
}

ECode CASN1NamedBitList::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1BitString::GetConstrId(constrId);
}

ECode CASN1NamedBitList::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::Decode(encoded, object);
}

ECode CASN1NamedBitList::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1NamedBitList::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::DecodeEx2(is, object);
}

ECode CASN1NamedBitList::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1BitString::Verify(encoded);
}

ECode CASN1NamedBitList::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1BitString::VerifyEx(is);
}

ECode CASN1NamedBitList::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1BitString::Encode(object, encode);
}

ECode CASN1NamedBitList::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::DecodeEx3(bis, object);
}

ECode CASN1NamedBitList::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1BitString::CheckTag(identifier, checkTag);
}

ECode CASN1NamedBitList::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1BitString::EncodeASN(bos);
}

ECode CASN1NamedBitList::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1BitString::EncodeContent(bos);
}

ECode CASN1NamedBitList::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1BitString::GetEncodedLength(bos, length);
}

ECode CASN1NamedBitList::ToString(
    /* [out] */ String* result)
{
    return ASN1BitString::ToString(result);
}

ECode CASN1NamedBitList::constructor(
    /* [in] */ Int32 minBits)
{
    mMinBits = minBits;
    mMaxBits = INDEFINITE_SIZE;
    return NOERROR;
}

ECode CASN1NamedBitList::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out, callee] */ ArrayOf<Boolean>** object)
{
    AutoPtr<ArrayOf<Boolean> > value;
    AutoPtr<ArrayOf<Byte> >buffer;
    Int32 contentOffset,length;
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    bis->GetContentOffset(&contentOffset);
    bis->GetLength(&length);
    Int32 unusedBits = (*buffer)[contentOffset];
    Int32 bitsNumber = (length - 1) * 8 - unusedBits;

    if (mMaxBits == sINDEFINITE_SIZE) {
        if (mMinBits == sINDEFINITE_SIZE) {
            value = ArrayOf<Boolean>::Alloc(bitsNumber);
        } else {
            if (bitsNumber > mMinBits) {
                value = ArrayOf<Boolean>::Alloc(bitsNumber);
            } else {
                value = ArrayOf<Boolean>::Alloc(mMinBits);
            }
        }
    } else {
        if (bitsNumber > mMaxBits) {
            return E_ASN1_EXCEPTION;
        }
        value = ArrayOf<Boolean>::Alloc(mMaxBits);
    }

    if (bitsNumber == 0) {
        // empty bit string
        *object = value;
        REFCOUNT_ADD(*object)
        return NOERROR;
    }

    Int32 i = 1;
    Int32 j = 0;

    Byte octet = (*buffer)[contentOffset + 1];
    for (Int32 size = length - 1; i < size; i++) {
        for (Int32 k = 0; k < 8; k++, j++) {
            (*value)[j] = ((*sSET_MASK)[k] & octet) != 0;
        }
        i++;
        octet = (*buffer)[contentOffset + i];
    }

    // final octet
    for (Int32 k = 0; k < (8 - unusedBits); k++, j++) {
        (*value)[j] = ((*sSET_MASK)[k] & octet) != 0;
    }

    *object = value;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CASN1NamedBitList::SetEncodingContent(
    /* [in] */ IBerOutputStream *bos)
{
    AutoPtr<ArrayOf<Boolean> > toEncode;
    bos->GetContent((ArrayOf<Boolean>**)&toEncode);

    Int32 index = toEncode->GetLength() - 1;
    while (index > -1 && !(*toEncode)[index]) {
        index--;
    }

    if (index == -1) {
        bos->SetContent(sEmptyString.Get());
        bos->SetLength(1);
    } else {
        Int32 unusedBits = 7 - index % 8;
        AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(index / 8 + 1);

        Int32 j = 0;
        index = bytes->GetLength() - 1;
        for (Int32 i = 0; i < index; i++) {
            for (Int32 k = 0; k < 8; k++, j++) {
                if ((*toEncode)[j]) {
                    (*bytes)[i] = (Byte) ((*bytes)[i] | (*sSET_MASK)[k]);
                }
            }
        }

        //final octet
        for (Int32 k = 0; k < (8 - unusedBits); k++, j++) {
            if ((*toEncode)[j]) {
                (*bytes)[index] = (Byte) ((*bytes)[index] | (*sSET_MASK)[k]);
            }
        }

        AutoPtr<IBitString> bs;
        CBitString::New(bytes, unusedBits, (IBitString**)&bs);
        bos->SetContent(bs.Get());
        bos->SetLength(bytes->GetLength() + 1);
    }
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

