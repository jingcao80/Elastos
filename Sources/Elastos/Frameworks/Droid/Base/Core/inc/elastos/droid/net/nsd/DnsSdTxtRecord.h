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

#ifndef __ELASTOS_DROID_NET_NSD_DNSSDTXTRECORD_H__
#define __ELASTOS_DROID_NET_NSD_DNSSDTXTRECORD_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

/**
 * This class handles TXT record data for DNS based service discovery as specified at
 * http://tools.ietf.org/html/draft-cheshire-dnsext-dns-sd-11
 *
 * DNS-SD specifies that a TXT record corresponding to an SRV record consist of
 * a packed array of bytes, each preceded by a length byte. Each string
 * is an attribute-value pair.
 *
 * The DnsSdTxtRecord object stores the entire TXT data as a single byte array, traversing it
 * as need be to implement its various methods.
 * @hide
 *
 */
class DnsSdTxtRecord
    : public Object
    , public IParcelable
    , public IDnsSdTxtRecord
{
public:
    CAR_INTERFACE_DECL()

    /** Constructs a new, empty TXT record. */
    CARAPI constructor();

    /** Constructs a new TXT record from a byte array in the standard format. */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* data);

    /** Copy constructor */
    CARAPI constructor(
        /* [in] */ IDnsSdTxtRecord* src);

    /**
     * Set a key/value pair. Setting an existing key will replace its value.
     * @param key Must be ascii with no '='
     * @param value matching value to key
     */
    CARAPI Set(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Get a value for a key
     *
     * @param key
     * @return The value associated with the key
     */
    CARAPI Get(
        /* [in] */ const String& key,
        /* [out] */ String* result);

    /** Remove a key/value pair. If found, returns the index or -1 if not found */
    CARAPI Remove(
        /* [in] */ const String& key,
        /* [out] */ Int32* result);

    /** Return the count of keys */
    CARAPI KeyCount(
        /* [out] */ Int32* result);

    /** Return true if key is present, false if not. */
    CARAPI Contains(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    /* Gets the size in bytes */
    CARAPI GetSize(
        /* [out] */ Int32* result);

    /* Gets the raw data in bytes */
    CARAPI GetRawData(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Return a string representation.
     * Example : {key1=value1},{key2=value2}..
     *
     * For a key say like "key3" with null value
     * {key1=value1},{key2=value2}{key3}
     */
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* val);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    CARAPI Insert(
        /* [in] */ ArrayOf<Byte>* keyBytes,
        /* [in] */ ArrayOf<Byte>* value,
        /* [in] */ Int32 index);

    /** Return a key in the TXT record by zero-based index. Returns null if index exceeds the total number of keys. */
    CARAPI GetKey(
        /* [in] */ Int32 index,
        /* [out] */ String* result);

    /**
     * Look up a key in the TXT record by zero-based index and return its value.
     * Returns null if index exceeds the total number of keys.
     * Returns null if the key is present with no value.
     */
    CARAPI GetValue(
        /* [in] */ Int32 index,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI GetValueAsString(
        /* [in] */ Int32 index,
        /* [out] */ String* result);

    CARAPI GetValue(
        /* [in] */ const String& forKey,
        /* [out, callee] */ ArrayOf<Byte>** result);

private:
    static const Byte mSeperator;

    AutoPtr<ArrayOf<Byte> > mData;
};

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_NSD_DNSSDTXTRECORD_H__
