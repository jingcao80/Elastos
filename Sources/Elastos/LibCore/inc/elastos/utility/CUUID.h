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

#ifndef __ELASTOS_UTILITY_CUUID_H__
#define __ELASTOS_UTILITY_CUUID_H__

#include "_Elastos_Utility_CUUID.h"
#include "Object.h"

using Elastos::Core::IComparable;
using Elastos::IO::ISerializable;
using Elastos::IO::IObjectInputStream;
using Elastos::Security::ISecureRandom;

namespace Elastos {
namespace Utility {

CarClass(CUUID)
    , public Object
    , public IUUID
    , public IComparable
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUUID();

    CARAPI constructor(
        /* [in] */ Int64 mostSigBits,
        /* [in] */ Int64 leastSigBits);

    /**
     * <p>
     * The 64 least significant bits of the UUID.
     *
     * @return the 64 least significant bits.
     */
    CARAPI GetLeastSignificantBits(
        /* [out] */ Int64* leastSigBits);

    /**
     * <p>
     * The 64 most significant bits of the UUID.
     *
     * @return the 64 most significant bits.
     */
    CARAPI GetMostSignificantBits(
        /* [out] */ Int64* mostSigBits);

    /**
     * <p>
     * The version of the variant 2 UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>. If the variant
     * is not 2, then the version will be 0.
     * <ul>
     * <li>1 - Time-based UUID</li>
     * <li>2 - DCE Security UUID</li>
     * <li>3 - Name-based with MD5 hashing UUID ({@link #nameUUIDFromBytes(byte[])})</li>
     * <li>4 - Randomly generated UUID ({@link #randomUUID()})</li>
     * <li>5 - Name-based with SHA-1 hashing UUID</li>
     * </ul>
     *
     * @return an {@code Int32} value.
     */
    CARAPI Version(
        /* [out] */ Int32* version);

    /**
     * <p>
     * The variant of the UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     * <ul>
     * <li>0 - Reserved for NCS compatibility</li>
     * <li>2 - RFC 4122/Leach-Salz</li>
     * <li>6 - Reserved for Microsoft Corporation compatibility</li>
     * <li>7 - Reserved for future use</li>
     * </ul>
     *
     * @return an {@code int} value.
     */
    CARAPI Variant(
        /* [out] */ Int32* variant);

    /**
     * <p>
     * The timestamp value of the version 1, variant 2 UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @return a {@code long} value.
     * @throws UnsupportedOperationException
     *             if {@link #version()} is not 1.
     */
    CARAPI Timestamp(
        /* [out] */ Int64* version);

    /**
     * <p>
     * The clock sequence value of the version 1, variant 2 UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @return a {@code long} value.
     * @throws UnsupportedOperationException
     *             if {@link #version()} is not 1.
     */
    CARAPI ClockSequence(
        /* [out] */ Int32* clockSequence);

    /**
     * <p>
     * The node value of the version 1, variant 2 UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @return a {@code long} value.
     * @throws UnsupportedOperationException
     *             if {@link #version()} is not 1.
     */
    CARAPI Node(
        /* [out] */ Int64* node);

    CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    /**
     * <p>
     * Generates a variant 2, version 4 (randomly generated number) UUID as per
     * <a href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @return an UUID instance.
     */
    static CARAPI RandomUUID(
        /* [out] */ IUUID** uuid);

    /**
     * <p>
     * Generates a variant 2, version 3 (name-based, MD5-hashed) UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @param name
     *            the name used as byte array to create an UUID.
     * @return an UUID instance.
     */
    static CARAPI NameUUIDFromBytes(
        /* [in] */ ArrayOf<Byte>* name,
        /* [out] */ IUUID** uuid);

    /**
     * <p>
     * Parses a UUID string with the format defined by {@link #toString()}.
     *
     * @param uuid
     *            the UUID string to parse.
     * @return an UUID instance.
     * @throws NullPointerException
     *             if {@code uuid} is {@code null}.
     * @throws IllegalArgumentException
     *             if {@code uuid} is not formatted correctly.
     */
    static CARAPI FromString(
        /* [in] */ const String& name,
        /* [out] */ IUUID** uuid);

private:
    /**
     * <p>
     * Sets up the transient fields of this instance based on the current values
     * of the {@code mostSigBits} and {@code leastSigBits} fields.
     */
    CARAPI Init();

    static AutoPtr<IUUID> MakeUuid(
        /* [in] */ ArrayOf<Byte>* hash,
        /* [in] */ Int32 version);

    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* in);

private:
    static AutoPtr<ISecureRandom> sRng;
    static Object sRngLock;

    Int64 mMostSigBits;
    Int64 mLeastSigBits;

    /* transient */ Int32 mVariant;
    /* transient */ Int32 mVersion;
    /* transient */ Int64 mTimestamp;
    /* transient */ Int32 mClockSequence;
    /* transient */ Int64 mNode;
    /* transient */ Int32 mHash;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CUUID_H__
