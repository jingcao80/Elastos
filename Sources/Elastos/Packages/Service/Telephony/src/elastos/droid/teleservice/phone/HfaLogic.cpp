
#include "elastos/droid/teleservice/phone/HfaLogic.h"
#include "elastos/droid/teleservice/phone/CHfaLogicBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

HfaLogic::MyHandler::MyHandler(
    /* [in] */ HfaLogic* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode HfaLogic::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case SERVICE_STATE_CHANGED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> result = (AsyncResult*)IObject::Probe(obj);
            AutoPtr<IServiceState> state = IServiceState::Probe(result->mResult);
            mHost->OnServiceStateChange(state);
            break;
        }
        default:
            break;
    }
    return NOERROR;
}

ECode HfaLogic::MyBroadcastReceiver::constructor(
    /* [in] */ IHfaLogic* host)
{
    mHost = (HfaLogic*)host;
    return BroadcastReceiver::constructor();
}

ECode HfaLogic::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(ACTION_ERROR)) {
        String str;
        intent->GetStringExtra(String("errorCode"), &str);
        mHost->OnHfaError(str);
    }
    else if (action.Equals(ACTION_COMPLETE)) {
        Logger::I(TAG, "Hfa Successful");
        mHost->OnHfaSuccess();
    }
    return NOERROR;
}

const String HfaLogic::TAG("HfaLogic");// = HfaLogic.class.getSimpleName();

const String HfaLogic::ACTION_START("com.android.action.START_HFA");
const String HfaLogic::ACTION_ERROR("com.android.action.ERROR_HFA");
const String HfaLogic::ACTION_CANCEL("com.android.action.CANCEL_HFA");
const String HfaLogic::ACTION_COMPLETE("com.android.action.COMPLETE_HFA");

const Int32 HfaLogic::DEFAULT_RETRY_COUNT = 0;

CAR_INTERFACE_IMPL(HfaLogic, Object, IHfaLogic)

HfaLogic::HfaLogic(
    /* [in] */ IContext* context,
    /* [in] */ IHfaLogicHfaLogicCallback* _callback,
    /* [in] */ IPendingIntent* intent)
    : mPhoneMonitorState(NOT_WAITING)
    , mRetryCount(0)
{
    AutoPtr<IPreconditions> helper;
    CPreconditions::AcquireSingleton((IPreconditions**)&helper);
    helper->CheckNotNull(_callback);
    mCallback = _callback;
    helper->CheckNotNull(context);
    mContext = context;
    mResponseIntent = intent;

    mHandler = new MyHandler(this);
}

ECode HfaLogic::Start()
{
    Logger::I(TAG, "start:");
    mRetryCount = DEFAULT_RETRY_COUNT;
    StartHfaIntentReceiver();
    StartProvisioning();
    return NOERROR;
}

void HfaLogic::StartProvisioning()
{
    Logger::I(TAG, "startProvisioning:");
    SendHfaCommand(ACTION_START);
}

void HfaLogic::SendHfaCommand(
    /* [in] */ const String& action)
{
    StringBuilder sb;
    sb += "sendHfaCommand: command=";
    sb += action;
    Logger::I(TAG, sb.ToString());

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    mContext->SendBroadcast(intent);
}

void HfaLogic::OnHfaError(
    /* [in] */ const String& errorMsg)
{
    StringBuilder sb;
    sb += "onHfaError: call mCallBack.onError errorMsg=";
    sb += errorMsg;
    sb += " mRetryCount=";
    sb += mRetryCount;
    Logger::I(TAG, sb.ToString());
    mRetryCount -= 1;
    if (mRetryCount >= 0) {
        Logger::I(TAG, "onHfaError: retry");
        StartProvisioning();
    }
    else {
        Logger::I(TAG, "onHfaError: Declare OTASP_FAILURE");
        mRetryCount = 0;
        StopHfaIntentReceiver();
        SendFinalResponse(OTASP_FAILURE, errorMsg);
        mCallback->OnError(errorMsg);
    }
}

void HfaLogic::OnHfaSuccess()
{
    Logger::I(TAG, "onHfaSuccess: NOT bouncing radio call onTotalSuccess");
    StopHfaIntentReceiver();
    // bounceRadio();
    OnTotalSuccess();
}

void HfaLogic::OnTotalSuccess()
{
    Logger::I(TAG, "onTotalSuccess: call mCallBack.onSuccess");
    SendFinalResponse(OTASP_SUCCESS, String(NULL));
    mCallback->OnSuccess();
}

void HfaLogic::BounceRadio()
{
    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    AutoPtr<IPhone> phone = globals->GetPhone();
    phone->RegisterForServiceStateChanged(mHandler, SERVICE_STATE_CHANGED, NULL);

    mPhoneMonitorState = WAITING_FOR_RADIO_OFF;
    phone->SetRadioPower(FALSE);
    AutoPtr<IServiceState> state;
    phone->GetServiceState((IServiceState**)&state);
    OnServiceStateChange(state);
}

void HfaLogic::OnServiceStateChange(
    /* [in] */ IServiceState* state)
{
    Int32 _state;
    state->GetVoiceRegState(&_state);
    Boolean radioIsOff = _state == IServiceState::STATE_POWER_OFF;

    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    AutoPtr<IPhone> phone = globals->GetPhone();

    StringBuilder sb;
    sb += "Radio is on: ";
    sb += !radioIsOff;
    Logger::I(TAG, sb.ToString());

    if (mPhoneMonitorState == WAITING_FOR_RADIO_OFF) {
        if (radioIsOff) {
            mPhoneMonitorState = WAITING_FOR_RADIO_ON;
            phone->SetRadioPower(TRUE);
        }
    }
    else if (mPhoneMonitorState == WAITING_FOR_RADIO_ON) {
        if (!radioIsOff) {
            mPhoneMonitorState = NOT_WAITING;
            phone->UnregisterForServiceStateChanged(mHandler);

            OnTotalSuccess();
        }
    }
}

void HfaLogic::StartHfaIntentReceiver()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(ACTION_COMPLETE, (IIntentFilter**)&filter);
    filter->AddAction(ACTION_ERROR);

    CHfaLogicBroadcastReceiver::New(this, (IBroadcastReceiver**)&mReceiver);
    AutoPtr<IIntent> tmp;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&tmp);
}

void HfaLogic::StopHfaIntentReceiver()
{
    if (mReceiver != NULL) {
        mContext->UnregisterReceiver(mReceiver);
        mReceiver = NULL;
    }
}

void HfaLogic::SendFinalResponse(
    /* [in] */ Int32 responseCode,
    /* [in] */ const String& errorCode)
{
    if (mResponseIntent != NULL) {
        AutoPtr<IIntent> extraStuff;
        CIntent::New((IIntent**)&extraStuff);
        extraStuff->PutExtra(IOtaUtils::EXTRA_OTASP_RESULT_CODE, responseCode);

        if (responseCode == OTASP_FAILURE && errorCode != NULL) {
            extraStuff->PutExtra(IOtaUtils::EXTRA_OTASP_ERROR_CODE, errorCode);
        }

        //try {
        StringBuilder sb;
        sb += "Sending OTASP confirmation with result code: ";
        sb += responseCode;
        Logger::I(TAG, sb.ToString());
        mResponseIntent->Send(mContext, 0 /* resultCode (not used) */, extraStuff);
        //} catch (CanceledException e) {
        //    Log.e(TAG, "Pending Intent canceled");
        //}
    }
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos