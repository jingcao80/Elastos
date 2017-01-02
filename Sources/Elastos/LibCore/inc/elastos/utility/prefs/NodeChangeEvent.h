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

#ifndef __ELASTOS_UTILITY_PREFS_NODECHANGEEVENT_H__
#define __ELASTOS_UTILITY_PREFS_NODECHANGEEVENT_H__

#include "EventObject.h"

using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;

namespace Elastos {
namespace Utility {
namespace Prefs {

/**
 * This is the event class to indicate that one child of the preference node has
 * been added or deleted.
 * <p>
 * Please note that although the class is marked as {@code Serializable} by
 * inheritance from {@code EventObject}, this type is not intended to be serialized
 * so the serialization methods do nothing but throw a {@code NotSerializableException}.
 *
 * @see java.util.prefs.Preferences
 * @see java.util.prefs.NodeChangeListener
 *
 * @since 1.4
 */
class NodeChangeEvent
    : public EventObject
    , public INodeChangeEvent
{
public:
    CAR_INTERFACE_DECL()

    NodeChangeEvent();

    virtual ~NodeChangeEvent();

    /**
     * Constructs a new {@code NodeChangeEvent} instance.
     *
     * @param p
     *            the {@code Preferences} instance that fired this event; this object is
     *            considered as the event source.
     * @param c
     *            the child {@code Preferences} instance that was added or deleted.
     */
    CARAPI constructor(
        /* [in] */ IPreferences* p,
        /* [in] */ IPreferences* c);

    /**
     * Gets the {@code Preferences} instance that fired this event.
     *
     * @return the {@code Preferences} instance that fired this event.
     */
    CARAPI GetParent(
        /* [out] */ IPreferences** parent);

    /**
     * Gets the child {@code Preferences} node that was added or removed.
     *
     * @return the added or removed child {@code Preferences} node.
     */
    CARAPI GetChild(
        /* [out] */ IPreferences** child);

private:
    /**
     * This method always throws a <code>NotSerializableException</code>,
     * because this object cannot be serialized,
     */
    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* out)/* throws IOException */;

    /**
     * This method always throws a <code>NotSerializableException</code>,
     * because this object cannot be serialized,
     */
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* in) /*throws IOException, ClassNotFoundException*/;

private:
    // private static final long serialVersionUID = 8068949086596572957L;

    AutoPtr<IPreferences> mParent;
    AutoPtr<IPreferences> mChild;
};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_NODECHANGEEVENT_H__
