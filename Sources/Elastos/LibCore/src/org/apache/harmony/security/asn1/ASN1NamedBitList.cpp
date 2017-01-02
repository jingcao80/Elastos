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

#include "ASN1NamedBitList.h"
#include "CArrayOf.h"
#include "CBoolean.h"
#include "CBitString.h"
#include "CBerOutputStream.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CBoolean;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::IArrayOf;
using Elastos::Core::IBoolean;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

static AutoPtr<IInterface> GetBooleanArray(
    /* [in] */ ArrayOf<Boolean>* values)
{
    Int32 len = values->GetLength();
    AutoPtr<IArrayOf> arrObj;
    CArrayOf::New(EIID_IBoolean, len, (IArrayOf**)&arrObj);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IBoolean> b;
        CBoolean::New((*values)[i], (IBoolean**)&b);
        arrObj->Set(i, b);
    }
    return arrObj;
}

AutoPtr<ArrayOf<Byte> > ASN1NamedBitList::SET_MASK;
AutoPtr<IBitString> ASN1NamedBitList::EmptyString = InitStatic();
Int32 ASN1NamedBitList::INDEFINITE_SIZE = -1;
ASN1NamedBitList::ASN1NamedBitList()
    : mMinBits(0)
    , mMaxBits(0)
{}

AutoPtr<IBitString> ASN1NamedBitList::InitStatic()
{
    SET_MASK = ArrayOf<Byte>::Alloc(8);
    Byte mask[] = { 128, 64, 32, 16, 8, 4, 2, 1};
    for (Int32 i = 0; i < /*sizeof(mask)/sizeof(mask[0])*/8; i++) {
        (*SET_MASK)[i] = mask[i];
    }
    AutoPtr<CBitString> ret;
    AutoPtr<ArrayOf<Byte> > emptyByteArray = ArrayOf<Byte>::Alloc(0);
    CBitString::NewByFriend(emptyByteArray, 0, (CBitString**)&ret);
    return ret;
}

ECode ASN1NamedBitList::constructor(
    /* [in] */ Int32 minBits)
{
    mMinBits = minBits;
    mMaxBits = INDEFINITE_SIZE;
    return NOERROR;
}

ECode ASN1NamedBitList::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<ArrayOf<Boolean> > value;
    AutoPtr<ArrayOf<Byte> >buffer;
    Int32 contentOffset,length;
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    bis->GetContentOffset(&contentOffset);
    bis->GetLength(&length);
    Int32 unusedBits = (*buffer)[contentOffset];
    Int32 bitsNumber = (length - 1) * 8 - unusedBits;

    if (mMaxBits == INDEFINITE_SIZE) {
        if (mMinBits == INDEFINITE_SIZE) {
            value = ArrayOf<Boolean>::Alloc(bitsNumber);
        }
        else {
            if (bitsNumber > mMinBits) {
                value = ArrayOf<Boolean>::Alloc(bitsNumber);
            }
            else {
                value = ArrayOf<Boolean>::Alloc(mMinBits);
            }
        }
    }
    else {
        if (bitsNumber > mMaxBits) {
            return E_ASN1_EXCEPTION;
        }
        value = ArrayOf<Boolean>::Alloc(mMaxBits);
    }

    if (bitsNumber == 0) {
        // empty bit string
        AutoPtr<IInterface> arr = GetBooleanArray(value);
        *object = arr;
        REFCOUNT_ADD(*object)
        return NOERROR;
    }

    Int32 i = 1;
    Int32 j = 0;

    Byte octet = (*buffer)[contentOffset + 1];
    for (Int32 size = length - 1; i < size; i++) {
        for (Int32 k = 0; k < 8; k++, j++) {
            (*value)[j] = ((*SET_MASK)[k] & octet) != 0;
        }
        i++;
        octet = (*buffer)[contentOffset + i];
    }

    // final octet
    for (Int32 k = 0; k < (8 - unusedBits); k++, j++) {
        (*value)[j] = ((*SET_MASK)[k] & octet) != 0;
    }

    AutoPtr<IInterface> arr = GetBooleanArray(value);
    *object = arr;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ASN1NamedBitList::SetEncodingContent(
    /* [in] */ IBerOutputStream *bos)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(((CBerOutputStream*)bos)->mContent);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Boolean> > toEncode = ArrayOf<Boolean>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        array->Get(i, (IInterface**)&o);
        Boolean v;
        IBoolean::Probe(o)->GetValue(&v);
        (*toEncode)[i] = v;
    }

    Int32 index = toEncode->GetLength() - 1;
    while (index > -1 && !(*toEncode)[index]) {
        index--;
    }

    if (index == -1) {
        bos->SetContent(EmptyString.Get());
        bos->SetLength(1);
    } else {
        Int32 unusedBits = 7 - index % 8;
        AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(index / 8 + 1);

        Int32 j = 0;
        index = bytes->GetLength() - 1;
        for (Int32 i = 0; i < index; i++) {
            for (Int32 k = 0; k < 8; k++, j++) {
                if ((*toEncode)[j]) {
                    (*bytes)[i] = (Byte) ((*bytes)[i] | (*SET_MASK)[k]);
                }
            }
        }

        //final octet
        for (Int32 k = 0; k < (8 - unusedBits); k++, j++) {
            if ((*toEncode)[j]) {
                (*bytes)[index] = (Byte) ((*bytes)[index] | (*SET_MASK)[k]);
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

