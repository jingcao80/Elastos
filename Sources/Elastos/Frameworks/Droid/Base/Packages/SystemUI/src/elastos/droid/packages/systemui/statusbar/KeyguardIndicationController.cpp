
#include "elastos/droid/packages/systemui/statusbar/KeyguardIndicationController.h"
#include "../R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/text/format/Formatter.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::View::IView;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

const String KeyguardIndicationController::TAG("KeyguardIndicationController");
const Int32 KeyguardIndicationController::MSG_HIDE_TRANSIENT = 1;

KeyguardIndicationController::ControllerBroadcastReceiver::ControllerBroadcastReceiver(
    /* [in] */ KeyguardIndicationController* host)
    : mHost(host)
{}

ECode KeyguardIndicationController::ControllerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mHost->mVisible) {
        mHost->UpdateIndication();
    }
    return NOERROR;
}

KeyguardIndicationController::ControllerHandler::ControllerHandler(
    /* [in] */ KeyguardIndicationController* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode KeyguardIndicationController::ControllerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == MSG_HIDE_TRANSIENT && mHost->mTransientIndication != NULL) {
        mHost->mTransientIndication = NULL;
        mHost->UpdateIndication();
    }
    return NOERROR;
}

KeyguardIndicationController::KeyguardIndicationController(
    /* [in] */ IContext* context,
    /* [in] */ IKeyguardIndicationTextView* textView)
{
    mReceiver = new ControllerBroadcastReceiver(this);
    mHandler = new ControllerHandler(this);
    mContext = context;
    mTextView = textView;

    AutoPtr<IInterface> obj = ServiceManager::GetService(IBatteryStats::SERVICE_NAME);
    mBatteryInfo = IIBatteryStats::Probe(obj);
    assert(0 && "TODO");
    // KeyguardUpdateMonitor.getInstance(context).registerCallback(mUpdateMonitor);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> user;
    helper->GetOWNER((IUserHandle**)&user);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_TIME_TICK, (IIntentFilter**)&filter);
    AutoPtr<IIntent> i;
    context->RegisterReceiverAsUser(mReceiver, user, filter, String(NULL), NULL, (IIntent**)&i);
}

void KeyguardIndicationController::SetVisible(
    /* [in] */ Boolean visible)
{
    mVisible = visible;
    IView::Probe(mTextView)->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    if (visible) {
        HideTransientIndication();
        UpdateIndication();
    }
}

void KeyguardIndicationController::SetRestingIndication(
    /* [in] */ const String& restingIndication)
{
    mRestingIndication = restingIndication;
    UpdateIndication();
}

void KeyguardIndicationController::HideTransientIndicationDelayed(
    /* [in] */ Int64 delayMs)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_HIDE_TRANSIENT, (IMessage**)&msg);
    Boolean tmp = 0;
    mHandler->SendMessageDelayed(msg, delayMs, &tmp);
}

void KeyguardIndicationController::ShowTransientIndication(
    /* [in] */ Int32 transientIndication)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String str;
    res->GetString(transientIndication, &str);
    ShowTransientIndication(str);
}

void KeyguardIndicationController::ShowTransientIndication(
    /* [in] */ const String& transientIndication)
{
    mTransientIndication = transientIndication;
    mHandler->RemoveMessages(MSG_HIDE_TRANSIENT);
    UpdateIndication();
}

void KeyguardIndicationController::HideTransientIndication()
{
    if (mTransientIndication != NULL) {
        mTransientIndication = NULL;
        mHandler->RemoveMessages(MSG_HIDE_TRANSIENT);
        UpdateIndication();
    }
}

void KeyguardIndicationController::UpdateIndication()
{
    if (mVisible) {
        AutoPtr<ICharSequence> value;
        CString::New(ComputeIndication(), (ICharSequence**)&value);
        mTextView->SwitchIndication(value);
    }
}

String KeyguardIndicationController::ComputeIndication()
{
    if (!TextUtils::IsEmpty(mTransientIndication)) {
        return mTransientIndication;
    }
    if (mPowerPluggedIn) {
        return ComputePowerIndication();
    }
    return mRestingIndication;
}

String KeyguardIndicationController::ComputePowerIndication()
{
    String str;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    if (mPowerCharged) {
        res->GetString(R::string::keyguard_charged, &str);
        return str;
    }

    // Try fetching charging time from battery stats.
    // try {
    Int64 chargingTimeRemaining = 0;
    mBatteryInfo->ComputeChargeTimeRemaining(&chargingTimeRemaining);
    if (chargingTimeRemaining > 0) {
        String chargingTimeFormatted =
                Formatter::FormatShortElapsedTime(mContext, chargingTimeRemaining);

        AutoPtr<ICharSequence> value;
        CString::New(chargingTimeFormatted, (ICharSequence**)&value);
        AutoPtr<ArrayOf<IInterface*> > values = ArrayOf<IInterface*>::Alloc(1);
        values->Set(0, value);
        res->GetString(R::string::keyguard_indication_charging_time, values, &str);
        return str;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling IBatteryStats: ", e);
    // }

    // Fall back to simple charging label.
    res->GetString(R::string::keyguard_plugged_in, &str);
    return str;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos
