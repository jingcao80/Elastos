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

#include "CDerInputStream.h"
#include "utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

const AutoPtr<ArrayOf<Byte> > CDerInputStream::UNUSED_BITS_MASK = InitUNUSED_BITS_MASK();

AutoPtr<ArrayOf<Byte> > CDerInputStream::InitUNUSED_BITS_MASK()
{
    AutoPtr< ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(7);
    (*byteArray)[0] = 0x01;
    (*byteArray)[1] = 0x03;
    (*byteArray)[2] = 0x07;
    (*byteArray)[3] = 0x0F;
    (*byteArray)[4] = 0x1F;
    (*byteArray)[5] = 0x3F;
    (*byteArray)[6] = 0x7F;
    return byteArray;
}

ECode CDerInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return BerInputStream::constructor(encoded, 0, encoded->GetLength());
}

ECode CDerInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen)
{
    return BerInputStream::constructor(encoded, offset, encodingLen);
}

ECode CDerInputStream::constructor(
    /* [in] */ IInputStream* is)
{
    return BerInputStream::constructor(is);
}

CAR_INTERFACE_IMPL(CDerInputStream, BerInputStream, IDerInputStream)

CAR_OBJECT_IMPL(CDerInputStream)

ECode CDerInputStream::Next(
    /* [out] */ Int32* next)
{
    VALIDATE_NOT_NULL(next)
    *next = 0;

    Int32 tag;
    FAIL_RETURN(BerInputStream::Next(&tag));

    if (mLength == INDEFINIT_LENGTH) {
        Logger::E("CDerInputStream", "DER: only definite length encoding MUST be used");
        return E_ASN1_EXCEPTION;
    }

    // FIXME add check: length encoding uses minimum number of octets
    *next = tag;
    return NOERROR;
}

ECode CDerInputStream::ReadBitString()
{
    if (mTag == IASN1Constants::TAG_C_BITSTRING) {
        Logger::E("CDerInputStream", "ASN.1 bitstring: constructed identifier at [%d]. Not valid for DER.", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    FAIL_RETURN(BerInputStream::ReadBitString());

    //check: unused bits values - MUST be 0
    if (mLength > 1
            && (*mBuffer)[mContentOffset] != 0
            && ((*mBuffer)[mOffset - 1] & (*UNUSED_BITS_MASK)[(*mBuffer)[mContentOffset] - 1]) != 0) {
        Logger::E("CDerInputStream", "ASN.1 bitstring: wrong content at [%d]. "
                "DER requires zero unused bits in final octet.", mContentOffset);
        return E_ASN1_EXCEPTION;
    }
    return NOERROR;
}

ECode CDerInputStream::ReadBoolean()
{
    FAIL_RETURN(BerInputStream::ReadBoolean());

    // check encoded content
    if ((*mBuffer)[mContentOffset] != 0 && (*mBuffer)[mContentOffset] != (Byte) 0xFF) {
        Logger::E("CDerInputStream", "ASN.1 boolean: wrong content at [%d]. "
                "DER allows only 0x00 or 0xFF values", mContentOffset);
        return E_ASN1_EXCEPTION;
    }
    return NOERROR;
}

ECode CDerInputStream::ReadOctetString()
{
    if (mTag == IASN1Constants::TAG_C_OCTETSTRING) {
        Logger::E("CDerInputStream", "ASN.1 octetstring: constructed identifier at [%d]. "
                "Not valid for DER.", mTagOffset);
        return E_ASN1_EXCEPTION;
    }
    return BerInputStream::ReadOctetString();
}

ECode CDerInputStream::ReadSequence(
    /* [in] */ IASN1Sequence* sequence)
{
    //
    // According to ASN.1 DER spec. sequence MUST not include
    // any encoding which value is equal to its default value
    //
    // Verification of this assertion is not implemented
    //
    return BerInputStream::ReadSequence(sequence);
}

ECode CDerInputStream::ReadSetOf(
    /* [in] */ IASN1SetOf* setOf)
{
    //
    // According to ASN.1 DER spec. set of MUST appear in
    // ascending order (short component are padded for comparison)
    //
    // Verification of this assertion is not implemented
    //
    return BerInputStream::ReadSetOf(setOf);
}

ECode CDerInputStream::ReadString(
    /* [in] */ IASN1StringType* type)
{
    Int32 id;
    IASN1Type::Probe(type)->GetConstrId(&id);
    if (mTag == id) {
        Logger::E("CDerInputStream", "ASN.1 string: constructed identifier at [%d]. Not valid for DER.", mTagOffset);
        return E_ASN1_EXCEPTION;
    }
    return BerInputStream::ReadString(type);
}

ECode CDerInputStream::ReadUTCTime()
{
    if (mTag == IASN1Constants::TAG_C_UTCTIME) {
        // It is a string type and it can be encoded as primitive or constructed.
        Logger::E("CDerInputStream", "ASN.1 UTCTime: constructed identifier at [%d]. Not valid for DER.", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    // check format: DER uses YYMMDDHHMMSS'Z' only
    if (mLength != IASN1UTCTime::UTC_HMS) {
        Logger::E("CDerInputStream", "ASN.1 UTCTime: wrong format for DER, identifier at [%d]", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    return BerInputStream::ReadUTCTime();
}

ECode CDerInputStream::ReadGeneralizedTime()
{
    if (mTag == IASN1Constants::TAG_C_GENERALIZEDTIME) {
        // It is a string type and it can be encoded as primitive or constructed.
        Logger::E("CDerInputStream", "ASN.1 GeneralizedTime: constructed identifier at [%d]. Not valid for DER.", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    return BerInputStream::ReadGeneralizedTime();
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
