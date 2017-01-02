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
#include "elastos/droid/contacts/common/preference/ContactsPreferences.h"
#include "R.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_ISharedPreferencesOnSharedPreferenceChangeListener;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Preference {

//=================================================================
// ContactsPreferences::OnChangeRunnable
//=================================================================
ECode ContactsPreferences::OnChangeRunnable::Run()
{
    if (DISPLAY_ORDER_KEY.Equals(mKey)) {
        mHost->mDisplayOrder = mHost->GetDisplayOrder();
    }
    else if (SORT_ORDER_KEY.Equals(mKey)) {
        mHost->mSortOrder = mHost->GetSortOrder();
    }
    if (mHost->mListener != NULL) mHost->mListener->OnChange();
    return NOERROR;
}

const Int32 ContactsPreferences::DISPLAY_ORDER_PRIMARY;
const Int32 ContactsPreferences::DISPLAY_ORDER_ALTERNATIVE;
const String ContactsPreferences::DISPLAY_ORDER_KEY("android.contacts.DISPLAY_ORDER");
const Int32 ContactsPreferences::SORT_ORDER_PRIMARY;
const String ContactsPreferences::SORT_ORDER_KEY("android.contacts.SORT_ORDER");
const Int32 ContactsPreferences::SORT_ORDER_ALTERNATIVE;
const String ContactsPreferences::PREF_DISPLAY_ONLY_PHONES("only_phones");
const Boolean ContactsPreferences::PREF_DISPLAY_ONLY_PHONES_DEFAULT;

CAR_INTERFACE_IMPL(ContactsPreferences, Object, ISharedPreferencesOnSharedPreferenceChangeListener)

ContactsPreferences::ContactsPreferences(
    /* [in] */ IContext* context)
    : mSortOrder(-1)
    , mDisplayOrder(-1)
{
    mContext = context;
    CHandler::New((IHandler**)&mHandler);
    String pkgName;
    context->GetPackageName(&pkgName);
    mContext->GetSharedPreferences(pkgName,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&mPreferences);
    MaybeMigrateSystemSettings();
}

Boolean ContactsPreferences::IsSortOrderUserChangeable()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean value;
    res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_sort_order_user_changeable, &value);
    return value;
}

Int32 ContactsPreferences::GetDefaultSortOrder()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean value;
    if (res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_default_sort_order_primary, &value), value) {
        return SORT_ORDER_PRIMARY;
    }
    else {
        return SORT_ORDER_ALTERNATIVE;
    }
}

Int32 ContactsPreferences::GetSortOrder()
{
    if (!IsSortOrderUserChangeable()) {
        return GetDefaultSortOrder();
    }
    if (mSortOrder == -1) {
        mPreferences->GetInt32(SORT_ORDER_KEY, GetDefaultSortOrder(), &mSortOrder);
    }
    return mSortOrder;
}

void ContactsPreferences::SetSortOrder(
    /* [in] */ Int32 sortOrder)
{
    mSortOrder = sortOrder;
    AutoPtr<ISharedPreferencesEditor> editor;
    mPreferences->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutInt32(SORT_ORDER_KEY, sortOrder);
    Boolean result;
    editor->Commit(&result);
}

Boolean ContactsPreferences::IsDisplayOrderUserChangeable()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean value;
    res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_display_order_user_changeable, &value);
    return value;
}

Int32 ContactsPreferences::GetDefaultDisplayOrder()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean value;
    if (res->GetBoolean(Elastos::Droid::Dialer::R::bool_::config_default_display_order_primary, &value), value) {
        return DISPLAY_ORDER_PRIMARY;
    }
    else {
        return DISPLAY_ORDER_ALTERNATIVE;
    }
}

Int32 ContactsPreferences::GetDisplayOrder()
{
    if (!IsDisplayOrderUserChangeable()) {
        return GetDefaultDisplayOrder();
    }
    if (mDisplayOrder == -1) {
        mPreferences->GetInt32(DISPLAY_ORDER_KEY, GetDefaultDisplayOrder(), &mDisplayOrder);
    }
    return mDisplayOrder;
}

void ContactsPreferences::SetDisplayOrder(
    /* [in] */ Int32 displayOrder)
{
    mDisplayOrder = displayOrder;
    AutoPtr<ISharedPreferencesEditor> editor;
    mPreferences->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutInt32(DISPLAY_ORDER_KEY, displayOrder);
    Boolean result;
    editor->Commit(&result);
}

void ContactsPreferences::RegisterChangeListener(
    /* [in] */ IContactsPreferencesChangeListener* listener)
{
    if (mListener != NULL) UnregisterChangeListener();

    mListener = listener;

    // Reset preferences to "unknown" because they may have changed while the
    // listener was unregistered.
    mDisplayOrder = -1;
    mSortOrder = -1;

    mPreferences->RegisterOnSharedPreferenceChangeListener(this);
}

void ContactsPreferences::UnregisterChangeListener()
{
    if (mListener != NULL) {
        mListener = NULL;
    }

    mPreferences->UnregisterOnSharedPreferenceChangeListener(this);
}

ECode ContactsPreferences::OnSharedPreferenceChanged(
    /* [in] */ ISharedPreferences* sharedPreferences,
    /* [in] */ const String& key)
{
    // This notification is not sent on the Ui thread. Use the previously created Handler
    // to switch to the Ui thread
    AutoPtr<IRunnable> runnable = (IRunnable*)new OnChangeRunnable(key, this);
    Boolean result;
    mHandler->Post(runnable, &result);
    return NOERROR;
}

void ContactsPreferences::MaybeMigrateSystemSettings()
{
    Boolean contains;
    if (mPreferences->Contains(SORT_ORDER_KEY, &contains), !contains) {
        Int32 sortOrder = GetDefaultSortOrder();
        // try {
        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        settingsSystem->GetInt32(cr, SORT_ORDER_KEY, &sortOrder);
        // } catch (SettingNotFoundException e) {
        // }
        SetSortOrder(sortOrder);
    }

    if (mPreferences->Contains(DISPLAY_ORDER_KEY, &contains), !contains) {
        Int32 displayOrder = GetDefaultDisplayOrder();
        // try {
        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        settingsSystem->GetInt32(cr, DISPLAY_ORDER_KEY, &displayOrder);
        // } catch (SettingNotFoundException e) {
        // }
        SetDisplayOrder(displayOrder);
    }
}

} // namespace Preference
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
