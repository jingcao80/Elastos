
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CSearchRecentSuggestionsProvider.h"
#include "elastos/droid/content/CUriMatcher.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::App::ISearchManager;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

const Int32 CSearchRecentSuggestionsProvider::DATABASE_MODE_QUERIES;
const Int32 CSearchRecentSuggestionsProvider::DATABASE_MODE_2LINES;
const String CSearchRecentSuggestionsProvider::TAG("SuggestionsProvider");
const String CSearchRecentSuggestionsProvider::sDatabaseName("suggestions.db");
const String CSearchRecentSuggestionsProvider::sSuggestions("suggestions");
const String CSearchRecentSuggestionsProvider::ORDER_BY("date DESC");
const String CSearchRecentSuggestionsProvider::NULL_COLUMN("query");
const Int32 CSearchRecentSuggestionsProvider::DATABASE_VERSION;
const Int32 CSearchRecentSuggestionsProvider::URI_MATCH_SUGGEST;

//==============================================================
// CSearchRecentSuggestionsProvider::DatabaseHelper
//==============================================================
CSearchRecentSuggestionsProvider::DatabaseHelper::DatabaseHelper()
    : mNewVersion(0)
{}

CSearchRecentSuggestionsProvider::DatabaseHelper::~DatabaseHelper()
{}

ECode CSearchRecentSuggestionsProvider::DatabaseHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 newVersion)
{
    FAIL_RETURN(SQLiteOpenHelper::constructor(context, sDatabaseName, NULL, newVersion))
    mNewVersion = newVersion;
    return NOERROR;
}

ECode CSearchRecentSuggestionsProvider::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    StringBuilder sb;
    sb += "CREATE TABLE suggestions (";
    sb += "_id INTEGER PRIMARY KEY";
    sb += ",display1 TEXT UNIQUE ON CONFLICT REPLACE";
    if (0 != (mNewVersion & DATABASE_MODE_2LINES)) {
        sb += ",display2 TEXT";
    }
    sb += ",query TEXT";
    sb += ",date LONG";
    sb += ");";
    FAIL_RETURN(db->ExecSQL(sb.ToString()))
    return NOERROR;
}

ECode CSearchRecentSuggestionsProvider::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    Logger::W("CSearchRecentSuggestionsProvider::DatabaseHelper", "Upgrading database from version %s to %s, which will destroy all old data",
            StringUtils::ToString(oldVersion).string(), StringUtils::ToString(newVersion).string());
    FAIL_RETURN(db->ExecSQL(String("DROP TABLE IF EXISTS suggestions")))
    FAIL_RETURN(OnCreate(db))
    return NOERROR;
}

//==============================================================
// CSearchRecentSuggestionsProvider
//==============================================================

CAR_INTERFACE_IMPL(CSearchRecentSuggestionsProvider, ContentProvider, ISearchRecentSuggestionsProvider)

CAR_OBJECT_IMPL(CSearchRecentSuggestionsProvider)

CSearchRecentSuggestionsProvider::CSearchRecentSuggestionsProvider()
    : mMode(0)
    , mTwoLineDisplay(FALSE)
{}

CSearchRecentSuggestionsProvider::~CSearchRecentSuggestionsProvider()
{}

ECode CSearchRecentSuggestionsProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected)
    *rowsAffected = 0;
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
    AutoPtr<IList> segmentsArray;
    FAIL_RETURN(uri->GetPathSegments((IList**)&segmentsArray))
    Int32 length;
    segmentsArray->GetSize(&length);

    if (length != 1) {
        //throw new IllegalArgumentException("Unknown Uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterface> obj;
    segmentsArray->Get(0, (IInterface**)&obj);
    String base = Object::ToString(obj);
    if (base.Equals(sSuggestions)) {
        FAIL_RETURN(db->Delete(sSuggestions, selection, selectionArgs, rowsAffected))
    }
    else {
        // throw new IllegalArgumentException("Unknown Uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContext> context;
    FAIL_RETURN(ContentProvider::GetContext((IContext**)&context))
    AutoPtr<IContentResolver> contentResolver;
    FAIL_RETURN(context->GetContentResolver((IContentResolver**)&contentResolver))
    FAIL_RETURN(contentResolver->NotifyChange(uri, NULL))
    return NOERROR;
}

ECode CSearchRecentSuggestionsProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    Int32 matchCode = 0;
    FAIL_RETURN(mUriMatcher->Match(uri, &matchCode))

    if (URI_MATCH_SUGGEST == matchCode) {
        *type = ISearchManager::SUGGEST_MIME_TYPE;
        return NOERROR;
    }

    AutoPtr<IList> segmentsArray;
    FAIL_RETURN(uri->GetPathSegments((IList**)&segmentsArray))
    Int32 length;
    segmentsArray->GetSize(&length);
    if (length >= 1) {
        AutoPtr<IInterface> obj;
        segmentsArray->Get(0, (IInterface**)&obj);
        String base = Object::ToString(obj);
        if (base.Equals(sSuggestions)) {
            if (1 == length) {
                *type = "vnd.android.cursor.dir/suggestion";
                return NOERROR;
            }
            else if (2 == length) {
                *type = "vnd.android.cursor.item/suggestion";
                return NOERROR;
            }
        }
    }
    // throw new IllegalArgumentException("Unknown Uri");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSearchRecentSuggestionsProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** insertedUri)
{
    VALIDATE_NOT_NULL(insertedUri)
    *insertedUri = NULL;

    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
    AutoPtr<IList> segmentsArray;
    FAIL_RETURN(uri->GetPathSegments((IList**)&segmentsArray))
    Int32 length;
    segmentsArray->GetSize(&length);

    if (length < 1) {
        // throw new IllegalArgumentException("Unknown Uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Note:  This table has on-conflict-replace semantics, so insert() may actually replace()
    Int64 rowID = -1;
    AutoPtr<IInterface> obj;
    segmentsArray->Get(0, (IInterface**)&obj);
    String base = Object::ToString(obj);

    if (base.Equals(sSuggestions)) {
        if (length == 1) {
            FAIL_RETURN(db->Insert(sSuggestions, NULL_COLUMN, values, &rowID))
            if (rowID > 0) {
//***                AutoPtr<IUriBuilder> builder;
//***                FAIL_RETURN(mSuggestionsUri->BuildUpon((IUriBuilder**)&builder))
//***                FAIL_RETURN(builder->AppendEncodedPath(StringUtils::Int64ToString(rowID)))
//***                FAIL_RETURN(builder->Build(insertedUri))
            }
        }
    }
    if (rowID < 0) {
        // throw new IllegalArgumentException("Unknown Uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContext> context;
    // FAIL_RETURN(ContentProvider::GetContext((IContext**)&context))
    AutoPtr<IContentResolver> contentResolver;
    FAIL_RETURN(context->GetContentResolver((IContentResolver**)&contentResolver))
    FAIL_RETURN(contentResolver->NotifyChange(*insertedUri, NULL))
    return NOERROR;
}


ECode CSearchRecentSuggestionsProvider::OnCreate(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;
    if (mAuthority.IsNull() || mMode == 0) {
        // throw new IllegalArgumentException("Provider not configured");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

//    Int32 mWorkingDbVersion = DATABASE_VERSION + mMode;
    AutoPtr<IContext> context;
    // FAIL_RETURN(ContentProvider::GetContext((IContext**)&context))
//    mOpenHelper = new DatabaseHelper(context, mWorkingDbVersion);
    *succeeded = TRUE;
    return NOERROR;
}

ECode CSearchRecentSuggestionsProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db))
    AutoPtr<IContext> context;
    // FAIL_RETURN(ContentProvider::GetContext((IContext**)&context))
    AutoPtr<IContentResolver> contentResolver;
    FAIL_RETURN(context->GetContentResolver((IContentResolver**)&contentResolver))

    // special case for actual suggestions (from search manager)
    Int32 matchCode = 0;
    FAIL_RETURN(mUriMatcher->Match(uri, &matchCode))
    if (URI_MATCH_SUGGEST == matchCode) {
        String suggestSelection;
        AutoPtr<ArrayOf<String> > myArgs;
        if ((*selectionArgs)[0].IsNullOrEmpty()) {
            suggestSelection = String(NULL);
            myArgs = NULL;
        }
        else {
            String like = String("%") + (*selectionArgs)[0] + String("%");
            if (mTwoLineDisplay) {
                myArgs = ArrayOf<String>::Alloc(2);
                (*myArgs)[0] = like;
                (*myArgs)[1] = like;
            }
            else {
                myArgs = ArrayOf<String>::Alloc(1);
                (*myArgs)[0] = like;
            }
            suggestSelection = mSuggestSuggestionClause;
        }
        // Suggestions are always performed with the default sort order
        FAIL_RETURN(db->Query(sSuggestions, mSuggestionProjection, suggestSelection, myArgs,
            String(NULL), String(NULL), ORDER_BY, String(NULL), cursor))
        FAIL_RETURN((*cursor)->SetNotificationUri(contentResolver, uri))
        return NOERROR;
    }

    // otherwise process arguments and perform a standard query
    AutoPtr<IList> segmentsArray;
    FAIL_RETURN(uri->GetPathSegments((IList**)&segmentsArray))
    Int32 length;
    segmentsArray->GetSize(&length);
    if (length != 1 && length != 2) {
        // throw new IllegalArgumentException("Unknown Uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterface> obj;
    segmentsArray->Get(0, (IInterface**)&obj);
    String base = Object::ToString(obj);
    if (!base.Equals(sSuggestions)) {
        // throw new IllegalArgumentException("Unknown Uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<String> > useProjection;
    if (NULL != projection && projection->GetLength() > 0) {
        useProjection = ArrayOf<String>::Alloc(projection->GetLength() + 1);
        useProjection->Copy(projection);
        (*useProjection)[projection->GetLength()] = String("_id AS _id");
    }

    StringBuilder whereClause(256);
    if (2 == length) {
        obj = NULL;
        segmentsArray->Get(1, (IInterface**)&obj);

        whereClause += "(_id = ";
        whereClause += Object::ToString(obj);
        whereClause += ")";
    }

    // Tack on the user's selection, if present
    if (!selection.IsNullOrEmpty()) {
        if (whereClause.GetLength() > 0) {
            whereClause += " AND ";
        }
        whereClause += '(';
        whereClause += selection;
        whereClause += ')';
    }

    // And perform the generic query as requested
    String nullStr;
    FAIL_RETURN(db->Query(base, useProjection, whereClause.ToString(),
        selectionArgs, nullStr, nullStr, sortOrder, nullStr, cursor))
    FAIL_RETURN((*cursor)->SetNotificationUri(contentResolver, uri))
    return NOERROR;
}

ECode CSearchRecentSuggestionsProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    // throw new UnsupportedOperationException("Not implemented");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CSearchRecentSuggestionsProvider::SetupSuggestions(
    /* [in] */ const String& authority,
    /* [in] */ Int32 mode)
{
    if (authority.IsNullOrEmpty() ||
            ((mode & DATABASE_MODE_QUERIES) == 0)) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // unpack mode flags
    mTwoLineDisplay = (0 != (mode & DATABASE_MODE_2LINES));

    // saved values
    mAuthority = authority;
    mMode = mode;

    // derived values
    assert(0 && "TODO");
    // FAIL_RETURN(CStringUri::New(String("content://") + mAuthority + String("/suggestions"), (IUri**)&mSuggestionsUri))
    FAIL_RETURN(CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&mUriMatcher))
    FAIL_RETURN(mUriMatcher->AddURI(mAuthority, ISearchManager::SUGGEST_URI_PATH_QUERY, URI_MATCH_SUGGEST))

    if (mTwoLineDisplay) {
        mSuggestSuggestionClause = "display1 LIKE ? OR display2 LIKE ?";

        mSuggestionProjection = ArrayOf<String>::Alloc(13);
        (*mSuggestionProjection)[0] = String("0 AS ");
        (*mSuggestionProjection)[1] = ISearchManager::SUGGEST_COLUMN_FORMAT;
        (*mSuggestionProjection)[2] = String("'android.resource://system/");
        (*mSuggestionProjection)[3] = StringUtils::ToString(R::drawable::ic_menu_recent_history);
        (*mSuggestionProjection)[4] = String("' AS ");
        (*mSuggestionProjection)[5] = ISearchManager::SUGGEST_COLUMN_ICON_1;
        (*mSuggestionProjection)[6] = String("display1 AS ");
        (*mSuggestionProjection)[7] = ISearchManager::SUGGEST_COLUMN_TEXT_1;
        (*mSuggestionProjection)[8] = String("display2 AS ");
        (*mSuggestionProjection)[9] = ISearchManager::SUGGEST_COLUMN_TEXT_2;
        (*mSuggestionProjection)[10] = String("query AS ");
        (*mSuggestionProjection)[11] = ISearchManager::SUGGEST_COLUMN_QUERY;
        (*mSuggestionProjection)[12] = String("_id");

    }
    else {
        mSuggestSuggestionClause = "display1 LIKE ?";

        mSuggestionProjection = ArrayOf<String>::Alloc(11);
        (*mSuggestionProjection)[0] = String("0 AS ");
        (*mSuggestionProjection)[1] = ISearchManager::SUGGEST_COLUMN_FORMAT;
        (*mSuggestionProjection)[2] = String("'android.resource://system/");
        (*mSuggestionProjection)[3] = StringUtils::ToString(R::drawable::ic_menu_recent_history);
        (*mSuggestionProjection)[4] = String("' AS ");
        (*mSuggestionProjection)[5] = ISearchManager::SUGGEST_COLUMN_ICON_1;
        (*mSuggestionProjection)[6] = String("display1 AS ");
        (*mSuggestionProjection)[7] = ISearchManager::SUGGEST_COLUMN_TEXT_1;
        (*mSuggestionProjection)[8] = String("query AS ");
        (*mSuggestionProjection)[9] = ISearchManager::SUGGEST_COLUMN_QUERY;
        (*mSuggestionProjection)[10] = String("_id");
    }
    return NOERROR;
}

}
}
}

