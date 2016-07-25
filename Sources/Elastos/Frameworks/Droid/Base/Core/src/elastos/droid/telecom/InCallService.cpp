
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/InCallService.h"
#include "elastos/droid/telecom/CInCallAdapter.h"
#include "elastos/droid/telecom/CPhone.h"
#include "elastos/droid/telecom/CInCallServiceBinder.h"

using Elastos::Droid::Internal::Telecom::IIInCallAdapter;
using Elastos::Droid::Internal::Telecom::IIInCallService;
using Elastos::Droid::Internal::Os::ISomeArgs;

namespace Elastos {
namespace Droid {
namespace Telecom {

const Int32 InCallService::MSG_SET_IN_CALL_ADAPTER = 1;
const Int32 InCallService::MSG_ADD_CALL = 2;
const Int32 InCallService::MSG_UPDATE_CALL = 3;
const Int32 InCallService::MSG_SET_POST_DIAL_WAIT = 4;
const Int32 InCallService::MSG_ON_AUDIO_STATE_CHANGED = 5;
const Int32 InCallService::MSG_BRING_TO_FOREGROUND = 6;

//===============================================================
// InCallService::MyHandler::
//===============================================================

InCallService::MyHandler::MyHandler(
    /* [in] */ ILooper* loop,
    /* [in] */ InCallService* host)
    : Handler(loop)
    , mHost(host)
{}

ECode InCallService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (mHost->mPhone == NULL && what != MSG_SET_IN_CALL_ADAPTER) {
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case MSG_SET_IN_CALL_ADAPTER: {
            AutoPtr<IIInCallAdapter> pObj = IIInCallAdapter::Probe(obj);
            AutoPtr<IInCallAdapter> pCallAdapter;
            CInCallAdapter::New(pObj, (IInCallAdapter**)&pCallAdapter);
            CPhone::New(pCallAdapter, (IPhone**)&(mHost->mPhone));
            mHost->OnPhoneCreated(mHost->mPhone);
            break;
        }
        case MSG_ADD_CALL: {
            AutoPtr<IParcelableCall> pObj = IParcelableCall::Probe(obj);
            mHost->mPhone->InternalAddCall(pObj);
            break;
        }
        case MSG_UPDATE_CALL: {
            AutoPtr<IParcelableCall> pObj = IParcelableCall::Probe(obj);
            mHost->mPhone->InternalUpdateCall(pObj);
            break;
        }
        case MSG_SET_POST_DIAL_WAIT: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String callId;
            pArg1->ToString(&callId);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<ICharSequence> pArg2 = ICharSequence::Probe(arg2);
            String remaining;
            pArg2->ToString(&remaining);
            mHost->mPhone->InternalSetPostDialWait(callId, remaining);
            args->Recycle();
            break;
        }
        case MSG_ON_AUDIO_STATE_CHANGED: {
            AutoPtr<IAudioState> pObj = IAudioState::Probe(obj);
            mHost->mPhone->InternalAudioStateChanged(pObj);
            break;
        }
        case MSG_BRING_TO_FOREGROUND: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->mPhone->InternalBringToForeground(arg1 == 1);
            break;
        }
        default:
            break;
    }
    return NOERROR;
}

//===============================================================
// InCallService::
//===============================================================
CAR_INTERFACE_IMPL(InCallService, Object, IInCallService)

InCallService::InCallService()
{
}

ECode InCallService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIInCallService> p;
    CInCallServiceBinder::New(mHandler, (IIInCallService**)&p);
    *result = IBinder::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode InCallService::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mPhone != NULL) {
        AutoPtr<IPhone> oldPhone = mPhone;
        mPhone = NULL;

        oldPhone->Destroy();
        OnPhoneDestroyed(oldPhone);
    }
    *result = FALSE;
    return NOERROR;
}

ECode InCallService::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhone;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode InCallService::OnPhoneCreated(
    /* [in] */ IPhone* phone)
{
    return NOERROR;
}

ECode InCallService::OnPhoneDestroyed(
    /* [in] */ IPhone* phone)
{
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos