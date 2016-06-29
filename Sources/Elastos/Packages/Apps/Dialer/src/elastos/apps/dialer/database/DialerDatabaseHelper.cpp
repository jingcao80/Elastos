
#include "elastos/apps/dialer/database/DialerDatabaseHelper.h"
#include "elastos/apps/dialer/dialpad/SmartDialPrefix.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "R.h"

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Database::Sqlite::ISQLiteProgram;
using Elastos::Droid::Database::Sqlite::ISQLiteStatement;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCommonColumns;
using Elastos::Droid::Provider::IContactsContractRawContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactNameColumns;
// TODO:
// using Elastos::Droid::Provider::IContactsContractDataUsageStatColumns;
using Elastos::Droid::Provider::IContactsContractDataColumns;
using Elastos::Droid::Provider::IContactsContractContactOptionsColumns;
using Elastos::Droid::Provider::IContactsContractDeletedContacts;
using Elastos::Droid::Provider::CContactsContractDeletedContacts;
using Elastos::Droid::Provider::IContactsContractDeletedContactsColumns;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::ICloseable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Apps::Dialer::Dialpad::ISmartDialMatchPosition;
using Elastos::Apps::Dialer::Dialpad::SmartDialPrefix;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Database {
//=================================================================
// DialerDatabaseHelper::Tables
//=================================================================
const String DialerDatabaseHelper::Tables::SMARTDIAL_TABLE("smartdial_table");
const String DialerDatabaseHelper::Tables::PREFIX_TABLE("prefix_table");
const String DialerDatabaseHelper::Tables::PROPERTIES("properties");

//=================================================================
// DialerDatabaseHelper::SmartDialDbColumns
//=================================================================
const String DialerDatabaseHelper::SmartDialDbColumns::_ID("id");
const String DialerDatabaseHelper::SmartDialDbColumns::DATA_ID("data_id");
const String DialerDatabaseHelper::SmartDialDbColumns::NUMBER("phone_number");
const String DialerDatabaseHelper::SmartDialDbColumns::CONTACT_ID("contact_id");
const String DialerDatabaseHelper::SmartDialDbColumns::LOOKUP_KEY("lookup_key");
const String DialerDatabaseHelper::SmartDialDbColumns::DISPLAY_NAME_PRIMARY("display_name");
const String DialerDatabaseHelper::SmartDialDbColumns::PHOTO_ID("photo_id");
const String DialerDatabaseHelper::SmartDialDbColumns::LAST_TIME_USED("last_time_used");
const String DialerDatabaseHelper::SmartDialDbColumns::TIMES_USED("times_used");
const String DialerDatabaseHelper::SmartDialDbColumns::STARRED("starred");
const String DialerDatabaseHelper::SmartDialDbColumns::IS_SUPER_PRIMARY("is_super_primary");
const String DialerDatabaseHelper::SmartDialDbColumns::IN_VISIBLE_GROUP("in_visible_group");
const String DialerDatabaseHelper::SmartDialDbColumns::IS_PRIMARY("is_primary");
const String DialerDatabaseHelper::SmartDialDbColumns::LAST_SMARTDIAL_UPDATE_TIME("last_smartdial_update_time");

//=================================================================
// DialerDatabaseHelper::PrefixColumns
//=================================================================
const String DialerDatabaseHelper::PrefixColumns::PREFIX("prefix");
const String DialerDatabaseHelper::PrefixColumns::CONTACT_ID("contact_id");


//=================================================================
// DialerDatabaseHelper::PropertiesColumns
//=================================================================
const String DialerDatabaseHelper::PropertiesColumns::PROPERTY_KEY("property_key");
const String DialerDatabaseHelper::PropertiesColumns::PROPERTY_VALUE("property_value");

//=================================================================
// DialerDatabaseHelper::PhoneQuery
//=================================================================
AutoPtr<IUri> CreatePhoneQueryUri()
{
    AutoPtr<IContactsContractCommonDataKindsPhone> phone;
    CContactsContractCommonDataKindsPhone::AcquireSingleton(
            (IContactsContractCommonDataKindsPhone**)&phone);
    AutoPtr<IUri> contentUri;
    phone->GetCONTENT_URI((IUri**)&contentUri);

    AutoPtr<IUriBuilder> builder;
    contentUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
        StringUtils::ToString(IContactsContractDirectory::DEFAULT));
    builder->AppendQueryParameter(IContactsContract::REMOVE_DUPLICATE_ENTRIES,
        String("true"));
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> DialerDatabaseHelper::PhoneQuery::URI = CreatePhoneQueryUri();
const String DialerDatabaseHelper::PhoneQuery::PROJECTION[] = {
    IBaseColumns::ID,                          // 0
    IContactsContractCommonDataKindsCommonColumns::TYPE,                         // 1
    IContactsContractCommonDataKindsCommonColumns::LABEL,                        // 2
    IContactsContractCommonDataKindsPhone::NUMBER,                       // 3
    IContactsContractRawContactsColumns::CONTACT_ID,                   // 4
    IContactsContractContactsColumns::LOOKUP_KEY,                   // 5
    IContactsContractContactNameColumns::DISPLAY_NAME_PRIMARY,         // 6
    IContactsContractContactsColumns::PHOTO_ID,                     // 7
    // TODO:
    // IContactsContractDataUsageStatColumns::LAST_TIME_USED,                // 8
    // IContactsContractDataUsageStatColumns::TIMES_USED,                    // 9
    IContactsContractContactOptionsColumns::STARRED,                   // 10
    IContactsContractDataColumns::IS_SUPER_PRIMARY,              // 11
    IContactsContractContactsColumns::IN_VISIBLE_GROUP,          // 12
    IContactsContractDataColumns::IS_PRIMARY,                    // 13
};

const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_ID = 0;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_TYPE = 1;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_LABEL = 2;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_NUMBER = 3;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_CONTACT_ID = 4;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_LOOKUP_KEY = 5;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_DISPLAY_NAME = 6;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_PHOTO_ID = 7;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_LAST_TIME_USED = 8;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_TIMES_USED = 9;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_STARRED = 10;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_IS_SUPER_PRIMARY = 11;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_IN_VISIBLE_GROUP = 12;
const Int32 DialerDatabaseHelper::PhoneQuery::PHONE_IS_PRIMARY = 13;

const String DialerDatabaseHelper::PhoneQuery::SELECT_UPDATED_CLAUSE =
        IContactsContractContactsColumns::CONTACT_LAST_UPDATED_TIMESTAMP + " > ?";

const String DialerDatabaseHelper::PhoneQuery::SELECT_IGNORE_LOOKUP_KEY_TOO_LONG_CLAUSE =
        String("length(") + IContactsContractContactsColumns::LOOKUP_KEY + ") < 1000";

const String DialerDatabaseHelper::PhoneQuery::SELECTION = SELECT_UPDATED_CLAUSE + " AND " +
                SELECT_IGNORE_LOOKUP_KEY_TOO_LONG_CLAUSE;

//=================================================================
// DialerDatabaseHelper::DeleteContactQuery
//=================================================================
AutoPtr<IUri> CreateDeleteContactQueryUri()
{
    AutoPtr<IContactsContractDeletedContacts> contacts;
    CContactsContractDeletedContacts::AcquireSingleton(
            (IContactsContractDeletedContacts**)&contacts);
    AutoPtr<IUri> uri;
    contacts->GetCONTENT_URI((IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> DialerDatabaseHelper::DeleteContactQuery::URI = CreateDeleteContactQueryUri();

const String DialerDatabaseHelper::DeleteContactQuery::PROJECTION[] = {
    IContactsContractDeletedContactsColumns::CONTACT_ID,                          // 0
    IContactsContractDeletedContactsColumns::CONTACT_DELETED_TIMESTAMP,           // 1
};

const Int32 DialerDatabaseHelper::DeleteContactQuery::DELETED_CONTACT_ID = 0;
const Int32 DialerDatabaseHelper::DeleteContactQuery::DELECTED_TIMESTAMP = 1;

//=================================================================
// DialerDatabaseHelper::SmartDialSortingOrder
//=================================================================
const Int64 DialerDatabaseHelper::SmartDialSortingOrder::LAST_TIME_USED_CURRENT_MS = 3LL * 24 * 60 * 60 * 1000;
const Int64 DialerDatabaseHelper::SmartDialSortingOrder::LAST_TIME_USED_RECENT_MS = 30LL * 24 * 60 * 60 * 1000;

const String DialerDatabaseHelper::SmartDialSortingOrder::TIME_SINCE_LAST_USED_MS = String("( ?1 - ") +
        DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "."
        + DialerDatabaseHelper::SmartDialDbColumns::LAST_TIME_USED + ")";

const String DialerDatabaseHelper::SmartDialSortingOrder::SORT_BY_DATA_USAGE =
        String("(CASE WHEN ") + TIME_SINCE_LAST_USED_MS + " < " + LAST_TIME_USED_CURRENT_MS +
        " THEN 0 " +
        " WHEN " + TIME_SINCE_LAST_USED_MS + " < " + LAST_TIME_USED_RECENT_MS +
        " THEN 1 " +
        " ELSE 2 END)";

const String DialerDatabaseHelper::SmartDialSortingOrder::SORT_ORDER =
        DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "." + DialerDatabaseHelper::SmartDialDbColumns::STARRED + " DESC, "
        + DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "." + DialerDatabaseHelper::SmartDialDbColumns::IS_SUPER_PRIMARY + " DESC, "
        + SORT_BY_DATA_USAGE + ", "
        + DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "." + DialerDatabaseHelper::SmartDialDbColumns::TIMES_USED + " DESC, "
        + DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "." + DialerDatabaseHelper::SmartDialDbColumns::IN_VISIBLE_GROUP + " DESC, "
        + DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "." + DialerDatabaseHelper::SmartDialDbColumns::DISPLAY_NAME_PRIMARY + ", "
        + DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "." + DialerDatabaseHelper::SmartDialDbColumns::CONTACT_ID + ", "
        + DialerDatabaseHelper::Tables::SMARTDIAL_TABLE + "." + DialerDatabaseHelper::SmartDialDbColumns::IS_PRIMARY + " DESC";

//=================================================================
// DialerDatabaseHelper::ContactNumber
//=================================================================
DialerDatabaseHelper::ContactNumber::ContactNumber(
    /* [in] */ Int64 id,
    /* [in] */ Int64 dataID,
    /* [in] */ const String& displayName,
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& lookupKey,
    /* [in] */ Int64 photoId)
    : mId(id)
    , mDataId(dataID)
    , mPhotoId(photoId)
{
    mDisplayName = displayName;
    mPhoneNumber = phoneNumber;
    mLookupKey = lookupKey;
}

ECode DialerDatabaseHelper::ContactNumber::HashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    assert(0 && "TODO");
    // Objects.hashCode(id, dataId, displayName, phoneNumber, lookupKey, photoId);
    return NOERROR;
}

ECode DialerDatabaseHelper::ContactNumber::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((IObject*)this == other) {
        *result = TRUE;
        return NOERROR;
    }
    if (IDialerDatabaseHelperContactNumber::Probe(other)) {
        ContactNumber* that = (ContactNumber*)IObject::Probe(other);
        *result = mId == that->mId
                && mDataId == that->mDataId
                && mDisplayName.Equals(that->mDisplayName)
                && mPhoneNumber.Equals(that->mPhoneNumber)
                && mLookupKey.Equals(that->mLookupKey)
                && mPhotoId == that->mPhotoId;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=================================================================
// DialerDatabaseHelper::ContactMatch
//=================================================================
DialerDatabaseHelper::ContactMatch::ContactMatch(
    /* [in] */ String lookupKey,
    /* [in] */ Int64 id)
    : mId(id)
{
    mLookupKey = lookupKey;
}

ECode DialerDatabaseHelper::ContactMatch::HashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    assert(0 && "TODO");
    // Objects.hashCode(lookupKey, id);
    return NOERROR;
}

ECode DialerDatabaseHelper::ContactMatch::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((IObject*)this == other) {
        *result = TRUE;
        return NOERROR;
    }
    if (IDialerDatabaseHelperContactMatch::Probe(other)) {
        ContactMatch* that = (ContactMatch*)IObject::Probe(other);
        *result = mId == that->mId
                && mLookupKey.Equals(that->mLookupKey);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=================================================================
// DialerDatabaseHelper::SmartDialUpdateAsyncTask
//=================================================================
DialerDatabaseHelper::SmartDialUpdateAsyncTask::SmartDialUpdateAsyncTask(
    /* [in] */ DialerDatabaseHelper* host)
    : mHost(host)
{}

ECode DialerDatabaseHelper::SmartDialUpdateAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    if (DialerDatabaseHelper::DEBUG) {
        Logger::V(DialerDatabaseHelper::TAG, "Updating database");
    }
    mHost->UpdateSmartDialDatabase();
    *result = NULL;
    return NOERROR;
}

ECode DialerDatabaseHelper::SmartDialUpdateAsyncTask::OnCancelled()
{
    if (DialerDatabaseHelper::DEBUG) {
        Logger::V(DialerDatabaseHelper::TAG, "Updating Cancelled");
    }
    return AsyncTask::OnCancelled();
}

ECode DialerDatabaseHelper::SmartDialUpdateAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    if (DialerDatabaseHelper::DEBUG) {
        Logger::V(DialerDatabaseHelper::TAG, "Updating Finished");
    }
    return AsyncTask::OnPostExecute(result);
}

//=================================================================
// DialerDatabaseHelper
//=================================================================
CAR_INTERFACE_IMPL(DialerDatabaseHelper, SQLiteOpenHelper, IDialerDatabaseHelper);

AutoPtr<IAtomicBoolean> CreateInUpdate()
{
    AutoPtr<IAtomicBoolean> inUpdate;
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&inUpdate);
    return inUpdate;
}

const String DialerDatabaseHelper::TAG("DialerDatabaseHelper");
const Boolean DialerDatabaseHelper::DEBUG = FALSE;

AutoPtr<IDialerDatabaseHelper> DialerDatabaseHelper::sSingleton;

Object DialerDatabaseHelper::mLock;
const AutoPtr<IAtomicBoolean> DialerDatabaseHelper::sInUpdate = CreateInUpdate();
const String DialerDatabaseHelper::DATABASE_LAST_CREATED_SHARED_PREF("com.android.dialer");
const String DialerDatabaseHelper::LAST_UPDATED_MILLIS("last_updated_millis");
const String DialerDatabaseHelper::DATABASE_VERSION_PROPERTY("database_version");

const Int32 DialerDatabaseHelper::MAX_ENTRIES = 20;

AutoPtr<IDialerDatabaseHelper> DialerDatabaseHelper::GetInstance(
    /* [in] */ IContext* context)
{
    if (DEBUG) {
        Logger::V(TAG, "Getting Instance");
    }
    if (sSingleton == NULL) {
        // Use application context instead of activity context because this is a singleton,
        // and we don't want to leak the activity if the activity is not running but the
        // dialer database helper is still doing work.
        AutoPtr<IContext> appContext;
        context->GetApplicationContext((IContext**)&appContext);
        sSingleton = (IDialerDatabaseHelper*)new DialerDatabaseHelper(
                appContext, DATABASE_NAME);
    }
    return sSingleton;
}

AutoPtr<IDialerDatabaseHelper> DialerDatabaseHelper::GetNewInstanceForTest(
    /* [in] */ IContext* context)
{
    AutoPtr<IDialerDatabaseHelper> helper = (IDialerDatabaseHelper*)
            new DialerDatabaseHelper(context, String(NULL));
    return helper;
}

DialerDatabaseHelper::DialerDatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ const String& databaseName)
{
    DialerDatabaseHelper(context, databaseName, DATABASE_VERSION);
}

DialerDatabaseHelper::DialerDatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ const String& databaseName,
    /* [in] */ Int32 dbVersion)
{
    SQLiteOpenHelper::constructor(context, databaseName, NULL, dbVersion);
    assert(0 && "TODO");
    // mContext = Preconditions.checkNotNull(context, "Context must not be null");
}

ECode DialerDatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    SetupTables(db);
    return NOERROR;
}

void DialerDatabaseHelper::SetupTables(
    /* [in] */ ISQLiteDatabase* db)
{
    DropTables(db);
    db->ExecSQL(String("CREATE TABLE ") + Tables::SMARTDIAL_TABLE + " (" +
            SmartDialDbColumns::_ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
            SmartDialDbColumns::DATA_ID + " INTEGER, " +
            SmartDialDbColumns::NUMBER + " TEXT," +
            SmartDialDbColumns::CONTACT_ID + " INTEGER," +
            SmartDialDbColumns::LOOKUP_KEY + " TEXT," +
            SmartDialDbColumns::DISPLAY_NAME_PRIMARY + " TEXT, " +
            SmartDialDbColumns::PHOTO_ID + " INTEGER, " +
            SmartDialDbColumns::LAST_SMARTDIAL_UPDATE_TIME + " LONG, " +
            SmartDialDbColumns::LAST_TIME_USED + " LONG, " +
            SmartDialDbColumns::TIMES_USED + " INTEGER, " +
            SmartDialDbColumns::STARRED + " INTEGER, " +
            SmartDialDbColumns::IS_SUPER_PRIMARY + " INTEGER, " +
            SmartDialDbColumns::IN_VISIBLE_GROUP + " INTEGER, " +
            SmartDialDbColumns::IS_PRIMARY + " INTEGER" +
            ");");

    db->ExecSQL(String("CREATE TABLE ") + Tables::PREFIX_TABLE + " (" +
            IBaseColumns::ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
            PrefixColumns::PREFIX + " TEXT COLLATE NOCASE, " +
            PrefixColumns::CONTACT_ID + " INTEGER" +
            ");");

    db->ExecSQL(String("CREATE TABLE ") + Tables::PROPERTIES + " (" +
            PropertiesColumns::PROPERTY_KEY + " TEXT PRIMARY KEY, " +
            PropertiesColumns::PROPERTY_VALUE + " TEXT " +
            ");");

    SetProperty(db, DATABASE_VERSION_PROPERTY, StringUtils::ToString(DATABASE_VERSION));
    ResetSmartDialLastUpdatedTime();
}

ECode DialerDatabaseHelper::DropTables(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + Tables::PREFIX_TABLE);
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + Tables::SMARTDIAL_TABLE);
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + Tables::PROPERTIES);
    return NOERROR;
}

ECode DialerDatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldNumber,
    /* [in] */ Int32 newVersion)
{
    // Disregard the old version and new versions provided by SQLiteOpenHelper, we will read
    // our own from the database.

    Int32 oldVersion;

    GetPropertyAsInt(db, DATABASE_VERSION_PROPERTY, 0, &oldVersion);

    if (oldVersion == 0) {
        Logger::E(TAG, "Malformed database version..recreating database");
    }

    if (oldVersion < 4) {
        SetupTables(db);
        return NOERROR;
    }

    if (oldVersion != DATABASE_VERSION) {
        // throw new IllegalStateException(
        //         "error upgrading the database to version " + DATABASE_VERSION);
        Logger::E(TAG, "error upgrading the database to version %d", DATABASE_VERSION);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    SetProperty(db, DATABASE_VERSION_PROPERTY, StringUtils::ToString(DATABASE_VERSION));
    return NOERROR;
}

ECode DialerDatabaseHelper::SetProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<ISQLiteDatabase> db;
    GetWritableDatabase((ISQLiteDatabase**)&db);
    return SetProperty(db, key, value);
}

ECode DialerDatabaseHelper::SetProperty(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(PropertiesColumns::PROPERTY_KEY, key);
    values->Put(PropertiesColumns::PROPERTY_VALUE, value);
    Int64 result;
    return db->Replace(Tables::PROPERTIES, String(NULL), values, &result);
}

ECode DialerDatabaseHelper::GetProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* property)
{
    VALIDATE_NOT_NULL(property);

    AutoPtr<ISQLiteDatabase> db;
    GetReadableDatabase((ISQLiteDatabase**)&db);
    return GetProperty(db, key, defaultValue, property);
}

ECode DialerDatabaseHelper::GetProperty(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* property)
{
    VALIDATE_NOT_NULL(property);
    // try {
    String value;
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(1);
    columns->Set(0, PropertiesColumns::PROPERTY_VALUE);
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    selectionArgs->Set(0, key);
    ECode ec = db->Query(Tables::PROPERTIES, columns,
            PropertiesColumns::PROPERTY_KEY + "=?", selectionArgs,
            String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
    if (FAILED(ec)) {
        *property = defaultValue;
        return NOERROR;
    }

    if (cursor != NULL) {
        // try {
        Boolean succeeded = FALSE;
        if (cursor->MoveToFirst(&succeeded), succeeded) {
            cursor->GetString(0, &value);
        }
        ICloseable::Probe(cursor)->Close();
        // } finally {
        //     cursor.close();
        // }
    }
    *property = !value.IsNull() ? value : defaultValue;
    return NOERROR;
    // } catch (SQLiteException e) {
    //     return defaultValue;
    // }
}

ECode DialerDatabaseHelper::GetPropertyAsInt(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& key,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* property)
{
    String stored;
    GetProperty(db, key, String(""), &stored);
    // try {
    ECode ec = StringUtils::Parse(stored, property);
    if (FAILED(ec)) {
        *property = defaultValue;
    }
    // } catch (NumberFormatException e) {
    //     return defaultValue;
    // }
    return NOERROR;
}

void DialerDatabaseHelper::ResetSmartDialLastUpdatedTime()
{
    AutoPtr<ISharedPreferences> databaseLastUpdateSharedPref;
    mContext->GetSharedPreferences(
            DATABASE_LAST_CREATED_SHARED_PREF, IContext::MODE_PRIVATE,
            (ISharedPreferences**)&databaseLastUpdateSharedPref);

    AutoPtr<ISharedPreferencesEditor> editor;
    databaseLastUpdateSharedPref->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutInt64(LAST_UPDATED_MILLIS, 0);
    Boolean succeeded;
    editor->Commit(&succeeded);
}

ECode DialerDatabaseHelper::StartSmartDialUpdateThread()
{
    AutoPtr<SmartDialUpdateAsyncTask> task = new SmartDialUpdateAsyncTask(this);
    task->Execute((ArrayOf<IInterface*>*)NULL);
    return NOERROR;
}

void DialerDatabaseHelper::RemoveDeletedContacts(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& last_update_time)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    selectionArgs->Set(0, last_update_time);
    AutoPtr<ICursor> deletedContactCursor;
    assert(0 && "TODO");
    // resolver->Query(
    //         DeleteContactQuery::URI,
    //         DeleteContactQuery::PROJECTION,
    //         IDialerDatabaseHelperDeleteContactQuery::SELECT_UPDATED_CLAUSE,
    //         selectionArgs, String(NULL), (ICursor**)&deletedContactCursor);
    if (deletedContactCursor == NULL) {
        return;
    }

    db->BeginTransaction();
    // try {
    Boolean succeeded = FALSE;
    while (deletedContactCursor->MoveToNext(&succeeded), succeeded) {
        Int64 deleteContactId;
        FAIL_GOTO(deletedContactCursor->GetInt64(
            DeleteContactQuery::DELETED_CONTACT_ID, &deleteContactId), exit);

        Int32 value;
        FAIL_GOTO(db->Delete(Tables::SMARTDIAL_TABLE,
                SmartDialDbColumns::CONTACT_ID + "=" + deleteContactId, NULL, &value), exit);
        FAIL_GOTO(db->Delete(Tables::PREFIX_TABLE,
                PrefixColumns::CONTACT_ID + "=" + deleteContactId, NULL, &value), exit);
    }

    FAIL_GOTO(db->SetTransactionSuccessful(), exit);
    // } finally {
    //     deletedContactCursor.close();
    //     db.endTransaction();
    // }
exit:
    ICloseable::Probe(deletedContactCursor)->Close();
    db->EndTransaction();
}

void DialerDatabaseHelper::RemovePotentiallyCorruptedContacts(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& last_update_time)
{
    Int32 value;
    db->Delete(Tables::PREFIX_TABLE,
            PrefixColumns::CONTACT_ID + " IN " +
            "(SELECT " + SmartDialDbColumns::CONTACT_ID + " FROM " + Tables::SMARTDIAL_TABLE +
            " WHERE " + SmartDialDbColumns::LAST_SMARTDIAL_UPDATE_TIME + " > " +
            last_update_time + ")",
            NULL, &value);
    db->Delete(Tables::SMARTDIAL_TABLE,
            SmartDialDbColumns::LAST_SMARTDIAL_UPDATE_TIME + " > " + last_update_time,
            NULL, &value);
}

void DialerDatabaseHelper::RemoveAllContacts(
    /* [in] */ ISQLiteDatabase* db)
{
    Int32 value;
    db->Delete(Tables::SMARTDIAL_TABLE, String(NULL), NULL, &value);
    db->Delete(Tables::PREFIX_TABLE, String(NULL), NULL, &value);
}

Int32 DialerDatabaseHelper::CountPrefixTableRows(
    /* [in] */ ISQLiteDatabase* db)
{
    Int64 result;
    AutoPtr<IDatabaseUtils> utils;
    CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&utils);
    utils->Int64ForQuery(db, String("SELECT COUNT(1) FROM ") + Tables::PREFIX_TABLE,
        NULL, &result);
    return (Int32)result;
}

void DialerDatabaseHelper::RemoveUpdatedContacts(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ICursor* updatedContactCursor)
{
    db->BeginTransaction();
    // try {
    Boolean succeeded = FALSE;
    while (updatedContactCursor->MoveToNext(&succeeded), succeeded) {
        Int64 contactId;
        FAIL_GOTO(updatedContactCursor->GetInt64(
            PhoneQuery::PHONE_CONTACT_ID, &contactId), exit);

        Int32 value;
        FAIL_GOTO(db->Delete(Tables::SMARTDIAL_TABLE, SmartDialDbColumns::CONTACT_ID + "=" +
                contactId, NULL, &value), exit);
        FAIL_GOTO(db->Delete(Tables::PREFIX_TABLE, PrefixColumns::CONTACT_ID + "=" +
                contactId, NULL, &value), exit);
    }

    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    // }
exit:
    db->EndTransaction();
}

void DialerDatabaseHelper::InsertUpdatedContactsAndNumberPrefix(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ICursor* updatedContactCursor,
    /* [in] */ IInteger64* currentMillis)
{
    db->BeginTransaction();
    // try {
    String sqlInsert = String("INSERT INTO ") + Tables::SMARTDIAL_TABLE + " (" +
            SmartDialDbColumns::DATA_ID + ", " +
            SmartDialDbColumns::NUMBER + ", " +
            SmartDialDbColumns::CONTACT_ID + ", " +
            SmartDialDbColumns::LOOKUP_KEY + ", " +
            SmartDialDbColumns::DISPLAY_NAME_PRIMARY + ", " +
            SmartDialDbColumns::PHOTO_ID + ", " +
            SmartDialDbColumns::LAST_TIME_USED + ", " +
            SmartDialDbColumns::TIMES_USED + ", " +
            SmartDialDbColumns::STARRED + ", " +
            SmartDialDbColumns::IS_SUPER_PRIMARY + ", " +
            SmartDialDbColumns::IN_VISIBLE_GROUP+ ", " +
            SmartDialDbColumns::IS_PRIMARY + ", " +
            SmartDialDbColumns::LAST_SMARTDIAL_UPDATE_TIME + ") " +
            " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    AutoPtr<ISQLiteStatement> insert;
    AutoPtr<ISQLiteStatement> numberInsert;
    String numberSqlInsert;
    FAIL_GOTO(db->CompileStatement(sqlInsert, (ISQLiteStatement**)&insert), exit);

    numberSqlInsert = String("INSERT INTO ") + Tables::PREFIX_TABLE + " (" +
            PrefixColumns::CONTACT_ID + ", " +
            PrefixColumns::PREFIX  + ") " +
            " VALUES (?, ?)";
    FAIL_GOTO(db->CompileStatement(numberSqlInsert,
            (ISQLiteStatement**)&numberInsert), exit);

    Boolean succeeded;
    updatedContactCursor->MoveToPosition(-1, &succeeded);
    while (updatedContactCursor->MoveToNext(&succeeded), succeeded) {
        ISQLiteProgram::Probe(insert)->ClearBindings();

        // Handle string columns which can possibly be null first. In the case of certain
        // null columns (due to malformed rows possibly inserted by third-party apps
        // or sync adapters), skip the phone number row.
        String number;
        updatedContactCursor->GetString(PhoneQuery::PHONE_NUMBER, &number);
        if (TextUtils::IsEmpty(number)) {
            continue;
        }
        else {
            ISQLiteProgram::Probe(insert)->BindString(2, number);
        }

        String lookupKey;
        updatedContactCursor->GetString(
                PhoneQuery::PHONE_LOOKUP_KEY, &lookupKey);
        if (TextUtils::IsEmpty(lookupKey)) {
            continue;
        }
        else {
            ISQLiteProgram::Probe(insert)->BindString(4, lookupKey);
        }

        String displayName;
        updatedContactCursor->GetString(
                PhoneQuery::PHONE_DISPLAY_NAME, &displayName);
        if (displayName == NULL) {
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            String name;
            resources->GetString(R::string::missing_name, &name);
            ISQLiteProgram::Probe(insert)->BindString(5, name);
        }
        else {
            ISQLiteProgram::Probe(insert)->BindString(5, displayName);
        }

        Int64 value;
        updatedContactCursor->GetInt64(PhoneQuery::PHONE_ID, &value);
        ISQLiteProgram::Probe(insert)->BindInt64(1, value);
        updatedContactCursor->GetInt64(PhoneQuery::PHONE_CONTACT_ID, &value);
        ISQLiteProgram::Probe(insert)->BindInt64(3, value);
        updatedContactCursor->GetInt64(PhoneQuery::PHONE_PHOTO_ID, &value);
        ISQLiteProgram::Probe(insert)->BindInt64(6, value);
        updatedContactCursor->GetInt64(PhoneQuery::PHONE_LAST_TIME_USED, &value);
        ISQLiteProgram::Probe(insert)->BindInt64(7, value);

        Int32 intValue;
        updatedContactCursor->GetInt32(PhoneQuery::PHONE_TIMES_USED, &intValue);
        ISQLiteProgram::Probe(insert)->BindInt64(8, intValue);
        updatedContactCursor->GetInt32(PhoneQuery::PHONE_STARRED, &intValue);
        ISQLiteProgram::Probe(insert)->BindInt64(9, intValue);
        updatedContactCursor->GetInt32(PhoneQuery::PHONE_IS_SUPER_PRIMARY, &intValue);
        ISQLiteProgram::Probe(insert)->BindInt64(10, intValue);
        updatedContactCursor->GetInt32(PhoneQuery::PHONE_IN_VISIBLE_GROUP, &intValue);
        ISQLiteProgram::Probe(insert)->BindInt64(11, intValue);
        updatedContactCursor->GetInt32(PhoneQuery::PHONE_IS_PRIMARY, &intValue);
        ISQLiteProgram::Probe(insert)->BindInt64(12, intValue);
        currentMillis->GetValue(&value);
        ISQLiteProgram::Probe(insert)->BindInt64(13, value);
        insert->ExecuteInsert(&value);
        String contactPhoneNumber;
        updatedContactCursor->GetString(PhoneQuery::PHONE_NUMBER, &contactPhoneNumber);

        AutoPtr<IArrayList> numberPrefixes =
                SmartDialPrefix::ParseToNumberTokens(contactPhoneNumber);

        AutoPtr<IIterator> it;
        numberPrefixes->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> item;
            it->GetNext((IInterface**)&item);
            String numberPrefix = CoreUtils::Unbox(ICharSequence::Probe(item));
            updatedContactCursor->GetInt64(PhoneQuery::PHONE_CONTACT_ID, &value);
            ISQLiteProgram::Probe(numberInsert)->BindInt64(1, value);
            ISQLiteProgram::Probe(numberInsert)->BindString(2, numberPrefix);
            numberInsert->ExecuteInsert(&value);
            ISQLiteProgram::Probe(numberInsert)->ClearBindings();
        }
    }

    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    // }
exit:
    db->EndTransaction();
}

void DialerDatabaseHelper::InsertNamePrefixes(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ICursor* nameCursor)
{
    Int32 columnIndexName;
    nameCursor->GetColumnIndex(
            SmartDialDbColumns::DISPLAY_NAME_PRIMARY, &columnIndexName);
    Int32 columnIndexContactId;
    nameCursor->GetColumnIndex(SmartDialDbColumns::CONTACT_ID, &columnIndexContactId);

    db->BeginTransaction();
    // try {
    String sqlInsert = String("INSERT INTO ") + Tables::PREFIX_TABLE + " (" +
            PrefixColumns::CONTACT_ID + ", " +
            PrefixColumns::PREFIX  + ") " +
            " VALUES (?, ?)";
    AutoPtr<ISQLiteStatement> insert;
    FAIL_GOTO(db->CompileStatement(sqlInsert,
            (ISQLiteStatement**)&insert), exit);

    Boolean succeeded;
    while (nameCursor->MoveToNext(&succeeded), succeeded) {
        /** Computes a list of prefixes of a given contact name. */
        String name;
        nameCursor->GetString(columnIndexName, &name);
        AutoPtr<IArrayList> namePrefixes =
                SmartDialPrefix::GenerateNamePrefixes(name);

        AutoPtr<IIterator> it;
        namePrefixes->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> item;
            it->GetNext((IInterface**)&item);
            String namePrefix = CoreUtils::Unbox(ICharSequence::Probe(item));
            Int64 value;
            nameCursor->GetInt64(columnIndexContactId, &value);
            ISQLiteProgram::Probe(insert)->BindInt64(1, value);
            ISQLiteProgram::Probe(insert)->BindString(2, namePrefix);
            insert->ExecuteInsert(&value);
            ISQLiteProgram::Probe(insert)->ClearBindings();
        }
    }

    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    // }
exit:
    db->EndTransaction();
}

ECode DialerDatabaseHelper::UpdateSmartDialDatabase()
{
    AutoPtr<ISQLiteDatabase> db;
    GetWritableDatabase((ISQLiteDatabase**)&db);

    {    AutoLock syncLock(mLock);
        if (DEBUG) {
            Logger::V(TAG, "Starting to update database");
        }
        assert(0 && "TODO");
        // AutoPtr<IStopWatch> stopWatch = DEBUG ? StopWatch::Start(String("Updating databases")) : NULL;

        /** Gets the last update time on the database. */
        AutoPtr<ISharedPreferences> databaseLastUpdateSharedPref;
        mContext->GetSharedPreferences(DATABASE_LAST_CREATED_SHARED_PREF,
                IContext::MODE_PRIVATE, (ISharedPreferences**)&databaseLastUpdateSharedPref);
        Int64 value;
        databaseLastUpdateSharedPref->GetInt64(LAST_UPDATED_MILLIS, 0, &value);
        String lastUpdateMillis = StringUtils::ToString(value);

        if (DEBUG) {
            Logger::V(TAG, "Last updated at %s", lastUpdateMillis.string());
        }
        /** Queries the contact database to get contacts that have been updated since the last
         * update time.
         */
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        args->Set(0, lastUpdateMillis);
        AutoPtr<ICursor> updatedContactCursor;
        assert(0 && "TODO");
        // resolver->Query(PhoneQuery::URI,
        //         PhoneQuery::PROJECTION, PhoneQuery::SELECTION,
        //         args, NULL, (ICursor**)&updatedContactCursor);
        if (updatedContactCursor == NULL) {
            if (DEBUG) {
                Logger::E(TAG, "SmartDial query received null for cursor");
            }
            return NOERROR;
        }

        /** Sets the time after querying the database as the current update time. */
        AutoPtr<ISystem> sysObj;
        CSystem::AcquireSingleton((ISystem**)&sysObj);
        Int64 currentMillis;
        sysObj->GetCurrentTimeMillis(&currentMillis);

        // try {
        if (DEBUG) {
        assert(0 && "TODO");
            // stopWatch->Lap("Queried the Contacts database");
        }

        /** Prevents the app from reading the dialer database when updating. */
        Boolean res;
        sInUpdate->GetAndSet(TRUE, &res);

        /** Removes contacts that have been deleted. */
        RemoveDeletedContacts(db, lastUpdateMillis);
        RemovePotentiallyCorruptedContacts(db, lastUpdateMillis);

        if (DEBUG) {
            assert(0 && "TODO");
            // stopWatch->Lap("Finished deleting deleted entries");
        }

        /** If the database did not exist before, jump through deletion as there is nothing
         * to delete.
         */
        if (!lastUpdateMillis.Equals("0")) {
            /** Removes contacts that have been updated. Updated contact information will be
             * inserted later.
             */
            RemoveUpdatedContacts(db, updatedContactCursor);
            if (DEBUG) {
                assert(0 && "TODO");
                // stopWatch->Lap("Finished deleting updated entries");
            }
        }

        /** Inserts recently updated contacts to the smartdial database.*/
        InsertUpdatedContactsAndNumberPrefix(db, updatedContactCursor, CoreUtils::Convert(currentMillis));
        if (DEBUG) {
            assert(0 && "TODO");
            // stopWatch->Lap("Finished building the smart dial table");
        }
        ICloseable::Probe(updatedContactCursor)->Close();
        // } finally {
            /** Inserts prefixes of phone numbers into the prefix table.*/
        //     updatedContactCursor.close();
        // }

        /** Gets a list of distinct contacts which have been updated, and adds the name prefixes
         * of these contacts to the prefix table.
         */
        AutoPtr<ArrayOf<String> > rowArgs = ArrayOf<String>::Alloc(0);
        AutoPtr<ICursor> nameCursor;
        db->RawQuery(
                String("SELECT DISTINCT ") +
                SmartDialDbColumns::DISPLAY_NAME_PRIMARY + ", " + SmartDialDbColumns::CONTACT_ID +
                " FROM " + Tables::SMARTDIAL_TABLE +
                " WHERE " + SmartDialDbColumns::LAST_SMARTDIAL_UPDATE_TIME +
                " = " + StringUtils::ToString(currentMillis),
                rowArgs, (ICursor**)&nameCursor);
        if (nameCursor != NULL) {
            // try {
            if (DEBUG) {
                assert(0 && "TODO");
                // stopWatch->Lap("Queried the smart dial table for contact names");
            }

            /** Inserts prefixes of names into the prefix table.*/
            InsertNamePrefixes(db, nameCursor);
            if (DEBUG) {
                assert(0 && "TODO");
                // stopWatch->Lap("Finished building the name prefix table");
            }
            ICloseable::Probe(nameCursor)->Close();
            // } finally {
            //     nameCursor.close();
            // }
        }

        /** Creates index on contact_id for fast JOIN operation. */
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS smartdial_contact_id_index ON ") +
                Tables::SMARTDIAL_TABLE + " (" + SmartDialDbColumns::CONTACT_ID  + ");");
        /** Creates index on last_smartdial_update_time for fast SELECT operation. */
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS smartdial_last_update_index ON ") +
                Tables::SMARTDIAL_TABLE + " (" +
                SmartDialDbColumns::LAST_SMARTDIAL_UPDATE_TIME + ");");
        /** Creates index on sorting fields for fast sort operation. */
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS smartdial_sort_index ON ") +
                Tables::SMARTDIAL_TABLE + " (" +
                SmartDialDbColumns::STARRED + ", " +
                SmartDialDbColumns::IS_SUPER_PRIMARY + ", " +
                SmartDialDbColumns::LAST_TIME_USED + ", " +
                SmartDialDbColumns::TIMES_USED + ", " +
                SmartDialDbColumns::IN_VISIBLE_GROUP +  ", " +
                SmartDialDbColumns::DISPLAY_NAME_PRIMARY + ", " +
                SmartDialDbColumns::CONTACT_ID + ", " +
                SmartDialDbColumns::IS_PRIMARY +
                ");");
        /** Creates index on prefix for fast SELECT operation. */
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS nameprefix_index ON ") +
                Tables::PREFIX_TABLE + " (" + PrefixColumns::PREFIX + ");");
        /** Creates index on contact_id for fast JOIN operation. */
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS nameprefix_contact_id_index ON ") +
                Tables::PREFIX_TABLE + " (" + PrefixColumns::CONTACT_ID + ");");

        if (DEBUG) {
            assert(0 && "TODO");
            // stopWatch->Lap(TAG + "Finished recreating index");
        }

        /** Updates the database index statistics.*/
        db->ExecSQL(String("ANALYZE ") + Tables::SMARTDIAL_TABLE);
        db->ExecSQL(String("ANALYZE ") + Tables::PREFIX_TABLE);
        db->ExecSQL(String("ANALYZE smartdial_contact_id_index"));
        db->ExecSQL(String("ANALYZE smartdial_last_update_index"));
        db->ExecSQL(String("ANALYZE nameprefix_index"));
        db->ExecSQL(String("ANALYZE nameprefix_contact_id_index"));
        if (DEBUG) {
            assert(0 && "TODO");
            // stopWatch->TopAndLog(TAG + "Finished updating index stats", 0);
        }

        sInUpdate->GetAndSet(FALSE, &res);

        AutoPtr<ISharedPreferencesEditor> editor;
        databaseLastUpdateSharedPref->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutInt64(LAST_UPDATED_MILLIS, currentMillis);
        Boolean succeeded;
        editor->Commit(&succeeded);
    }

    return NOERROR;
}

ECode DialerDatabaseHelper::GetLooseMatches(
    /* [in] */ const String& query,
    /* [in] */ ISmartDialNameMatcher* nameMatcher,
    /* [out] */ IArrayList** matches)
{
    VALIDATE_NOT_NULL(matches);

    Boolean inUpdate;
    sInUpdate->Get(&inUpdate);
    if (inUpdate) {
        CArrayList::New(matches);
        return NOERROR;
    }

    AutoPtr<ISQLiteDatabase> db;
    GetReadableDatabase((ISQLiteDatabase**)&db);

    /** Uses SQL query wildcard '%' to represent prefix matching.*/
    String looseQuery = query + "%";

    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);

    assert(0 && "TODO");
    // AutoPtr<IStopWatch> stopWatch = DEBUG ? StopWatch::Start(":Name Prefix query") : NULL;

    AutoPtr<ISystem> sysObj;
    CSystem::AcquireSingleton((ISystem**)&sysObj);
    Int64 currentMillis;
    sysObj->GetCurrentTimeMillis(&currentMillis);
    String currentTimeStamp = StringUtils::ToString(currentMillis);

    /** Queries the database to find contacts that have an index matching the query prefix. */
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    args->Set(0, currentTimeStamp);
    AutoPtr<ICursor> cursor;
    db->RawQuery(String("SELECT ") +
            SmartDialDbColumns::DATA_ID + ", " +
            SmartDialDbColumns::DISPLAY_NAME_PRIMARY + ", " +
            SmartDialDbColumns::PHOTO_ID + ", " +
            SmartDialDbColumns::NUMBER + ", " +
            SmartDialDbColumns::CONTACT_ID + ", " +
            SmartDialDbColumns::LOOKUP_KEY +
            " FROM " + Tables::SMARTDIAL_TABLE + " WHERE " +
            SmartDialDbColumns::CONTACT_ID + " IN " +
                " (SELECT " + PrefixColumns::CONTACT_ID +
                " FROM " + Tables::PREFIX_TABLE +
                " WHERE " + Tables::PREFIX_TABLE + "." + PrefixColumns::PREFIX +
                " LIKE '" + looseQuery + "')" +
            " ORDER BY " + SmartDialSortingOrder::SORT_ORDER,
           args, (ICursor**)&cursor);
    if (cursor == NULL) {
        *matches = result;
        REFCOUNT_ADD(*matches);
        return NOERROR;
    }
    // try {
    if (DEBUG) {
        assert(0 && "TODO");
        // stopWatch->Lap("Prefix query completed");
    }

    /** Gets the column ID from the cursor.*/
    Int32 columnDataId = 0;
    Int32 columnDisplayNamePrimary = 1;
    Int32 columnPhotoId = 2;
    Int32 columnNumber = 3;
    Int32 columnId = 4;
    Int32 columnLookupKey = 5;
    if (DEBUG) {
        assert(0 && "TODO");
        // stopWatch->Lap("Found column IDs");
    }

    AutoPtr<IHashSet> duplicates;
    CHashSet::New((IHashSet**)&duplicates);
    Int32 counter = 0;
    if (DEBUG) {
        assert(0 && "TODO");
        // stopWatch->Lap("Moved cursor to start");
    }
    /** Iterates the cursor to find top contact suggestions without duplication.*/
    Boolean succeeded;
    while ((cursor->MoveToNext(&succeeded), succeeded) && (counter < MAX_ENTRIES)) {
        Int64 dataID;
        cursor->GetInt64(columnDataId, &dataID);
        String displayName;
        cursor->GetString(columnDisplayNamePrimary, &displayName);
        String phoneNumber;
        cursor->GetString(columnNumber, &phoneNumber);
        Int64 id;
        cursor->GetInt64(columnId, &id);
        Int64 photoId;
        cursor->GetInt64(columnPhotoId, &photoId);
        String lookupKey;
        cursor->GetString(columnLookupKey, &lookupKey);

        /** If a contact already exists and another phone number of the contact is being
         * processed, skip the second instance.
         */
        AutoPtr<ContactMatch> contactMatch = new ContactMatch(lookupKey, id);
        Boolean contains;
        if (duplicates->Contains(
                (IObject*)contactMatch.Get(), &contains), contains) {
            continue;
        }

        /**
         * If the contact has either the name or number that matches the query, add to the
         * result.
         */
        Boolean nameMatches;
        nameMatcher->Matches(displayName, &nameMatches);
        AutoPtr<ISmartDialMatchPosition> position;
        nameMatcher->MatchesNumber(phoneNumber, query, (ISmartDialMatchPosition**)&position);
        Boolean numberMatches = (position != NULL);
        if (nameMatches || numberMatches) {
            /** If a contact has not been added, add it to the result and the hash set.*/
            duplicates->Add((IObject*)contactMatch.Get());
            AutoPtr<ContactNumber> contactNumber = new ContactNumber(id,
                    dataID, displayName, phoneNumber, lookupKey, photoId);
            result->Add((IObject*)contactNumber.Get());
            counter++;
            if (DEBUG) {
                assert(0 && "TODO");
                // stopWatch->Lap(String("Added one result: Name: ") + displayName);
            }
        }
    }

    if (DEBUG) {
        assert(0 && "TODO");
        // stopWatch->StopAndLog(TAG + "Finished loading cursor", 0);
    }
    // } finally {
    //     cursor.close();
    // }

    ICloseable::Probe(cursor)->Close();
    *matches = result;
    REFCOUNT_ADD(*matches);
    return NOERROR;
}

} // Database
} // Dialer
} // Apps
} // Elastos
