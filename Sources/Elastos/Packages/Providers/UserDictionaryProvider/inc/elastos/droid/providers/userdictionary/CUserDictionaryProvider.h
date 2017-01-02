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

#ifndef __ELASTOS_DROID_PROVIDER_USERDICTIONARY_CUSERDICTIONARYPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_USERDICTIONARY_CUSERDICTIONARYPROVIDER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Providers_UserDictionary_CUserDictionaryProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace UserDictionary {

/**
 * Provides access to a database of user defined words. Each item has a word and a frequency.
 */
CarClass(CUserDictionaryProvider)
    , public ContentProvider
{
private:
    /**
     * This class helps open, create, and upgrade the database file.
     */
    class DatabaseHelper: public SQLiteOpenHelper
    {
    public:

        DatabaseHelper(
            /* [in] */ IContext* context);

        // @Override
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        // @Override
        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);
    };

public:
    CAR_OBJECT_DECL()

    CUserDictionaryProvider();

    ~CUserDictionaryProvider();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* succeeded);

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
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** result);

    // @Override
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

private:
    static Boolean Initstatic();

private:
    static const String AUTHORITY;

    static const String TAG;

    static const AutoPtr<IUri> CONTENT_URI;

    static const String DATABASE_NAME;
    static const Int32 DATABASE_VERSION;

    static const String USERDICT_TABLE_NAME;

    static AutoPtr<IHashMap> /*HashMap<String, String>*/ sDictProjectionMap;

    static AutoPtr<IUriMatcher> sUriMatcher;

    static const Int32 WORDS;

    static const Int32 WORD_ID;

    static Boolean s_Init;

    AutoPtr<IBackupManager> mBackupManager;

    AutoPtr<DatabaseHelper> mOpenHelper;
};

} // namespace UserDictionary
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_USERDICTIONARY_CUSERDICTIONARYPROVIDER_H__
