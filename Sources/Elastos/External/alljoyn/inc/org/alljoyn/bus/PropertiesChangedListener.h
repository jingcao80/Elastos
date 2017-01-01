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

#ifndef __ORG_ALLJOYN_BUS_PROPERTY_CHANGED_LISTENER_H__
#define __ORG_ALLJOYN_BUS_PROPERTY_CHANGED_LISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IMap;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * Implemented by a user-defined classes that is interested in handling
 * property change notification.
 *
 * Listener objects are the Java objects that handle notification events and are
 * called from AllJoyn in the context of one of its threads.  All listener
 * objects are expected to be multithread safe (MT-Safe) between construction
 * and destruction.  That is, every thread executing in a listener object's
 * methods 1) gets a unique copy of all temporary data (it is re-entrant); and
 * 2) all shared data -- the object instance's member variables or any globals
 * must contain no read-modify-write access patterns (okay to write or read,
 * just never to read-modify-write).  If such access patterns are required, it
 * is the responsibility of the client to, for example, add the synchronized
 * keyword when overriding one of the listener methods or provide some other
 * serialization mechanism in order to preserve MT-Safe operation.
 *
 * This rule extends to other objects accessed during processing of
 * notifications.  For example, it is a programming error to allow a notifiation
 * method to update a collection in another object without serializing access
 * to the collection.
 *
 * The important consideration in this case is that as soon as one sets up a
 * listener object to receive notifications from AllJoyn, one is implicitly
 * dealing with multithreaded code.
 *
 * Since listener objects generally run in the context of the AllJoyn thread
 * which manages reception of events, If a blocking AllJoyn call is made in
 * the context of a notification, the necessary and sufficient conditions for
 * deadlock are established.
 *
 * The important consideration in this case is that when one receives a
 * notification from AllJoyn, that notification is executing in the context of
 * an AllJoyn thread.  If one makes a blocking call back into AllJoyn on that
 * thread, a deadlock cycle is likely, and if this happens your bus attachment
 * receive thread will deadlock (with itself).  The deadlock is typically broken
 * after a bus timeout eventually happens.
 */
class ECO_PUBLIC PropertiesChangedListener
    : public Object
    , public IPropertiesChangedListener
{
public:
    CAR_INTERFACE_DECL()

    PropertiesChangedListener();

    virtual ~PropertiesChangedListener();

    CARAPI constructor();

    /**
     * Called by the bus when the value of a property changes if that property has annotation
     * org.freedesktop.DBus.Properties.EmitChangedSignal=true or
     * org.freedesktop.DBus.Properties.EmitChangedSignal=invalidated
     *
     * Any implementation of this function must be multithread safe.  See the
     * class documentation for details.
     *
     * @param pObj          Bus Object that owns the property that changed.
     * @param ifaceName     The name of the interface the defines the property that changed.
     * @param changed       Property values that changed as an array of dictionary entries, signature "a{sv}".
     * @param invalidated   Properties whose values have been invalidated, signature "as".
     */
    virtual CARAPI PropertiesChanged(
        /* [in] */ IProxyBusObject* pObj,
        /* [in] */ const String& ifaceName,
        /* [in] */ IMap* changed,
        /* [in] */ ArrayOf<String>* invalidated) = 0;

private:
    /** Allocate native resources. */
    void Create();

    /** Release native resources. */
    void Destroy();

private:
    friend class NativeProxyBusObject;

    /**
     * The opaque pointer to the underlying C++ object which is actually tied
     * to the AllJoyn code.
     */
    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_PROPERTY_CHANGED_LISTENER_H__
