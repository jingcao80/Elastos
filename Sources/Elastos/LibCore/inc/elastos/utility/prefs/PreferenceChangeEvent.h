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

#ifndef __ELASTOS_UTILITY_PREFS_PREFERENCECHANGEEVENT_H__
#define __ELASTOS_UTILITY_PREFS_PREFERENCECHANGEEVENT_H__

#include "EventObject.h"
#include "Preferences.h"

using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Org::W3c::Dom::INodeList;
using Org::W3c::Dom::INode;

namespace Elastos {
namespace Utility {
namespace Prefs {

/**
 * This is the event class to indicate that a preference has been added, deleted
 * or updated.
 * <p>
 * Please note that although the class is marked as {@code Serializable} by
 * inheritance from {@code EventObject}, this type is not intended to be serialized
 * so the serialization methods do nothing but throw a {@code NotSerializableException}.
 *
 * @see java.util.prefs.Preferences
 * @see java.util.prefs.PreferenceChangeListener
 *
 * @since 1.4
 */
class PreferenceChangeEvent
    : public EventObject
    , public IPreferenceChangeEvent
{
public:
    CAR_INTERFACE_DECL()

    PreferenceChangeEvent();

    /**
     * Construct a new {@code PreferenceChangeEvent} instance.
     *
     * @param p
     *            the {@code Preferences} instance that fired this event; this object is
     *            considered as the event's source.
     * @param k
     *            the changed preference key.
     * @param v
     *            the new value of the changed preference, this value can be
     *            {@code null}, which means the preference has been removed.
     */
    CARAPI constructor(
        /* [in] */ Preferences* p,
        /* [in] */ const String& k,
        /* [in] */ const String& v);

    /**
     * Gets the key of the changed preference.
     *
     * @return the changed preference's key.
     */
    CARAPI GetKey(
        /* [out] */ String* key);

    /**
     * Gets the new value of the changed preference or {@code null} if the
     * preference has been removed.
     *
     * @return the new value of the changed preference or {@code null} if the
     *         preference has been removed.
     */
    CARAPI GetNewValue(
        /* [out] */ String* value);

    /**
     * Gets the {@code Preferences} instance that fired this event.
     *
     * @return the {@code Preferences} instance that fired this event.
     */
    CARAPI GetNode(
        /* [out] */ IPreferences** pf);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    /**
     * This method always throws a <code>NotSerializableException</code>,
     * because this object cannot be serialized,
     */
    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* out) /*throws IOException*/;

    /**
     * This method always throws a <code>NotSerializableException</code>,
     * because this object cannot be serialized,
     */
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* in) /*throws IOException*/;

private:
    // private static final long serialVersionUID = 793724513368024975L;
    AutoPtr<IPreferences> mNode;

    String mKey;

    String mValue;
};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_PREFERENCECHANGEEVENT_H__
