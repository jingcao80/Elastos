
#include "BerInputStream.h"
#include "core/CArrayOf.h"
#include "utility/CArrayList.h"
#include "utility/logging/Logger.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

const Int32 BerInputStream::INDEFINIT_LENGTH = -1;

const Int32 BerInputStream::BUF_INCREASE_SIZE = 1024 * 16;

BerInputStream::BerInputStream()
    : mTag(0)
    , mChoiceIndex(0)
    , mOidElement(0)
    , mOffset(0)
    , mLength(0)
    , mTagOffset(0)
    , mContentOffset(0)
    , mIsVerify(FALSE)
    , mIsIndefinedLength(FALSE)
{}

ECode BerInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return constructor(encoded, 0, encoded->GetLength());
}

ECode BerInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 expectedLength)
{
    mIn = NULL;
    mBuffer = encoded;
    mOffset = offset;

    Int32 value;
    FAIL_RETURN(Next(&value));

    // compare expected and decoded length
    if (mLength != INDEFINIT_LENGTH
            && (offset + expectedLength) != (mOffset + mLength)) {
        Logger::E("BerInputStream", "Wrong content length");
        return E_ASN1_EXCEPTION;
    }
    return NOERROR;
}

ECode BerInputStream::constructor(
    /* [in] */ IInputStream* is)
{
    return constructor(is, BUF_INCREASE_SIZE);
}

ECode BerInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ Int32 initialSize)
{
    mIn = is;
    mBuffer = ArrayOf<Byte>::Alloc(initialSize);

    Int32 value;
    FAIL_RETURN(Next(&value));

    if (mLength != INDEFINIT_LENGTH) {
        // input stream has definite length encoding
        // check allocated length to avoid further reallocations
        if (mBuffer->GetLength() < (mLength + mOffset)) {
            AutoPtr<ArrayOf<Byte> > newBuffer = ArrayOf<Byte>::Alloc(mLength + mOffset);
            newBuffer->Copy(0, mBuffer, 0, mOffset);
            mBuffer = newBuffer;
        }
    }
    else {
        mIsIndefinedLength = TRUE;
        Logger::E("BerInputStream", "Decoding indefinite length encoding is not supported");
        return E_ASN1_EXCEPTION;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(BerInputStream, Object, IBerInputStream)

ECode BerInputStream::Reset(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    mBuffer = encoded;
    Int32 ret;
    return Next(&ret);
}

ECode BerInputStream::Next(
    /* [out] */ Int32* next)
{
    VALIDATE_NOT_NULL(next)
    mTagOffset = mOffset;

    // read tag
    FAIL_RETURN(Read(&mTag));

    // read length
    FAIL_RETURN(Read(&mLength));
    if (mLength != 0x80) { // definite form
        // long or short length form
        if ((mLength & 0x80) != 0) { // long form
            Int32 numOctets = mLength & 0x7F;

            if (numOctets > 5) {
                Logger::E("BerInputStream", "Too long encoding at [%d]", mTagOffset);
                return E_ASN1_EXCEPTION;
            }

            // collect this value length
            FAIL_RETURN(Read(&mLength));
            for (Int32 i = 1; i < numOctets; i++) {
                Int32 ch;
                FAIL_RETURN(Read(&ch));
                mLength = (mLength << 8) + ch;//read();
            }

            if (mLength > 0xFFFFFF) {
                Logger::E("BerInputStream", "Too long encoding at [%d]", mTagOffset);
                return E_ASN1_EXCEPTION;
            }
        }
    }
    else { //indefinite form
        mLength = INDEFINIT_LENGTH;
    }
    mContentOffset = mOffset;

    *next = mTag;
    return NOERROR;
}

ECode BerInputStream::GetLength(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 length = (*encoding)[1] & 0xFF;
    Int32 numOctets = 0;
    if ((length & 0x80) != 0) { // long form
        numOctets = length & 0x7F;

        // collect this value length
        length = (*encoding)[2] & 0xFF;
        for (Int32 i = 3; i < numOctets + 2; i++) {
            length = (length << 8) + ((*encoding)[i] & 0xFF);
        }
    }
    //    tag length long_form content
    *ret = 1 + 1 + numOctets + length;
    return NOERROR;
}

ECode BerInputStream::ReadBitString()
{
    if (mTag == IASN1Constants::TAG_BITSTRING) {

        if (mLength == 0) {
            Logger::E("BerInputStream", "ASN.1 Bitstring: wrong length. Tag at [%d]", mTagOffset);
            return E_ASN1_EXCEPTION;
        }

        FAIL_RETURN(ReadContent());

        // content: check unused bits
        if ((*mBuffer)[mContentOffset] > 7) {
            Logger::E("BerInputStream", "ASN.1 Bitstring: wrong content at [%d]. "
                    "A number of unused bits MUST be in range 0 to 7", mContentOffset);
            return E_ASN1_EXCEPTION;
        }

        if (mLength == 1 && (*mBuffer)[mContentOffset] != 0) {
            Logger::E("BerInputStream", "ASN.1 Bitstring: wrong content at [%d]. "
                    "For empty string unused bits MUST be 0", mContentOffset);
            return E_ASN1_EXCEPTION;
        }

    } else if (mTag == IASN1Constants::TAG_C_BITSTRING) {
        Logger::E("BerInputStream", "Decoding constructed ASN.1 bitstring  type is not provided");
        return E_ASN1_EXCEPTION;
    }
    else {
        return Expected(String("bitstring"));
    }
    return NOERROR;
}

ECode BerInputStream::ReadEnumerated()
{
    if (mTag != IASN1Constants::TAG_ENUM) {
        return Expected(String("enumerated"));
    }

    // check encoded length
    if (mLength == 0) {
        Logger::E("BerInputStream", "ASN.1 enumerated: wrong length for identifier at [%d]", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    FAIL_RETURN(ReadContent());

    // check encoded content
    if (mLength > 1) {
        Int32 bits = (*mBuffer)[mContentOffset] & 0xFF;
        if ((*mBuffer)[mContentOffset + 1] < 0) {
            bits += 0x100;
        }

        if (bits == 0 || bits == 0x1FF) {
            Logger::E("BerInputStream", "ASN.1 enumerated: wrong content at [%d]. "
                    "An integer MUST be encoded in minimum number of octets", mContentOffset);
            return E_ASN1_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode BerInputStream::ReadBoolean()
{
    if (mTag != IASN1Constants::TAG_BOOLEAN) {
        return Expected(String("Boolean"));
    }

    // check encoded length
    if (mLength != 1) {
        Logger::E("BerInputStream", "Wrong length for ASN.1 boolean at [%d]", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    return ReadContent();
}

ECode BerInputStream::ReadGeneralizedTime()
{
    if (mTag == IASN1Constants::TAG_GENERALIZEDTIME) {
        // FIXME: any other optimizations?
        FAIL_RETURN(ReadContent());
        // FIXME store string somewhere to allow a custom time type perform
        // additional checks

        // check syntax: the last char MUST be Z
        if ((*mBuffer)[mOffset - 1] != 'Z') {
            // FIXME support only format that is acceptable for DER
            Logger::E("BerInputStream", "ASN.1 GeneralizedTime: encoded format is not implemented");
            return E_ASN1_EXCEPTION;
        }

        // check syntax: MUST be YYYYMMDDHHMMSS[(./,)DDD]'Z'
        if (mLength != 15 && (mLength < 17 || mLength > 19)) {
            Logger::E("BerInputStream", "ASN.1 GeneralizedTime wrongly encoded at [%d]", mContentOffset);
            return E_ASN1_EXCEPTION;
        }

        // check content: milliseconds
        if (mLength > 16) {
            Byte char14 = (*mBuffer)[mContentOffset + 14];
            if (char14 != '.' && char14 != ',') {
                Logger::E("BerInputStream", "ASN.1 GeneralizedTime wrongly encoded at [%d]", mContentOffset);
                return E_ASN1_EXCEPTION;
            }
        }

        if (mTimes == NULL) {
            mTimes = ArrayOf<Int32>::Alloc(7);
        }
        Int32 value;
        FAIL_RETURN(StrToInt(mContentOffset, 4, &value));
        (*mTimes)[0] = value; // year
        FAIL_RETURN(StrToInt(mContentOffset + 4, 2, &value));
        (*mTimes)[1] = value; // month
        FAIL_RETURN(StrToInt(mContentOffset + 6, 2, &value));
        (*mTimes)[2] = value; // day
        FAIL_RETURN(StrToInt(mContentOffset + 8, 2, &value));
        (*mTimes)[3] = value; // hour
        FAIL_RETURN(StrToInt(mContentOffset + 10, 2, &value));
        (*mTimes)[4] = value; // minute
        FAIL_RETURN(StrToInt(mContentOffset + 12, 2, &value));
        (*mTimes)[5] = value; // second

        if (mLength > 16) {
            // FIXME optimize me
            FAIL_RETURN(StrToInt(mContentOffset + 15, mLength - 16, &value));
            (*mTimes)[6] = value;

            if (mLength == 17) {
                (*mTimes)[6] = (*mTimes)[6] * 100;
            }
            else if (mLength == 18) {
                (*mTimes)[6] = (*mTimes)[6] * 10;
            }
        }
        // FIXME check all values for valid numbers!!!
    }
    else if (mTag == IASN1Constants::TAG_C_GENERALIZEDTIME) {
        Logger::E("BerInputStream", "Decoding constructed ASN.1 GeneralizedTime type is not supported");
        return E_ASN1_EXCEPTION;
    }
    else {
        return Expected(String("GeneralizedTime"));
    }
    return NOERROR;
}

ECode BerInputStream::ReadUTCTime()
{
    if (mTag == IASN1Constants::TAG_UTCTIME) {
        switch (mLength) {
        case IASN1UTCTime::UTC_HM:
        case IASN1UTCTime::UTC_HMS:
            break;
        case IASN1UTCTime::UTC_LOCAL_HM:
        case IASN1UTCTime::UTC_LOCAL_HMS:
            // FIXME only coordinated universal time formats are supported
            Logger::E("BerInputStream", "ASN.1 UTCTime: local time format is not supported");
            return E_ASN1_EXCEPTION;
        default:
            Logger::E("BerInputStream", "ASN.1 UTCTime: wrong length, identifier at %d", mTagOffset);
            return E_ASN1_EXCEPTION;
        }

        // FIXME: any other optimizations?
        FAIL_RETURN(ReadContent());

        // FIXME store string somewhere to allow a custom time type perform
        // additional checks

        // check syntax: the last char MUST be Z
        if ((*mBuffer)[mOffset - 1] != 'Z') {
            Logger::E("BerInputStream", "ASN.1 UTCTime wrongly encoded at [%d]", mContentOffset);
            return E_ASN1_EXCEPTION;
        }

        if (mTimes == NULL) {
            mTimes = ArrayOf<Int32>::Alloc(7);
        }

        Int32 value;
        FAIL_RETURN(StrToInt(mContentOffset, 2, &value));
        (*mTimes)[0] = value; // year
        if ((*mTimes)[0] > 49) {
            (*mTimes)[0] += 1900;
        }
        else {
            (*mTimes)[0] += 2000;
        }

        FAIL_RETURN(StrToInt(mContentOffset + 2, 2, &value));
        (*mTimes)[1] = value; // month
        FAIL_RETURN(StrToInt(mContentOffset + 4, 2, &value));
        (*mTimes)[2] = value; // day
        FAIL_RETURN(StrToInt(mContentOffset + 6, 2, &value));
        (*mTimes)[3] = value; // hour
        FAIL_RETURN(StrToInt(mContentOffset + 8, 2, &value));
        (*mTimes)[4] = value; // minute

        if (mLength == IASN1UTCTime::UTC_HMS) {
            FAIL_RETURN(StrToInt(mContentOffset + 10, 2, &value));
            (*mTimes)[5] = value; // second
        }

        // FIXME check all time values for valid numbers!!!
    }
    else if (mTag == IASN1Constants::TAG_C_UTCTIME) {
        Logger::E("BerInputStream", "Decoding constructed ASN.1 UTCTime type is not supported");
        return E_ASN1_EXCEPTION;
    }
    else {
        return Expected(String("UTCTime"));
    }
    return NOERROR;
}

ECode BerInputStream::StrToInt(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 result = 0;
    for (Int32 i = off, end = off + count; i < end; i++) {
        Int32 c = (*mBuffer)[i] - 48;
        if (c < 0 || c > 9) {
            Logger::E("BerInputStream", "Time encoding has invalid char");
            return E_ASN1_EXCEPTION;
        }
        result = result * 10 + c;
    }
    *ret = result;
    return NOERROR;
}

ECode BerInputStream::ReadInteger()
{
    if (mTag != IASN1Constants::TAG_INTEGER) {
        return Expected(String("integer"));
    }

    // check encoded length
    if (mLength < 1) {
        Logger::E("BerInputStream", "Wrong length for ASN.1 integer at [%d]", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    FAIL_RETURN(ReadContent());

    // check encoded content
    if (mLength > 1) {
        Byte firstByte = (*mBuffer)[mOffset - mLength];
        Byte secondByte = (Byte) ((*mBuffer)[mOffset - mLength + 1] & 0x80);

        if ((firstByte == 0 && secondByte == 0) || (firstByte == (Byte) 0xFF
                && secondByte == (Byte) 0x80)) {
            Logger::E("BerInputStream", "Wrong content for ASN.1 integer at [%d]."
                    "An integer MUST be encoded in minimum number of octets", mOffset - mLength);
            return E_ASN1_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode BerInputStream::ReadOctetString()
{
    if (mTag == IASN1Constants::TAG_OCTETSTRING) {
        return ReadContent();
    }
    else if (mTag == IASN1Constants::TAG_C_OCTETSTRING) {
        Logger::E("BerInputStream", "Decoding constructed ASN.1 octet string type is not supported");
        return E_ASN1_EXCEPTION;
    }
    else {
        return Expected(String("octetstring"));
    }
}

ECode BerInputStream::Expected(
    /* [in] */ const String& what)
{
    Logger::E("BerInputStream", "ASN.1 %s identifier expected at [%], got 0x%08x",
            what.string(), mTagOffset, mTag);
    return E_ASN1_EXCEPTION;
}

ECode BerInputStream::ReadOID()
{
    if (mTag != IASN1Constants::TAG_OID) {
        return Expected(String("OID"));
    }

    // check encoded length
    if (mLength < 1) {
        Logger::E("BerInputStream", "Wrong length for ASN.1 object identifier at [%d]", mTagOffset);
        return E_ASN1_EXCEPTION;
    }

    FAIL_RETURN(ReadContent());

    // check content: last encoded byte (8th bit MUST be zero)
    if (((*mBuffer)[mOffset - 1] & 0x80) != 0) {
        Logger::E("BerInputStream", "Wrong encoding at [%d]", mOffset - 1);
        return E_ASN1_EXCEPTION;
    }

    mOidElement = 1;
    for (Int32 i = 0; i < mLength; i++, ++mOidElement) {
        while (((*mBuffer)[mContentOffset + i] & 0x80) == 0x80) {
            i++;
        }
    }
    return NOERROR;
}

ECode BerInputStream::ReadSequence(
    /* [out] */ IASN1Sequence* sequence)
{
    if (mTag != IASN1Constants::TAG_C_SEQUENCE) {
        return Expected(String("sequence"));
    }

    Int32 begOffset = mOffset;
    Int32 endOffset = begOffset + mLength;

    AutoPtr< ArrayOf<IASN1Type*> > type;
    AutoPtr< ArrayOf<Boolean> > option;
    AutoPtr< ArrayOf<IInterface*> > def;
    IASN1TypeCollection* temp = IASN1TypeCollection::Probe(sequence);
    temp->GetType((ArrayOf<IASN1Type*>**)&type);
    temp->GetOPTIONAL((ArrayOf<Boolean>**)&option);
    temp->GetDEFAULT((ArrayOf<IInterface*>**)&def);

    Int32 i = 0;

    if (mIsVerify) {

        for (; (mOffset < endOffset) && (i < type->GetLength()); i++) {

            Int32 value;
            FAIL_RETURN(Next(&value));
            Boolean chk;
            while ((*type)[i]->CheckTag(mTag, &chk), !chk) {
                // check whether it is optional component or not
                if (!(*option)[i] || (i == type->GetLength() - 1)) {
                    Logger::E("BerInputStream", "ASN.1 Sequence: mandatory value is missing at [%d]", mTagOffset);
                    return E_ASN1_EXCEPTION;
                }
                i++;
            }
            AutoPtr<IInterface> ret;
            FAIL_RETURN((*type)[i]->Decode(this, (IInterface**)&ret));
        }

        // check the rest of components
        for (; i < type->GetLength(); i++) {
            if (!(*option)[i]) {
                Logger::E("BerInputStream", "ASN.1 Sequence: mandatory value is missing at [%d]", mTagOffset);
                return E_ASN1_EXCEPTION;
            }
        }

    }
    else {
        Int32 seqTagOffset = mTagOffset; //store tag offset

        AutoPtr<IArrayOf> values;
        CArrayOf::New(EIID_IInterface, type->GetLength(), (IArrayOf**)&values);
        for (; (mOffset < endOffset) && (i < type->GetLength()); i++) {
            Int32 next;
            FAIL_RETURN(Next(&next));
            Boolean chk;
            while ((*type)[i]->CheckTag(mTag, &chk), !chk) {
                // check whether it is optional component or not
                if (!(*option)[i] || (i == type->GetLength() - 1)) {
                    Logger::E("BerInputStream", "ASN.1 Sequence: mandatory value is missing at [%d]", mTagOffset);
                    return E_ASN1_EXCEPTION;
                }

                // sets default value
                if ((*def)[i] != NULL) {
                    values->Set(i, (*def)[i]);
                }
                i++;
            }
            AutoPtr<IInterface> obj;
            FAIL_RETURN((*type)[i]->Decode(this, (IInterface**)&obj));
            values->Set(i, obj);
        }

        // check the rest of components
        for (; i < type->GetLength(); i++) {
            if (!(*option)[i]) {
                Logger::E("BerInputStream", "ASN.1 Sequence: mandatory value is missing at [%d]", mTagOffset);
                return E_ASN1_EXCEPTION;
            }
            if ((*def)[i] != NULL) {
                values->Set(i, (*def)[i]);
            }
        }
        mContent = values.Get();

        mTagOffset = seqTagOffset; //retrieve tag offset
    }

    if (mOffset != endOffset) {
        Logger::E("BerInputStream",
                "Wrong encoding at [%d]. Content's length and encoded length are not the same", begOffset);
        return E_ASN1_EXCEPTION;
    }
    return NOERROR;
}

ECode BerInputStream::ReadSequenceOf(
    /* [in] */ IASN1SequenceOf* sequenceOf)
{
    if (mTag != IASN1Constants::TAG_C_SEQUENCEOF) {
        return Expected(String("sequenceOf"));
    }

    return DecodeValueCollection(IASN1ValueCollection::Probe(sequenceOf));
}

ECode BerInputStream::ReadSet(
    /* [in] */ IASN1Set* set)
{
    if (mTag != IASN1Constants::TAG_C_SET) {
        return Expected(String("set"));
    }

    Logger::E("BerInputStream", "Decoding ASN.1 Set type is not supported");
    return E_ASN1_EXCEPTION;
}

ECode BerInputStream::ReadSetOf(
    /* [in] */ IASN1SetOf* setOf)
{
    if (mTag != IASN1Constants::TAG_C_SETOF) {
        return Expected(String("setOf"));
    }

    return DecodeValueCollection(IASN1ValueCollection::Probe(setOf));
}

ECode BerInputStream::DecodeValueCollection(
    /* [in] */ IASN1ValueCollection* collection)
{
    Int32 begOffset = mOffset;
    Int32 endOffset = begOffset + mLength;

    AutoPtr<IASN1Type> type;
    collection->GetType((IASN1Type**)&type);

    if (mIsVerify) {
        while (endOffset > mOffset) {
            Int32 ret;
            FAIL_RETURN(Next(&ret));
            AutoPtr<IInterface> obj;
            FAIL_RETURN(type->Decode(this, (IInterface**)&obj));
        }
    }
    else {
        Int32 seqTagOffset = mTagOffset; //store tag offset

        AutoPtr<IArrayList> values;
        CArrayList::New((IArrayList**)&values);
        while (endOffset > mOffset) {
            Int32 ret;
            FAIL_RETURN(Next(&ret));
            AutoPtr<IInterface> obj;
            FAIL_RETURN(type->Decode(this, (IInterface**)&obj));
            Boolean result;
            values->Add(obj, &result);
        }

        values->TrimToSize();
        mContent = values;

        mTagOffset = seqTagOffset; //retrieve tag offset
    }

    if (mOffset != endOffset) {
        Logger::E("BerInputStream",
                "Wrong encoding at [%d]. Content's length and encoded length are not the same", begOffset);
        return E_ASN1_EXCEPTION;
    }
    return NOERROR;
}

ECode BerInputStream::ReadString(
    /* [in] */ IASN1StringType* type)
{
    Int32 id, constrId;
    if (IASN1Type::Probe(type)->GetId(&id), mTag == id) {
        return ReadContent();
    }
    else if (IASN1Type::Probe(type)->GetConstrId(&constrId), mTag == constrId) {
        Logger::E("BerInputStream", "Decoding constructed ASN.1 string type is not provided");
        return E_ASN1_EXCEPTION;
    }
    else {
        return Expected(String("string"));
    }
}

ECode BerInputStream::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    AutoPtr< ArrayOf<Byte> > enc = ArrayOf<Byte>::Alloc(mOffset - mTagOffset);
    enc->Copy(0, mBuffer, mTagOffset, enc->GetLength());
    *encoded = enc;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode BerInputStream::GetBuffer(
    /* [out, callee] */ ArrayOf<Byte>** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = mBuffer;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode BerInputStream::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mLength;
    return NOERROR;
}

ECode BerInputStream::GetOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    *offset = mOffset;
    return NOERROR;
}

ECode BerInputStream::GetEndOffset(
    /* [out] */ Int32* endOffset)
{
    VALIDATE_NOT_NULL(endOffset)
    *endOffset = mOffset + mLength;
    return NOERROR;
}

ECode BerInputStream::GetTagOffset(
    /* [out] */ Int32* tagOffset)
{
    VALIDATE_NOT_NULL(tagOffset)
    *tagOffset = mTagOffset;
    return NOERROR;
}

ECode BerInputStream::SetVerify()
{
    mIsVerify = TRUE;
    return NOERROR;
}

ECode BerInputStream::Read(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    if (mOffset == mBuffer->GetLength()) {
        Logger::E("BerInputStream", "Unexpected end of encoding");
        return E_ASN1_EXCEPTION;
    }

    if (mIn == NULL) {
        *ret = (*mBuffer)[mOffset++] & 0xFF;
        return NOERROR;
    }
    else {
        Int32 octet;
        FAIL_RETURN(mIn->Read(&octet));
        if (octet == -1) {
            Logger::E("BerInputStream", "Unexpected end of encoding");
            return E_ASN1_EXCEPTION;
        }

        (*mBuffer)[mOffset++] = (Byte)octet;

        *ret = octet;
        return NOERROR;
    }
}

ECode BerInputStream::ReadContent()
{
    if (mOffset + mLength > mBuffer->GetLength()) {
        Logger::E("BerInputStream", "Unexpected end of encoding");
        return E_ASN1_EXCEPTION;
    }

    if (mIn == NULL) {
        mOffset += mLength;
    }
    else {
        Int32 bytesRead;
        FAIL_RETURN(mIn->Read(mBuffer, mOffset, mLength, &bytesRead));

        if (bytesRead != mLength) {
            // if input stream didn't return all data at once
            // try to read it in several blocks
            Int32 c = bytesRead;
            do {
                if (c < 1 || bytesRead > mLength) {
                    Logger::E("BerInputStream", "Failed to read encoded content");
                    return E_ASN1_EXCEPTION;
                }
                FAIL_RETURN(mIn->Read(mBuffer, mOffset + bytesRead, mLength - bytesRead, &c));
                bytesRead += c;
            } while (bytesRead != mLength);
        }

        mOffset += mLength;
    }
    return NOERROR;
}

ECode BerInputStream::CompactBuffer()
{
    if (mOffset != mBuffer->GetLength()) {
        AutoPtr<ArrayOf<Byte> > newBuffer = ArrayOf<Byte>::Alloc(mOffset);
        // restore buffer content
        newBuffer->Copy(0, mBuffer, 0, mOffset);
        // set new buffer
        mBuffer = newBuffer;
    }
    return NOERROR;
}

ECode BerInputStream::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* entry)
{
    if (mPool == NULL) {
        mPool = ArrayOf< AutoPtr< ArrayOf<IInterface*> > >::Alloc(2);
        (*mPool)[0] = ArrayOf<IInterface*>::Alloc(10);
        (*mPool)[1] = ArrayOf<IInterface*>::Alloc(10);
    }

    Int32 i = 0;
    for (; i < (*mPool)[0]->GetLength() && (*(*mPool)[0])[i] != NULL; i++) {
        if ((*(*mPool)[0])[i] == key) {
            (*mPool)[1]->Set(i, entry);
            return NOERROR;
        }
    }

    if (i == (*mPool)[0]->GetLength()) {
        Int32 size = (*mPool)[0]->GetLength();
        AutoPtr< ArrayOf< AutoPtr< ArrayOf<IInterface*> > > > newPool =
                ArrayOf< AutoPtr< ArrayOf<IInterface*> > >::Alloc(size * 2);
        for (Int32 j = 0; j < size * 2; j++) {
            (*newPool)[j] = ArrayOf<IInterface*>::Alloc(2);
        }
        (*newPool)[0]->Copy(0, (*mPool)[0], 0, size);
        (*newPool)[1]->Copy(0, (*mPool)[1], 0, size);
        mPool = newPool;
    }
    else {
        (*mPool)[0]->Set(i, key);
        (*mPool)[1]->Set(i, entry);
    }
    return NOERROR;
}

ECode BerInputStream::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** entry)
{
    VALIDATE_NOT_NULL(entry);
    if (mPool == NULL) {
        *entry = NULL;
        return NOERROR;
    }

    for (Int32 i = 0; i < (*mPool)[0]->GetLength(); i++) {
        if ((*(*mPool)[0])[i] == key) {
            *entry = (*(*mPool)[1])[i];
            REFCOUNT_ADD(*entry);
            return NOERROR;
        }
    }
    *entry = NULL;
    return NOERROR;
}

//ECode BerInputStream::SetTag(
//    /* [in] */ Int32 tag)
//{
//    mTag = tag;
//    return NOERROR;
//}
//
//ECode BerInputStream::GetTag(
//    /* [out] */ Int32* tag)
//{
//    VALIDATE_NOT_NULL(tag)
//    *tag = mTag;
//    return NOERROR;
//}
//
//ECode BerInputStream::SetLength(
//    /* [in] */ Int32 length)
//{
//    mLength = length;
//    return NOERROR;
//}
//
//ECode BerInputStream::SetContent(
//    /* [in] */ IInterface* content)
//{
//    mContent = content;
//    return NOERROR;
//}

ECode BerInputStream::GetContent(
    /* [out] */ IInterface** content)
{
    VALIDATE_NOT_NULL(content)
    *content = mContent;
    REFCOUNT_ADD(*content)
    return NOERROR;
}

//ECode BerInputStream::GetTagOffSet(
//    /* [out] */ Int32* tagOffset)
//{
//    VALIDATE_NOT_NULL(tagOffset)
//    *tagOffset = mTagOffset;
//    return NOERROR;
//}
//ECode BerInputStream::SetTagOffSet(
//    /* [in] */ Int32 tagOffset)
//{
//    mTagOffset = tagOffset;
//    return NOERROR;
//}
//
ECode BerInputStream::GetContentOffset(
    /* [out] */ Int32* contentOffset)
{
    VALIDATE_NOT_NULL(contentOffset)
    *contentOffset = mContentOffset;
    return NOERROR;
}

//ECode BerInputStream::SetContentOffset(
//    /* [in] */ Int32 contentOffset)
//{
//    mContentOffset = contentOffset;
//    return NOERROR;
//}
//
//ECode BerInputStream::SetChoiceIndex(
//    /* [in] */ Int32 choiceIndex)
//{
//    mChoiceIndex = choiceIndex;
//    return NOERROR;
//}
//
//ECode BerInputStream::GetChoiceIndex(
//    /* [out] */ Int32* choiceIndex)
//{
//    VALIDATE_NOT_NULL(choiceIndex)
//    *choiceIndex = mChoiceIndex;
//    return NOERROR;
//}
//
//ECode BerInputStream::SetTimes(
//    /* [in] */ ArrayOf<Int32>* times)
//{
//    mTimes = times;
//    return NOERROR;
//}
//
//ECode BerInputStream::GetTimes(
//    /* [out, callee] */ ArrayOf<Int32>** times)
//{
//    VALIDATE_NOT_NULL(times)
//    *times = mTimes;
//    REFCOUNT_ADD(*times)
//    return NOERROR;
//}
//
//ECode BerInputStream::SetOidElement(
//    /* [in] */ Int32 oidElement)
//{
//    mOidElement = oidElement;
//    return NOERROR;
//}
//
//ECode BerInputStream::GetOidElement(
//    /* [out] */ Int32* oidElement)
//{
//    VALIDATE_NOT_NULL(oidElement)
//    *oidElement = mOidElement;
//    return NOERROR;
//}

ECode BerInputStream::GetVerify(
    /* [out] */ Boolean* isVerify)
{
    VALIDATE_NOT_NULL(isVerify);
    *isVerify = mIsVerify;
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

