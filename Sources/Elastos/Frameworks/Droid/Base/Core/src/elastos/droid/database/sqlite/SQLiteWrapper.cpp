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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/database/sqlite/SQLiteWrapper.h"
#include "elastos/droid/widget/Toast.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/R.h"

using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

const String SQLiteWrapper::TAG("SQLiteWrapper");
const String SQLiteWrapper::SQLITE_EXCEPTION_DETAIL_MESSAGE("unable to open database file");

Boolean SQLiteWrapper::IsLowMemory(
        /* [in] */ ECode e)
{
    return e == (ECode)E_SQLITE_EXCEPTION_DETAIL_MESSAGE;
}

ECode SQLiteWrapper::CheckSQLiteException(
    /* [in] */ IContext* context,
    /* [in] */ ECode e)
{
    if (IsLowMemory(e)) {
        AutoPtr<IToast> toast;
        assert(0 && "weit for adding dirs:widget to join compiled");
        //Toast::MakeText(context, R::string::low_memory, IToast::LENGTH_SHORT, (IToast**)&toast);
        return toast->Show();
    }
    else {
        return e;
    }
}

ECode SQLiteWrapper::Query(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<ICursor> cursor;
    ECode ec = resolver->Query(uri, projection, selection, selectionArgs, sortOrder, (ICursor**)&cursor);
    if (ec == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Catch a SQLiteException when query: 0x%08x", ec);
        *result = NULL;
        return CheckSQLiteException(context, ec);
    }
    *result = cursor;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when query: ", e);
    //     checkSQLiteException(context, e);
    //     return null;
    // }
}

ECode SQLiteWrapper::Requery(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [out] */ Boolean* isRequery)
{
    VALIDATE_NOT_NULL(isRequery)
    // try {
    Boolean result;
    ECode ec = cursor->Requery(&result);
    if (ec == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Catch a SQLiteException when query:  0x%08x", ec);
        *isRequery = FALSE;
        return CheckSQLiteException(context, ec);
    }
    *isRequery = result;
    return NOERROR;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when requery: ", e);
    //     checkSQLiteException(context, e);
    //     return false;
    // }
}

ECode SQLiteWrapper::Update(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    Int32 value;
    ECode ec = resolver->Update(uri, values, where, selectionArgs, &value);
    if (ec == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Catch a SQLiteException when update: 0x%08x", ec);
        *result = -1;
        return CheckSQLiteException(context, ec);
    }
    *result = value;
    return NOERROR;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when update: ", e);
    //     checkSQLiteException(context, e);
    //     return -1;
    // }
}

ECode SQLiteWrapper::Delete(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    Int32 value;
    ECode ec = resolver->Delete(uri, where, selectionArgs, &value);
    if (ec == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Catch a SQLiteException when delete: 0x%08x", ec);
        *result = -1;
        return CheckSQLiteException(context, ec);
    }
    *result = value;
    return NOERROR;
}

ECode SQLiteWrapper::Insert(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IUri> outUri;
    ECode ec = resolver->Insert(uri, values, (IUri**)&outUri);
    if (ec == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Catch a SQLiteException when insert: 0x%08x", ec);
        result = NULL;
        return CheckSQLiteException(context, ec);
    }
    *result = outUri;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when insert: ", e);
    //     checkSQLiteException(context, e);
    //     return null;
    // }
}

} //Sqlite
} //Database
} //Droid
} //Elastos
