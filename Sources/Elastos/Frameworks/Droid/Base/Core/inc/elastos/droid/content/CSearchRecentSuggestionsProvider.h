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

#ifndef __ELASTOS_DROID_CONTENT_CSEARCHRECENTSUGGESTIONSPROVIDER_H__
#define __ELASTOS_DROID_CONTENT_CSEARCHRECENTSUGGESTIONSPROVIDER_H__

#include "_Elastos_Droid_Content_CSearchRecentSuggestionsProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

namespace Elastos {
namespace Droid {
namespace Content {

class CSearchRecentSuggestionsProvider
    : public ContentProvider
    , public ISearchRecentSuggestionsProvider
{
    /**
     * Builds the database.  This version has extra support for using the version field
     * as a mode flags field, and configures the database columns depending on the mode bits
     * (features) requested by the extending class.
     *
     * @hide
     */
    class DatabaseHelper
        : public SQLiteOpenHelper
    {
    public:
        DatabaseHelper();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 newVersion);

        virtual ~DatabaseHelper();

        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

    private:
        Int32 mNewVersion;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSearchRecentSuggestionsProvider();

    virtual ~CSearchRecentSuggestionsProvider();

    CARAPI constructor();

    /**
     * This method is provided for use by the ContentResolver.  Do not override, or directly
     * call from your own code.
     */
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    /**
     * This method is provided for use by the ContentResolver.  Do not override, or directly
     * call from your own code.
     */
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    /**
     * This method is provided for use by the ContentResolver.  Do not override, or directly
     * call from your own code.
     */
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** insertedUri);

    /**
     * This method is provided for use by the ContentResolver.  Do not override, or directly
     * call from your own code.
     */
    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* succeeded);

    /**
     * This method is provided for use by the ContentResolver.  Do not override, or directly
     * call from your own code.
     */
    // TODO: Confirm no injection attacks here, or rewrite.
    // @Override
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    /**
     * This method is provided for use by the ContentResolver.  Do not override, or directly
     * call from your own code.
     */
    // TODO: Confirm no injection attacks here, or rewrite.
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

protected:
    /**
     * In order to use this class, you must extend it, and call this setup function from your
     * constructor.  In your application or activities, you must provide the same values when
     * you create the {@link android.provider.SearchRecentSuggestions} helper.
     *
     * @param authority This must match the authority that you've declared in your manifest.
     * @param mode You can use mode flags here to determine certain functional aspects of your
     * database.  Note, this value should not change from run to run, because when it does change,
     * your suggestions database may be wiped.
     *
     * @see #DATABASE_MODE_QUERIES
     * @see #DATABASE_MODE_2LINES
     */
    CARAPI SetupSuggestions(
        /* [in] */ const String& authority,
        /* [in] */ Int32 mode);

public:
    /**
     * This mode bit configures the database to record recent queries.  <i>required</i>
     *
     * @see #setupSuggestions(String, int)
     */
    static const Int32 DATABASE_MODE_QUERIES = 1;

    /**
     * This mode bit configures the database to include a 2nd annotation line with each entry.
     * <i>optional</i>
     *
     * @see #setupSuggestions(String, int)
     */
    static const Int32 DATABASE_MODE_2LINES = 2;

private:
    // debugging support
    static const String TAG;
    static const String sDatabaseName;
    static const String sSuggestions;
    static const String ORDER_BY;
    static const String NULL_COLUMN;

    // Table of database versions.  Don't forget to update!
    // NOTE:  These version values are shifted left 8 bits (x 256) in order to create space for
    // a small set of mode bitflags in the version int.
    //
    // 1      original implementation with queries, and 1 or 2 display columns
    // 1->2   added UNIQUE constraint to display1 column
    static const Int32 DATABASE_VERSION = 2 * 256;
    // Uri and query support
    static const Int32 URI_MATCH_SUGGEST = 1;

private:
    // client-provided configuration values
    String mAuthority;
    Int32 mMode;
    Boolean mTwoLineDisplay;
    // general database configuration and tables
    AutoPtr<ISQLiteOpenHelper> mOpenHelper;
    AutoPtr<IUri> mSuggestionsUri;
    AutoPtr<IUriMatcher> mUriMatcher;
    String mSuggestSuggestionClause;
    AutoPtr<ArrayOf<String> > mSuggestionProjection;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSEARCHRECENTSUGGESTIONSPROVIDER_H__
