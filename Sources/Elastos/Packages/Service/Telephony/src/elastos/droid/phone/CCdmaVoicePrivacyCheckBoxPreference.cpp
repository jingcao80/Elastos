
#include "elastos/droid/phone/CCdmaVoicePrivacyCheckBoxPreference.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const Int32 CCdmaVoicePrivacyCheckBoxPreference::MyHandler::MESSAGE_GET_VP = 0;
const Int32 CCdmaVoicePrivacyCheckBoxPreference::MyHandler::MESSAGE_SET_VP = 1;

CCdmaVoicePrivacyCheckBoxPreference::MyHandler::MyHandler(
    /* [in] */ CCdmaVoicePrivacyCheckBoxPreference* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCdmaVoicePrivacyCheckBoxPreference::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_GET_VP:
            HandleGetVPResponse(msg);
            break;
        case MESSAGE_SET_VP:
            HandleSetVPResponse(msg);
            break;
    }
    return NOERROR;
}

void CCdmaVoicePrivacyCheckBoxPreference::MyHandler::HandleGetVPResponse(
    /* [in] */ IMessage* msg)
{
    AsyncResult ar = (AsyncResult) msg.obj;

    if (ar.exception != null) {
        if (DBG) Log.d(LOG_TAG, "handleGetVPResponse: ar.exception=" + ar.exception);
        SetEnabled(FALSE);
    }
    else {
        if (DBG) Log.d(LOG_TAG, "handleGetVPResponse: VP state successfully queried.");
        Int32 enable = ((int[]) ar.result)[0];
        SetChecked(enable != 0);

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);

        AutoPtr<ISettingsSecur> helper;
        CSettingsSecure::AcquireSingleton((ISettingsSecur**)&helper);
        helper->PutInt32(cr, ISettingsSecure::ENHANCED_VOICE_PRIVACY_ENABLED, enable);
    }
}

void CCdmaVoicePrivacyCheckBoxPreference::MyHandler::HandleSetVPResponse(
    /* [in] */ IMessage* msg)
{
    AsyncResult ar = (AsyncResult) msg.obj;

    if (ar.exception != null) {
        if (DBG) Log.d(LOG_TAG, "handleSetVPResponse: ar.exception=" + ar.exception);
    }
    if (DBG) Logger::D(LOG_TAG, "handleSetVPResponse: re get");

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MESSAGE_GET_VP, (IMessage**)&m);
    mPhone->GetEnhancedVoicePrivacy(m);
}

const String CCdmaVoicePrivacyCheckBoxPreference::LOG_TAG("CdmaVoicePrivacyCheckBoxPreference");

CAR_INTERFACE_IMPL(CCdmaVoicePrivacyCheckBoxPreference, CheckBoxPreference, ICdmaVoicePrivacyCheckBoxPreference)

CAR_OBJECT_IMPL(CCdmaVoicePrivacyCheckBoxPreference)

CCdmaVoicePrivacyCheckBoxPreference::CCdmaVoicePrivacyCheckBoxPreference()
    : DBG(PhoneGlobals::DBG_LEVEL >= 2)
{
    mHandler = new MyHandler(this);
}

ECode CCdmaVoicePrivacyCheckBoxPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int232 defStyle)
{
    CheckBoxPreference::constructor(context, attrs, defStyle);

    mPhone = PhoneGlobals::GetPhone();
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MyHandler::MESSAGE_GET_VP, (IMessage**)&m);
    return mPhone->GetEnhancedVoicePrivacy(m);
}

ECode CCdmaVoicePrivacyCheckBoxPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, com.android.internal.R.attr.checkBoxPreferenceStyle);
}

ECode CCdmaVoicePrivacyCheckBoxPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CCdmaVoicePrivacyCheckBoxPreference::OnClick()
{
    CheckBoxPreference::OnClick();

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MyHandler::MESSAGE_SET_VP, (IMessage**)&m);
    Boolean res;
    IsChecked(&res);
    return mPhone->EnableEnhancedVoicePrivacy(res, m);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos