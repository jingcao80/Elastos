
#include "elastos/droid/providers/userdictionary/CUserDictionaryProvider.h"
#include "R.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Net.h"
#include <elastos/droid/R.h>
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

// using Elastos::Droid::App::Backup::CBackupManager;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Provider::CUserDictionary;
using Elastos::Droid::Provider::IUserDictionary;
using Elastos::Droid::Provider::CUserDictionaryWords;
using Elastos::Droid::Provider::IUserDictionaryWords;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace UserDictionary {

const String CUserDictionaryProvider::AUTHORITY = IUserDictionary::AUTHORITY;

const String CUserDictionaryProvider::TAG("CUserDictionaryProvider");

static const AutoPtr<IUri> InitCONTENT_URI()
{
    AutoPtr<IUserDictionary> ud;
    CUserDictionary::AcquireSingleton((IUserDictionary**)&ud);
    AutoPtr<IUri> uri;
    ud->GetCONTENT_URI((IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> CUserDictionaryProvider::CONTENT_URI = InitCONTENT_URI();

const String CUserDictionaryProvider::DATABASE_NAME("user_dict.db");
const Int32 CUserDictionaryProvider::DATABASE_VERSION = 2;

const String CUserDictionaryProvider::USERDICT_TABLE_NAME("words");

AutoPtr<IHashMap> CUserDictionaryProvider::sDictProjectionMap;

AutoPtr<IUriMatcher> CUserDictionaryProvider::sUriMatcher;

const Int32 CUserDictionaryProvider::WORDS = 1;

const Int32 CUserDictionaryProvider::WORD_ID = 2;

Boolean CUserDictionaryProvider::s_Init = Initstatic();

Boolean CUserDictionaryProvider::Initstatic()
{
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&sUriMatcher);
    sUriMatcher->AddURI(AUTHORITY, String("words"), WORDS);
    sUriMatcher->AddURI(AUTHORITY, String("words/#"), WORD_ID);

    CHashMap::New((IHashMap**)&sDictProjectionMap);
    sDictProjectionMap->Put(CoreUtils::Convert(IUserDictionaryWords::_ID),
            CoreUtils::Convert(IUserDictionaryWords::_ID));
    sDictProjectionMap->Put(CoreUtils::Convert(IUserDictionaryWords::WORD),
            CoreUtils::Convert(IUserDictionaryWords::WORD));
    sDictProjectionMap->Put(CoreUtils::Convert(IUserDictionaryWords::FREQUENCY),
            CoreUtils::Convert(IUserDictionaryWords::FREQUENCY));
    sDictProjectionMap->Put(CoreUtils::Convert(IUserDictionaryWords::LOCALE),
            CoreUtils::Convert(IUserDictionaryWords::LOCALE));
    sDictProjectionMap->Put(CoreUtils::Convert(IUserDictionaryWords::APP_ID),
            CoreUtils::Convert(IUserDictionaryWords::APP_ID));
    sDictProjectionMap->Put(CoreUtils::Convert(IUserDictionaryWords::SHORTCUT),
            CoreUtils::Convert(IUserDictionaryWords::SHORTCUT));

    return TRUE;
}

//===============================================================================
//                 CUserDictionaryProvider::DatabaseHelper
//===============================================================================

CUserDictionaryProvider::DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context)
{
    SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, DATABASE_VERSION);
}

ECode CUserDictionaryProvider::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    StringBuilder builder("CREATE TABLE ");
    builder += USERDICT_TABLE_NAME;
    builder += " (";
    builder += IUserDictionaryWords::_ID;
    builder += " INTEGER PRIMARY KEY,";
    builder += IUserDictionaryWords::WORD;
    builder += " TEXT,";
    builder += IUserDictionaryWords::FREQUENCY;
    builder += " INTEGER,";
    builder += IUserDictionaryWords::LOCALE;
    builder += " TEXT,";
    builder += IUserDictionaryWords::APP_ID;
    builder += " INTEGER,";
    builder += IUserDictionaryWords::SHORTCUT;
    builder += " TEXT";
    builder += ");";

    return db->ExecSQL(builder.ToString());
}

ECode CUserDictionaryProvider::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    if (oldVersion == 1 && newVersion == 2) {
        Logger::I(TAG, "Upgrading database from version %d to version 2: adding %s column",
                oldVersion, IUserDictionaryWords::SHORTCUT.string());
        StringBuilder builder("ALTER TABLE ");
        builder += USERDICT_TABLE_NAME;
        builder += " ADD ";
        builder += IUserDictionaryWords::SHORTCUT;
        builder += " TEXT;";
        db->ExecSQL(builder.ToString());
    }
    else {
        Logger::W(TAG, "Upgrading database from version %d to %d, which will destroy all old data",
                oldVersion, newVersion);
        db->ExecSQL(String("DROP TABLE IF EXISTS ") + USERDICT_TABLE_NAME);
        OnCreate(db);
    }

    return NOERROR;
}

//===============================================================================
//                 CUserDictionaryProvider
//===============================================================================

CAR_OBJECT_IMPL(CUserDictionaryProvider)

CUserDictionaryProvider::CUserDictionaryProvider()
{}

CUserDictionaryProvider::~CUserDictionaryProvider()
{}

ECode CUserDictionaryProvider::constructor()
{
    return ContentProvider::constructor();
}

ECode CUserDictionaryProvider::OnCreate(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mOpenHelper = new DatabaseHelper(ctx);
    // CBackupManager::New(ctx, (IBackupManager**)&mBackupManager);
    *succeeded = TRUE;
    return NOERROR;
}

ECode CUserDictionaryProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);

    Int32 match;
    sUriMatcher->Match(uri, &match);
    switch (match) {
        case WORDS:
            qb->SetTables(USERDICT_TABLE_NAME);
            qb->SetProjectionMap(IMap::Probe(sDictProjectionMap));
            break;

        case WORD_ID: {
            qb->SetTables(USERDICT_TABLE_NAME);
            qb->SetProjectionMap(IMap::Probe(sDictProjectionMap));
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            StringBuilder builder("_id");
            builder += "=";
            builder += obj;
            qb->AppendWhere(builder.ToCharSequence());
            break;
        }

        default:
            Logger::E("CUserDictionaryProvider::Query", "Unknown URI %s", TO_CSTR(uri));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // If no sort order is specified use the default
    String orderBy;
    if (TextUtils::IsEmpty(sortOrder)) {
        orderBy = IUserDictionaryWords::DEFAULT_SORT_ORDER;
    }
    else {
        orderBy = sortOrder;
    }

    // Get the database and run the query
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> c;
    qb->Query(db, projection, selection, selectionArgs, String(NULL), String(NULL), orderBy, (ICursor**)&c);

    // Tell the cursor what uri to watch, so it knows when its source data changes
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    c->SetNotificationUri(resolver, uri);

    *result = c;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CUserDictionaryProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 match = 0;
    sUriMatcher->Match(uri, &match);
    switch (match) {
        case WORDS:
            *result = IUserDictionaryWords::CONTENT_TYPE;
            return NOERROR;

        case WORD_ID:
            *result = IUserDictionaryWords::CONTENT_ITEM_TYPE;
            return NOERROR;

        default:
            Logger::E("CUserDictionaryProvider::GetType", "Unknown URI %s", TO_CSTR(uri));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CUserDictionaryProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // Validate the requested uri
    Int32 match = 0;
    sUriMatcher->Match(uri, &match);
    if (match != WORDS) {
        Logger::E("CUserDictionaryProvider::Insert", "Unknown URI %s", TO_CSTR(uri));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContentValues> values;
    if (initialValues != NULL) {
        CContentValues::New(initialValues, (IContentValues**)&values);
    }
    else {
        CContentValues::New((IContentValues**)&values);
    }

    Boolean res;
    values->ContainsKey(IUserDictionaryWords::WORD, &res);
    if (res == FALSE) {
        Logger::E("CUserDictionaryProvider::Insert", "Word must be specified");
        return E_SQL_EXCEPTION;
    }

    values->ContainsKey(IUserDictionaryWords::FREQUENCY, &res);
    if (res == FALSE) {
        values->Put(IUserDictionaryWords::FREQUENCY, String("1"));
    }

    values->ContainsKey(IUserDictionaryWords::LOCALE, &res);
    if (res == FALSE) {
        values->Put(IUserDictionaryWords::LOCALE, String(NULL));
    }

    values->ContainsKey(IUserDictionaryWords::SHORTCUT, &res);
    if (res == FALSE) {
        values->Put(IUserDictionaryWords::SHORTCUT, String(NULL));
    }

    values->Put(IUserDictionaryWords::APP_ID, 0);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    Int64 rowId;
    db->Insert(USERDICT_TABLE_NAME, IUserDictionaryWords::WORD, values, &rowId);
    if (rowId > 0) {
        AutoPtr<IUserDictionaryWords> udw;
        CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&udw);
        AutoPtr<IUri> wordsUri;
        udw->GetCONTENT_URI((IUri**)&wordsUri);
        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        AutoPtr<IUri> wordUri;
        cu->WithAppendedId(wordsUri, rowId, (IUri**)&wordUri);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        resolver->NotifyChange(wordUri, NULL);
        // mBackupManager->DataChanged();
        *result = wordUri;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Logger::E("CUserDictionaryProvider::Insert", "Failed to insert row into %s", TO_CSTR(uri));
    return E_SQL_EXCEPTION;
}

ECode CUserDictionaryProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 count = 0;

    Int32 match = 0;
    sUriMatcher->Match(uri, &match);
    switch (match) {
        case WORDS:
            db->Delete(USERDICT_TABLE_NAME, where, whereArgs, &count);
            break;

        case WORD_ID: {
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            String wordId;
            ICharSequence::Probe(obj)->ToString(&wordId);
            StringBuilder builder("");
            builder += IUserDictionaryWords::_ID;
            builder += "=";
            builder += wordId;
            if (!TextUtils::IsEmpty(where)) {
                builder += " AND (";
                builder += where;
                builder += ')';
            }
            else {
                builder += "";
            }
            db->Delete(USERDICT_TABLE_NAME, builder.ToString(), whereArgs, &count);
            break;
        }

        default:
            Logger::E("CUserDictionaryProvider::Delete", "Unknown URI %s", TO_CSTR(uri));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> resolver;
    ctx->GetContentResolver((IContentResolver**)&resolver);
    resolver->NotifyChange(uri, NULL);
    // mBackupManager->DataChanged();

    *result = count;
    return NOERROR;
}

ECode CUserDictionaryProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 count = 0;
    Int32 match = 0;
    sUriMatcher->Match(uri, &match);
    switch (match) {
        case WORDS:
            db->Update(USERDICT_TABLE_NAME, values, where, whereArgs, &count);
            break;

        case WORD_ID: {
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            String wordId;
            ICharSequence::Probe(obj)->ToString(&wordId);
            StringBuilder builder("");
            builder += IUserDictionaryWords::_ID;
            builder += "=";
            builder += wordId;
            if (!TextUtils::IsEmpty(where)) {
                builder += " AND (";
                builder += where;
                builder += ')';
            }
            else {
                builder += "";
            }
            db->Update(USERDICT_TABLE_NAME, values, builder.ToString(), whereArgs, &count);
            break;
        }

        default:
            Logger::E("CUserDictionaryProvider::Update", "Unknown URI %s", TO_CSTR(uri));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> resolver;
    ctx->GetContentResolver((IContentResolver**)&resolver);
    resolver->NotifyChange(uri, NULL);
    // mBackupManager->DataChanged();

    *result = count;
    return NOERROR;
}

} // namespace UserDictionary
} // namespace Providers
} // namespace Droid
} // namespace Elastos