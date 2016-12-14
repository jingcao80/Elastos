#include "BusHandler.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::IMessageContext;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

static const String TAG("BusHandler");

//======================================================================
// BusHandler::DoorEventListener
//======================================================================

ECode BusHandler::DoorEventListener::constructor(
    /* [in] */ IHandler* busHandler)
{
    mHost = (BusHandler*)busHandler;
    return PropertiesChangedListener::constructor();
}

ECode BusHandler::DoorEventListener::PersonPassedThrough(
    /* [in] */ const String& person)
{
    AutoPtr<IMessageContext> msgCtx;
    mHost->mBus->GetMessageContext((IMessageContext**)&msgCtx);
    String sender;
    msgCtx->GetSender(&sender);

    StringBuilder sb("personPassedThrough");
    sb += person;
    sb += ") event received from ";
    sb += sender;
    mHost->mDoorAdapter->SendSignal(sb.ToString());
    return NOERROR;
}

ECode BusHandler::DoorEventListener::PropertiesChanged(
    /* [in] */ IProxyBusObject* pObj,
    /* [in] */ const String& ifaceName,
    /* [in] */ IMap* changed,
    /* [in] */ ArrayOf<String>* invalidated)
{
    assert(0 && "TODO");
    // Variant v = changed.get("IsOpen");
    // if (v != null) {
    //     try {
    //         Boolean value = v.getObject(Boolean.class);
    //         for (Map.Entry<DoorAdapterItem, ProxyBusObject> entry : map.entrySet()) {
    //             if (pObj == entry.getValue()) {
    //                 DoorAdapterItem item = entry.getKey();
    //                 item.setIsOpen(value);
    //                 mDoorAdapter.propertyUpdate(item);
    //             }
    //         }
    //     } catch (BusException e) {
    //         e.printStackTrace();
    //     }
    // }
}

//=================================================================
// BusHandler::InnerSessionPortListener
//=================================================================
ECode BusHandler::InnerSessionPortListener::AcceptSessionJoiner(
    /* [in] */ Int16 sessionPort,
    /* [in] */ const String& joiner,
    /* [in] */ ISessionOpts* opts,
    /* [out] */ Boolean* accepted)
{
    Logger::I(TAG, "SessionPortListener.acceptSessionJoiner(%d, %s, %s)", sessionPort, joiner.string(), TO_CSTR(opts));
    VALIDATE_NOT_NULL(accepted);
    // Allow all connections on our contact port.
    *accepted = sessionPort == BusHandler::CONTACT_PORT;
    return NOERROR;
}

//=================================================================
// BusHandler
//=================================================================

const Int32 BusHandler::CONNECT;
const Int32 BusHandler::JOIN_SESSION;
const Int32 BusHandler::DISCONNECT;
const Int32 BusHandler::KNOCK_ON_DOOR;
const Int32 BusHandler::TOGGLE_DOOR;
const Int32 BusHandler::CREATE_DOOR;
const Int32 BusHandler::EXECUTE_TASK;
const Int32 BusHandler::DELETE_DOOR;

/** The port used to listen on for new incoming messages. */
const Int16 BusHandler::CONTACT_PORT;

BusHandler::BusHandler(
    /* [in] */ DoorAdapter* adapter,
    /* [in] */ ILooper* looper)
    : Handler(looper)
{
    mDoorAdapter = adapter;
    CConcurrentHashMap::New((IConcurrentHashMap**)&mMap);
}

ECode BusHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    // switch (what) {
    // /*
    //  * Connect to a remote instance of an object implementing the
    //  * BasicInterface.
    //  */
    // case CONNECT: {
    //     AutoPtr<IInterface> obj;
    //     msg->GetObj((IInterface**)&obj);
    //     IActivity* client = IActivity::Probe(obj);

    //     AutoPtr<IContext> context;
    //     client->GetApplicationContext((IContext**)&context);
    //     AutoPtr<IDaemonInit> di;
    //     CDaemonInit::AcquireSingleton((IDaemonInit**)&di);
    //     Boolean result;
    //     di->PrepareDaemon(context, &result);

    //     /*
    //      * All communication through AllJoyn begins with a
    //      * BusAttachment.
    //      *
    //      * A BusAttachment needs a name. The actual name is unimportant
    //      * except for internal security. As a default we use the class
    //      * name as the name.
    //      *
    //      * By default AllJoyn does not allow communication between
    //      * devices (i.e. bus to bus communication). The second argument
    //      * must be set to Receive to allow communication between
    //      * devices.
    //      */
    //     String pkgName;
    //     client->GetPackageName(&pkgName);
    //     CBusAttachment::New(pkgName, RemoteMessage_Receive, (IBusAttachment**)&mBus);

    //     /*
    //      * To communicate with AllJoyn objects, we must connect the
    //      * BusAttachment to the bus.
    //      */
    //     ECode ec = mBus->Connect();
    //     if (ec != (ECode)E_STATUS_OK) {
    //         Logger::E(TAG, "Failed to connect BusAttachment.");
    //         client->Finish();
    //         return NOERROR;
    //     }

    //     /*
    //      * Create a SessionPortListener, so we provider door locally and
    //      * have peers set-up a session and used the doors.
    //      */
    //     AutoPtr<IMutableInteger16Value> contactPort;
    //     CMutableInteger16Value::New(CONTACT_PORT, (IMutableInteger16Value**)&contactPort);
    //     AutoPtr<ISessionOpts> sessionOpts;
    //     CSessionOpts::New(ISessionOpts::TRAFFIC_MESSAGES, FALSE,
    //         ISessionOpts::PROXIMITY_ANY, ISessionOpts::TRANSPORT_ANY, (ISessionOpts**)&sessionOpts);

    //     AutoPtr<InnerSessionPortListener> listener = new InnerSessionPortListener(this);
    //     ec = mBus->BindSessionPort(contactPort, sessionOpts, listener);
    //     if (ec != (ECode)E_STATUS_OK) {
    //         Logger::E(TAG, "Unable to bind session contact port: (%08x)", ec);
    //         return;
    //     }

    //     /*
    //      * Create About data and ask the About service to publish this.
    //      */
    //     CAboutObj::New(mBus, (IAboutObj**)&mAboutObj);
    //     mAboutData = new MyAboutData();
    //     ec = mAboutObj->Announce(CONTACT_PORT, mAboutData);
    //     if (ec != (ECode)E_STATUS_OK) {
    //         Logger::I(TAG, "Problem while sending about info");
    //         return;
    //     }

    //     /*
    //      * Register a listener in order to receive signals emmited by doors.
    //      */
    //     mDoorListener = new DoorEventListener(this);
    //     ec = mBus->RegisterSignalHandlers(mDoorListener);
    //     if (ec != (ECode)E_STATUS_OK) {
    //         Logger::I(TAG, "Problem while registering signal handler");
    //         return;
    //     }

    //     /*
    //      * Create an observer and add a listener to it.
    //      */
    //     mObserver = new Observer(mBus, new Class[] { Door.class });
    //     mObserver.registerListener(new Observer.Listener() {
    //         @Override
    //         public void objectLost(ProxyBusObject obj) {
    //             // A door is no longer available. removed it from the local
    //             // list.
    //             for (Map.Entry<DoorAdapterItem, ProxyBusObject> entry : map.entrySet()) {
    //                 if (obj == entry.getValue()) {
    //                     // Notify the UI.
    //                     mDoorAdapter.remove(entry.getKey());
    //                     // Remove from internal map.
    //                     map.remove(entry.getKey());
    //                     return;
    //                 }
    //             }
    //         }

    //         @Override
    //         public void objectDiscovered(ProxyBusObject obj) {
    //             // A new door is found. Send a message to our worker thread.
    //             Message msg = obtainMessage(JOIN_SESSION);
    //             msg.obj = obj;
    //             sendMessage(msg);
    //         }
    //     });
    //     break;
    // }
    // case JOIN_SESSION: {
    //     /*
    //      * We have discovered a new door. Add it to our list and register
    //      * event listeners on it.
    //      */
    //     ProxyBusObject obj = (ProxyBusObject) msg.obj;
    //     obj.enablePropertyCaching();
    //     Door door = obj.getInterface(Door.class);
    //     try {
    //         String displayName = door.getLocation() + " (" + obj.getBusName() + obj.getObjPath() + ")";
    //         DoorAdapterItem item = new DoorAdapterItem(displayName, door.getIsOpen());
    //         map.put(item, obj);
    //         obj.registerPropertiesChangedListener(Door.DOOR_INTF_NAME, new String[] { "IsOpen" }, dl);
    //         mDoorAdapter.add(item);
    //     } catch (BusException e) {
    //         e.printStackTrace();
    //     }
    //     break;
    // }

    // /* Release all resources acquired in the connect. */
    // case DISCONNECT: {
    //     mObserver.close();
    //     for (IDoorService door : doors) {
    //         mBus.unregisterBusObject(door);
    //     }
    //     doors.clear();
    //     mBus.disconnect();
    //     getLooper().quit();
    //     break;
    // }

    // /*
    //  * Call the service's KnockOnDoor method through the ProxyBusObject.
    //  */
    // case KNOCK_ON_DOOR: {
    //     try {
    //         ProxyBusObject obj = map.get(msg.obj);
    //         if (obj != null) {
    //             Door door = obj.getInterface(Door.class);
    //             door.knockAndRun();
    //         }
    //     } catch (BusException ex) {
    //         ex.printStackTrace();
    //     }
    //     break;
    // }

    // /*
    //  * Call Toggle a door.
    //  */
    // case TOGGLE_DOOR: {
    //     try {
    //         ProxyBusObject obj = map.get(msg.obj);
    //         if (obj != null) {
    //             Door door = obj.getInterface(Door.class);
    //             if (door.getIsOpen()) {
    //                 door.close();
    //             } else {
    //                 door.open();
    //             }
    //         }
    //     } catch (BusException ex) {
    //         ex.printStackTrace();
    //     }
    //     break;
    // }
    // /*
    //  * Creates a local door
    //  */
    // case CREATE_DOOR: {
    //     String location = (String) msg.obj;
    //     IDoorService newDoor = new IDoorService(location, this);
    //     if (!location.startsWith("/")) {
    //         location = "/" + location;
    //     }
    //     // Register the door on the local bus.
    //     if ((ECode)E_STATUS_OK != mBus.registerBusObject(newDoor, location)) {
    //         mDoorAdapter.sendDoorEvent("Failed to create door '" + location + "'");
    //         break;
    //     }
    //     doors.add(newDoor);
    //     // Have About send a signal a new door is available.
    //     mAboutObj.announce(CONTACT_PORT, mAboutData);
    //     break;
    // }
    // case DELETE_DOOR: {
    //     @SuppressWarnings("unchecked")
    //     ArrayList<String> locations = (ArrayList<String>) msg.obj;

    //     Iterator<IDoorService> it = doors.iterator();
    //     while (it.hasNext()) {
    //         IDoorService door = it.next();
    //         if (locations.remove(door.getInternalLocation())) {
    //             it.remove();
    //             // unregister the door from the bus.
    //             mBus.unregisterBusObject(door);
    //             if (locations.size() == 0) {
    //                 // All doors are removed. Have About send a signal some
    //                 // doors no longer are available.
    //                 mAboutObj.announce(CONTACT_PORT, mAboutData);
    //                 break;
    //             }
    //         }
    //     }
    //     break;
    // }
    // /*
    //  * Executes a local task.
    //  */
    // case EXECUTE_TASK: {
    //     Runnable task = (Runnable) msg.obj;
    //     task.run();
    // }
    // default:
    //     break;
    // }
    return NOERROR;
}

ECode BusHandler::SendUIMessage(
    /* [in] */ const String& string,
    /* [in] */ IDoorService* doorService)
{
    String str;
    doorService->GetInternalLocation(&str);
    StringBuilder sb(str);
    sb += ": ";
    sb += string;
    mDoorAdapter->SendDoorEvent(sb.ToString());
    return NOERROR;
}

AutoPtr<ArrayOf<String> > BusHandler::GetDoorNames()
{
    AutoPtr<ArrayOf<String> > names = ArrayOf<String>::Alloc(mDoors.GetSize());
    List< AutoPtr<IDoorService> >::Iterator it = mDoors.Begin();
    for (Int32 i = 0; it != mDoors.End(); ++it) {
        String str;
        (*it)->GetInternalLocation(&str);
        names->Set(i++, str);
    }

    return names;
}


} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos