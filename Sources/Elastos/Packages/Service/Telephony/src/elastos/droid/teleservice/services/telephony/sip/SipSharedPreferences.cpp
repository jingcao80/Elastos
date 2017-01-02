//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/teleservice/services/telephony/sip/SipSharedPreferences.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

const String SipSharedPreferences::PREFIX("[SipSharedPreferences] ");
const Boolean SipSharedPreferences::VERBOSE = FALSE; /* STOP SHIP if true */

const String SipSharedPreferences::SIP_SHARED_PREFERENCES("SIP_PREFERENCES");

const String SipSharedPreferences::KEY_PRIMARY_ACCOUNT("primary");

const String SipSharedPreferences::KEY_NUMBER_OF_PROFILES("profiles");

SipSharedPreferences::SipSharedPreferences(
    /* [in] */ IContext* context)
    : mContext(context)
{
    context->GetSharedPreferences(SIP_SHARED_PREFERENCES,
            IContext::MODE_WORLD_READABLE, (ISharedPreferences**)&mPreferences);
}

ECode SipSharedPreferences::GetPrimaryAccount(
    /* [out] */ String* account)
{
    VALIDATE_NOT_NULL(account)

    return mPreferences->GetString(KEY_PRIMARY_ACCOUNT, String(NULL), account);
}

ECode SipSharedPreferences::SetProfilesCount(
    /* [in] */ Int32 number)
{
    AutoPtr<ISharedPreferencesEditor> editor;
    mPreferences->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutInt32(KEY_NUMBER_OF_PROFILES, number);
    return editor->Apply();
}

ECode SipSharedPreferences::GetProfilesCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    return mPreferences->GetInt32(KEY_NUMBER_OF_PROFILES, 0, count);
}

ECode SipSharedPreferences::SetSipCallOption(
    /* [in] */ const String& option)
{
    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean res;
    helper->PutString(cr, ISettingsSystem::SIP_CALL_OPTIONS, option, &res);

    // Notify SipAccountRegistry in the telephony layer that the configuration has changed.
    // This causes the SIP PhoneAccounts to be re-registered.  This ensures the supported URI
    // schemes for the SIP PhoneAccounts matches the new SIP_CALL_OPTIONS setting.
    AutoPtr<IIntent> intent;
    assert(0);
    //CIntent::New(ISipManager::ACTION_SIP_CALL_OPTION_CHANGED);
    return mContext->SendBroadcast(intent);
}

ECode SipSharedPreferences::GetSipCallOption(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    String option;
    helper->GetString(cr, ISettingsSystem::SIP_CALL_OPTIONS, &option);
    String value;
    *str = (!option.IsNull()) ? option :
            (mContext->GetString(R::string::sip_address_only, &value), value);
    return NOERROR;
}

ECode SipSharedPreferences::SetReceivingCallsEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    Boolean res;
    return helper->PutInt32(cr, ISettingsSystem::SIP_RECEIVE_CALLS, (enabled ? 1 : 0), &res);
}

ECode SipSharedPreferences::IsReceivingCallsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    //try
    ECode ec;
    {
        AutoPtr<ISettingsSystem> helper;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);

        Int32 value;
        FAIL_GOTO(ec = helper->GetInt32(cr, ISettingsSystem::SIP_RECEIVE_CALLS, &value), ERROR)
        *result = value != 0;
        return NOERROR;
    }
    // catch (SettingNotFoundException e)
ERROR:
    if (ec == (ECode)E_SETTING_NOT_FOUND_EXCEPTION) {
        Log(String("isReceivingCallsEnabled, option not set; use default value, exception: SettingNotFoundException"));
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode SipSharedPreferences::CleanupPrimaryAccountSetting()
{
    Boolean res;
    if (mPreferences->Contains(KEY_PRIMARY_ACCOUNT, &res), res) {
        AutoPtr<ISharedPreferencesEditor> editor;
        mPreferences->Edit((ISharedPreferencesEditor**)&editor);
        editor->Remove(KEY_PRIMARY_ACCOUNT);
        editor->Apply();
    }
    return NOERROR;
}

void SipSharedPreferences::Log(
    /* [in] */ const String& msg)
{
    Logger::D(SipUtil::TAG, PREFIX + msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos