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

#ifndef __Elastos_DevSamples_DFObserver_BusHandler_H__
#define __Elastos_DevSamples_DFObserver_BusHandler_H__

#include "_Elastos.DevSamples.DFObserver.h"
#include <elastos/droid/os/Handler.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <org/alljoyn/bus/SessionPortListener.h>
#include <org/alljoyn/bus/PropertiesChangedListener.h>
#include "MyAboutData.h"
#include "DoorAdapter.h"

using Org::Alljoyn::Bus::IProxyBusObject;
using Org::Alljoyn::Bus::PropertiesChangedListener;
using Org::Alljoyn::Bus::SessionPortListener;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IObserver;
using Org::Alljoyn::Bus::IAboutObj;
using Org::Alljoyn::Bus::IObserverListener;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Utility::Concurrent::IConcurrentHashMap;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

/** This class will handle all AllJoyn calls. */
class BusHandler
    : public Handler
{
public:
    class ObserverListener
        : public Object
        , public IObserverListener
    {
    public:
        TO_STRING_IMPL("BusHandler::ObserverListener")

        CAR_INTERFACE_DECL()

        ObserverListener(
            /* [in] */ BusHandler* host);

        CARAPI ObjectDiscovered(
            /* [in] */ IProxyBusObject* object);

        CARAPI ObjectLost(
            /* [in] */ IProxyBusObject* object);
    private:
        BusHandler* mHost;
    };

    /**
     * The DoorEventListener listens to both propertyChanged events and signals.
     */
    class DoorEventListener
        : public PropertiesChangedListener
        , public IDoorEventListener
    {
    public:
        TO_STRING_IMPL("BusHandler::DoorEventListener")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IHandler* busHandler);

        /*
         * Called when a personPassedThrough signal is received.
         */
        CARAPI PersonPassedThrough(
            /* [in] */ const String& person);

        /*
         * handle incoming property state changes.
         */
        CARAPI PropertiesChanged(
            /* [in] */ IProxyBusObject* pObj,
            /* [in] */ const String& ifaceName,
            /* [in] */ IMap* changed,
            /* [in] */ ArrayOf<String>* invalidated);

    private:
        BusHandler* mHost;
    };

    class InnerSessionPortListener
        : public SessionPortListener
    {
    public:
        TO_STRING_IMPL("BusHandler::InnerSessionPortListener")

        CARAPI AcceptSessionJoiner(
            /* [in] */ Int16 sessionPort,
            /* [in] */ const String& joiner,
            /* [in] */ ISessionOpts* opts,
            /* [out] */ Boolean* accepted);
    };

public:
    TO_STRING_IMPL("BusHandler")

    BusHandler(
        /* [in] */ DoorAdapter* adapter,
        /* [in] */ ILooper* looper);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI SendUIMessage(
        /* [in] */ const String& string,
        /* [in] */ IDoorService* doorService);


    /**
     * Provides the list of local provided doornames
     *
     * @return a non-null possible empty list.
     */
    AutoPtr<ArrayOf<String> > GetDoorNames();

public:
    /*
     * These are the messages sent to the BusHandler from the UI or event
     * callbacks.
     */
    static const Int32 CONNECT = 1;
    static const Int32 JOIN_SESSION = 2;
    static const Int32 DISCONNECT = 3;
    static const Int32 KNOCK_ON_DOOR = 4;
    static const Int32 TOGGLE_DOOR = 5;
    static const Int32 CREATE_DOOR = 6;
    static const Int32 EXECUTE_TASK = 7;
    static const Int32 DELETE_DOOR = 8;

private:
    friend class DoorEventListener;

    AutoPtr<IBusAttachment> mBus;
    AutoPtr<IObserver> mObserver;
    List< AutoPtr<IDoorService> > mDoors;

    AutoPtr<IDoorEventListener> mDoorListener;
    AutoPtr<IConcurrentHashMap> mMap;   // ConcurrentHashMap<DoorAdapterItem, ProxyBusObject>
    AutoPtr<DoorAdapter> mDoorAdapter;
    AutoPtr<IAboutObj> mAboutObj;
    AutoPtr<MyAboutData> mAboutData;

    /** The port used to listen on for new incoming messages. */
    static const Int16 CONTACT_PORT = 2134;
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_BusHandler_H__
