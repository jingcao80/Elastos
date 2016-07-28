
#include "elastos/droid/teleservice/phone/CCdmaSubscriptionListPreference.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/provider/Settings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
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
    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    helper->GetDefaultPhone((IPhone**)&(mHost->mPhone));

    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*) IAsyncResult::Probe(obj);

    if (ar->mException == NULL) {
        // Get the original string entered by the user
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(ar->mUserObj);
        String s;
        cs->ToString(&s);
        Int32 cdmaSubscriptionMode = StringUtils::ParseInt32(s);
        AutoPtr<IContext> ctx;
        mHost->mPhone->GetContext((IContext**)&ctx);
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);
        Boolean tmp = FALSE;
        Elastos::Droid::Provider::Settings::Global::PutInt32(cr, ISettingsGlobal::CDMA_SUBSCRIPTION_MODE,
                cdmaSubscriptionMode, &tmp);
    }
    else {
        Logger::E(TAG, "Setting Cdma subscription source failed");
    }
}

const String CCdmaSubscriptionListPreference::TAG("CdmaSubscriptionListPreference");

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

    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    helper->GetDefaultPhone((IPhone**)&mPhone);
    mHandler = new CdmaSubscriptionButtonHandler(this);
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
    return NOERROR;
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
    Int32 buttonCdmaSubscriptionMode = StringUtils::ParseInt32(value);
    Logger::D(TAG, "Setting new value %d", buttonCdmaSubscriptionMode);
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
    AutoPtr<ICharSequence> cs;
    CString::New(value2, (ICharSequence**)&cs);
    mHandler->ObtainMessage(CdmaSubscriptionButtonHandler::MESSAGE_SET_CDMA_SUBSCRIPTION,
            cs, (IMessage**)&message);
    return mPhone->SetCdmaSubscription(statusCdmaSubscriptionMode, message);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos