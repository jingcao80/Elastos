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
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsSettings.h"
#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::Uri;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsSettings)

CAR_INTERFACE_IMPL_3(CContactsSettings, Singleton, IBaseColumns, IContactsSettings, IContactsSettingsColumns)

ECode CContactsSettings::constructor()
{
    return NOERROR;
}

ECode CContactsSettings::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/settings"), uri);
}

ECode CContactsSettings::GetSetting(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& account,
    /* [in] */ const String& key,
    /* [out] */ String* setting)
{
    VALIDATE_NOT_NULL(setting);

    // For now we only support a single account and the UI doesn't know what
    // the account name is, so we're using a global setting for SYNC_EVERYTHING.
    // Some day when we add multiple accounts to the UI this should honor the account
    // that was asked for.
    String selectString;
    AutoPtr<ArrayOf<String> > selectArgs;
    if (FALSE) {
        selectString = (account.IsNull())
                ? String("_sync_account is null AND key=?")
                : String("_sync_account=? AND key=?");
//                : "_sync_account=? AND _sync_account_type=? AND key=?";
        if (account.IsNull()) {
            selectArgs = ArrayOf<String>::Alloc(1);
            (*selectArgs)[0] = key;
        }
        else {
            selectArgs = ArrayOf<String>::Alloc(2);
            (*selectArgs)[0] = account;
            (*selectArgs)[1] = key;
        }
    }
    else {
        selectString = String("key=?");
        selectArgs = ArrayOf<String>::Alloc(1);
        (*selectArgs)[0] = key;
    }
    AutoPtr<ICursor> cursor;
    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    AutoPtr<ArrayOf<String> > args;
    args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IContactsSettingsColumns::VALUE;
    FAIL_RETURN(cr->Query(uri, args, selectString, selectArgs, String(NULL), (ICursor**)&cursor))

    //try {
    Boolean isNext;
    FAIL_GOTO(cursor->MoveToNext(&isNext), EXIT)
    if (!isNext) {
        *setting = String(NULL);
    }
    else {
        FAIL_GOTO(cursor->GetString(0, setting), EXIT)
    }
    //} finally {
EXIT:
    return ICloseable::Probe(cursor)->Close();
    //}
}

ECode CContactsSettings::SetSetting(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IContentValues> values;
    FAIL_RETURN(CContentValues::New((IContentValues**)&values))
    // For now we only support a single account and the UI doesn't know what
    // the account name is, so we're using a global setting for SYNC_EVERYTHING.
    // Some day when we add multiple accounts to the UI this should honor the account
    // that was asked for.
    //values.put(_SYNC_ACCOUNT, account.mName);
    //values.put(_SYNC_ACCOUNT_TYPE, account.mType);
    AutoPtr<ICharSequence> keyStr;
    FAIL_RETURN(CString::New(key, (ICharSequence**)&keyStr))
    FAIL_RETURN(values->Put(IContactsSettingsColumns::KEY, keyStr))
    AutoPtr<ICharSequence> valueStr;
    FAIL_RETURN(CString::New(value, (ICharSequence**)&valueStr))
    FAIL_RETURN(values->Put(IContactsSettingsColumns::VALUE, valueStr))
    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    Int32 result;
    return cr->Update(uri, values, String(NULL), NULL, &result);
}

} // Provider
} // Droid
} // Elastos