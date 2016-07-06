
#include "elastos/droid/teleservice/phone/CCdmaSystemSelectListPreference.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Core::StringUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const Int32 CCdmaSystemSelectListPreference::MyHandler::MESSAGE_GET_ROAMING_PREFERENCE = 0;
const Int32 CCdmaSystemSelectListPreference::MyHandler::MESSAGE_SET_ROAMING_PREFERENCE = 1;

CCdmaSystemSelectListPreference::MyHandler::MyHandler(
    /* [in] */ CCdmaSystemSelectListPreference* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCdmaSystemSelectListPreference::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_GET_ROAMING_PREFERENCE:
            HandleQueryCdmaRoamingPreference(msg);
            break;

        case MESSAGE_SET_ROAMING_PREFERENCE:
            HandleSetCdmaRoamingPreference(msg);
            break;
    }
    return NOERROR;
}

void CCdmaSystemSelectListPreference::MyHandler::HandleQueryCdmaRoamingPreference(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*) IObject::Probe(obj);

    if (ar->mException == NULL) {
        Int32 statusCdmaRoamingMode = 0;
        AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
        AutoPtr<IInterface> obj;
        array->Get(0, (IInterface**)&obj);
        AutoPtr<IInteger32> value = IInteger32::Probe(obj);
        value->GetValue(&statusCdmaRoamingMode);

        AutoPtr<IContext> context;
        mHost->mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsRoamingMode;
        helper->GetInt32(cr, ISettingsGlobal::CDMA_ROAMING_MODE,
                IPhone::CDMA_RM_HOME, &settingsRoamingMode);

        //check that statusCdmaRoamingMode is from an accepted value
        if (statusCdmaRoamingMode == IPhone::CDMA_RM_HOME ||
                statusCdmaRoamingMode == IPhone::CDMA_RM_ANY ) {
            //check changes in statusCdmaRoamingMode and updates settingsRoamingMode
            if (statusCdmaRoamingMode != settingsRoamingMode) {
                settingsRoamingMode = statusCdmaRoamingMode;
                //changes the Settings.Secure accordingly to statusCdmaRoamingMode
                Boolean tmp = FALSE;
                helper->PutInt32(cr, ISettingsGlobal::CDMA_ROAMING_MODE, settingsRoamingMode, &tmp);
            }
            //changes the mButtonPreferredNetworkMode accordingly to modemNetworkMode
            mHost->SetValue(StringUtils::ToString(statusCdmaRoamingMode));
        }
        else {
            if(DBG) Logger::I(TAG, "reset cdma roaming mode to default");
            ResetCdmaRoamingModeToDefault();
        }
    }
}

void CCdmaSystemSelectListPreference::MyHandler::HandleSetCdmaRoamingPreference(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*) IObject::Probe(obj);

    String value;
    mHost->GetValue(&value);
    if ((ar->mException == NULL) && !value.IsNull()) {
        Int32 cdmaRoamingMode = StringUtils::ParseInt32(value);

        AutoPtr<IContext> context;
        mHost->mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Boolean tmp = FALSE;
        helper->PutInt32(cr, ISettingsGlobal::CDMA_ROAMING_MODE, cdmaRoamingMode, &tmp);
    }
    else {
        AutoPtr<IMessage> m;
        ObtainMessage(MESSAGE_GET_ROAMING_PREFERENCE, (IMessage**)&m);
        mHost->mPhone->QueryCdmaRoamingPreference(m);
    }
}

void CCdmaSystemSelectListPreference::MyHandler::ResetCdmaRoamingModeToDefault()
{
    //set the mButtonCdmaRoam
    mHost->SetValue(StringUtils::ToString(IPhone::CDMA_RM_ANY));
    //set the Settings.System
    AutoPtr<IContext> context;
    mHost->mPhone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Boolean tmp = FALSE;
    helper->PutInt32(cr, ISettingsGlobal::CDMA_ROAMING_MODE, IPhone::CDMA_RM_ANY, &tmp);
    //Set the Status
    AutoPtr<IMessage> m;
    ObtainMessage(MESSAGE_SET_ROAMING_PREFERENCE, (IMessage**)&m);
    mHost->mPhone->SetCdmaRoamingPreference(IPhone::CDMA_RM_ANY, m);
}

const String CCdmaSystemSelectListPreference::TAG("CdmaRoamingListPreference");
const Boolean CCdmaSystemSelectListPreference::DBG = FALSE;

CAR_INTERFACE_IMPL(CCdmaSystemSelectListPreference, ListPreference, ICdmaSystemSelectListPreference)

CAR_OBJECT_IMPL(CCdmaSystemSelectListPreference)

CCdmaSystemSelectListPreference::CCdmaSystemSelectListPreference()
{
    mHandler = new MyHandler(this);
}

ECode CCdmaSystemSelectListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ListPreference::constructor(context, attrs);

    mPhone = PhoneGlobals::GetPhone();
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MyHandler::MESSAGE_GET_ROAMING_PREFERENCE, (IMessage**)&m);
    return mPhone->QueryCdmaRoamingPreference(m);
}

ECode CCdmaSystemSelectListPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CCdmaSystemSelectListPreference::ShowDialog(
    /* [in] */ IBundle* state)
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    String mode;
    helper->Get(ITelephonyProperties::PROPERTY_INECM_MODE, &mode);
    if (StringUtils::ParseBoolean(mode)) {
        // In ECM mode do not show selection options
    }
    else {
        ListPreference::ShowDialog(state);
    }
    return NOERROR;
}

ECode CCdmaSystemSelectListPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    ListPreference::OnDialogClosed(positiveResult);

    String value;
    GetValue(&value);
    if (positiveResult && !value.IsNull()) {
        Int32 buttonCdmaRoamingMode = StringUtils::ParseInt32(value);

        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsCdmaRoamingMode;
        helper->GetInt32(cr, ISettingsGlobal::CDMA_ROAMING_MODE, IPhone::CDMA_RM_HOME, &settingsCdmaRoamingMode);
        if (buttonCdmaRoamingMode != settingsCdmaRoamingMode) {
            Int32 statusCdmaRoamingMode;
            switch(buttonCdmaRoamingMode) {
                case IPhone::CDMA_RM_ANY:
                    statusCdmaRoamingMode = IPhone::CDMA_RM_ANY;
                    break;
                case IPhone::CDMA_RM_HOME:
                default:
                    statusCdmaRoamingMode = IPhone::CDMA_RM_HOME;
            }
            //Set the Settings.Secure network mode
            Boolean tmp = FALSE;
            helper->PutInt32(cr, ISettingsGlobal::CDMA_ROAMING_MODE, buttonCdmaRoamingMode, &tmp);
            //Set the roaming preference mode
            AutoPtr<IMessage> m;
            mHandler->ObtainMessage(MyHandler::MESSAGE_SET_ROAMING_PREFERENCE, (IMessage**)&m);
            mPhone->SetCdmaRoamingPreference(statusCdmaRoamingMode, m);
        }
    }
    else {
        Logger::D(TAG, "onDialogClosed: positiveResult=%b value=%s -- do nothing",
                positiveResult, value.string());
    }
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos