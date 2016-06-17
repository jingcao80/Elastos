
#include "elastos/droid/internal/telephony/PhoneStateIntentReceiver.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CSignalStrength.h"

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Telephony::ISignalStrengthHelper;
using Elastos::Droid::Telephony::IServiceStateHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String PhoneStateIntentReceiver::LOGTAG("PhoneStatIntentReceiver");
const Boolean PhoneStateIntentReceiver::DBG = FALSE;

const Int32 PhoneStateIntentReceiver::NOTIF_PHONE    = 1 << 0;
const Int32 PhoneStateIntentReceiver::NOTIF_SERVICE  = 1 << 1;
const Int32 PhoneStateIntentReceiver::NOTIF_SIGNAL   = 1 << 2;

CAR_INTERFACE_IMPL(PhoneStateIntentReceiver, BroadcastReceiver, IPhoneStateIntentReceiver)

PhoneStateIntentReceiver::PhoneStateIntentReceiver()
    : mPhoneState(PhoneConstantsState_IDLE)
    , mWants(0)
    , mPhoneStateEventWhat(0)
    , mServiceStateEventWhat(0)
    , mAsuEventWhat(0)
{
    CServiceState::New((IServiceState**)&mServiceState);
    CSignalStrength::New((ISignalStrength**)&mSignalStrength);
}

ECode PhoneStateIntentReceiver::constructor()
{
    BroadcastReceiver::constructor();
    CIntentFilter::New((IIntentFilter**)&mFilter);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    BroadcastReceiver::constructor();
    SetContext(context);
    SetTarget(target);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::SetContext(
    /* [in] */ IContext* c)
{
    mContext = c;
    return NOERROR;
}

ECode PhoneStateIntentReceiver::SetTarget(
    /* [in] */ IHandler* h)
{
    mTarget = h;
    return NOERROR;
}

ECode PhoneStateIntentReceiver::GetPhoneState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mWants & NOTIF_PHONE) == 0) {
        // throw new RuntimeException
        //     ("client must call NotifyPhoneCallState(Int32)");
        return E_RUNTIME_EXCEPTION;
    }
    *result = mPhoneState;
    return NOERROR;
}

ECode PhoneStateIntentReceiver::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result)
    if ((mWants & NOTIF_SERVICE) == 0) {
        // throw new RuntimeException
        //     ("client must call NotifyServiceState(Int32)");
        return E_RUNTIME_EXCEPTION;
    }
    *result = mServiceState;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneStateIntentReceiver::GetSignalStrengthLevelAsu(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // TODO: use new SignalStrength instead of asu
    if ((mWants & NOTIF_SIGNAL) == 0) {
        // throw new RuntimeException
        //     ("client must call NotifySignalStrength(Int32)");
        return E_RUNTIME_EXCEPTION;
    }
    return mSignalStrength->GetAsuLevel(result);
}

ECode PhoneStateIntentReceiver::GetSignalStrengthDbm(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mWants & NOTIF_SIGNAL) == 0) {
        // throw new RuntimeException
        //     ("client must call NotifySignalStrength(Int32)");
        return E_RUNTIME_EXCEPTION;
    }
    return mSignalStrength->GetDbm(result);
}

ECode PhoneStateIntentReceiver::NotifyPhoneCallState(
    /* [in] */ Int32 eventWhat)
{
    mWants |= NOTIF_PHONE;
    mPhoneStateEventWhat = eventWhat;
    mFilter->AddAction(ITelephonyManager::ACTION_PHONE_STATE_CHANGED);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::GetNotifyPhoneCallState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((mWants & NOTIF_PHONE) != 0);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::NotifyServiceState(
    /* [in] */ Int32 eventWhat)
{
    mWants |= NOTIF_SERVICE;
    mServiceStateEventWhat = eventWhat;
    mFilter->AddAction(ITelephonyIntents::ACTION_SERVICE_STATE_CHANGED);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::GetNotifyServiceState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((mWants & NOTIF_SERVICE) != 0);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::NotifySignalStrength (
    /* [in] */ Int32 eventWhat)
{
    mWants |= NOTIF_SIGNAL;
    mAsuEventWhat = eventWhat;
    mFilter->AddAction(ITelephonyIntents::ACTION_SIGNAL_STRENGTH_CHANGED);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::GetNotifySignalStrength(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((mWants & NOTIF_SIGNAL) != 0);
    return NOERROR;
}

ECode PhoneStateIntentReceiver::RegisterIntent()
{
    AutoPtr<IIntent> intent;
    return mContext->RegisterReceiver(this, mFilter, (IIntent**)&intent);
}

ECode PhoneStateIntentReceiver::UnregisterIntent()
{
    return mContext->UnregisterReceiver(this);
}

ECode PhoneStateIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    // try {
        if (ITelephonyIntents::ACTION_SIGNAL_STRENGTH_CHANGED.Equals(action)) {
            AutoPtr<IBundle> bundle;
            intent->GetExtras((IBundle**)&bundle);
            AutoPtr<ISignalStrengthHelper> hlp;
            assert(0 && "TODO");
            // CSignalStrengthHelper::AcquireSingleton((ISignalStrengthHelper**)&hlp);
            hlp->NewFromBundle(bundle, (ISignalStrength**)&mSignalStrength);

            Boolean bSignal = FALSE;
            GetNotifySignalStrength(&bSignal);
            if (mTarget != NULL && bSignal) {
                AutoPtr<IMessageHelper> mhlp;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&mhlp);
                AutoPtr<IMessage> message;
                mhlp->Obtain(mTarget, mAsuEventWhat, (IMessage**)&message);
                Boolean bSend = FALSE;
                mTarget->SendMessage(message, &bSend);
            }
        }
        else if (ITelephonyManager::ACTION_PHONE_STATE_CHANGED.Equals(action)) {
            if (DBG) {
                String str;
                intent->GetStringExtra(IPhoneConstants::STATE_KEY, &str);
                // Rlog::D(LOG_TAG, "onReceiveIntent: ACTION_PHONE_STATE_CHANGED, state=%s",
                //         str);
            }
            String phoneState;
            intent->GetStringExtra(IPhoneConstants::STATE_KEY, &phoneState);
            assert(0 && "TODO");
            // mPhoneState = Enum::ValueOf(
            //         PhoneConstants.State.class, phoneState);

            Boolean bCallState = FALSE;
            GetNotifyPhoneCallState(&bCallState);
            if (mTarget != NULL && bCallState) {
                AutoPtr<IMessageHelper> mhlp;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&mhlp);
                AutoPtr<IMessage> message;
                mhlp->Obtain(mTarget,
                        mPhoneStateEventWhat, (IMessage**)&message);
                Boolean bSend = FALSE;
                mTarget->SendMessage(message, &bSend);
            }
        }
        else if (ITelephonyIntents::ACTION_SERVICE_STATE_CHANGED.Equals(action)) {
            AutoPtr<IBundle> bundle;
            intent->GetExtras((IBundle**)&bundle);
            AutoPtr<IServiceStateHelper> sshlp;
            assert(0 && "TODO");
            // CServiceStateHelper::AcquireSingleton((IServiceStateHelper**)&sshlp);
            sshlp->NewFromBundle(bundle, (IServiceState**)&mServiceState);

            Boolean bServiceState = FALSE;
            GetNotifyServiceState(&bServiceState);
            if (mTarget != NULL && bServiceState) {
                AutoPtr<IMessageHelper> mhlp;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&mhlp);
                AutoPtr<IMessage> message;
                mhlp->Obtain(mTarget, mServiceStateEventWhat, (IMessage**)&message);
                Boolean bSend = FALSE;
                mTarget->SendMessage(message, &bSend);
            }
        }
    // } Catch (Exception ex) {
    //     Rlog->E(LOG_TAG, "[PhoneStateIntentRecv] caught " + ex);
    //     ex->PrintStackTrace();
    // }
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
