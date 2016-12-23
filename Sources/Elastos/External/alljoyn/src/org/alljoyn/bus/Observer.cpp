#include "org/alljoyn/bus/Observer.h"
#include "org/alljoyn/bus/Globals.h"
#include "org/alljoyn/bus/NativeObserver.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

#include <qcc/String.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::CTreeMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("Observer");
static const Boolean DEBUG = FALSE;

//============================================================================
// Observer::ObjectId
//============================================================================

CAR_INTERFACE_IMPL(Observer::ObjectId, Object, IComparable)

Observer::ObjectId::ObjectId(
    /* [in] */ const String& _busname,
    /* [in] */ const String& _path)
    : mBusname(_busname)
    , mPath(_path)
{
}

Observer::ObjectId::ObjectId(
    /* [in] */ IProxyBusObject* proxy)
{
    assert(proxy != NULL);
    proxy->GetBusName(&mBusname);
    proxy->GetObjPath(&mPath);
}

ECode Observer::ObjectId::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = mBusname.GetHashCode() ^ mPath.GetHashCode();
    return NOERROR;
}

ECode Observer::ObjectId::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IComparable* cmp = IComparable::Probe(other);
    if (cmp == NULL) {
        return NOERROR;
    }

    if (cmp == this) {
        *result = TRUE;
        return NOERROR;
    }

    ObjectId* oid = (ObjectId*)cmp;
    *result = mBusname.Equals(oid->mBusname) && mPath.Equals(oid->mPath);
    return NOERROR;
}

ECode Observer::ObjectId::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 1;

    IComparable* cmp = IComparable::Probe(other);
    if (cmp == NULL) {
        return NOERROR;
    }

    ObjectId* oid = (ObjectId*)cmp;
    Int32 nameCmp = mBusname.Compare(oid->mBusname);
    if (nameCmp == 0) {
        return mPath.Compare(oid->mPath);
    }
    *result = nameCmp;
    return NOERROR;
}

//============================================================================
// Observer
//============================================================================
CAR_INTERFACE_IMPL_2(Observer, Object, IObserver, ICloseable)

Observer::Observer()
    : mHandle(0)
    , mBus(0)
{
}

Observer::~Observer()
{
    Close();
}

ECode Observer::constructor(
    /* [in] */ IBusAttachment* bus,
    /* [in] */ ArrayOf<IInterfaceInfo*>* mandatoryInterfaces,
    /* [in] */ ArrayOf<IInterfaceInfo*>* optionalInterfaces)
{
    if (mandatoryInterfaces == NULL || mandatoryInterfaces->GetLength() == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mBus = bus;
    CTreeMap::New((INavigableMap**)&mProxies);

    /* build the list of mandatory AllJoyn interface names */
    IInterfaceInfo* info;
    AutoPtr<ArrayOf<String> > mandatoryNames = ArrayOf<String>::Alloc(mandatoryInterfaces->GetLength());
    for (Int32 i = 0; i < mandatoryInterfaces->GetLength(); ++i) {
        info = (*mandatoryInterfaces)[i];
        String name = GetBusInterfaceName(info);
        (*mandatoryNames)[i] = name;
        mInterfaceMap[name] = info;
    }
    if (optionalInterfaces != NULL) {

        for (Int32 i = 0; i < optionalInterfaces->GetLength(); ++i) {
            info = (*optionalInterfaces)[i];
            String name = GetBusInterfaceName(info);
            mInterfaceMap[name] = info;
        }
    }

    Create(bus, mandatoryNames);
    return NOERROR;
}

ECode Observer::constructor(
    /* [in] */ IBusAttachment* bus,
    /* [in] */ ArrayOf<IInterfaceInfo*>* mandatoryInterfaces)
{
    return constructor(bus, mandatoryInterfaces, NULL);
}

ECode Observer::Close()
{
    // Add an explicit close function, so we don't have to rely on the
    // finalizer to clean-up.
    Destroy();
    mListeners.Clear();
    mProxies->Clear();
    mInterfaceMap.Clear();
    mBus = NULL;
    return NOERROR;
}

ECode Observer::RegisterListener(
    /* [in] */ IObserverListener* listener,
    /* [in] */ Boolean triggerOnExisting)
{
    AutoPtr<WrappedListener> wrpper = new WrappedListener(listener, !triggerOnExisting);
    mListeners.PushBack(wrpper);
    if (triggerOnExisting) {
        TriggerEnablePendingListeners();
    }
    return NOERROR;
}

ECode Observer::RegisterListener(
    /* [in] */ IObserverListener* listener)
{
    return RegisterListener(listener, TRUE);
}

ECode Observer::UnregisterListener(
    /* [in] */ IObserverListener* listener)
{
    List<AutoPtr<WrappedListener> >::Iterator it;
    for (it = mListeners.Begin(); it != mListeners.End(); ++it) {
        if (Object::Equals((*it)->mListener, listener)) {
            mListeners.Erase(it);
            break;
        }
    }
    return NOERROR;
}

ECode Observer::UnregisterAllListeners()
{
    mListeners.Clear();
    return NOERROR;
}

ECode Observer::Get(
    /* [in] */ const String& busname,
    /* [in] */ const String& path,
    /* [out] */ IProxyBusObject** object)
{
    VALIDATE_NOT_NULL(object)
    // both busname and path could be null arguments.
    // No extra checks added as passing null is harmless
    AutoPtr<ObjectId> oid = new ObjectId(busname, path);
    AutoPtr<IInterface> obj;
    mProxies->Get(TO_IINTERFACE(oid), (IInterface**)&obj);
    *object = IProxyBusObject::Probe(obj);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode Observer::GetFirst(
    /* [out] */ IProxyBusObject** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;

    Int32 size;
    mProxies->GetSize(&size);
    if (size == 0) {
        return NOERROR;
    }

    AutoPtr<IMapEntry> entry;
    mProxies->GetFirstEntry((IMapEntry**)&entry);
    AutoPtr<IInterface> obj;
    entry->GetValue((IInterface**)&obj);
    *object = IProxyBusObject::Probe(obj);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode Observer::GetNext(
    /* [in] */ IProxyBusObject* current,
    /* [out] */ IProxyBusObject** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;

    if (current == NULL) {
        return NOERROR;
    }

    AutoPtr<IMapEntry> entry;
    AutoPtr<ObjectId> oid = new ObjectId(current);
    mProxies->GetHigherEntry(TO_IINTERFACE(oid), (IMapEntry**)&entry);
    if (entry == NULL) {
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    entry->GetValue((IInterface**)&obj);
    *object = IProxyBusObject::Probe(obj);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

void Observer::Create(
    /* [in] */ IBusAttachment* bus,
    /* [in] */ ArrayOf<String>* interfaceNames)
{
    typedef std::set<qcc::String> InterfaceSet;
    InterfaceSet mandatory;
    int size = interfaceNames->GetLength();
    for (int i = 0; i < size; ++i) {
        String jstr = (*interfaceNames)[i];
        if (jstr == NULL) {
            Logger::E(TAG, "NullPointerException");
            return;
        }
        else {
            /* nested scope here is needed to ensure scopedstring is
             * descoped before the DeleteLocalRef invocation */
            mandatory.insert(jstr.string());
        }
    }

    mHandle = reinterpret_cast<Int64>(new NativeObserver(bus, this, mandatory));
}

void Observer::Destroy()
{
    NativeObserver* observer = reinterpret_cast<NativeObserver*>(mHandle);
    if (observer) {
        observer->Detach();  // ObserverManager will take care of deletion
        mHandle = 0;
    }
}

void Observer::TriggerEnablePendingListeners()
{
    NativeObserver* observer = reinterpret_cast<NativeObserver*>(mHandle);
    if (observer) {
        observer->TriggerEnablePendingListeners();
    }
}

void Observer::EnablePendingListeners()
{
    List<AutoPtr<IObserverListener> > pendingListeners;

    {
        AutoLock lock(this);
        List<AutoPtr<WrappedListener> >::Iterator it;
        for (it = mListeners.Begin(); it != mListeners.End(); ++it) {
            if (!(*it)->mEnabled) {
                pendingListeners.PushBack((*it)->mListener);
                (*it)->mEnabled = TRUE;
            }
        }
    }

    /* we have threading guarantees from the Core that the proxies set will not change
     * during this call. Therefore, we don't need to do anything special to safeguard
     * proxies during iteration. */
    AutoPtr<ICollection> collection;
    mProxies->GetValues((ICollection**)&collection);
    List<AutoPtr<IObserverListener> >::Iterator it;
    for (it = pendingListeners.Begin(); it != pendingListeners.End(); ++it) {
        AutoPtr<IIterator> pit;
        collection->GetIterator((IIterator**)&pit);
        Boolean hasNext;
        while (pit->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            pit->GetNext((IInterface**)&obj);
            (*it)->ObjectDiscovered(IProxyBusObject::Probe(obj));
        }
    }
}

void Observer::ObjectDiscovered(
    /* [in] */ const String& busname,
    /* [in] */ const String& path,
    /* [in] */ ArrayOf<String>* interfaces,
    /* [in] */ Int32 sessionId)
{
    if (DEBUG) {
		Logger::I(TAG, " >> ObjectDiscovered: busname: %s, path: %s, sessionId: %08x", busname.string(), path.string(), sessionId);
		for (Int32 i = 0; i < interfaces->GetLength(); ++i) {
		    Logger::I(TAG, "     > interface %d: %s", i, (*interfaces)[i].string());
		}
	}

    List<AutoPtr<IInterfaceInfo> > intfList;

    HashMap<String, AutoPtr<IInterfaceInfo> >::Iterator mit;
    for (Int32 i = 0; i < interfaces->GetLength(); ++i) {
        mit = mInterfaceMap.Find((*interfaces)[i]);
        if (mit != mInterfaceMap.End()) {
            intfList.PushBack(mit->mSecond);
        }
    }

    //TODO figure out what to do with secure bus objects
    Int32 i = 0;
    AutoPtr<ArrayOf<IInterfaceInfo*> > infos = ArrayOf<IInterfaceInfo*>::Alloc(intfList.GetSize());
    List<AutoPtr<IInterfaceInfo> >::Iterator lit;
    for (lit = intfList.Begin(); lit != intfList.End(); ++lit) {
        infos->Set(i++, *lit);
    }
    AutoPtr<IProxyBusObject> proxy;
    mBus->GetProxyBusObject(busname, path, sessionId, infos, (IProxyBusObject**)&proxy);

    AutoPtr< List<AutoPtr<WrappedListener> > > copiedListeners;
    {
        AutoLock lock(this);
        AutoPtr<ObjectId> oid = new ObjectId(busname, path);
        mProxies->Put(TO_IINTERFACE(oid), proxy);
        copiedListeners = new List<AutoPtr<WrappedListener> >(mListeners);
    }

    /* we do the listener invocation outside of the critical region to avoid
     * lock ordering issues */
    List<AutoPtr<WrappedListener> >::Iterator wit;
    for (wit = copiedListeners->Begin(); wit != copiedListeners->End(); ++wit) {
        if ((*wit)->mEnabled) {
            // protect against exceptions in listener code.
            if (DEBUG) {
				Logger::I(TAG, " >> %s ObjectDiscovered: %s", TO_CSTR((*wit)->mListener), TO_CSTR(proxy));
			}
            (*wit)->mListener->ObjectDiscovered(proxy);
        }
    }
}

void Observer::ObjectLost(
    /* [in] */ const String& busname,
    /* [in] */ const String& path)
{
    AutoPtr< List<AutoPtr<WrappedListener> > > copiedListeners;
    AutoPtr<IProxyBusObject> proxy;
    {
        AutoLock lock(this);
        AutoPtr<ObjectId> oid = new ObjectId(busname, path);
        AutoPtr<IInterface> obj;
        mProxies->Remove(TO_IINTERFACE(oid), (IInterface**)&obj);
        if (obj != NULL) {
            proxy = IProxyBusObject::Probe(obj);
            copiedListeners = new List<AutoPtr<WrappedListener> >(mListeners);
        }
    }

    if (proxy == NULL) {
        return;
    }

    /* we do the listener invocation outside of the critical region to avoid
     * lock ordering issues */
    List<AutoPtr<WrappedListener> >::Iterator wit;
    for (wit = copiedListeners->Begin(); wit != copiedListeners->End(); ++wit) {
        if ((*wit)->mEnabled) {
            // protect against exceptions in listener code.
            (*wit)->mListener->ObjectLost(proxy);
        }
    }
}

String Observer::GetBusInterfaceName(
    /* [in] */ IInterfaceInfo* intf)
{
    AutoPtr<IAnnotationInfo> annotation;
    intf->GetAnnotation(Globals::Annotation_BusInterface, (IAnnotationInfo**)&annotation);

    if (annotation != NULL) {
        String name;
        annotation->GetValue(String("name"), &name);
        if (!name.IsNullOrEmpty()) {
            return name;
        }
    }

    String name, ns;
    intf->GetName(&name);
    intf->GetNamespace(&ns);
    if (ns.IsNullOrEmpty()) {
        return name;
    }

    StringBuilder sb(ns);
    sb += ".";
    sb += name;
    return sb.ToString();
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

