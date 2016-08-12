
#include "elastos/droid/server/CCountryDetectorService.h"
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Location.h>
#include <Elastos.Droid.Internal.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Location::EIID_IICountryDetector;
using Elastos::Droid::Location::EIID_ICountryListener;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {

const String CCountryDetectorService::TAG("CountryDetector");
const Boolean CCountryDetectorService::DEBUG = FALSE;

ECode CCountryDetectorService::CountryDetectedRunnable::Run()
{
    mHost->NotifyReceivers(mCountry);
    return NOERROR;
}

ECode CCountryDetectorService::SetCountryListenerRunnable::Run()
{
    mHost->mCountryDetector->SetCountryListener(mListener);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CCountryDetectorService::Receiver, Object, IProxyDeathRecipient);

CCountryDetectorService::Receiver::Receiver(
    /* [in] */ IICountryListener* listener,
    /* [in] */ CCountryDetectorService* owner)
    : mListener(listener)
    , mKey(IBinder::Probe(listener))
    , mHost(owner)
{
}

ECode CCountryDetectorService::Receiver::ProxyDied()
{
    return mHost->RemoveListener(mKey);
}

ECode CCountryDetectorService::Receiver::GetListener(
    /* [out] */ IICountryListener **listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mListener;
    REFCOUNT_ADD(*listener);

    return NOERROR;
}

CAR_INTERFACE_IMPL(CCountryDetectorService::LocationBasedDetectorListener, Object, ICountryListener);

CCountryDetectorService::LocationBasedDetectorListener::LocationBasedDetectorListener(
    /* [in] */ CCountryDetectorService* owner)
    : mHost(owner)
{
}

ECode CCountryDetectorService::LocationBasedDetectorListener::OnCountryDetected(
    /* [in] */ ICountry* country)
{
    AutoPtr<IRunnable> runnable = new CountryDetectedRunnable(country, mHost);
    Boolean result;
    return mHost->mHandler->Post(runnable, &result);
}

CAR_INTERFACE_IMPL_3(CCountryDetectorService, Object, IICountryDetector, IBinder, IRunnable)

CAR_OBJECT_IMPL(CCountryDetectorService)

ECode CCountryDetectorService::constructor(
    /* [in] */ IContext *context)
{
    mContext = context;
    mSystemReady = FALSE;

    return NOERROR;
}

ECode CCountryDetectorService::DetectCountry(
    /* [out] */  ICountry** country)
{
    VALIDATE_NOT_NULL(country);
    *country = NULL;

    if (!mSystemReady) {
        return NOERROR;   // server not yet active
    }

    return mCountryDetector->DetectCountry(country);
}

ECode CCountryDetectorService::AddCountryListener(
    /* [in] */ IICountryListener* listener)
{
    if (!mSystemReady) {
        //throw new RemoteException();
        return E_REMOTE_EXCEPTION;
    }
    return AddListener(listener);
}

ECode CCountryDetectorService::RemoveCountryListener(
    /* [in] */ IICountryListener *listener)
{
    if (!mSystemReady) {
        //throw new RemoteException();
        return E_REMOTE_EXCEPTION;
    }
    AutoPtr<IBinder> binder = IBinder::Probe(listener);
    return RemoveListener(binder);
}

ECode CCountryDetectorService::AddListener(
    /* [in] */ IICountryListener *listener)
{
    AutoLock lock(mReceiverslock);
    AutoPtr<Receiver> r = new Receiver(listener, this);
    //try {
    AutoPtr<IBinder> binder = IBinder::Probe(listener);
    IProxy* proxy = ((IProxy*)listener->Probe(EIID_IProxy));
    if (proxy != NULL) proxy->LinkToDeath(r, 0);
    mReceivers[binder] = r;
    if (mReceivers.GetSize() == 1) {
        Slogger::D(TAG, "The first listener is added");
        SetCountryListener(mLocationBasedDetectorListener);
    }
    //} catch (RemoteException e) {
    //    Slog.e(TAG, "linkToDeath failed:", e);
    //}

    return NOERROR;
}

ECode CCountryDetectorService::RemoveListener(
    /* [in] */ IBinder *key)
{
    AutoLock lock(mReceiverslock);
    mReceivers.Erase(key);
    if (mReceivers.IsEmpty()) {
        SetCountryListener(NULL);
        Slogger::D(String(TAG), String("No listener is left") );
    }

    return NOERROR;
}

ECode CCountryDetectorService::NotifyReceivers(
    /* [in] */ ICountry *country)
{
    AutoLock lock(mReceiverslock);
    HashMap<AutoPtr<IBinder>, AutoPtr<Receiver> >::Iterator iter = mReceivers.Begin();
    AutoPtr<Receiver> receiver;
    for (; iter != mReceivers.End(); ++iter) {
        receiver = iter->mSecond;
        AutoPtr<IICountryListener> listener;
        receiver->GetListener((IICountryListener**)&listener);
        listener->OnCountryDetected(country);
    }
    return NOERROR;
}

ECode CCountryDetectorService::SystemRunning()
{
    // Shall we wait for the initialization finish.
    AutoPtr<IBackgroundThreadHelper> helper;
    CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&helper);
    AutoPtr<IHandler> hander;
    helper->GetHandler((IHandler**)&hander);
    Boolean bval;
    hander->Post(this, &bval);
    return NOERROR;
}

ECode CCountryDetectorService::Initialize()
{
    mCountryDetector = new ComprehensiveCountryDetector(mContext);
    mLocationBasedDetectorListener = new LocationBasedDetectorListener(this);
    return NOERROR;
}

ECode CCountryDetectorService::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    Looper::Prepare();
    CHandler::New((IHandler**)&mHandler);
    Initialize();
    mSystemReady = TRUE;
    Looper::Loop();

    return NOERROR;
}

ECode CCountryDetectorService::SetCountryListener(
    /* [in] */ ICountryListener *listener)
{
    AutoPtr<IRunnable> runnable = new SetCountryListenerRunnable(listener, this);
    Boolean result;
    return mHandler->Post(runnable, &result);
}

// For testing
ECode CCountryDetectorService::IsSystemReady(
    /* [out] */ Boolean *isReady)
{
    VALIDATE_NOT_NULL(isReady);
    *isReady = mSystemReady;
    return NOERROR;
}

// //@SuppressWarnings("unused")
// //@Override
// ECode CCountryDetectorService::Dump(
//     /* [in] */ IFileDescriptor *fd,
//     /* [in] */ IPrintWriter *fout,
//     /* [in] */ ArrayOf<String> *args)
// {
//     String permission = Elastos::Droid::Manifest::permission::DUMP;
//     FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(permission, String(TAG)));

//     if (!DEBUG) {
//         return NULL;
//     }
//     //try {
//     final Printer p = new PrintWriterPrinter(fout);
//     p.println("CountryDetectorService state:");
//     p.println("  Number of listeners=" + mReceivers.keySet().size());
//     if (mCountryDetector == null) {
//         p.println("  ComprehensiveCountryDetector not initialized");
//     } else {
//         p.println("  " + mCountryDetector.toString());
//     }
//     //} catch (Exception e) {
//     //    Slog.e(TAG, "Failed to dump CountryDetectorService: ", e);
//     //}
//     return E_NOT_IMPLEMENTED;
// }

ECode CCountryDetectorService::ToString(
        /* [out] */ String* str)
{
    return Object::ToString(str);
}

}
}
}
