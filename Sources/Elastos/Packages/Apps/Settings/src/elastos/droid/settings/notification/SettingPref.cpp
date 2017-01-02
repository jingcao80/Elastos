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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/notification/SettingPref.h"
#include "elastos/droid/settings/notification/CDropDownPreference.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Core::CoreUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  SettingPref::PreferenceOnPreferenceChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(SettingPref::PreferenceOnPreferenceChangeListener, Object, IPreferenceOnPreferenceChangeListener)

SettingPref::PreferenceOnPreferenceChangeListener::PreferenceOnPreferenceChangeListener(
    /* [in] */ SettingPref* host,
    /* [in] */ IContext* context)
    : mHost(host)
    , mContext(context)
{}

ECode SettingPref::PreferenceOnPreferenceChangeListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    mHost->SetSetting(mContext, IBoolean::Probe(newValue) != NULL ? 1 : 0, &res);
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  SettingPref::DropDownPreferenceCallback
//===============================================================================

CAR_INTERFACE_IMPL(SettingPref::DropDownPreferenceCallback, Object, IDropDownPreferenceCallback)

SettingPref::DropDownPreferenceCallback::DropDownPreferenceCallback(
    /* [in] */ SettingPref* host,
    /* [in] */ IContext* context)
    : mHost(host)
    , mContext(context)
{}

ECode SettingPref::DropDownPreferenceCallback::OnItemSelected(
    /* [in] */ Int32 pos,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 data;
    IInteger32::Probe(value)->GetValue(&data);
    return mHost->SetSetting(mContext, data, result);
}

//===============================================================================
//                  SettingPref
//===============================================================================

const Int32 SettingPref::TYPE_GLOBAL;
const Int32 SettingPref::TYPE_SYSTEM;

CAR_INTERFACE_IMPL(SettingPref, Object, ISettingPref)

SettingPref::SettingPref(
    /* [in] */ Int32 type,
    /* [in] */ const String& key,
    /* [in] */ const String& setting,
    /* [in] */ Int32 def,
    /* [in] */ ArrayOf<Int32>* values)
{
    mType = type;
    mKey = key;
    mSetting = setting;
    mDefault = def;
    mValues = values;
    GetUriFor(mType, mSetting, (IUri**)&mUri);
}

Boolean SettingPref::IsApplicable(
    /* [in] */ IContext* context)
{
    return TRUE;
}

ECode SettingPref::GetCaption(
    /* [in] */ IResources* res,
    /* [in] */ Int32 value,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

AutoPtr<IPreference> SettingPref::Init(
    /* [in] */ SettingsPreferenceFragment* settings)
{
    AutoPtr<IActivity> activity;
    settings->GetActivity((IActivity**)&activity);
    AutoPtr<IContext> context = IContext::Probe(activity);
    AutoPtr<IPreferenceScreen> screen;
    settings->GetPreferenceScreen((IPreferenceScreen**)&screen);
    IPreferenceGroup* _screen = IPreferenceGroup::Probe(screen);
    AutoPtr<IPreference> p;
    _screen->FindPreference(CoreUtils::Convert(mKey), (IPreference**)&p);
    Boolean res;
    if (p != NULL && !IsApplicable(context)) {
        _screen->RemovePreference(p, &res);
        p = NULL;
    }
    if (ITwoStatePreference::Probe(p) != NULL) {
        mTwoState = ITwoStatePreference::Probe(p);
    }
    else if (IDropDownPreference::Probe(p) != NULL) {
        mDropDown = IDropDownPreference::Probe(p);
        for (Int32 i = 0; i < mValues->GetLength(); i++) {
            Int32 value = (*mValues)[i];
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            String str;
            GetCaption(resources, value, &str);
            ((CDropDownPreference*)mDropDown.Get())->AddItem(str, CoreUtils::Convert(value));
        }
    }
    Update(context);
    if (mTwoState != NULL) {
        AutoPtr<PreferenceOnPreferenceChangeListener> listener = new PreferenceOnPreferenceChangeListener(this, context);
        p->SetOnPreferenceChangeListener(listener);
        return IPreference::Probe(mTwoState);
    }
    if (mDropDown != NULL) {
        AutoPtr<DropDownPreferenceCallback> callback = new DropDownPreferenceCallback(this, context);
        ((CDropDownPreference*)mDropDown.Get())->SetCallback(callback);
        return IPreference::Probe(mDropDown);
    }
    return NULL;
}

ECode SettingPref::SetSetting(
    /* [in] */ IContext* context,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    return PutInt32(mType, resolver, mSetting, value, result);
}

AutoPtr<IUri> SettingPref::GetUri()
{
    return mUri;
}

String SettingPref::GetKey()
{
    return mKey;
}

ECode SettingPref::Update(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    Int32 val;
    GetInt32(mType, resolver, mSetting, mDefault, &val);
    if (mTwoState != NULL) {
        mTwoState->SetChecked(val != 0);
    }
    else if (mDropDown != NULL) {
        ((CDropDownPreference*)mDropDown.Get())->SetSelectedValue(CoreUtils::Convert(val));
    }
    return NOERROR;
}

ECode SettingPref::GetUriFor(
    /* [in] */ Int32 type,
    /* [in] */ const String& setting,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    switch (type) {
        case TYPE_GLOBAL: {
            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            return global->GetUriFor(setting, result);
        }
        case TYPE_SYSTEM: {
            AutoPtr<ISettingsSystem> sys;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
            return sys->GetUriFor(setting, result);
        }
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode SettingPref::PutInt32(
    /* [in] */ Int32 type,
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& setting,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    switch (type) {
        case TYPE_GLOBAL: {
            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            return global->PutInt32(cr, setting, value, result);
        }
        case TYPE_SYSTEM: {
            AutoPtr<ISettingsSystem> sys;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
            return sys->PutInt32(cr, setting, value, result);
        }
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode SettingPref::GetInt32(
    /* [in] */ Int32 type,
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& setting,
    /* [in] */ Int32 def,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    switch (type) {
        case TYPE_GLOBAL: {
            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            return global->GetInt32(cr, setting, def, result);
        }
        case TYPE_SYSTEM: {
            AutoPtr<ISettingsSystem> sys;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
            return sys->GetInt32(cr, setting, def, result);
        }
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos