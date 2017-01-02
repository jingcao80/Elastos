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

#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CMMSPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CMMSPROVIDER_H__

#include "_Elastos_Droid_Providers_Telephony_CMmsProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

/**
 * The class to provide base facility to access MMS related content,
 * which is stored in a SQLite database and in the file system.
 */
CarClass(CMmsProvider)
    , public ContentProvider
    , public IMmsProvider
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** result);

    // @Override
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* result);

    // @Override
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** _result);

    // @Override
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** result) /*throws FileNotFoundException*/;

    static CARAPI_(Int32) DeleteMessages(
        /* [in] */ IContext* context,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ IUri* uri);

private:
    CARAPI_(void) ConstructQueryForBox(
        /* [in] */ ISQLiteQueryBuilder* qb,
        /* [in] */ Int32 msgBox);

    CARAPI GetMessageBoxByMatch(
        /* [in] */ Int32 match,
        /* [out] */ Int32* result);

    static CARAPI_(Int32) DeleteParts(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    static CARAPI_(Int32) DeleteTempDrmData(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    static CARAPI_(Int32) DeleteDataRows(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    CARAPI_(void) FilterUnsupportedKeys(
        /* [in] */ IContentValues* values);

    CARAPI_(void) NotifyChange();

    static CARAPI_(Boolean) Init();

    static CARAPI_(String) ConcatSelections(
        /* [in] */ const String& selection1,
        /* [in] */ const String& selection2);

private:
    static const String TAG;
    static const String VND_ANDROID_MMS;
    static const String VND_ANDROID_DIR_MMS;
    static const Boolean DEBUG;
    static const Boolean LOCAL_LOGV;

    static const Int32 MMS_ALL;
    static const Int32 MMS_ALL_ID;
    static const Int32 MMS_INBOX;
    static const Int32 MMS_INBOX_ID;
    static const Int32 MMS_SENT;
    static const Int32 MMS_SENT_ID;
    static const Int32 MMS_DRAFTS;
    static const Int32 MMS_DRAFTS_ID;
    static const Int32 MMS_OUTBOX;
    static const Int32 MMS_OUTBOX_ID;
    static const Int32 MMS_ALL_PART;
    static const Int32 MMS_MSG_PART;
    static const Int32 MMS_PART_ID;
    static const Int32 MMS_MSG_ADDR;
    static const Int32 MMS_SENDING_RATE;
    static const Int32 MMS_REPORT_STATUS;
    static const Int32 MMS_REPORT_REQUEST;
    static const Int32 MMS_DRM_STORAGE;
    static const Int32 MMS_DRM_STORAGE_ID;
    static const Int32 MMS_THREADS;
    static const Int32 MMS_PART_RESET_FILE_PERMISSION;
    static const Int32 MMS_THREAD_ID;

    static AutoPtr<IUriMatcher> sURLMatcher;
    static Boolean sInit;

    AutoPtr<SQLiteOpenHelper> mOpenHelper;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CMMSPROVIDER_H__
