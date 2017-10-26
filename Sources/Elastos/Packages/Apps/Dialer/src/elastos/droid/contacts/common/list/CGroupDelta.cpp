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

#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/contacts/common/list/CGroupDelta.h"
#include "elastos/droid/contacts/common/list/CCustomContactListFilterActivity.h"
#include "elastos/droid/contacts/common/util/LocalizedNameResolver.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::List::EIID_IGroupDelta;
using Elastos::Droid::Contacts::Common::Util::LocalizedNameResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentProviderOperationHelper;
using Elastos::Droid::Content::CContentProviderOperationHelper;
using Elastos::Droid::Content::IContentProviderOperationBuilder;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IContactsContractSettings;
using Elastos::Droid::Provider::CContactsContractSettings;
using Elastos::Droid::Provider::IContactsContractSettingsColumns;
using Elastos::Droid::Provider::IContactsContractGroupsColumns;
using Elastos::Droid::Provider::IContactsContractGroups;
using Elastos::Droid::Provider::CContactsContractGroups;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

CGroupDelta::CGroupDelta()
    : ValuesDelta()
    , mUngrouped(FALSE)
    , mAccountHasGroups(FALSE)
{}

CAR_INTERFACE_IMPL(CGroupDelta, ValuesDelta, IGroupDelta)

CAR_OBJECT_IMPL(CGroupDelta)

ECode CGroupDelta::constructor()
{
    return ValuesDelta::constructor();
}

AutoPtr<IGroupDelta> CGroupDelta::FromSettings(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& accountName,
    /* [in] */ const String& accountType,
    /* [in] */ const String& dataSet,
    /* [in] */ Boolean accountHasGroups)
{
    AutoPtr<IContactsContractSettings> settings;
    CContactsContractSettings::AcquireSingleton((IContactsContractSettings**)&settings);
    AutoPtr<IUri> uri;
    settings->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> settingsUri;
    uri->BuildUpon((IUriBuilder**)&settingsUri);
    settingsUri->AppendQueryParameter(IContactsContractSettingsColumns::ACCOUNT_NAME, accountName);
    settingsUri->AppendQueryParameter(IContactsContractSettingsColumns::ACCOUNT_TYPE, accountType);
    if (!dataSet.IsNull()) {
        settingsUri->AppendQueryParameter(IContactsContractSettingsColumns::DATA_SET, dataSet);
    }
    AutoPtr<IUri> result;
    settingsUri->Build((IUri**)&result);
    AutoPtr<ArrayOf<String> > atts = ArrayOf<String>::Alloc(2);
    (*atts)[0] = IContactsContractSettingsColumns::SHOULD_SYNC;
    (*atts)[1] = IContactsContractSettingsColumns::UNGROUPED_VISIBLE;
    AutoPtr<ICursor> cursor;
    resolver->Query(result, atts, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    // try {
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IContactsContractSettingsColumns::ACCOUNT_NAME, accountName);
    values->Put(IContactsContractSettingsColumns::ACCOUNT_TYPE, accountType);
    values->Put(IContactsContractSettingsColumns::DATA_SET, dataSet);

    Boolean first;
    if (cursor != NULL && (cursor->MoveToFirst(&first), first)) {
        // Read existing values when present
        Int32 value0, value1;
        cursor->GetInt32(0, &value0);
        cursor->GetInt32(1, &value1);
        values->Put(IContactsContractSettingsColumns::SHOULD_SYNC, value0);
        values->Put(IContactsContractSettingsColumns::UNGROUPED_VISIBLE, value1);
        AutoPtr<IGroupDelta> delta = FromBefore(values);
        delta->SetUngrouped(accountHasGroups);
        if (cursor != NULL) ICloseable::Probe(cursor)->Close();
        return delta;
    }
    else {
        // Nothing found, so treat as create
        values->Put(IContactsContractSettingsColumns::SHOULD_SYNC, CCustomContactListFilterActivity::DEFAULT_SHOULD_SYNC);
        values->Put(IContactsContractSettingsColumns::UNGROUPED_VISIBLE, CCustomContactListFilterActivity::DEFAULT_VISIBLE);
        AutoPtr<IGroupDelta> delta = FromAfter(values);
        delta->SetUngrouped(accountHasGroups);
        if (cursor != NULL) ICloseable::Probe(cursor)->Close();
        return delta;
    }
    // } finally {
    //     if (cursor != null) cursor.close();
    // }
}

AutoPtr<IGroupDelta> CGroupDelta::FromBefore(
    /* [in] */ IContentValues* before)
{
    AutoPtr<CGroupDelta> entry;
    CGroupDelta::NewByFriend((CGroupDelta**)&entry);
    entry->mBefore = before;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    entry->mAfter = values;
    return IGroupDelta::Probe(entry);
}

AutoPtr<IGroupDelta> CGroupDelta::FromAfter(
    /* [in] */ IContentValues* after)
{
    AutoPtr<CGroupDelta> entry;
    CGroupDelta::NewByFriend((CGroupDelta**)&entry);
    entry->mBefore = NULL;
    entry->mAfter = after;
    return IGroupDelta::Probe(entry);
}

ECode CGroupDelta::GetUngrouped(
    /* [out] */ Boolean* ungrouped)
{
    VALIDATE_NOT_NULL(ungrouped)
    *ungrouped = mUngrouped;
    return NOERROR;
}

ECode CGroupDelta::SetUngrouped(
    /* [in] */ Boolean accountHasGroups)
{
    mUngrouped = TRUE;
    mAccountHasGroups = accountHasGroups;
    return NOERROR;
}

ECode CGroupDelta::BeforeExists(
    /* [out] */ Boolean* isBefore)
{
    VALIDATE_NOT_NULL(isBefore)
    *isBefore = mBefore != NULL;
    return NOERROR;
}

ECode CGroupDelta::GetShouldSync(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> defaultValue;
    CInteger32::New(CCustomContactListFilterActivity::DEFAULT_SHOULD_SYNC, (IInteger32**)&defaultValue);
    AutoPtr<IInteger32> integer;
    GetAsInteger(mUngrouped ? IContactsContractSettingsColumns::SHOULD_SYNC : IContactsContractGroupsColumns::SHOULD_SYNC,
            defaultValue, (IInteger32**)&integer);
    Int32 value;
    integer->GetValue(&value);
    *result = value != 0;
    return NOERROR;
}

ECode CGroupDelta::GetVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> integer;
    GetAsInteger(mUngrouped ? IContactsContractSettingsColumns::UNGROUPED_VISIBLE : IContactsContractGroupsColumns::GROUP_VISIBLE,
            CoreUtils::Convert(CCustomContactListFilterActivity::DEFAULT_VISIBLE), (IInteger32**)&integer);
    Int32 value;
    integer->GetValue(&value);
    *result = value != 0;
    return NOERROR;
}

ECode CGroupDelta::PutShouldSync(
    /* [in] */ Boolean shouldSync)
{
    return Put(mUngrouped ? IContactsContractSettingsColumns::SHOULD_SYNC : IContactsContractGroupsColumns::SHOULD_SYNC,
            shouldSync ? 1 : 0);
}

ECode CGroupDelta::PutVisible(
    /* [in] */ Boolean visible)
{
    return Put(mUngrouped ? IContactsContractSettingsColumns::UNGROUPED_VISIBLE : IContactsContractGroupsColumns::GROUP_VISIBLE,
            visible ? 1 : 0);
}

String CGroupDelta::GetAccountType()
{
    String str;
    AutoPtr<IContentValues> values = mBefore == NULL ? mAfter : mBefore;
    values->GetAsString(IContactsContractSettingsColumns::ACCOUNT_TYPE, &str);
    return str;
}

ECode CGroupDelta::GetTitle(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    if (mUngrouped) {
        String customAllContactsName;
        LocalizedNameResolver::GetAllContactsName(context, GetAccountType(), &customAllContactsName);
        if (!customAllContactsName.IsNull()) {
            return CString::New(customAllContactsName, title);
        }
        if (mAccountHasGroups) {
            return context->GetText(Elastos::Droid::Dialer::R::string::display_ungrouped, title);
        }
        else {
            return context->GetText(Elastos::Droid::Dialer::R::string::display_all_contacts, title);
        }
    }
    else {
        AutoPtr<IInteger32> titleRes;
        GetAsInteger(IContactsContractGroupsColumns::TITLE_RES, (IInteger32**)&titleRes);
        if (titleRes != NULL) {
            String packageName;
            GetAsString(IContactsContractGroupsColumns::RES_PACKAGE, &packageName);
            AutoPtr<IPackageManager> pkgManager;
            context->GetPackageManager((IPackageManager**)&pkgManager);
            Int32 value;
            titleRes->GetValue(&value);
            return pkgManager->GetText(packageName, value, NULL, title);
        }
        else {
            String str;
            GetAsString(IContactsContractGroupsColumns::TITLE, &str);
            return CString::New(str, title);
        }
    }
}

ECode CGroupDelta::BuildDiff(
    /* [out] */ IContentProviderOperation** operation)
{
    VALIDATE_NOT_NULL(operation)
    *operation = NULL;
    Boolean isInsert;
    Boolean isUpdate;
    if (IsInsert(&isInsert), isInsert) {
        // Only allow inserts for Settings
        if (mUngrouped) {
            mAfter->Remove(mIdColumn);
            AutoPtr<IContentProviderOperationHelper> helper;
            CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
            AutoPtr<IContactsContractSettings> settings;
            CContactsContractSettings::AcquireSingleton((IContactsContractSettings**)&settings);
            AutoPtr<IUri> uri;
            settings->GetCONTENT_URI((IUri**)&uri);
            AutoPtr<IContentProviderOperationBuilder> builder;
            helper->NewInsert(uri, (IContentProviderOperationBuilder**)&builder);
            builder->WithValues(mAfter);
            return builder->Build(operation);
        }
        else {
            Logger::E("CGroupDelta", "Unexpected diff");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else if (IsUpdate(&isUpdate), isUpdate) {
        if (mUngrouped) {
            String accountName;
            GetAsString(IContactsContractSettingsColumns::ACCOUNT_NAME, &accountName);
            String accountType;
            GetAsString(IContactsContractSettingsColumns::ACCOUNT_TYPE, &accountType);
            String dataSet;
            GetAsString(IContactsContractSettingsColumns::DATA_SET, &dataSet);
            StringBuilder selection(IContactsContractSettingsColumns::ACCOUNT_NAME);
            selection.Append("=? AND ");
            selection.Append(IContactsContractSettingsColumns::ACCOUNT_TYPE);
            selection.Append("=?");
            AutoPtr<ArrayOf<String> > selectionArgs;
            if (dataSet.IsNull()) {
                selection.Append(" AND ");
                selection.Append(IContactsContractSettingsColumns::DATA_SET);
                selection.Append(" IS NULL");
                selectionArgs = ArrayOf<String>::Alloc(2);
                (*selectionArgs)[0] = accountName;
                (*selectionArgs)[1] = accountType;
            }
            else {
                selection.Append(" AND ");
                selection.Append(IContactsContractSettingsColumns::DATA_SET);
                selection.Append("=?");
                selectionArgs = ArrayOf<String>::Alloc(3);
                (*selectionArgs)[0] = accountName;
                (*selectionArgs)[1] = accountType;
                (*selectionArgs)[2] = dataSet;
            }
            AutoPtr<IContentProviderOperationHelper> helper;
            CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
            AutoPtr<IContactsContractSettings> settings;
            CContactsContractSettings::AcquireSingleton((IContactsContractSettings**)&settings);
            AutoPtr<IUri> uri;
            settings->GetCONTENT_URI((IUri**)&uri);
            AutoPtr<IContentProviderOperationBuilder> builder;
            helper->NewUpdate(uri, (IContentProviderOperationBuilder**)&builder);
            builder->WithSelection(selection.ToString(), selectionArgs);
            builder->WithValues(mAfter);
            return builder->Build(operation);
        }
        else {
            AutoPtr<IContentProviderOperationHelper> helper;
            CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
            AutoPtr<IContactsContractGroups> groups;
            CContactsContractGroups::AcquireSingleton((IContactsContractGroups**)&groups);
            AutoPtr<IUri> groupUri;
            groups->GetCONTENT_URI((IUri**)&groupUri);
            AutoPtr<IUri> uri = CCustomContactListFilterActivity::AddCallerIsSyncAdapterParameter(groupUri);
            AutoPtr<IContentProviderOperationBuilder> builder;
            helper->NewUpdate(uri, (IContentProviderOperationBuilder**)&builder);
            AutoPtr<IInteger64> id;
            GetId((IInteger64**)&id);
            Int64 value;
            id->GetValue(&value);
            builder->WithSelection(IBaseColumns::ID + "=" + StringUtils::ToString(value), NULL);
            builder->WithValues(mAfter);
            return builder->Build(operation);
        }
    }
    else {
        return NOERROR;
    }
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
