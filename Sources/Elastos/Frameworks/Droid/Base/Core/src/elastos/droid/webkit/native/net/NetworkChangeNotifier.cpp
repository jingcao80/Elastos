
#include "elastos/droid/webkit/native/net/NetworkChangeNotifier.h"
#include "elastos/droid/webkit/native/net/api/NetworkChangeNotifier_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

//=====================================================================
// NetworkChangeNotifier::InnerNetworkChangeNotifierAutoDetectObserver
//=====================================================================
NetworkChangeNotifier::InnerNetworkChangeNotifierAutoDetectObserver::InnerNetworkChangeNotifierAutoDetectObserver(
    /* [in] */ NetworkChangeNotifier* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode NetworkChangeNotifier::InnerNetworkChangeNotifierAutoDetectObserver::OnConnectionTypeChanged(
    /* [in] */ Int32 newConnectionType)
{
    // ==================before translated======================
    // updateCurrentConnectionType(newConnectionType);

    assert(NULL != mOwner);
    mOwner->UpdateCurrentConnectionType(newConnectionType);
    return NOERROR;
}

//=====================================================================
//                        NetworkChangeNotifier
//=====================================================================
const Int32 NetworkChangeNotifier::CONNECTION_UNKNOWN;
const Int32 NetworkChangeNotifier::CONNECTION_ETHERNET;
const Int32 NetworkChangeNotifier::CONNECTION_WIFI;
const Int32 NetworkChangeNotifier::CONNECTION_2G;
const Int32 NetworkChangeNotifier::CONNECTION_3G;
const Int32 NetworkChangeNotifier::CONNECTION_4G;
const Int32 NetworkChangeNotifier::CONNECTION_NONE;
const Int32 NetworkChangeNotifier::CONNECTION_BLUETOOTH;
AutoPtr<NetworkChangeNotifier> NetworkChangeNotifier::sInstance;

AutoPtr<NetworkChangeNotifier> NetworkChangeNotifier::Init(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // if (sInstance == null) {
    //     sInstance = new NetworkChangeNotifier(context);
    // }
    // return sInstance;

    if (sInstance.Get() == NULL) {
        sInstance = new NetworkChangeNotifier(context);
    }
    return sInstance;
}

Boolean NetworkChangeNotifier::IsInitialized()
{
    // ==================before translated======================
    // return sInstance != null;

    return Boolean(sInstance != NULL);
}

ECode NetworkChangeNotifier::ResetInstanceForTests(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context);
    // ==================before translated======================
    // sInstance = new NetworkChangeNotifier(context);

    sInstance = new NetworkChangeNotifier(context);
    return NOERROR;
}

Int32 NetworkChangeNotifier::GetCurrentConnectionType()
{
    // ==================before translated======================
    // return mCurrentConnectionType;

    return mCurrentConnectionType;
}

ECode NetworkChangeNotifier::AddNativeObserver(
    /* [in] */ Int64 nativeChangeNotifier)
{
    // ==================before translated======================
    // mNativeChangeNotifiers.add(nativeChangeNotifier);

    mNativeChangeNotifiers->PushBack(nativeChangeNotifier);
    return NOERROR;
}

ECode NetworkChangeNotifier::RemoveNativeObserver(
    /* [in] */ Int64 nativeChangeNotifier)
{
    // ==================before translated======================
    // mNativeChangeNotifiers.remove(nativeChangeNotifier);

    mNativeChangeNotifiers->Remove(nativeChangeNotifier);
    return NOERROR;
}

AutoPtr<NetworkChangeNotifier> NetworkChangeNotifier::GetInstance()
{
    // ==================before translated======================
    // assert sInstance != null;
    // return sInstance;

    assert (NULL == sInstance);
    return sInstance;
}

ECode NetworkChangeNotifier::SetAutoDetectConnectivityState(
    /* [in] */ Boolean shouldAutoDetect)
{
    // ==================before translated======================
    // getInstance().setAutoDetectConnectivityStateInternal(shouldAutoDetect);

    GetInstance()->SetAutoDetectConnectivityStateInternal(shouldAutoDetect);
    return NOERROR;
}

void NetworkChangeNotifier::ForceConnectivityState(
    /* [in] */ Boolean networkAvailable)
{
    // ==================before translated======================
    // setAutoDetectConnectivityState(false);
    // getInstance().forceConnectivityStateInternal(networkAvailable);

    SetAutoDetectConnectivityState(FALSE);
    GetInstance()->ForceConnectivityStateInternal(networkAvailable);
}

ECode NetworkChangeNotifier::NotifyObserversOfConnectionTypeChange(
    /* [in] */ Int32 newConnectionType)
{
    // ==================before translated======================
    // for (Long nativeChangeNotifier : mNativeChangeNotifiers) {
    //     nativeNotifyConnectionTypeChanged(nativeChangeNotifier, newConnectionType);
    // }
    // for (ConnectionTypeObserver observer : mConnectionTypeObservers) {
    //     observer.onConnectionTypeChanged(newConnectionType);
    // }

    Int64 nativeChangeNotifier;
    for (Int32 i=0; i<(Int32)mNativeChangeNotifiers->GetSize(); ++i) {
        nativeChangeNotifier = (*mNativeChangeNotifiers)[i];
        NativeNotifyConnectionTypeChanged(nativeChangeNotifier, newConnectionType);
    }
    Int32 size = 0;
    mConnectionTypeObservers->GetSize(&size);
    for (Int32 i=0; i<size; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mConnectionTypeObservers->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        ConnectionTypeObserver* observer = (ConnectionTypeObserver*)objTmp;
        observer->OnConnectionTypeChanged(newConnectionType);
    }
    return NOERROR;
}

ECode NetworkChangeNotifier::AddConnectionTypeObserver(
    /* [in] */ ConnectionTypeObserver* observer)
{
    VALIDATE_NOT_NULL(observer);
    // ==================before translated======================
    // getInstance().addConnectionTypeObserverInternal(observer);

    GetInstance()->AddConnectionTypeObserverInternal(observer);
    return NOERROR;
}

ECode NetworkChangeNotifier::RemoveConnectionTypeObserver(
    /* [in] */ ConnectionTypeObserver* observer)
{
    VALIDATE_NOT_NULL(observer);
    // ==================before translated======================
    // getInstance().removeConnectionTypeObserverInternal(observer);

    GetInstance()->RemoveConnectionTypeObserverInternal(observer);
    return NOERROR;
}

AutoPtr<NetworkChangeNotifierAutoDetect> NetworkChangeNotifier::GetAutoDetectorForTest()
{
    // ==================before translated======================
    // return getInstance().mAutoDetector;

    return GetInstance()->mAutoDetector;
}

Boolean NetworkChangeNotifier::IsOnline()
{
    // ==================before translated======================
    // int connectionType = getInstance().getCurrentConnectionType();
    // return connectionType != CONNECTION_UNKNOWN && connectionType != CONNECTION_NONE;

    Int32 connectionType = GetInstance()->GetCurrentConnectionType();
    return Boolean(connectionType != CONNECTION_UNKNOWN && connectionType != CONNECTION_NONE);
}

NetworkChangeNotifier::NetworkChangeNotifier(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mContext = context.getApplicationContext();
    // mNativeChangeNotifiers = new ArrayList<Long>();
    // mConnectionTypeObservers = new ObserverList<ConnectionTypeObserver>();

    context->GetApplicationContext((IContext**)&mContext);
    mNativeChangeNotifiers = new List<Int64>();
    CArrayList::New((IList**)&mConnectionTypeObservers);
}

ECode NetworkChangeNotifier::DestroyAutoDetector()
{
    // ==================before translated======================
    // if (mAutoDetector != null) {
    //     mAutoDetector.destroy();
    //     mAutoDetector = null;
    // }

    if (mAutoDetector.Get() != NULL) {
        mAutoDetector->Destroy();
        mAutoDetector = NULL;
    }
    return NOERROR;
}

ECode NetworkChangeNotifier::SetAutoDetectConnectivityStateInternal(
    /* [in] */ Boolean shouldAutoDetect)
{
    // ==================before translated======================
    // if (shouldAutoDetect) {
    //     if (mAutoDetector == null) {
    //         mAutoDetector = new NetworkChangeNotifierAutoDetect(
    //             new NetworkChangeNotifierAutoDetect.Observer() {
    //                 @Override
    //                 public void onConnectionTypeChanged(int newConnectionType) {
    //                     updateCurrentConnectionType(newConnectionType);
    //                 }
    //             },
    //             mContext);
    //         updateCurrentConnectionType(mAutoDetector.getCurrentConnectionType());
    //     }
    // } else {
    //     destroyAutoDetector();
    // }

    if (shouldAutoDetect) {
        if (NULL == mAutoDetector) {
            AutoPtr<NetworkChangeNotifierAutoDetect::Observer> innerObserver = new InnerNetworkChangeNotifierAutoDetectObserver(this);
            mAutoDetector = new NetworkChangeNotifierAutoDetect(innerObserver, mContext);
            UpdateCurrentConnectionType(mAutoDetector->GetCurrentConnectionType());
        }
    }
    else {
        DestroyAutoDetector();
    }
    return NOERROR;
}

ECode NetworkChangeNotifier::ForceConnectivityStateInternal(
    /* [in] */ Boolean forceOnline)
{
    // ==================before translated======================
    // boolean connectionCurrentlyExists = mCurrentConnectionType != CONNECTION_NONE;
    // if (connectionCurrentlyExists != forceOnline) {
    //     updateCurrentConnectionType(forceOnline ? CONNECTION_UNKNOWN : CONNECTION_NONE);
    // }

    Boolean connectionCurrentlyExists = Boolean(mCurrentConnectionType != CONNECTION_NONE);
    if (connectionCurrentlyExists != forceOnline) {
        UpdateCurrentConnectionType(forceOnline ? CONNECTION_UNKNOWN : CONNECTION_NONE);
    }
    return NOERROR;
}

ECode NetworkChangeNotifier::UpdateCurrentConnectionType(
    /* [in] */ Int32 newConnectionType)
{
    // ==================before translated======================
    // mCurrentConnectionType = newConnectionType;
    // notifyObserversOfConnectionTypeChange(newConnectionType);

    mCurrentConnectionType = newConnectionType;
    NotifyObserversOfConnectionTypeChange(newConnectionType);
    return NOERROR;
}

ECode NetworkChangeNotifier::AddConnectionTypeObserverInternal(
    /* [in] */ ConnectionTypeObserver* observer)
{
    VALIDATE_NOT_NULL(observer);
    // ==================before translated======================
    // mConnectionTypeObservers.addObserver(observer);

    mConnectionTypeObservers->Add(TO_IINTERFACE(observer));
    return NOERROR;
}

ECode NetworkChangeNotifier::RemoveConnectionTypeObserverInternal(
    /* [in] */ ConnectionTypeObserver* observer)
{
    VALIDATE_NOT_NULL(observer);
    // ==================before translated======================
    // mConnectionTypeObservers.removeObserver(observer);

    mConnectionTypeObservers->Remove(TO_IINTERFACE(observer));
    return NOERROR;
}

ECode NetworkChangeNotifier::NativeNotifyConnectionTypeChanged(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 newConnectionType)
{
    Elastos_NetworkChangeNotifier_nativeNotifyConnectionTypeChanged(THIS_PROBE(IInterface), (Handle32)nativePtr, newConnectionType);
    return NOERROR;
}

AutoPtr<IInterface> NetworkChangeNotifier::Init(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return TO_IINTERFACE(Init(c));
}

Int32 NetworkChangeNotifier::GetCurrentConnectionType(
    /* [in] */ IInterface* obj)
{
    NetworkChangeNotifier* mObj = (NetworkChangeNotifier*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("NetworkChangeNotifier", "NetworkChangeNotifier::GetCurrentConnectionType, mObj is NULL");
        return 0;
    }
    return mObj->GetCurrentConnectionType();
}

void NetworkChangeNotifier::AddNativeObserver(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 nativeChangeNotifier)
{
    NetworkChangeNotifier* mObj = (NetworkChangeNotifier*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("NetworkChangeNotifier", "NetworkChangeNotifier::AddNativeObserver, mObj is NULL");
        return;
    }
    mObj->AddNativeObserver(nativeChangeNotifier);
}

void NetworkChangeNotifier::RemoveNativeObserver(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 nativeChangeNotifier)
{
    NetworkChangeNotifier* mObj = (NetworkChangeNotifier*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("NetworkChangeNotifier", "NetworkChangeNotifier::RemoveNativeObserver, mObj is NULL");
        return;
    }
    mObj->RemoveNativeObserver(nativeChangeNotifier);
}

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


