// TODO: Need CBackupManager
// #include "elastos/droid/app/backup/CBackupManager.h"
// #include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/providers/telephony/CBlacklistProvider.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

// TODO: Need CBackupManager
// using Elastos::Droid::App::Backup::CBackupManager;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::CTelephonyBlacklist;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ITelephonyBlacklist;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Core::Character;
using Elastos::Core::CoreUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::IO::ICloseable;
using Elastos::Utility::CLocale;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

static AutoPtr<IUriMatcher> InitUriMatcher()
{
    AutoPtr<IUriMatcher> matcher;
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&matcher);
    return matcher;
}

const Boolean CBlacklistProvider::DEBUG = TRUE;

const String CBlacklistProvider::TAG("BlacklistProvider");
const String CBlacklistProvider::DATABASE_NAME("blacklist.db");
const String CBlacklistProvider::BLACKLIST_TABLE("blacklist");
const String CBlacklistProvider::COLUMN_NORMALIZED("normalized_number");

const Int32 CBlacklistProvider::DATABASE_VERSION;
const Int32 CBlacklistProvider::BL_ALL;
const Int32 CBlacklistProvider::BL_ID;
const Int32 CBlacklistProvider::BL_NUMBER;
const Int32 CBlacklistProvider::BL_PHONE;
const Int32 CBlacklistProvider::BL_MESSAGE;

AutoPtr<IUriMatcher> CBlacklistProvider::sURIMatcher = InitUriMatcher();

Boolean CBlacklistProvider::InitMember()
{
    assert(sURIMatcher != NULL);
    sURIMatcher->AddURI(String("blacklist"), String(NULL), BL_ALL);
    sURIMatcher->AddURI(String("blacklist"), String("#"), BL_ID);
    sURIMatcher->AddURI(String("blacklist"), String("bynumber/*"), BL_NUMBER);
    sURIMatcher->AddURI(String("blacklist"), String("phone"), BL_PHONE);
    sURIMatcher->AddURI(String("blacklist"), String("message"), BL_MESSAGE);

    return TRUE;
}

Boolean CBlacklistProvider::s_Init = CBlacklistProvider::InitMember();

CBlacklistProvider::DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ CBlacklistProvider* host)
    : mContext(context)
    , mHost(host)
{
    SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, DATABASE_VERSION);
}

ECode CBlacklistProvider::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    // Set up the database schema
    return db->ExecSQL(String("CREATE TABLE ") + BLACKLIST_TABLE +
            "(_id INTEGER PRIMARY KEY," +
            "number TEXT," +
            "normalized_number TEXT," +
            "is_regex INTEGER," +
            "phone INTEGER DEFAULT 0," +
            "message INTEGER DEFAULT 0);");
}

ECode CBlacklistProvider::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    if (oldVersion < 2) {
        // drop the uniqueness constraint that was present on the DB in V1
        db->ExecSQL(String("ALTER TABLE ") + BLACKLIST_TABLE +
                " RENAME TO " + BLACKLIST_TABLE + "_old;");
        OnCreate(db);
        db->ExecSQL(String("INSERT INTO ") + BLACKLIST_TABLE +
                " SELECT * FROM " + BLACKLIST_TABLE + "_old;");
    }

    if (oldVersion < 4) {
        // update the normalized number column, v1 and v2 didn't handle
        // alphanumeric 'numbers' correctly
        // v3 doesn't handle E164 format, which is used in v4 to normalize numbers

        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
        (*array)[0] = IBaseColumns::ID;
        (*array)[1] = ITelephonyBlacklist::NUMBER;

        AutoPtr<ICursor> rows;
        db->Query(BLACKLIST_TABLE,
                array, String(NULL), NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&rows);

        // try {
        db->BeginTransaction();
        if (rows != NULL) {
            AutoPtr<IContentValues> cv;
            CContentValues::New((IContentValues**)&cv);
            AutoPtr<ArrayOf<String> > rowId = ArrayOf<String>::Alloc(1);

            Boolean b;
            while (rows->MoveToNext(&b), b) {
                String originalNumber;
                rows->GetString(1, &originalNumber);
                AutoPtr<IPair> pair = NormalizeNumber(mContext, originalNumber);
                AutoPtr<IInterface> obj;
                pair->GetFirst((IInterface**)&obj);
                String normalized;
                ICharSequence::Probe(obj)->ToString(&normalized);
                rows->GetString(0, &(*rowId)[0]);
                cv->Clear();
                cv->Put(COLUMN_NORMALIZED, normalized);
                Int32 val;
                db->Update(BLACKLIST_TABLE, cv, IBaseColumns::ID + "= ?", rowId, &val);
            }
        }
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (rows != NULL) {
            ICloseable::Probe(rows)->Close();
        }
        // }
    }

    return NOERROR;
}

CAR_OBJECT_IMPL(CBlacklistProvider)

ECode CBlacklistProvider::constructor()
{
    return ContentProvider::constructor();
}

ECode CBlacklistProvider::OnCreate(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mOpenHelper = new DatabaseHelper(ctx, this);
// TODO: Need CBackupManager
    // CBackupManager::New(ctx, (IBackupManager**)&mBackupManager);
    *succeeded = TRUE;
    return NOERROR;
}

// @Override
ECode CBlacklistProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& _sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String sortOrder = _sortOrder;
    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);

    qb->SetTables(BLACKLIST_TABLE);

    // Generate the body of the query.
    Int32 match;
    sURIMatcher->Match(uri, &match);
    if (DEBUG) {
        String str;
        ((Uri*)uri)->ToString(&str);
        Logger::V(TAG, "Query uri=%s, match=%d", str.string(), match);
    }

    String str;
    uri->GetLastPathSegment(&str);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    switch (match) {
        case BL_ALL:
            break;
        case BL_ID:
            qb->AppendWhere(CoreUtils::Convert(IBaseColumns::ID + " = " + str));
            break;
        case BL_NUMBER: {
            AutoPtr<IPair> pair = NormalizeNumber(ctx, str);
            AutoPtr<IInterface> obj;
            pair->GetFirst((IInterface**)&obj);
            String number;
            ICharSequence::Probe(obj)->ToString(&number);

            Boolean regex;
            uri->GetBooleanQueryParameter(ITelephonyBlacklist::REGEX_KEY, FALSE, &regex);

            if (regex) {
                qb->AppendWhere(CoreUtils::Convert(
                        String("\"") + number + "\" like " + COLUMN_NORMALIZED));
            }
            else {
                qb->AppendWhere(CoreUtils::Convert(
                        COLUMN_NORMALIZED + " = \"" + number + "\""));
            }
            break;
        }
        case BL_PHONE:
            qb->AppendWhere(CoreUtils::Convert(
                    ITelephonyBlacklist::PHONE_MODE + " != 0"));
            break;
        case BL_MESSAGE:
            qb->AppendWhere(CoreUtils::Convert(
                    ITelephonyBlacklist::MESSAGE_MODE + " != 0"));
            break;
        default: {
            String uriStr;
            ((Uri*)uri)->ToString(&uriStr);
            Logger::E(TAG, "query: invalid request: %s", uriStr.string());
            *result = NULL;
            return NOERROR;
        }
    }

    if (TextUtils::IsEmpty(sortOrder)) {
        sortOrder = ITelephonyBlacklist::DEFAULT_SORT_ORDER;
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> ret;
    qb->Query(db, projection, selection, selectionArgs,
            String(NULL), String(NULL), sortOrder, (ICursor**)&ret);

    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    ret->SetNotificationUri(cr, uri);

    *result = ret;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override
ECode CBlacklistProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    Int32 match = 0;
    sURIMatcher->Match(uri, &match);

    switch (match) {
        case BL_ALL:
        case BL_PHONE:
        case BL_MESSAGE:
            *result = String("vnd.android.cursor.dir/blacklist-entry");
            return NOERROR;
        case BL_ID:
        case BL_NUMBER:
            *result = String("vnd.android.cursor.item/blacklist-entry");
            return NOERROR;
        default:
            // throw new IllegalArgumentException("Unknown URI " + uri);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

// @Override
ECode CBlacklistProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 match;
    sURIMatcher->Match(uri, &match);
    if (DEBUG) {
        String str;
        ((Uri*)uri)->ToString(&str);
        Logger::V(TAG, "Insert uri=%s, match=%d", str.string(), match);
    }

    if (match != BL_ALL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IContentValues> values = ValidateAndPrepareContentValues(initialValues, String(NULL));
    if (values == NULL) {
        // Logger::E(TAG, "Invalid insert values " + initialValues);
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int64 rowID;
    db->Insert(BLACKLIST_TABLE, String(NULL), values, &rowID);
    if (rowID <= 0) {
        *result = NULL;
        return NOERROR;
    }

    // if (DEBUG) Logger::D(TAG, "inserted " + values + " rowID = " + rowID);
    NotifyChange();

    AutoPtr<IContentUris> cUris;
    CContentUris::AcquireSingleton((IContentUris**)&cUris);
    AutoPtr<ITelephonyBlacklist> bl;
    CTelephonyBlacklist::AcquireSingleton((ITelephonyBlacklist**)&bl);
    AutoPtr<IUri> contentUri;
    bl->GetCONTENT_URI((IUri**)&contentUri);
    return cUris->WithAppendedId(contentUri, rowID, result);
}

// @Override
ECode CBlacklistProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& _where,
    /* [in] */ ArrayOf<String>* _whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    String where = _where;
    AutoPtr<ArrayOf<String> > whereArgs = _whereArgs;
    Int32 count = 0;
    Int32 match;
    sURIMatcher->Match(uri, &match);
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    if (DEBUG) {
        String str;
        ((Uri*)uri)->ToString(&str);
        Logger::V(TAG, "Delete uri=%s, match=%d", str.string(), match);
    }

    switch (match) {
        case BL_ALL:
            break;
        case BL_ID: {
            if (where != NULL || whereArgs != NULL) {
                // throw new UnsupportedOperationException(
                //         "Cannot delete URI " + uri + " with a where clause");
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            }
            where = IBaseColumns::ID + " = ?";
            whereArgs = ArrayOf<String>::Alloc(1);
            String str;
            uri->GetLastPathSegment(&str);
            (*whereArgs)[0] = str;
            break;
        }
        case BL_NUMBER: {
            if (where != NULL || whereArgs != NULL) {
                // throw new UnsupportedOperationException(
                //         "Cannot delete URI " + uri + " with a where clause");
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            }
            where = COLUMN_NORMALIZED + " = ?";
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            whereArgs = ArrayOf<String>::Alloc(1);
            String seg;
            uri->GetLastPathSegment(&seg);

            AutoPtr<IPair> pair = NormalizeNumber(ctx, seg);
            AutoPtr<IInterface> obj;
            pair->GetFirst((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            (*whereArgs)[0] = str;
            break;
        }
        default:
            // throw new UnsupportedOperationException("Cannot delete that URI: " + uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    db->Delete(BLACKLIST_TABLE, where, whereArgs, &count);
    if (DEBUG) Logger::D(TAG, "delete result count %d", count);

    if (count > 0) {
        NotifyChange();
    }

    *result = count;
    return NOERROR;
}

// @Override
ECode CBlacklistProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count = 0;
    Int32 match;
    sURIMatcher->Match(uri, &match);
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    if (DEBUG) {
        String str;
        ((Uri*)uri)->ToString(&str);
        Logger::V(TAG, "Update uri=%s, match=%d", str.string(), match);
    }

    String uriNumber = String(NULL);
    if (match == BL_NUMBER) {
        uri->GetLastPathSegment(&uriNumber);
    }

    AutoPtr<IContentValues> values = ValidateAndPrepareContentValues(initialValues, uriNumber);

    if (values == NULL) {
        // Logger::E(TAG, "Invalid update values " + initialValues);
        *result = 0;
        return NOERROR;
    }

    switch (match) {
        case BL_ALL:
            db->Update(BLACKLIST_TABLE, values, where, whereArgs, &count);
            break;
        case BL_NUMBER: {
            if (where != NULL || whereArgs != NULL) {
                // throw new UnsupportedOperationException(
                //         "Cannot update URI " + uri + " with a where clause");
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            }
            db->BeginTransaction();
            // try {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            AutoPtr<IPair> pair = NormalizeNumber(ctx, uriNumber);
            AutoPtr<IInterface> obj;
            pair->GetFirst((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            (*array)[0] = str;

            db->Update(BLACKLIST_TABLE, values, COLUMN_NORMALIZED + " = ?",
                    array, &count);
            if (count == 0) {
                // convenience: fall back to insert if number wasn't present
                Int64 val;
                db->Insert(BLACKLIST_TABLE, String(NULL), values, &val);
                if (val > 0) {
                    count = 1;
                }
            }
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            // }
            break;
        }
        case BL_ID: {
            if (where != NULL || whereArgs != NULL) {
                // throw new UnsupportedOperationException(
                //         "Cannot update URI " + uri + " with a where clause");
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            }
            String str;
            uri->GetLastPathSegment(&str);
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            (*array)[0] = str;
            db->Update(BLACKLIST_TABLE, values, IBaseColumns::ID + " = ?",
                    array, &count);
            break;
        }
        default:
            // throw new UnsupportedOperationException("Cannot update that URI: " + uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    if (DEBUG) Logger::D(TAG, "Update result count %d", count);

    if (count > 0) {
        NotifyChange();
    }

    *result = count;
    return NOERROR;
}

AutoPtr<IContentValues> CBlacklistProvider::ValidateAndPrepareContentValues(
    /* [in] */ IContentValues* initialValues,
    /* [in] */ const String& uriNumber)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(initialValues, (IContentValues**)&values);

    // apps are not supposed to mess with the normalized number or the regex state
    values->Remove(COLUMN_NORMALIZED);
    values->Remove(ITelephonyBlacklist::IS_REGEX);

    // on 'upsert', insert the number passed via URI if no other number was specified
    Boolean b;
    if (uriNumber != NULL && (values->ContainsKey(ITelephonyBlacklist::NUMBER, &b), !b)) {
        values->Put(ITelephonyBlacklist::NUMBER, uriNumber);
    }

    if (values->ContainsKey(ITelephonyBlacklist::NUMBER, &b), b) {
        String number;
        values->GetAsString(ITelephonyBlacklist::NUMBER, &number);
        if (TextUtils::IsEmpty(number)) {
            return NULL;
        }

        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IPair> normalizeResult = NormalizeNumber(ctx, number);
        AutoPtr<IInterface> obj;
        normalizeResult->GetFirst((IInterface**)&obj);
        String normalizedNumber;
        ICharSequence::Probe(obj)->ToString(&normalizedNumber);
        obj = NULL;
        normalizeResult->GetSecond((IInterface**)&obj);
        Boolean b;
        IBoolean::Probe(obj)->GetValue(&b);

        Boolean isRegex = normalizedNumber.IndexOf('%') >= 0 ||
                normalizedNumber.IndexOf('_') >= 0;
        // For non-regex numbers, apply additional validity checking if
        // they didn't pass e164 normalization
        if (!isRegex && !b && !IsValidPhoneNumber(number)) {
            // number was invalid
            return NULL;
        }

        values->Put(COLUMN_NORMALIZED, normalizedNumber);
        values->Put(ITelephonyBlacklist::IS_REGEX, isRegex ? 1 : 0);
    }

    return values;
}

void CBlacklistProvider::NotifyChange()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ITelephonyBlacklist> bl;
    CTelephonyBlacklist::AcquireSingleton((ITelephonyBlacklist**)&bl);
    AutoPtr<IUri> contentUri;
    bl->GetCONTENT_URI((IUri**)&contentUri);
    cr->NotifyChange(contentUri, NULL);
    mBackupManager->DataChanged();
}

AutoPtr<IPair> CBlacklistProvider::NormalizeNumber(
    /* [in] */ IContext* context,
    /* [in] */ const String& number)
{
    Int32 len = number.GetLength();
    StringBuilder ret(len);

    for (Int32 i = 0; i < len; i++) {
        Char32 c = number.GetChar(i);
        // Character.digit() supports ASCII and Unicode digits (fullwidth, Arabic-Indic, etc.)
        Int32 digit;
        digit = Character::ToDigit(c, 10);
        if (digit != -1) {
            ret.Append(digit);
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            String actualNumber;
            AutoPtr<IPhoneNumberUtils> pu;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pu);
            pu->ConvertKeypadLettersToDigits(number, &actualNumber);
            return NormalizeNumber(context, actualNumber);
        }
        else if (i == 0 && c == '+') {
            ret.AppendChar(c);
        }
        else if (c == '*') {
            // replace regex match-multiple character by SQL equivalent
            ret.Append('%');
        }
        else if (c == '.') {
            // replace regex-match-single character by SQL equivalent
            ret.Append('_');
        }
    }

    String normalizedNumber;
    ret.ToString(&normalizedNumber);
    String e164Number = ToE164Number(context, normalizedNumber);
    AutoPtr<IPairHelper> helper;
    CPairHelper::AcquireSingleton((IPairHelper**)&helper);
    AutoPtr<IPair> pair;
    helper->Create(e164Number != NULL ?
            CoreUtils::Convert(e164Number) : CoreUtils::Convert(normalizedNumber),
            CoreUtils::Convert(e164Number != NULL), (IPair**)&pair);
    return pair;
}

String CBlacklistProvider::ToE164Number(
    /* [in] */ IContext* context,
    /* [in] */ const String& src)
{
    // Try to retrieve the current ISO Country code
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj);
    String countryCode;
    tm->GetSimCountryIso(&countryCode);
    AutoPtr<ILocale> numberLocale;
    if (TextUtils::IsEmpty(countryCode)) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> conf;
        res->GetConfiguration((IConfiguration**)&conf);
        conf->GetLocale((ILocale**)&numberLocale);
    }
    else {
        CLocale::New(String(""), countryCode, (ILocale**)&numberLocale);
    }

    String countryStr;
    numberLocale->GetCountry(&countryStr);
    AutoPtr<IPhoneNumberUtils> pu;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pu);
    String str;
    pu->FormatNumberToE164(src, countryStr, &str);
    return str;
}

Boolean CBlacklistProvider::IsValidPhoneNumber(
    /* [in] */ const String& address)
{
    for (Int32 i = 0, count = address.GetLength(); i < count; i++) {
        AutoPtr<IPhoneNumberUtils> pu;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pu);
        Boolean b;
        if (pu->IsISODigit(address.GetChar(i), &b), !b) {
            return FALSE;
        }
    }
    return TRUE;
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
