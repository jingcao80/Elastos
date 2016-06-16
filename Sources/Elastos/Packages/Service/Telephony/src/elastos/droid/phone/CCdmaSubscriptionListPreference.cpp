
#include "elastos/droid/phone/CCdmaSubscriptionListPreference.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const Int32 CCdmaSubscriptionListPreference::CdmaSubscriptionButtonHandler::MESSAGE_SET_CDMA_SUBSCRIPTION = 0;

CCdmaSubscriptionListPreference::CdmaSubscriptionButtonHandler::CdmaSubscriptionButtonHandler(
    /* [in] */ CCdmaSubscriptionListPreference* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCdmaSubscriptionListPreference::CdmaSubscriptionButtonHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_SET_CDMA_SUBSCRIPTION:
            HandleSetCdmaSubscriptionMode(msg);
            break;
    }
    return NOERROR;
}

void CCdmaSubscriptionListPreference::CdmaSubscriptionButtonHandler::HandleSetCdmaSubscriptionMode(
    /* [in] */ IMessage* msg)
{
    mHost->mPhone = PhoneFactory::GetDefaultPhone();
    assert(0);
    // AsyncResult ar = (AsyncResult) msg.obj;

    // if (ar.exception == null) {
    //     // Get the original string entered by the user
    //     int cdmaSubscriptionMode = Integer.valueOf((String) ar.userObj).intValue();
    //     Settings.Global.putInt(mPhone.getContext().getContentResolver(),
    //             Settings.Global.CDMA_SUBSCRIPTION_MODE,
    //             cdmaSubscriptionMode );
    // }
    // else {
    //     Logger::E(LOG_TAG, "Setting Cdma subscription source failed");
    // }
}

const String CCdmaSubscriptionListPreference::LOG_TAG("CdmaSubscriptionListPreference");

const Int32 CCdmaSubscriptionListPreference::CDMA_SUBSCRIPTION_RUIM_SIM = 0;
const Int32 CCdmaSubscriptionListPreference::CDMA_SUBSCRIPTION_NV = 1;

const Int32 CCdmaSubscriptionListPreference::sPreferredSubscriptionMode = CDMA_SUBSCRIPTION_NV;

CAR_INTERFACE_IMPL(CCdmaSubscriptionListPreference, ListPreference, ICdmaSubscriptionListPreference)

CAR_OBJECT_IMPL(CCdmaSubscriptionListPreference)

ECode CCdmaSubscriptionListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ListPreference::constructor(context, attrs);

    mPhone = PhoneFactory::GetDefaultPhone();
    mHandler = new CdmaSubscriptionButtonHandler();
    SetCurrentCdmaSubscriptionModeValue();
    return NOERROR;
}

void CCdmaSubscriptionListPreference::SetCurrentCdmaSubscriptionModeValue()
{
    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 cdmaSubscriptionMode;
    helper->GetInt32(cr, ISettingsGlobal::CDMA_SUBSCRIPTION_MODE,
            sPreferredSubscriptionMode, &cdmaSubscriptionMode);

    SetValue(StringUtils::ToString(cdmaSubscriptionMode));
}

ECode CCdmaSubscriptionListPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CCdmaSubscriptionListPreference::ShowDialog(
    /* [in] */ IBundle* state)
{
    SetCurrentCdmaSubscriptionModeValue();

    ListPreference::ShowDialog(state);
}

ECode CCdmaSubscriptionListPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    ListPreference::OnDialogClosed(positiveResult);

    if (!positiveResult) {
        //The button was dismissed - no need to set new value
        return NOERROR;
    }

    String value;
    GetValue(&value);
    Int32 buttonCdmaSubscriptionMod = StringUtils::ParseInt32(value);
    Logger::D(LOG_TAG, "Setting new value %d", buttonCdmaSubscriptionMode);
    Int32 statusCdmaSubscriptionMode;
    switch(buttonCdmaSubscriptionMode) {
        case CDMA_SUBSCRIPTION_NV:
            statusCdmaSubscriptionMode = IPhone::CDMA_SUBSCRIPTION_NV;
            break;
        case CDMA_SUBSCRIPTION_RUIM_SIM:
            statusCdmaSubscriptionMode = IPhone::CDMA_SUBSCRIPTION_RUIM_SIM;
            break;
        default:
            statusCdmaSubscriptionMode = IPhone::PREFERRED_CDMA_SUBSCRIPTION;
    }

    // Set the CDMA subscription mode, when mode has been successfully changed
    // handleSetCdmaSubscriptionMode will be invoked and the value saved.
    String value2;
    GetValue(&value2);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(ICdmaSubscriptionButtonHandler::MESSAGE_SET_CDMA_SUBSCRIPTION,
            value2, (IMessage**)&message);
    return mPhone->SetCdmaSubscription(statusCdmaSubscriptionMode, message);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos