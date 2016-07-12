
#include "Elastos.CoreLibrary.Security.h"
#include "CUUID.h"
#include "StringUtils.h"
#include "StringBuilder.h"
#include "Memory.h"
#include "CMessageDigestHelper.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::IRandom;
using Libcore::IO::Memory;
using Elastos::IO::ByteOrder_BIG_ENDIAN;
using Elastos::IO::EIID_ISerializable;
using Elastos::Security::EIID_ISecureRandom;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::CMessageDigestHelper;

namespace Elastos {
namespace Utility {

AutoPtr<ISecureRandom> CUUID::sRng;
Object CUUID::sRngLock;

CAR_INTERFACE_IMPL_3(CUUID, Object, IUUID, IComparable, ISerializable)

CAR_OBJECT_IMPL(CUUID)

CUUID::CUUID()
    : mMostSigBits(0)
    , mLeastSigBits(0)
    , mVariant(0)
    , mVersion(0)
    , mTimestamp(0)
    , mClockSequence(0)
    , mNode(0)
    , mHash(0)
{
}

ECode CUUID::constructor(
    /* [in] */ Int64 mostSigBits,
    /* [in] */ Int64 leastSigBits)
{
    mMostSigBits = mostSigBits;
    mLeastSigBits = leastSigBits;
    return Init();
}

ECode CUUID::Init()
{
    // setup hash field
    Int32 msbHash = (Int32) (mMostSigBits ^ ((UInt64)mMostSigBits >> 32));
    Int32 lsbHash = (Int32) (mLeastSigBits ^ ((UInt64)mLeastSigBits >> 32));
    mHash = msbHash ^ lsbHash;

    // setup variant field
    if ((mLeastSigBits & 0x8000000000000000LL) == 0) {
        // MSB0 not set, NCS backwards compatibility variant
        mVariant = 0;
    }
    else if ((mLeastSigBits & 0x4000000000000000LL) != 0) {
        // MSB1 set, either MS reserved or future reserved
        mVariant = (Int32) (UInt64(mLeastSigBits & 0xE000000000000000LL) >> 61);
    }
    else {
        // MSB1 not set, RFC 4122 variant
        mVariant = 2;
    }

    // setup version field
    mVersion = (Int32) ((UInt64)(mMostSigBits & 0x000000000000F000) >> 12);

    if (mVariant != 2 && mVersion != 1) {
        return NOERROR;
    }

    // setup timestamp field
    Int64 timeLow = UInt64(mMostSigBits & 0xFFFFFFFF00000000LL) >> 32;
    Int64 timeMid = (mMostSigBits & 0x00000000FFFF0000LL) << 16;
    Int64 timeHigh = (mMostSigBits & 0x0000000000000FFFLL) << 48;
    mTimestamp = timeLow | timeMid | timeHigh;

    // setup clock sequence field
    mClockSequence = (Int32) (UInt64(mLeastSigBits & 0x3FFF000000000000LL) >> 48);

    // setup node field
    mNode = (mLeastSigBits & 0x0000FFFFFFFFFFFFLL);

    return NOERROR;
}

ECode CUUID::RandomUUID(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = NULL;

    // lock on the class to protect lazy init
    {
        AutoLock syncLock(sRngLock);
        if (sRng == NULL) {
            assert(0);
            //TODO CSecureRandom::New((ISecureRandom**)&sRng);
        }
    }

    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(16);
    IRandom::Probe(sRng)->NextBytes(data);
    AutoPtr<IUUID> id = MakeUuid(data, 4);
    *uuid = id;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode CUUID::NameUUIDFromBytes(
    /* [in] */ ArrayOf<Byte>* name,
    /* [out] */ IUUID** uuid)
{
    assert(name);
    VALIDATE_NOT_NULL(uuid)
    *uuid = NULL;
    VALIDATE_NOT_NULL(name)

    if (name->GetLength() == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new NullPointerException("name == null");
    }
    // try {
    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    AutoPtr<IMessageDigest> md;
    helper->GetInstance(String("MD5"), (IMessageDigest**)&md);

    AutoPtr<ArrayOf<Byte> > hash;
    md->Digest(name, (ArrayOf<Byte>**)&hash);
    AutoPtr<IUUID> id = MakeUuid(hash, 3);
    *uuid = id;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

AutoPtr<IUUID> CUUID::MakeUuid(
    /* [in] */ ArrayOf<Byte>* hash,
    /* [in] */ Int32 version)
{
    Int64 msb = Memory::PeekInt64(hash, 0, ByteOrder_BIG_ENDIAN);
    Int64 lsb = Memory::PeekInt64(hash, 8, ByteOrder_BIG_ENDIAN);
    // Set the version field.
    msb &= ~(0xfLL << 12);
    msb |= ((Int64) version) << 12;
    // Set the variant field to 2. Note that the variant field is variable-width,
    // so supporting other variants is not just a matter of changing the constant 2 below!
    lsb &= ~(0x3LL << 62);
    lsb |= 2LL << 62;

    AutoPtr<IUUID> uuid;
    CUUID::New(msb, lsb, (IUUID**)&uuid);
    return uuid;
}

ECode CUUID::FromString(
    /* [in] */ const String& name,
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = NULL;
    if (name.IsNull())
        return E_NULL_POINTER_EXCEPTION;

    AutoPtr<ArrayOf<String> > parts;
    StringUtils::Split(name, "-", (ArrayOf<String> **)&parts);

    if (parts == NULL || parts->GetLength() != 5) {
        // throw new IllegalArgumentException("Invalid UUID: " + uuid);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 m1, m2, m3;
    FAIL_RETURN(StringUtils::ParsePositiveInt64((*parts)[0], 16, &m1));
    FAIL_RETURN(StringUtils::ParsePositiveInt64((*parts)[1], 16, &m2));
    FAIL_RETURN(StringUtils::ParsePositiveInt64((*parts)[2], 16, &m3));

    Int64 lsb1, lsb2;
    FAIL_RETURN(StringUtils::ParsePositiveInt64((*parts)[3], 16, &lsb1));
    FAIL_RETURN(StringUtils::ParsePositiveInt64((*parts)[4], 16, &lsb2));

    Int64 msb = (m1 << 32) | (m2 << 16) | m3;
    Int64 lsb = (lsb1 << 48) | lsb2;
    return CUUID::New(msb, lsb, uuid);
}

ECode CUUID::GetLeastSignificantBits(
    /* [out] */ Int64* leastSigBits)
{
    VALIDATE_NOT_NULL(leastSigBits)
    *leastSigBits = mLeastSigBits;
    return NOERROR;
}

ECode CUUID::GetMostSignificantBits(
    /* [out] */ Int64* mostSigBits)
{
    VALIDATE_NOT_NULL(mostSigBits)
    *mostSigBits = mMostSigBits;
    return NOERROR;
}

ECode CUUID::Version(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version)
    *version = mVersion;
    return NOERROR;
}

ECode CUUID::Variant(
    /* [out] */ Int32* variant)
{
    VALIDATE_NOT_NULL(variant)
    *variant = mVariant;
    return NOERROR;
}

ECode CUUID::Timestamp(
    /* [out] */ Int64* timeStamp)
{
    VALIDATE_NOT_NULL(timeStamp)
    *timeStamp = 0;

    if (mVersion != 1) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *timeStamp = mTimestamp;
    return NOERROR;
}

ECode CUUID::ClockSequence(
    /* [out] */ Int32* clockSequence)
{
    VALIDATE_NOT_NULL(clockSequence)
    *clockSequence = 0;

    if (mVersion != 1) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *clockSequence = mClockSequence;
    return NOERROR;
}

ECode CUUID::Node(
    /* [out] */ Int64* node)
{
    VALIDATE_NOT_NULL(node)
    *node = 0;

    if (mVersion != 1) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *node = mNode;
    return NOERROR;
}

ECode CUUID::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    IUUID* uo = IUUID::Probe(other);
    if (other == NULL || uo == NULL) {
        *result = -1;
        return E_CLASS_CAST_EXCEPTION;
    }

    *result = 0;
    if (other == TO_IINTERFACE(this)) {
        return NOERROR;
    }

    Int64 om, ol;
    uo->GetMostSignificantBits(&om);
    uo->GetLeastSignificantBits(&ol);

    if (mMostSigBits != om) {
        *result = mMostSigBits < om ? -1 : 1;
        return NOERROR;
    }

    if (mLeastSigBits != ol) {
        *result = mLeastSigBits < ol ? -1 : 1;
        return NOERROR;
    }

    return NOERROR;
}

ECode CUUID::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (other == NULL) {
        return NOERROR;
    }

    if (IUUID::Probe(other) == NULL)
        return NOERROR;

    if (IUUID::Probe(other) == this) {
        *result = TRUE;
        return NOERROR;
    }

    Int64 om, ol;
    IUUID::Probe(other)->GetMostSignificantBits(&om);
    IUUID::Probe(other)->GetLeastSignificantBits(&ol);
    *result = (mLeastSigBits == ol) && (mMostSigBits == om);
    return NOERROR;
}

ECode CUUID::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = mHash;
    return NOERROR;
}

ECode CUUID::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder(36);

    String msbStr = StringUtils::ToHexString(mMostSigBits);
    if (msbStr.GetLength() < 16) {
        Int32 diff = 16 - msbStr.GetLength();
        for (Int32 i = 0; i < diff; i++) {
            builder.AppendChar('0');
        }
    }
    builder.Append(msbStr);
    builder.InsertChar(8, '-');
    builder.InsertChar(13, '-');
    builder.AppendChar('-');
    String lsbStr = StringUtils::ToHexString(mLeastSigBits);
    if (lsbStr.GetLength() < 16) {
        Int32 diff = 16 - lsbStr.GetLength();
        for (Int32 i = 0; i < diff; i++) {
            builder.AppendChar('0');
        }
    }
    builder.Append(lsbStr);
    builder.InsertChar(23, '-');
    *str = builder.ToString();
    return NOERROR;
}

ECode CUUID::ReadObject(
    /* [in] */ IObjectInputStream* in)
{
    VALIDATE_NOT_NULL(in);
    // read in non-transient fields
    in->DefaultReadObject();
    // setup transient fields
    return Init();
}

} // namespace Utility
} // namespace Elastos
