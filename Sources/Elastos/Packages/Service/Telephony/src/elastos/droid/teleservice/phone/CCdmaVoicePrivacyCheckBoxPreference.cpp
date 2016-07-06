
#include "elastos/droid/teleservice/phone/CCdmaVoicePrivacyCheckBoxPreference.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/R.h>
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String CCdmaVoicePrivacyCheckBoxPreference::TAG("CdmaVoicePrivacyCheckBoxPreference");
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
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*) IObject::Probe(obj);

    if (ar->mException != NULL) {
        if (mHost->DBG) Logger::D(TAG, "handleGetVPResponse: ar.exception=%s", TO_CSTR(ar->mException));
        mHost->SetEnabled(FALSE);
    }
    else {
        if (mHost->DBG) Logger::D(TAG, "handleGetVPResponse: VP state successfully queried.");

        AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
        AutoPtr<IInterface> obj;
        array->Get(0, (IInterface**)&obj);
        AutoPtr<IInteger32> value = IInteger32::Probe(obj);
        Int32 enable;
        value->GetValue(&enable);

        mHost->SetChecked(enable != 0);

        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);

        AutoPtr<ISettingsSecure> helper;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&helper);
        Boolean tmp = FALSE;
        helper->PutInt32(cr, ISettingsSecure::ENHANCED_VOICE_PRIVACY_ENABLED, enable, &tmp);
    }
}

void CCdmaVoicePrivacyCheckBoxPreference::MyHandler::HandleSetVPResponse(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*) IObject::Probe(obj);

    if (ar->mException != NULL) {
        if (mHost->DBG) Logger::D(TAG, "handleSetVPResponse: ar.exception=%s", TO_CSTR(ar->mException));
    }
    if (mHost->DBG) Logger::D(TAG, "handleSetVPResponse: re get");

    AutoPtr<IMessage> m;
    mHost->mHandler->ObtainMessage(MESSAGE_GET_VP, (IMessage**)&m);
    mHost->mPhone->GetEnhancedVoicePrivacy(m);
}

CAR_OBJECT_IMPL(CCdmaVoicePrivacyCheckBoxPreference)
CCdmaVoicePrivacyCheckBoxPreference::CCdmaVoicePrivacyCheckBoxPreference()
    : DBG(IPhoneGlobals::DBG_LEVEL >= 2)
{
    mHandler = new MyHandler(this);
}

ECode CCdmaVoicePrivacyCheckBoxPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    CheckBoxPreference::constructor(context, attrs, defStyle);

    assert(0 && "TODO need PhoneGlobals");
    // mPhone = PhoneGlobals::GetPhone();
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MyHandler::MESSAGE_GET_VP, (IMessage**)&m);
    return mPhone->GetEnhancedVoicePrivacy(m);
}

ECode CCdmaVoicePrivacyCheckBoxPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, Elastos::Droid::R::attr::checkBoxPreferenceStyle);
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos