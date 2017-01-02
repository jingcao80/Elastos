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

#include "BerOutputStream.h"
#include "utility/logging/Logger.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::IBoolean;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

// using Elastos.Core.IBoolean;
// using Elastos.Core.IArrayOf;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

BerOutputStream::BerOutputStream()
    : mLength(0)
    , mOffset(0)
{}

CAR_INTERFACE_IMPL(BerOutputStream, Object, IBerOutputStream)

ECode BerOutputStream::EncodeTag(
    /* [in] */ Int32 tag)
{
    (*mEncoded)[mOffset++] = (Byte) tag; //FIXME long form?

    if (mLength > 127) { //long form
        Int32 eLen = mLength >> 8;
        Byte numOctets = 1;
        for (; eLen > 0; eLen = eLen >> 8) {
            numOctets++;
        }

        (*mEncoded)[mOffset] = (Byte) (numOctets | 0x80);
        mOffset++;

        eLen = mLength;
        Int32 numOffset = mOffset + numOctets - 1;
        for (Int32 i = 0; i < numOctets; i++, eLen = eLen >> 8) {
            (*mEncoded)[numOffset - i] = (Byte) eLen; //FIXME long value?
        }
        mOffset += numOctets;
    }
    else { //short form
        (*mEncoded)[mOffset++] = (Byte) mLength;
    }
    return NOERROR;
}

ECode BerOutputStream::EncodeANY()
{
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(mContent);
    if (IArrayOf::Probe(mContent) == NULL) {
        return E_ARRAY_STORE_EXCEPTION;
    }

    Int32 length;
    arrayObj->GetLength(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        arrayObj->Get(i, (IInterface**)&elem);
        if (IByte::Probe(elem) == NULL) {
            return E_ARRAY_STORE_EXCEPTION;
        }
        Byte val;
        IByte::Probe(elem)->GetValue(&val);
        (*mEncoded)[mOffset + i] = val;
    }
    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::EncodeBitString()
{
    //FIXME check encoding
    AutoPtr<IBitString> bStr = IBitString::Probe(mContent);
    Int32 unusedBits;
    bStr->GetUnusedBits(&unusedBits);
    (*mEncoded)[mOffset] = (Byte)unusedBits;
    AutoPtr< ArrayOf<Byte> > bytes;
    bStr->GetBytes((ArrayOf<Byte>**)&bytes);
    mEncoded->Copy(mOffset + 1, bytes, 0, mLength - 1);
    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::EncodeBoolean()
{
    if (IBoolean::Probe(mContent)) {
        (*mEncoded)[mOffset] = (Byte) 0xFF;
    }
    else {
        (*mEncoded)[mOffset] = 0x00;
    }
    mOffset++;
    return NOERROR;
}

ECode BerOutputStream::EncodeChoice(
    /* [in] */ IASN1Choice* choice)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::EncodeExplicit(
    /* [in] */ IASN1Explicit* xplicit)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::EncodeGeneralizedTime()
{
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(mContent);
    if (IArrayOf::Probe(mContent) == NULL) {
        return E_ARRAY_STORE_EXCEPTION;
    }

    Int32 length;
    arrayObj->GetLength(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        arrayObj->Get(i, (IInterface**)&elem);
        if (IByte::Probe(elem) == NULL) {
            return E_ARRAY_STORE_EXCEPTION;
        }
        Byte val;
        IByte::Probe(elem)->GetValue(&val);
        (*mEncoded)[mOffset + i] = val;
    }
    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::EncodeUTCTime()
{
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(mContent);
    if (IArrayOf::Probe(mContent) == NULL) {
        return E_ARRAY_STORE_EXCEPTION;
    }

    Int32 length;
    arrayObj->GetLength(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        arrayObj->Get(i, (IInterface**)&elem);
        if (IByte::Probe(elem) == NULL) {
            return E_ARRAY_STORE_EXCEPTION;
        }
        Byte val;
        IByte::Probe(elem)->GetValue(&val);
        (*mEncoded)[mOffset + i] = val;
    }
    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::EncodeInteger()
{
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(mContent);
    if (IArrayOf::Probe(mContent) == NULL) {
        return E_ARRAY_STORE_EXCEPTION;
    }

    Int32 length;
    arrayObj->GetLength(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        arrayObj->Get(i, (IInterface**)&elem);
        if (IByte::Probe(elem) == NULL) {
            return E_ARRAY_STORE_EXCEPTION;
        }
        Byte val;
        IByte::Probe(elem)->GetValue(&val);
        (*mEncoded)[mOffset + i] = val;
    }
    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::EncodeOctetString()
{
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(mContent);
    if (IArrayOf::Probe(mContent) == NULL) {
        return E_ARRAY_STORE_EXCEPTION;
    }

    Int32 length;
    arrayObj->GetLength(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        arrayObj->Get(i, (IInterface**)&elem);
        if (IByte::Probe(elem) == NULL) {
            return E_ARRAY_STORE_EXCEPTION;
        }
        Byte val;
        IByte::Probe(elem)->GetValue(&val);
        (*mEncoded)[mOffset + i] = val;
    }
    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::EncodeOID()
{
    Int32 length;
    IArrayOf::Probe(mContent)->GetLength(&length);
    AutoPtr<ArrayOf<Int32> > oid = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        IArrayOf::Probe(mContent)->Get(i, (IInterface**)&elem);
        Int32 val;
        IInteger32::Probe(elem)->GetValue(&val);
        (*oid)[i] = val;
    }

    Int32 oidLen = mLength;

    // all subidentifiers except first
    Int32 elem;
    for (Int32 i = oid->GetLength() - 1; i > 1; i--, oidLen--) {
        elem = (*oid)[i];
        if (elem > 127) {
            (*mEncoded)[mOffset + oidLen - 1] = (Byte) (elem & 0x7F);
            elem = elem >> 7;
            for (; elem > 0;) {
                oidLen--;
                (*mEncoded)[mOffset + oidLen - 1] = (Byte) (elem | 0x80);
                elem = elem >> 7;
            }
        }
        else {
            (*mEncoded)[mOffset + oidLen - 1] = (Byte) elem;
        }
    }

    // first subidentifier
    elem = (*oid)[0] * 40 + (*oid)[1];
    if (elem > 127) {
        (*mEncoded)[mOffset + oidLen - 1] = (Byte) (elem & 0x7F);
        elem = elem >> 7;
        for (; elem > 0;) {
            oidLen--;
            (*mEncoded)[mOffset + oidLen - 1] = (Byte) (elem | 0x80);
            elem = elem >> 7;
        }
    }
    else {
        (*mEncoded)[mOffset + oidLen - 1] = (Byte) elem;
    }

    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::EncodeSequence(
    /* [in] */ IASN1Sequence* sequence)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::EncodeSequenceOf(
    /* [in] */ IASN1SequenceOf* sequenceOf)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::EncodeSet(
    /* [in] */ IASN1Set* set)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::EncodeSetOf(
    /* [in] */ IASN1SetOf* setOf)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::EncodeString()
{
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(mContent);
    if (IArrayOf::Probe(mContent) == NULL) {
        return E_ARRAY_STORE_EXCEPTION;
    }

    Int32 length;
    arrayObj->GetLength(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        arrayObj->Get(i, (IInterface**)&elem);
        if (IByte::Probe(elem) == NULL) {
            return E_ARRAY_STORE_EXCEPTION;
        }
        Byte val;
        IByte::Probe(elem)->GetValue(&val);
        (*mEncoded)[mOffset + i] = val;
    }
    mOffset += mLength;
    return NOERROR;
}

ECode BerOutputStream::GetChoiceLength(
    /* [in] */ IASN1Choice* choice)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::GetExplicitLength(
    /* [in] */ IASN1Explicit* sequence)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::GetSequenceLength(
    /* [in] */ IASN1Sequence* sequence)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::GetSequenceOfLength(
    /* [in] */ IASN1SequenceOf* sequence)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::GetSetLength(
    /* [in] */ IASN1Set* set)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

ECode BerOutputStream::GetSetOfLength(
    /* [in] */ IASN1SetOf* setOf)
{
    Logger::E("BerOutputStream", "Is not implemented yet");
    return E_RUNTIME_EXCEPTION;
}

//ECode BerOutputStream::SetEncoded(
//    /* [in] */ ArrayOf<Byte>* encoded)
//{
//    mEncoded = encoded;
//    return NOERROR;
//}
//
ECode BerOutputStream::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    *encoded = mEncoded;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode BerOutputStream::SetLength(
    /* [in] */ Int32 length)
{
    mLength = length;
    return NOERROR;
}

ECode BerOutputStream::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mLength;
    return NOERROR;
}

ECode BerOutputStream::SetContent(
    /* [in] */ IInterface* content)
{
    mContent = content;
    return NOERROR;
}

ECode BerOutputStream::GetContent(
    /* [out] */ IInterface** content)
{
    VALIDATE_NOT_NULL(content)
    *content = mContent;
    REFCOUNT_ADD(*content)
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
