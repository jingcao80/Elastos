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

#ifndef __ELASTOS_DROID_OS_CPERSISTABLE_BUNDLE_H__
#define __ELASTOS_DROID_OS_CPERSISTABLE_BUNDLE_H__

#include "_Elastos_Droid_Os_CPersistableBundle.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/BaseBundle.h"

using Elastos::Droid::Internal::Utility::IXmlUtilsWriteMapCallback;
using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Core::ICharSequence;
using Elastos::Core::ICloneable;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * A mapping from String values to various types that can be saved to persistent and later
 * restored.
 *
 */
CarClass(CPersistableBundle)
    , public BaseBundle
    , public IPersistableBundle
    , public ICloneable
    , public IParcelable
    , public IXmlUtilsWriteMapCallback
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Constructs a new, empty PersistableBundle.
     */
    CARAPI constructor();

    /**
     * Constructs a new, empty PersistableBundle sized to hold the given number of
     * elements. The PersistableBundle will grow as needed.
     *
     * @param capacity the initial capacity of the PersistableBundle
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity);

    /**
     * Constructs a PersistableBundle containing a copy of the mappings from the given
     * PersistableBundle.
     *
     * @param b a PersistableBundle to be copied.
     */
    CARAPI constructor(
        /* [in] */ IPersistableBundle* b);

    /**
     * Constructs a PersistableBundle containing the mappings passed in.
     *
     * @param map a Map containing only those items that can be persisted.
     * @throws IllegalArgumentException if any element of #map cannot be persisted.
     */
    CARAPI constructor(
        /* [in] */ IMap* map);

    /**
     * Make a PersistableBundle for a single key/value pair.
     *
     * @hide
     */
    static AutoPtr<IPersistableBundle> ForPair(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Clones the current PersistableBundle. The internal map is cloned, but the keys and
     * values to which it refers are copied by reference.
     */
    CARAPI Clone(
        /* [out] */ IInterface** obj);

    /**
     * Inserts a PersistableBundle value into the mapping of this Bundle, replacing
     * any existing value for the given key.  Either key or value may be null.
     *
     * @param key a String, or null
     * @param value a Bundle object, or null
     */
    CARAPI PutPersistableBundle(
        /* [in] */ const String& key,
        /* [in] */ IPersistableBundle* value);

    CARAPI PutPersistableBundle(
        /* [in] */ ICharSequence* key,
        /* [in] */ IPersistableBundle* value);

    /**
     * Returns the value associated with the given key, or null if
     * no mapping of the desired type exists for the given key or a null
     * value is explicitly associated with the key.
     *
     * @param key a String, or null
     * @return a Bundle value, or null
     */
    CARAPI GetPersistableBundle(
        /* [in] */ const String& key,
        /* [out] */ IPersistableBundle** value);

    CARAPI GetPersistableBundle(
        /* [in] */ ICharSequence* key,
        /* [out] */ IPersistableBundle** value);

    CARAPI WriteUnknownObject(
        /* [in] */ IInterface* v,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* out);

    static CARAPI RestoreFromXml(
        /* [in] */ IXmlPullParser* in,
        /* [out] */ IPersistableBundle** pb);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    static const AutoPtr<IPersistableBundle> EMPTY;

private:
    static const String TAG_PERSISTABLEMAP;// = "pbundle_as_map";
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CPERSISTABLE_BUNDLE_H__
