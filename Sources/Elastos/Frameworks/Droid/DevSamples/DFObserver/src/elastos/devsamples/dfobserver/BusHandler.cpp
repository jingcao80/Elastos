#include "BusHandler.h"
#include "DoorAdapterItem.h"
#include "CDoorService.h"
#include "CDoorEventListener.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Org::Alljoyn::Bus::IVariant;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::IMessageContext;
using Org::Alljoyn::Bus::IBusObject;
using Org::Alljoyn::Bus::CObserver;
using Org::Alljoyn::Bus::CAboutObj;
using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::IMutableInteger16Value;
using Org::Alljoyn::Bus::CMutableInteger16Value;
using Org::Alljoyn::Bus::IPropertiesChangedListener;
using Org::Alljoyn::Bus::EIID_IObserverListener;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Core::IClassLoader;
using Elastos::Core::CPathClassLoader;
using Elastos::Core::IBoolean;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

static const String TAG("BusHandler");

//======================================================================
// BusHandler::DoorEventListener
//======================================================================

CAR_INTERFACE_IMPL(BusHandler::DoorEventListener, PropertiesChangedListener, IDoorEventListener)

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

    StringBuilder sb("personPassedThrough person:(");
    sb += person;
    sb += ") event received from ";
    sb += sender;

    Logger::I(TAG, " >> PersonPassedThrough: %s", sb.ToString().string());
    mHost->mDoorAdapter->SendSignal(sb.ToString());
    return NOERROR;
}

ECode BusHandler::DoorEventListener::PropertiesChanged(
    /* [in] */ IProxyBusObject* pObj,
    /* [in] */ const String& ifaceName,
    /* [in] */ IMap* changed,
    /* [in] */ ArrayOf<String>* invalidated)
{
    AutoPtr<ICharSequence> csq = CoreUtils::Convert("IsOpen");
    AutoPtr<IInterface> obj;
    changed->Get(csq, (IInterface**)&obj);

    Logger::I(TAG, " >> PropertiesChanged: %s, ifaceName: %s, changed: %s",
        TO_CSTR(pObj), ifaceName.string(), TO_CSTR(obj));

    IVariant* v = IVariant::Probe(obj);
    if (v != NULL) {
        AutoPtr<IInterface> valueObj;
        v->GetObject((IInterface**)&valueObj);
        Boolean value;
        IBoolean::Probe(valueObj)->GetValue(&value);

        AutoPtr<ISet> set;
        mHost->mMap->GetEntrySet((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext = FALSE;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            IMapEntry* entry = IMapEntry::Probe(p);
            AutoPtr<IInterface> second;
            entry->GetValue((IInterface**)&second);
            if (pObj == IProxyBusObject::Probe(second)) {
                AutoPtr<IInterface> first;
                entry->GetKey((IInterface**)&first);
                IDoorAdapterItem* item = IDoorAdapterItem::Probe(first);
                item->SetIsOpen(value);
                mHost->mDoorAdapter->PropertyUpdate(item);
            }
        }
    }
    return NOERROR;
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
// BusHandler::ObserverListener
//=================================================================
CAR_INTERFACE_IMPL(BusHandler::ObserverListener, Object, IObserverListener)

BusHandler::ObserverListener::ObserverListener(
    /* [in] */ BusHandler* host)
    : mHost(host)
{}

ECode BusHandler::ObserverListener::ObjectDiscovered(
    /* [in] */ IProxyBusObject* object)
{
    Logger::D(TAG, " >> ObjectDiscovered: %s", TO_CSTR(object));

    // A new door is found. Send a message to our worker thread.
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(BusHandler::JOIN_SESSION, object, (IMessage**)&msg);
    Boolean bval;
    return mHost->SendMessage(msg, &bval);
}

ECode BusHandler::ObserverListener::ObjectLost(
    /* [in] */ IProxyBusObject* object)
{
    Logger::D(TAG, " >> ObjectLost: %s", TO_CSTR(object));

    // A door is no longer available. removed it from the local
    // list.
    AutoPtr<ISet> set;
    mHost->mMap->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        IMapEntry* entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> second;
        entry->GetValue((IInterface**)&second);
        if (object == IProxyBusObject::Probe(second)) {
            AutoPtr<IInterface> first;
            entry->GetKey((IInterface**)&first);
            // Notify the UI.
            mHost->mDoorAdapter->Remove(IDoorAdapterItem::Probe(first));
            // Remove from internal map.
            mHost->mMap->Remove(first);
            return NOERROR;
        }
    }
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

static AutoPtr<IInterfaceInfo> GetDoorInterfaceInfo()
{
    AutoPtr<IClassLoader> loader;
    CPathClassLoader::New(String("/system/priv-app/Elastos.DevSamples.DFObserver.eco"), NULL, (IClassLoader**)&loader);
    AutoPtr<IInterfaceInfo> ifceInfo;
    loader->LoadInterface(String("Elastos.DevSamples.DFObserver.IDoor"), (IInterfaceInfo**)&ifceInfo);
    return ifceInfo;
}

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
    ECode ec = NOERROR;

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        /*
         * Connect to a remote instance of an object implementing the
         * BasicInterface.
         */
        case CONNECT: {
            Logger::I(TAG, " >> HandleMessage CONNECT");

            IActivity* client = IActivity::Probe(obj);
            AutoPtr<IContext> context;
            IContext::Probe(client)->GetApplicationContext((IContext**)&context);
            AutoPtr<IDaemonInit> di;
            CDaemonInit::AcquireSingleton((IDaemonInit**)&di);
            Boolean result;
            di->PrepareDaemon(context, &result);

            /*
             * All communication through AllJoyn begins with a
             * BusAttachment.
             *
             * A BusAttachment needs a name. The actual name is unimportant
             * except for internal security. As a default we use the class
             * name as the name.
             *
             * By default AllJoyn does not allow communication between
             * devices (i.e. bus to bus communication). The second argument
             * must be set to Receive to allow communication between
             * devices.
             */
            String pkgName;
            IContext::Probe(client)->GetPackageName(&pkgName);
            CBusAttachment::New(pkgName, RemoteMessage_Receive, (IBusAttachment**)&mBus);

            /*
             * To communicate with AllJoyn objects, we must connect the
             * BusAttachment to the bus.
             */
            ec = mBus->Connect();
            if (ec != (ECode)E_STATUS_OK) {
                Logger::E(TAG, "Failed to connect BusAttachment.");
                client->Finish();
                return NOERROR;
            }

            /*
             * Create a SessionPortListener, so we provider door locally and
             * have peers set-up a session and used the doors.
             */
            AutoPtr<IMutableInteger16Value> contactPort;
            CMutableInteger16Value::New(CONTACT_PORT, (IMutableInteger16Value**)&contactPort);
            AutoPtr<ISessionOpts> sessionOpts;
            CSessionOpts::New(ISessionOpts::TRAFFIC_MESSAGES, FALSE,
                ISessionOpts::PROXIMITY_ANY, ISessionOpts::TRANSPORT_ANY, (ISessionOpts**)&sessionOpts);

            AutoPtr<InnerSessionPortListener> listener = new InnerSessionPortListener();
            ec = mBus->BindSessionPort(contactPort, sessionOpts, listener);
            if (ec != (ECode)E_STATUS_OK) {
                Logger::E(TAG, "Unable to bind session contact port: (%08x)", ec);
                return ec;
            }

            /*
             * Create About data and ask the About service to publish this.
             */
            CAboutObj::New(mBus, (IAboutObj**)&mAboutObj);
            mAboutData = new MyAboutData();
            ec = mAboutObj->Announce(CONTACT_PORT, mAboutData);
            if (ec != (ECode)E_STATUS_OK) {
                Logger::I(TAG, "Problem while sending about info");
                return ec;
            }

            /*
             * Register a listener in order to receive signals emmited by doors.
             */
            CDoorEventListener::New(this, (IDoorEventListener**)&mDoorListener);
            ec = mBus->RegisterSignalHandlers(mDoorListener);
            if (ec != (ECode)E_STATUS_OK) {
                Logger::I(TAG, "Problem while registering signal handler");
                return ec;
            }

            /*
             * Create an observer and add a listener to it.
             */
            AutoPtr< ArrayOf<IInterfaceInfo*> > mandatoryInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
            mandatoryInterfaces->Set(0, GetDoorInterfaceInfo());
            CObserver::New(mBus, mandatoryInterfaces, (IObserver**)&mObserver);

            AutoPtr<ObserverListener> ol = new ObserverListener(this);
            mObserver->RegisterListener(ol);
            break;
        }

        case JOIN_SESSION: {
            Logger::I(TAG, " >> HandleMessage JOIN_SESSION: %s", TO_CSTR(obj));

            /*
             * We have discovered a new door. Add it to our list and register
             * event listeners on it.
             */
            AutoPtr<IProxyBusObject> proxy = IProxyBusObject::Probe(obj);
            proxy->EnablePropertyCaching();
            AutoPtr<IDoor> door;
            proxy->GetInterface(EIID_IDoor, (IInterface**)&door);

            StringBuilder sb("");
            String str;
            door->GetLocation(&str);
            sb += str;
            sb += " (";
            proxy->GetBusName(&str);
            sb += str;
            proxy->GetObjPath(&str);
            sb += str;
            sb += ")";
            String displayName = sb.ToString();
            Logger::I(TAG, " new DoorAdapterItem: %s", displayName.string());
            door->SetDisplayName(displayName);
            Boolean isOpen;
            door->GetIsOpen(&isOpen);
            AutoPtr<IDoorAdapterItem> item = (IDoorAdapterItem*)new DoorAdapterItem(displayName, isOpen);
            mMap->Put(item, proxy);
            AutoPtr<ArrayOf<String> > props = ArrayOf<String>::Alloc(1);
            props->Set(0, String("IsOpen"));
            proxy->RegisterPropertiesChangedListener(IDoor::DOOR_INTF_NAME, props,
                IPropertiesChangedListener::Probe(mDoorListener));
            mDoorAdapter->Add(item);
            break;
        }

        /* Release all resources acquired in the connect. */
        case DISCONNECT: {
            Logger::I(TAG, " >> HandleMessage DISCONNECT");

            ICloseable::Probe(mObserver)->Close();
            List< AutoPtr<IDoorService> >::Iterator it;
            for (it = mDoors.Begin(); it != mDoors.End(); ++it) {
                mBus->UnregisterBusObject(IBusObject::Probe(*it));
            }
            mDoors.Clear();
            mBus->Disconnect();
            AutoPtr<ILooper> looper;
            GetLooper((ILooper**)&looper);
            looper->Quit();
            break;
        }

        /*
         * Call the service's KnockOnDoor method through the ProxyBusObject.
         */
        case KNOCK_ON_DOOR: {
            Logger::I(TAG, " >> HandleMessage KNOCK_ON_DOOR");

            AutoPtr<IInterface> pboObj;
            mMap->Get(obj, (IInterface**)&pboObj);
            IProxyBusObject* pbo = IProxyBusObject::Probe(pboObj);
            if (pbo != NULL) {
                AutoPtr<IInterface> doorObj;
                pbo->GetInterface(EIID_IDoor, (IInterface**)&doorObj);
                IDoor* door = IDoor::Probe(doorObj);
                assert(door);
                door->KnockAndRun();
            }
            break;
        }

        /*
         * Call Toggle a door.
         */
        case TOGGLE_DOOR: {
            Logger::I(TAG, " >> HandleMessage TOGGLE_DOOR");

            AutoPtr<IInterface> pboObj;
            mMap->Get(obj, (IInterface**)&pboObj);
            IProxyBusObject* pbo = IProxyBusObject::Probe(pboObj);
            if (pbo != NULL) {
                AutoPtr<IInterface> doorObj;
                pbo->GetInterface(EIID_IDoor, (IInterface**)&doorObj);
                IDoor* door = IDoor::Probe(doorObj);
                assert(door);
                Boolean isOpen;
                door->GetIsOpen(&isOpen);
                if (isOpen) {
                    door->Close();
                }
                else {
                    door->Open();
                }
            }
            break;
        }

        /*
         * Creates a local door
         */
        case CREATE_DOOR: {
            Logger::I(TAG, " >> HandleMessage CREATE_DOOR");

            String location = Object::ToString(obj);
            AutoPtr<IDoorService> newDoor;
            CDoorService::New(location, this, (IDoorService**)&newDoor);
            if (!location.StartWith("/")) {
                location = String("/") + location;
            }
            // Register the door on the local bus.
            if ((ECode)E_STATUS_OK != mBus->RegisterBusObject(IBusObject::Probe(newDoor), location)) {
                StringBuilder sb("Failed to create door '");
                sb += location; sb += "'";
                Logger::E(TAG, sb.ToString());
                mDoorAdapter->SendDoorEvent(sb.ToString());
                break;
            }
            Logger::I(TAG, " >> HandleMessage created door: %s", location.string());

            mDoors.PushBack(newDoor);
            // Have About send a signal a new door is available.
            mAboutObj->Announce(CONTACT_PORT, mAboutData);
            break;
        }

        case DELETE_DOOR: {
            Logger::I(TAG, " >> HandleMessage DELETE_DOOR");

            AutoPtr<IArrayList> locations = IArrayList::Probe(obj);
            List< AutoPtr<IDoorService> >::Iterator it;
            Boolean bval;
            String il;
            for (it = mDoors.Begin(); it != mDoors.End(); ) {
                AutoPtr<IDoorService> door = *it;
                door->GetInternalLocation(&il);
                AutoPtr<ICharSequence> csq = CoreUtils::Convert(il);
                locations->Remove(csq, &bval);
                if (bval) {
                    it = mDoors.Erase(it);

                    // unregister the door from the bus.
                    mBus->UnregisterBusObject(IBusObject::Probe(door));
                    locations->IsEmpty(&bval);
                    if (bval) {
                        // All doors are removed. Have About send a signal some
                        // doors no longer are available.
                        mAboutObj->Announce(CONTACT_PORT, mAboutData);
                        break;
                    }
                }
                else {
                    ++it;
                }
            }
            break;
        }

        /*
         * Executes a local task.
         */
        case EXECUTE_TASK: {
            Logger::I(TAG, " >> HandleMessage EXECUTE_TASK %s", TO_CSTR(obj));

            IRunnable* task = IRunnable::Probe(obj);
            task->Run();
            break;
        }

        default:
            break;
    }
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