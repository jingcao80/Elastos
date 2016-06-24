
#include "elastos/droid/phone/CNetworkQueryService.h"
#include "elastos/droid/phone/CNetworkQueryServiceLocalBinder.h"
#include "elastos/droid/phone/CNetworkQueryServiceNetworkQueryService.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CNetworkQueryService::LocalBinder, Binder, INetworkQueryServiceLocalBinder)

ECode CNetworkQueryService::LocalBinder::constructor(
    /* [in] */ INetworkQueryService* host)
{
    mHost = (CNetworkQueryService*)host;
    return NOERROR;
}

ECode CNetworkQueryService::LocalBinder::GetService(
    /* [out] */ IINetworkQueryService** service)
{
    VALIDATE_NOT_NULL(service)

    *service = mHost->mBinder;
    REFCOUNT_ADD(*service)
    return NOERROR;
}

CNetworkQueryService::MyHandler::MyHandler(
    /* [in] */ CNetworkQueryService* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CNetworkQueryService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        // if the scan is complete, broadcast the results.
        // to all registerd callbacks.
        case EVENT_NETWORK_SCAN_COMPLETED:
        {
            if (DBG) mHost->Log(String("scan completed, broadcasting results"));
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> result = (AsyncResult*)IObject::Probe(obj);
            mHost->BroadcastQueryResults(result);
            break;
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CNetworkQueryService::MyNetworkQueryService, Object, IINetworkQueryService,
        IBinder)

ECode CNetworkQueryService::MyNetworkQueryService::constructor(
    /* [in] */ INetworkQueryService* host)
{
    mHost = (CNetworkQueryService*)host;
    return NOERROR;
}

ECode CNetworkQueryService::MyNetworkQueryService::StartNetworkQuery(
    /* [in] */ IINetworkQueryServiceCallback* cb)
{
    if (cb != NULL) {
        // register the callback to the list of callbacks.
        {
            AutoLock syncLock(mHost->mCallbacks);
            Boolean res;
            mHost->mCallbacks->Register(TO_IINTERFACE(cb), &res);
            if (DBG) {
                assert(0);
                //Log("registering callback " + cb.getClass().toString());
            }

            switch (mHost->mState) {
                case QUERY_READY:
                {
                    // TODO: we may want to install a timeout here in case we
                    // do not get a timely response from the RIL.
                    AutoPtr<IMessage> m;
                    mHost->mHandler->ObtainMessage(EVENT_NETWORK_SCAN_COMPLETED, (IMessage**)&m);
                    mHost->mPhone->GetAvailableNetworks(m);
                    mHost->mState = QUERY_IS_RUNNING;
                    if (DBG) mHost->Log(String("starting new query"));
                    break;
                }
                // do nothing if we're currently busy.
                case QUERY_IS_RUNNING:
                    if (DBG) mHost->Log(String("query already in progress"));
                    break;
                default:
                    ;
            }
        }
    }
    return NOERROR;
}


ECode CNetworkQueryService::MyNetworkQueryService::StopNetworkQuery(
    /* [in] */ IINetworkQueryServiceCallback* cb)
{
    // currently we just unregister the callback, since there is
    // no way to tell the RIL to terminate the query request.
    // This means that the RIL may still be busy after the stop
    // request was made, but the state tracking logic ensures
    // that the delay will only last for 1 request even with
    // repeated button presses in the NetworkSetting activity.
    if (cb != NULL) {
        {
            AutoLock syncLock(mHost->mCallbacks);
            if (DBG) {
                assert(0);
                //Log("unregistering callback " + cb.getClass().toString());
            }
            Boolean res;
            mHost->mCallbacks->Unregister(TO_IINTERFACE(cb), &res);
        }
    }
    return NOERROR;
}

const String CNetworkQueryService::TAG("NetworkQuery");
const Boolean CNetworkQueryService::DBG = FALSE;

CAR_INTERFACE_IMPL(CNetworkQueryService, Service, INetworkQueryService)

CAR_OBJECT_IMPL(CNetworkQueryService)

CNetworkQueryService::CNetworkQueryService()
    : mState(0)
{
    CNetworkQueryServiceLocalBinder::New(this, (IBinder**)&mLocalBinder);

    mHandler = new MyHandler(this);

    CRemoteCallbackList::New((IRemoteCallbackList**)&mCallbacks);

    CNetworkQueryServiceNetworkQueryService::New(this, (IINetworkQueryService**)&mBinder);
}

ECode CNetworkQueryService::constructor()
{
    return Service::constructor();
}

ECode CNetworkQueryService::OnCreate()
{
    mState = QUERY_READY;
    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    helper->GetDefaultPhone((IPhone**)&mPhone);
    return NOERROR;
}

ECode CNetworkQueryService::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    return NOERROR;
}

ECode CNetworkQueryService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)

    // TODO: Currently, return only the LocalBinder instance.  If we
    // end up requiring support for a remote binder, we will need to
    // return mBinder as well, depending upon the intent.
    if (DBG) Log(String("binding service implementation"));
    *binder = mLocalBinder;
    REFCOUNT_ADD(*binder)
    return NOERROR;
}

void CNetworkQueryService::BroadcastQueryResults(
    /* [in] */ AsyncResult* ar)
{
    // reset the state.
    {
        AutoLock syncLock(mCallbacks);
        mState = QUERY_READY;

        // see if we need to do any work.
        if (ar == NULL) {
            if (DBG) Log(String("AsyncResult is null."));
            return;
        }

        // TODO: we may need greater accuracy here, but for now, just a
        // simple status integer will suffice.
        Int32 exception = (ar->mException == NULL) ? QUERY_OK : QUERY_EXCEPTION;
        if (DBG) {
            StringBuilder sb;
            sb += "AsyncResult has exception ";
            sb += exception;
            Log(sb.ToString());
        }

        // Make the calls to all the registered callbacks.
        Int32 num;
        mCallbacks->BeginBroadcast(&num);
        for (Int32 i = (num - 1); i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mCallbacks->GetBroadcastItem(i, (IInterface**)&obj);
            AutoPtr<IINetworkQueryServiceCallback> cb = IINetworkQueryServiceCallback::Probe(obj);
            if (DBG) {
                assert(0);
                //Log("broadcasting results to " + cb.getClass().toString());
            }
            //try {
            cb->OnQueryComplete(IList::Probe(ar->mResult), exception);
            //} catch (RemoteException e) {
            //}
        }

        // finish up.
        mCallbacks->FinishBroadcast();
    }
}

void CNetworkQueryService::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos