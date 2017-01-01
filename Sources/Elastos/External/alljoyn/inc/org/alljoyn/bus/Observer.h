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

#ifndef __ORG_ALLJOYN_BUS_OBSERVER_H__
#define __ORG_ALLJOYN_BUS_OBSERVER_H__

#include "_Org.Alljoyn.Bus.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::Core::IComparable;
using Elastos::IO::ICloseable;
using Elastos::Utility::INavigableMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * A convenient object discovery and access mechanism.
 * <p>
 * An Observer takes care of discovery, session management and ProxyBusObject
 * creation for bus objects that implement a specific set of interfaces.
 * <p>
 * The Observer monitors About announcements, and automatically sets up sessions
 * with all peers that offer objects of interest (i.e. objects that implement at
 * least the set of mandatory interfaces for this Observer). The Observer creates
 * a proxy bus object for each discovered object. The Observer.Listener class
 * is used to inform the application about the discovery of new objects, and about
 * the disappearance of objects from the bus.
 * <p>
 * Objects are considered lost in the following cases:
 * <ul>
 * <li> they are un-announced via About
 * <li> the hosting peer has closed the session
 * <li> the hosting peer stopped responding to Ping requests
 * </ul>
 */
class Observer
    : public Object
    , public IObserver
    , public ICloseable
{
private:
    /**
     * Encapsulates the unique id of a bus object (the busname/path pair).
     */
    class ObjectId
        : public Object
        , public IComparable
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("Observer::ObjectId")

        ObjectId(
            /* [in] */ const String& _busname,
            /* [in] */ const String& _path);

        ObjectId(
            /* [in] */ IProxyBusObject* proxy);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        CARAPI CompareTo(
            /* [in] */ IInterface* other,
            /* [out] */ Int32* result);
    public:
        /**
         * unique bus name
         */
        String mBusname;

        /**
         * object path
         */
        String mPath;
    };

    /**
     * Wraps a listener to add some bookkeeping information.
     */
    class WrappedListener
        : public Object
    {
    public:
        TO_STRING_IMPL("Observer::WrappedListener")

        WrappedListener(
            /* [in] */ IObserverListener* l,
            /* [in] */ Boolean enable)
            : mListener(l)
            , mEnabled(enable)
        {
            assert(l != NULL);
        }
    public:
        /**
         * the listener
         */
        AutoPtr<IObserverListener> mListener;

        /**
         * Is the listener enabled?
         *
         * triggerOnExisting listeners start out disabled, they get enabled
         * when native calls us back on the dispatcher thread via
         * enablePendingListeners.
         */
        Boolean mEnabled;
    };

public:
    CAR_INTERFACE_DECL()

    Observer();

    virtual ~Observer();

    /**
     * Create an Observer that discovers all objects implementing a minimum set
     * of Interfaces.
     *
     * @param bus the bus attachment to be used by this Observer
     * @param mandatoryInterfaces the minimal set of Interfaces that have to be
     *            implemented by a bus object for the Observer to discover that
     *            object. This must be a non-null, non empty array.
     * @param optionalInterfaces additional Interfaces that will be added to the
     *            {@link ProxyBusObject}s managed created by this Observer, if
     *            they are supported by the discovered bus objects or null if no
     *            optional interfaces are required
     */
    CARAPI constructor(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ ArrayOf<IInterfaceInfo*>* mandatoryInterfaces,
        /* [in] */ ArrayOf<IInterfaceInfo*>* optionalInterfaces);

    /**
     * Create an Observer that discovers all objects implementing a minimum set
     * of Interfaces.
     *
     * @param bus the bus attachment to be used by this Observer
     * @param mandatoryInterfaces the minimal set of Interfaces that have to be
     *            implemented by a bus object for the Observer to discover that
     *            object. This must be a non-null, non empty array.
     */
    CARAPI constructor(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ ArrayOf<IInterfaceInfo*>* mandatoryInterfaces);

    /**
     * Closes this observer and frees up any resource attached to it.
     *
     * @see java.io.Closeable#close()
     */
    CARAPI Close();

    /**
     * Register an {@link Observer.Listener}.
     *
     * @param listener the listener to register
     * @param triggerOnExisting indicates whether the listener's
     *            objectDiscovered callback should be invoked for all objects
     *            that have been discovered prior to the registration of this
     *            listener.
     */
    CARAPI RegisterListener(
        /* [in] */ IObserverListener* listener,
        /* [in] */ Boolean triggerOnExisting);

    /**
     * Register an {@link Observer.Listener}.
     *
     * The listener's objectDiscovered callback will be invoked for all objects
     * that have been discovered prior to the registration of the listener.
     *
     * @param listener the listener to register
     */
    CARAPI RegisterListener(
        /* [in] */ IObserverListener* listener);

    /**
     * Unregister a previously registered {@link Observer.Listener}.
     *
     * @param listener the listener to unregister
     */
    CARAPI UnregisterListener(
        /* [in] */ IObserverListener* listener);

    /**
     * Unregister all listeners.
     */
    CARAPI UnregisterAllListeners();

    /**
     * Retrieve a {@link ProxyBusObject}.
     *
     * If the supplied (busname, path) pair does not identify an object that has
     * been discovered by this Observer, or identifies an object that has since
     * disappeared from the bus, null will be returned.
     *
     * @param busname the non-null unique bus name of the peer hosting the bus
     *            object
     * @param path the non-null object's path
     * @return the ProxyBusObject or null if not found
     */
    CARAPI Get(
        /* [in] */ const String& busname,
        /* [in] */ const String& path,
        /* [out] */ IProxyBusObject** object);

    /**
     * Retrieve the first {@link ProxyBusObject}.
     *
     * The getFirst/getNext pair of methods is used for iteration over all
     * discovered objects. null is returned to signal end of iteration.
     *
     * @return the first ProxyBusObject or null if no objects discovered
     */
    CARAPI GetFirst(
        /* [out] */ IProxyBusObject** object);

    /**
     * Retrieve the next {@link ProxyBusObject}.
     *
     * The getFirst/getNext pair of methods is used for iteration over all
     * discovered objects. null is returned to signal end of iteration.
     *
     * @param object the previous ProxyBusObject
     * @return the next ProxyBusObject or null if iteration is finished
     */
    CARAPI GetNext(
        /* [in] */ IProxyBusObject* current,
        /* [out] */ IProxyBusObject** object);

private:
    /**
     * Create native counterpart.
     */
    void Create(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ ArrayOf<String>* interfaceNames);

    /**
     * Destroy native counterpart.
     */
    void Destroy();

    /**
     * Make native call us back on the dispatcher thread.
     */
    void TriggerEnablePendingListeners();

    /**
     * Enable pending listeners.
     *
     * Listeners that are registered with triggerOnExisting == true, start off
     * in pending state. That means we don't dispatch callbacks to them until
     * we've been able to invoke objectDiscovered for all objects that are
     * already in our proxy list. Native calls us via this method to do those
     * initial callbacks.
     */
    void EnablePendingListeners();

    /**
     * Native tells us a new object was discovered.
     *
     * @param busname unique bus name
     * @param path the object path
     * @param interfaces list of announced AllJoyn Interfaces for the discovered object
     * @param sessionId id of the session we can use to construct a ProxyBusObject
     */
    void ObjectDiscovered(
        /* [in] */ const String& busname,
        /* [in] */ const String& path,
        /* [in] */ ArrayOf<String>* interfaces,
        /* [in] */ Int32 sessionId);

    /**
     * Native tells us an object has disappeared.
     *
     * @param busname unique bus name
     * @param path object path
     */
    void ObjectLost(
        /* [in] */ const String& busname,
        /* [in] */ const String& path);

    /**
     * Extract AllJoyn interface name from a Java interface.
     */
    static String GetBusInterfaceName(
        /* [in] */ IInterfaceInfo* intf);

private:
    friend class NativeObserver;

    /**
     * Points to the underlying native JObserver
     */
    Int64 mHandle;

    /** Bus attachment used by this Observer */
    IBusAttachment* mBus;

    /** Proxies for the discovered objects */
    AutoPtr<INavigableMap> mProxies;  // NavigableMap<ObjectId, ProxyBusObject>

    /** Registered listeners */
    List<AutoPtr<WrappedListener> > mListeners;

    /** Maps AllJoyn interface names to Java interfaces (needed for ProxyBusObject creation) */
    HashMap<String, AutoPtr<IInterfaceInfo> > mInterfaceMap;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_OBSERVER_H__
