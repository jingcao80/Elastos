
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Text.h"
#include "elastos/droid/settings/inputmethod/CInputMethodAndLanguageSettings.h"
#include "elastos/droid/settings/notification/CNotificationSettings.h"
#include "elastos/droid/settings/notification/COtherSoundSettings.h"
#include "elastos/droid/settings/notification/CZenModeSettings.h"
#include "elastos/droid/settings/search/Index.h"
#include "elastos/droid/settings/search/IndexDatabaseHelper.h"
#include "elastos/droid/settings/search/SearchIndexableResources.h"
#include "elastos/droid/settings/search/Ranking.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "elastos/droid/settings/wifi/CSavedAccessPointsWifiSettings.h"
#include "elastos/droid/settings/wifi/CWifiSettings.h"
#include "elastos/droid/settings/CSecuritySettings.h"
#include "elastos/droid/settings/CWirelessSettings.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/droid/utility/Xml.h>
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/regex/Pattern.h>
#include "../R.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Settings::CSecuritySettings;
using Elastos::Droid::Settings::CWirelessSettings;
using Elastos::Droid::Settings::Inputmethod::CInputMethodAndLanguageSettings;
using Elastos::Droid::Settings::Notification::CNotificationSettings;
using Elastos::Droid::Settings::Notification::COtherSoundSettings;
using Elastos::Droid::Settings::Notification::CZenModeSettings;
using Elastos::Droid::Settings::Wifi::CWifiSettings;
using Elastos::Droid::Settings::Wifi::CSavedAccessPointsWifiSettings;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Database::CMergeCursor;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::CSearchIndexablesContract;
using Elastos::Droid::Provider::ISearchIndexablesContract;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Text::INormalizer;
using Elastos::Text::CNormalizerHelper;
using Elastos::Text::NormalizerForm_NFD;
using Elastos::Core::AutoLock;
using Elastos::Core::IInteger64;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Regex::Pattern;
using Elastos::Utility::Regex::IMatcher;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

const Int32 Index::COLUMN_INDEX_RANK = 0;
const Int32 Index::COLUMN_INDEX_TITLE = 1;
const Int32 Index::COLUMN_INDEX_SUMMARY_ON = 2;
const Int32 Index::COLUMN_INDEX_SUMMARY_OFF = 3;
const Int32 Index::COLUMN_INDEX_ENTRIES = 4;
const Int32 Index::COLUMN_INDEX_KEYWORDS = 5;
const Int32 Index::COLUMN_INDEX_CLASS_NAME = 6;
const Int32 Index::COLUMN_INDEX_SCREEN_TITLE = 7;
const Int32 Index::COLUMN_INDEX_ICON = 8;
const Int32 Index::COLUMN_INDEX_INTENT_ACTION = 9;
const Int32 Index::COLUMN_INDEX_INTENT_ACTION_TARGET_PACKAGE = 10;
const Int32 Index::COLUMN_INDEX_INTENT_ACTION_TARGET_CLASS = 11;
const Int32 Index::COLUMN_INDEX_ENABLED = 12;
const Int32 Index::COLUMN_INDEX_KEY = 13;
const Int32 Index::COLUMN_INDEX_USER_ID = 14;

const String Index::ENTRIES_SEPARATOR("|");

const String Index::TAG("Index");

static AutoPtr< ArrayOf<String> > InitSELECT_COLUMNS()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(14);
    (*args)[0] = IIndexDatabaseHelperIndexColumns::DATA_RANK;
    (*args)[1] = IIndexDatabaseHelperIndexColumns::DATA_TITLE;
    (*args)[2] = IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_ON;
    (*args)[3] = IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_OFF;
    (*args)[4] = IIndexDatabaseHelperIndexColumns::DATA_ENTRIES;
    (*args)[5] = IIndexDatabaseHelperIndexColumns::DATA_KEYWORDS;
    (*args)[6] = IIndexDatabaseHelperIndexColumns::CLASS_NAME;
    (*args)[7] = IIndexDatabaseHelperIndexColumns::SCREEN_TITLE;
    (*args)[8] = IIndexDatabaseHelperIndexColumns::ICON;
    (*args)[9] = IIndexDatabaseHelperIndexColumns::INTENT_ACTION;
    (*args)[10] = IIndexDatabaseHelperIndexColumns::INTENT_TARGET_PACKAGE;
    (*args)[11] = IIndexDatabaseHelperIndexColumns::INTENT_TARGET_CLASS;
    (*args)[12] = IIndexDatabaseHelperIndexColumns::ENABLED;
    (*args)[13] = IIndexDatabaseHelperIndexColumns::DATA_KEY_REF;

    return args;
}

const AutoPtr< ArrayOf<String> > Index::SELECT_COLUMNS = InitSELECT_COLUMNS();

static AutoPtr< ArrayOf<String> > InitMATCH_COLUMNS_PRIMARY()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
    (*args)[0] = IIndexDatabaseHelperIndexColumns::DATA_TITLE;
    (*args)[1] = IIndexDatabaseHelperIndexColumns::DATA_TITLE_NORMALIZED;
    (*args)[2] = IIndexDatabaseHelperIndexColumns::DATA_KEYWORDS;

    return args;
}

const AutoPtr< ArrayOf<String> > Index::MATCH_COLUMNS_PRIMARY = InitMATCH_COLUMNS_PRIMARY();

static AutoPtr< ArrayOf<String> > InitMATCH_COLUMNS_SECONDARY()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(5);
    (*args)[0] = IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_ON;
    (*args)[1] = IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_ON_NORMALIZED;
    (*args)[2] = IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_OFF;
    (*args)[3] = IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_OFF_NORMALIZED;
    (*args)[4] = IIndexDatabaseHelperIndexColumns::DATA_ENTRIES;

    return args;
}

const AutoPtr< ArrayOf<String> > Index::MATCH_COLUMNS_SECONDARY = InitMATCH_COLUMNS_SECONDARY();

Int64 Index::MAX_SAVED_SEARCH_QUERY = 64;
const Int32 Index::MAX_PROPOSED_SUGGESTIONS = 5;

const String Index::BASE_AUTHORITY("com.android.settings");

const String Index::EMPTY("");
const String Index::NON_BREAKING_HYPHEN("\u2011");
const String Index::HYPHEN("-");

const String Index::FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER("SEARCH_INDEX_DATA_PROVIDER");

const String Index::NODE_NAME_PREFERENCE_SCREEN("PreferenceScreen");
const String Index::NODE_NAME_CHECK_BOX_PREFERENCE("CheckBoxPreference");
const String Index::NODE_NAME_LIST_PREFERENCE("ListPreference");

static AutoPtr<IList> InitEMPTY_LIST()
{
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);

    AutoPtr<IList> list;
    coll->GetEmptyList((IList**)&list);
    return list;
}

const AutoPtr<IList> Index::EMPTY_LIST = InitEMPTY_LIST();

AutoPtr<Index> Index::sInstance;

static AutoPtr<IPattern> InitREMOVE_DIACRITICALS_PATTERN()
{
    AutoPtr<IPattern> pat;
    Pattern::Compile(String("\\p{InCombiningDiacriticalMarks}+"), (IPattern**)&pat);
    return pat;
}

const AutoPtr<IPattern> Index::REMOVE_DIACRITICALS_PATTERN = InitREMOVE_DIACRITICALS_PATTERN();

//===============================================================================
//                  Index::UpdateData
//===============================================================================

Index::UpdateData::UpdateData()
    : mForceUpdate(FALSE)
{
    CArrayList::New((IList**)&mDataToUpdate);
    CArrayList::New((IList**)&mDataToDelete);
    CHashMap::New((IMap**)&mNonIndexableKeys);
}

Index::UpdateData::UpdateData(
    /* [in] */ UpdateData* other)
    : mForceUpdate(FALSE)
{
    CArrayList::New(ICollection::Probe(other->mDataToUpdate), (IList**)&mDataToUpdate);
    CArrayList::New(ICollection::Probe(other->mDataToDelete), (IList**)&mDataToDelete);
    CHashMap::New(other->mNonIndexableKeys, (IMap**)&mNonIndexableKeys);
    mForceUpdate = other->mForceUpdate;
}

Index::UpdateData::~UpdateData()
{}

AutoPtr<Index::UpdateData> Index::UpdateData::Copy()
{
    return new UpdateData(this);
}

ECode Index::UpdateData::Clear()
{
    mDataToUpdate->Clear();
    mDataToDelete->Clear();
    mNonIndexableKeys->Clear();
    mForceUpdate = FALSE;
    return NOERROR;
}

//===============================================================================
//                  Index::UpdateIndexTask
//===============================================================================

Index::UpdateIndexTask::UpdateIndexTask(
    /* [in] */ Index* host)
    : mHost(host)
{}

Index::UpdateIndexTask::~UpdateIndexTask()
{}

ECode Index::UpdateIndexTask::OnPreExecute()
{
    AsyncTask::OnPreExecute();
    mHost->mIsAvailable->Set(FALSE);
    return NOERROR;
}

ECode Index::UpdateIndexTask::OnPostExecute(
    /* [in] */ IInterface* result) // Void
{
    AsyncTask::OnPostExecute(NULL);
    mHost->mIsAvailable->Set(TRUE);
    return NOERROR;
}

ECode Index::UpdateIndexTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<UpdateData> updateData = (UpdateData*)IObject::Probe((*params)[0]);
    AutoPtr<IList> dataToUpdate = updateData->mDataToUpdate;
    AutoPtr<IList> dataToDelete = updateData->mDataToUpdate;
    AutoPtr<IMap> nonIndexableKeys = updateData->mNonIndexableKeys;

    Boolean forceUpdate = updateData->mForceUpdate;

    AutoPtr<ISQLiteDatabase> database = mHost->GetWritableDatabase();
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    String localeStr;
    locale->ToString(&localeStr);

    // try {
    database->BeginTransaction();
    Int32 size;
    if ((dataToDelete->GetSize(&size), size) > 0) {
        ProcessDataToDelete(database, localeStr, dataToDelete);
    }
    if ((dataToUpdate->GetSize(&size), size) > 0) {
        ProcessDataToUpdate(database, localeStr, dataToUpdate, nonIndexableKeys,
                forceUpdate);
    }
    database->SetTransactionSuccessful();
    // } finally {
    database->EndTransaction();
    // }

    *result = NULL;
    return NOERROR;
}

Boolean Index::UpdateIndexTask::ProcessDataToUpdate(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& localeStr,
    /* [in] */ IList* dataToUpdate, //List<SearchIndexableData>
    /* [in] */ IMap* nonIndexableKeys, //Map<String, List<String>>
    /* [in] */ Boolean forceUpdate)
{
    if (!forceUpdate && IsLocaleAlreadyIndexed(database, localeStr)) {
        Logger::D(Index::TAG, "Locale '%s' is already indexed", localeStr.string());
        return TRUE;
    }

    Boolean result = FALSE;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 current;
    sys->GetCurrentTimeMillis(&current);

    Int32 count;
    dataToUpdate->GetSize(&count);
    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IInterface> obj;
        dataToUpdate->Get(n, (IInterface**)&obj);
        ISearchIndexableData* data = ISearchIndexableData::Probe(obj);
        // try {
        mHost->IndexOneSearchIndexableData(database, localeStr, data, nonIndexableKeys);
        // } catch (Exception e) {
        //     Logger::E(TAG,
        //             "Cannot index: " + data.className + " for locale: " + localeStr, e);
        // }
    }

    Int64 now;
    sys->GetCurrentTimeMillis(&now);
    Logger::D(Index::TAG, "Indexing locale '%s' took %lld millis", localeStr.string(), now - current);
    return result;
}

Boolean Index::UpdateIndexTask::ProcessDataToDelete(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& localeStr,
    /* [in] */ IList* dataToDelete) // List<SearchIndexableData>
{
    Boolean result = FALSE;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 current;
    sys->GetCurrentTimeMillis(&current);

    Int32 count;
    dataToDelete->GetSize(&count);
    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IInterface> obj;
        dataToDelete->Get(n, (IInterface**)&obj);
        ISearchIndexableData* data = ISearchIndexableData::Probe(obj);
        if (data == NULL) {
            continue;
        }
        String className;
        data->GetClassName(&className);
        if (!TextUtils::IsEmpty(className)) {
            Delete(database, IIndexDatabaseHelperIndexColumns::CLASS_NAME, className);
        } else  {
            if (ISearchIndexableRaw::Probe(data) != NULL) {
                AutoPtr<SearchIndexableRaw> raw = (SearchIndexableRaw*)ISearchIndexableRaw::Probe(data);
                if (!TextUtils::IsEmpty(raw->mTitle)) {
                    Delete(database, IIndexDatabaseHelperIndexColumns::DATA_TITLE, raw->mTitle);
                }
            }
        }
    }

    Int64 now;
    sys->GetCurrentTimeMillis(&now);
    Logger::D(Index::TAG, "Deleting data for locale '%s' took %lld millis",
            localeStr.string(), now - current);
    return result;
}

Int32 Index::UpdateIndexTask::Delete(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& columName,
    /* [in] */ const String& value)
{
    String whereClause = columName + String("=?");
    AutoPtr< ArrayOf<String> > whereArgs = ArrayOf<String>::Alloc(1);
    (*whereArgs)[0] = value;

    Int32 result;
    database->Delete(IIndexDatabaseHelperTables::TABLE_PREFS_INDEX,
            whereClause, whereArgs, &result);
    return result;
}

Boolean Index::UpdateIndexTask::IsLocaleAlreadyIndexed(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& locale)
{
    AutoPtr<ICursor> cursor;
    Boolean result = FALSE;
    AutoPtr<StringBuilder> sb = new StringBuilder(IIndexDatabaseHelperIndexColumns::LOCALE);
    sb->Append(String(" = "));
    AutoPtr<IDatabaseUtils> utils;
    CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&utils);
    utils->AppendEscapedSQLString(sb, locale);
    // try {
        // We care only for 1 row
    database->Query(IIndexDatabaseHelperTables::TABLE_PREFS_INDEX, NULL,
            sb->ToString(), NULL, String(NULL), String(NULL), String(NULL),
            String("1"), (ICursor**)&cursor);
    Int32 count;
    cursor->GetCount(&count);
    result = (count >= 1);
    // } finally {
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    // }
    return result;
}

//===============================================================================
//                  Index::SaveSearchQueryTask::
//===============================================================================

Index::SaveSearchQueryTask::SaveSearchQueryTask(
    /* [in] */ Index* host)
    : mHost(host)
{}

Index::SaveSearchQueryTask::~SaveSearchQueryTask()
{}

ECode Index::SaveSearchQueryTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    Int64 now;
    date->GetTime(&now);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<ICharSequence> seq = ICharSequence::Probe((*params)[0]);
    values->Put(IIndexDatabaseHelperSavedQueriesColums::QUERY, seq);
    values->Put(IIndexDatabaseHelperSavedQueriesColums::TIME_STAMP, now);

    AutoPtr<ISQLiteDatabase> database = mHost->GetWritableDatabase();

    Int64 lastInsertedRowId = -1;
    // try {
    // First, delete all saved queries that are the same
    String str;
    seq->ToString(&str);
    AutoPtr< ArrayOf<String> > whereArgs = ArrayOf<String>::Alloc(1);
    (*whereArgs)[0] = str;

    Int32 value;
    database->Delete(IIndexDatabaseHelperTables::TABLE_SAVED_QUERIES,
            IIndexDatabaseHelperSavedQueriesColums::QUERY + String(" = ?"),
            whereArgs, &value);

    // Second, insert the saved query
    database->InsertOrThrow(IIndexDatabaseHelperTables::TABLE_SAVED_QUERIES,
            String(NULL), values, &lastInsertedRowId);

    // Last, remove "old" saved queries
    const Int64 delta = lastInsertedRowId - MAX_SAVED_SEARCH_QUERY;
    if (delta > 0) {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = StringUtils::ToString(delta);
        Int32 count;
        database->Delete(IIndexDatabaseHelperTables::TABLE_SAVED_QUERIES,
                String("rowId <= ?"), args, &count);
        Logger::D(Index::TAG, "Deleted '%d' saved Search Query(ies)", count);
    }
    // } catch (Exception e) {
    //     Logger::D(TAG, "Cannot update saved Search queries", e);
    // }
    AutoPtr<IInteger64> iobj = CoreUtils::Convert(lastInsertedRowId);
    *result = iobj.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===============================================================================
//                  Index::SaveSearchQueryTask
//===============================================================================

Index::Index(
    /* [in] */ IContext* context,
    /* [in] */ const String& baseAuthority)
{
    mContext = context;
    mBaseAuthority = baseAuthority;

    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mIsAvailable);
    mDataToProcess = new UpdateData();
}

Index::~Index()
{}

AutoPtr<Index> Index::GetInstance(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        sInstance = new Index(context, BASE_AUTHORITY);
    }
    else {
        sInstance->SetContext(context);
    }
    return sInstance;
}


ECode Index::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

Boolean Index::IsAvailable()
{
    Boolean res;
    mIsAvailable->Get(&res);
    return res;
}

AutoPtr<ICursor> Index::Search(
    /* [in] */ const String& query)
{
    AutoPtr<ISQLiteDatabase> database = GetReadableDatabase();
    AutoPtr< ArrayOf<ICursor*> > cursors = ArrayOf<ICursor*>::Alloc(2);

    String primarySql = BuildSearchSQL(query, MATCH_COLUMNS_PRIMARY, TRUE);
    Logger::D(TAG, "Search primary query: %s", primarySql.string());
    AutoPtr<ICursor> cursor;
    database->RawQuery(primarySql, NULL, (ICursor**)&cursor);
    cursors->Set(0, cursor);

    // We need to use an EXCEPT operator as negate MATCH queries do not work.
    AutoPtr<StringBuilder> sql = new StringBuilder(
            BuildSearchSQL(query, MATCH_COLUMNS_SECONDARY, FALSE));
    sql->Append(" EXCEPT ");
    sql->Append(primarySql);

    const String secondarySql = sql->ToString();
    Logger::D(TAG, "Search secondary query: %s", secondarySql.string());
    cursor = NULL;
    database->RawQuery(secondarySql, NULL, (ICursor**)&cursor);
    cursors->Set(1, cursor);

    AutoPtr<ICursor> result;
    CMergeCursor::New(cursors, (ICursor**)&result);
    return result;
}

AutoPtr<ICursor> Index::GetSuggestions(
    /* [in] */ const String& query)
{
    const String sql = BuildSuggestionsSQL(query);
    Logger::D(TAG, "Suggestions query: %s", sql.string());
    AutoPtr<ICursor> cursor;
    GetReadableDatabase()->RawQuery(sql, NULL, (ICursor**)&cursor);
    return cursor;
}

String Index::BuildSuggestionsSQL(
    /* [in] */ const String& query)
{
    StringBuilder sb;

    sb.Append("SELECT ");
    sb.Append(IIndexDatabaseHelperSavedQueriesColums::QUERY);
    sb.Append(" FROM ");
    sb.Append(IIndexDatabaseHelperTables::TABLE_SAVED_QUERIES);

    if (TextUtils::IsEmpty(query)) {
        sb.Append(" ORDER BY rowId DESC");
    }
    else {
        sb.Append(" WHERE ");
        sb.Append(IIndexDatabaseHelperSavedQueriesColums::QUERY);
        sb.Append(" LIKE ");
        sb.Append("'");
        sb.Append(query);
        sb.Append("%");
        sb.Append("'");
    }

    sb.Append(" LIMIT ");
    sb.Append(MAX_PROPOSED_SUGGESTIONS);

    return sb.ToString();
}

ECode Index::AddSavedQuery(
    /* [in] */ const String& query,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<SaveSearchQueryTask> task = new SaveSearchQueryTask(this);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(query));
    task->Execute(args);
    // try {
    AutoPtr<IInterface> obj;
    ECode ec = task->Get((IInterface**)&obj);
    if (FAILED(ec)) {
        Logger::E(TAG, "Cannot insert saved query: %s", query.string());
        *result = -1;
        return ec;
    }
    return IInteger64::Probe(obj)->GetValue(result);
    // } catch (InterruptedException e) {
    //     Logger::E(TAG, "Cannot insert saved query: " + query, e);
    //     return -1 ;
    // } catch (ExecutionException e) {
    //     Logger::E(TAG, "Cannot insert saved query: " + query, e);
    //     return -1;
    // }
}

ECode Index::Update()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchIndexablesContract::PROVIDER_INTERFACE, (IIntent**)&intent);
    AutoPtr<IPackageManager> manager;
    mContext->GetPackageManager((IPackageManager**)&manager);
    AutoPtr<IList> list;
    manager->QueryIntentContentProviders(intent, 0, (IList**)&list);

    Int32 size;
    list->GetSize(&size);
    for (Int32 n = 0; n < size; n++) {
        AutoPtr<IInterface> obj;
        list->Get(n, (IInterface**)&obj);
        IResolveInfo* info = IResolveInfo::Probe(obj);
        if (!IsWellKnownProvider(info)) {
            continue;
        }
        AutoPtr<IProviderInfo> providerInfo;
        info->GetProviderInfo((IProviderInfo**)&providerInfo);
        String authority;
        providerInfo->GetAuthority(&authority);
        String packageName;
        IPackageItemInfo::Probe(providerInfo)->GetPackageName(&packageName);

        AddIndexablesFromRemoteProvider(packageName, authority);
        AddNonIndexablesKeysFromRemoteProvider(packageName, authority);
    }

    UpdateInternal();
    return NOERROR;
}

Boolean Index::AddIndexablesFromRemoteProvider(
    /* [in] */ const String& packageName,
    /* [in] */ const String& authority)
{
    // try {
    const Int32 baseRank = Ranking::GetBaseRankForAuthority(authority);

    AutoPtr<IContext> context;
    if (mBaseAuthority.Equals(authority)) {
        context = mContext;
    }
    else {
        mContext->CreatePackageContext(packageName, 0, (IContext**)&context);
    }

    AutoPtr<IUri> uriForResources = BuildUriForXmlResources(authority);

    AutoPtr<ISearchIndexablesContract> contract;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&contract);
    AutoPtr< ArrayOf<String> > args1;
    contract->GetINDEXABLES_XML_RES_COLUMNS((ArrayOf<String>**)&args1);

    AutoPtr< ArrayOf<String> > args2;
    contract->GetINDEXABLES_RAW_COLUMNS((ArrayOf<String>**)&args2);

    AddIndexablesForXmlResourceUri(context, packageName, uriForResources,
            args1, baseRank);

    AutoPtr<IUri> uriForRawData = BuildUriForRawData(authority);
    AddIndexablesForRawDataUri(context, packageName, uriForRawData,
            args2, baseRank);
    return TRUE;
    // } catch (PackageManager.NameNotFoundException e) {
    //     Logger::W(TAG, "Could not create context for " + packageName + ": "
    //             + Log->GetStackTraceString(e));
    //     return FALSE;
    // }
}

void Index::AddNonIndexablesKeysFromRemoteProvider(
    /* [in] */ const String& packageName,
    /* [in] */ const String& authority)
{
    AutoPtr<IList> keys;
    GetNonIndexablesKeysFromRemoteProvider(packageName, authority, (IList**)&keys);
    AddNonIndexableKeys(packageName, keys);
}

ECode Index::GetNonIndexablesKeysFromRemoteProvider(
    /* [in] */ const String& packageName,
    /* [in] */ const String& authority,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    // try {
    AutoPtr<IContext> packageContext;
    mContext->CreatePackageContext(packageName, 0, (IContext**)&packageContext);

    AutoPtr<IUri> uriForNonIndexableKeys = BuildUriForNonIndexableKeys(authority);

    AutoPtr<ISearchIndexablesContract> contract;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&contract);
    AutoPtr< ArrayOf<String> > args;
    contract->GetNON_INDEXABLES_KEYS_COLUMNS((ArrayOf<String>**)&args);

    AutoPtr<IList> result;
    ECode ec = GetNonIndexablesKeys(packageContext, uriForNonIndexableKeys,
            args, (IList**)&result);
    if (SUCCEEDED(ec)) {
        *list = result;
        REFCOUNT_ADD(*list);
        return NOERROR;
    }
    Logger::W(TAG, "Could not create context for %s", packageName.string());
    *list = EMPTY_LIST;
    REFCOUNT_ADD(*list);
    return ec;
    // } catch (PackageManager.NameNotFoundException e) {
    //     Logger::W(TAG, "Could not create context for " + packageName + ": "
    //             + Log->GetStackTraceString(e));
    //     return EMPTY_LIST;
    // }
}

ECode Index::GetNonIndexablesKeys(
    /* [in] */ IContext* packageContext,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IContentResolver> resolver;
    packageContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, projection, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    if (cursor == NULL) {
        Logger::W(TAG, "Cannot add index data for Uri: %s", TO_CSTR(uri));
        *list = EMPTY_LIST;
        REFCOUNT_ADD(*list);
        return NOERROR;
    }

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    // try {
    Int32 count;
    cursor->GetCount(&count);
    if (count > 0) {
        Boolean res;
        while (cursor->MoveToNext(&res), res) {
            String key;
            cursor->GetString(ISearchIndexablesContract::COLUMN_INDEX_NON_INDEXABLE_KEYS_KEY_VALUE, &key);
            result->Add(CoreUtils::Convert(key));
        }
    }
    ICloseable::Probe(cursor)->Close();
    *list = result;
    REFCOUNT_ADD(*list);
    return NOERROR;
    // } finally {
    //    cursor->Close();
    // }
}

void Index::AddIndexableData(
    /* [in] */ ISearchIndexableData* data)
{
    {
        AutoLock syncLock(mDataToProcess);
        mDataToProcess->mDataToUpdate->Add(data);
    }
}

void Index::AddIndexableData(
    /* [in] */ ArrayOf<ISearchIndexableResource*>* array)
{
    {
        AutoLock syncLock(mDataToProcess);
        const Int32 count = array->GetLength();
        for (Int32 n = 0; n < count; n++) {
            mDataToProcess->mDataToUpdate->Add((*array)[n]);
        }
    }
}

void Index::DeleteIndexableData(
    /* [in] */ ISearchIndexableData* data)
{
    {
        AutoLock syncLock(mDataToProcess);
        mDataToProcess->mDataToDelete->Add(data);
    }
}

void Index::AddNonIndexableKeys(
    /* [in] */ const String& authority,
    /* [in] */ IList* keys) //List<String>
{
    {
        AutoLock syncLock(mDataToProcess);
        mDataToProcess->mNonIndexableKeys->Put(CoreUtils::Convert(authority), keys);
    }
}

Boolean Index::IsWellKnownProvider(
    /* [in] */ IResolveInfo* info)
{
    AutoPtr<IProviderInfo> providerInfo;
    info->GetProviderInfo((IProviderInfo**)&providerInfo);
    String authority;
    providerInfo->GetAuthority(&authority);
    String packageName;
    IPackageItemInfo::Probe(providerInfo)->GetPackageName(&packageName);

    if (TextUtils::IsEmpty(authority) || TextUtils::IsEmpty(packageName)) {
        return FALSE;
    }

    String readPermission;
    providerInfo->GetReadPermission(&readPermission);
    String writePermission;
    providerInfo->GetWritePermission(&writePermission);

    if (TextUtils::IsEmpty(readPermission) || TextUtils::IsEmpty(writePermission)) {
        return FALSE;
    }

    if (!Elastos::Droid::Manifest::permission::READ_SEARCH_INDEXABLES.Equals(readPermission) ||
        !Elastos::Droid::Manifest::permission::READ_SEARCH_INDEXABLES.Equals(writePermission)) {
        return FALSE;
    }

    Boolean res;
    IsPrivilegedPackage(packageName, &res);
    return res;
}

ECode Index::IsPrivilegedPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean* result)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    // try {
    AutoPtr<IPackageInfo> packInfo;
    ECode ec = pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&packInfo);
    if (FAILED(ec)) {
        *result = FALSE;
        return ec;
    }

    AutoPtr<IApplicationInfo> applicationInfo;
    packInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 flags;
    applicationInfo->GetFlags(&flags);
    *result = (flags & IApplicationInfo::FLAG_PRIVILEGED) != 0;
    return NOERROR;
    // } catch (PackageManager.NameNotFoundException e) {
    //     return FALSE;
    // }
}

void Index::UpdateFromRemoteProvider(
    /* [in] */ const String& packageName,
    /* [in] */ const String& authority)
{
    if (AddIndexablesFromRemoteProvider(packageName, authority)) {
        UpdateInternal();
    }
}

ECode Index::UpdateFromClassNameResource(
    /* [in] */ const String& className,
    /* [in] */ Boolean rebuild,
    /* [in] */ Boolean includeInSearchResults)
{
    if (className == NULL) {
        Logger::E(TAG, "class name cannot be NULL!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("class name cannot be NULL!");
    }
    AutoPtr<ISearchIndexableResource> _res = SearchIndexableResources::GetResourceByName(className);
    if (_res == NULL ) {
        Logger::E(TAG, "Cannot find SearchIndexableResources for class name: %s", className.string());
        return NOERROR;
    }
    ISearchIndexableData* res = ISearchIndexableData::Probe(_res);
    res->SetContext(mContext);
    res->SetEnabled(includeInSearchResults);
    if (rebuild) {
        DeleteIndexableData(res);
    }
    AddIndexableData(res);
    mDataToProcess->mForceUpdate = TRUE;
    UpdateInternal();
    res->SetEnabled(FALSE);
    return NOERROR;
}

ECode Index::UpdateFromSearchIndexableData(
    /* [in] */ ISearchIndexableData* data)
{
    AddIndexableData(data);
    mDataToProcess->mForceUpdate = TRUE;
    UpdateInternal();
    return NOERROR;
}

AutoPtr<ISQLiteDatabase> Index::GetReadableDatabase()
{
    AutoPtr<ISQLiteDatabase> database;
    IndexDatabaseHelper::GetInstance(mContext)->GetReadableDatabase((ISQLiteDatabase**)&database);
    return database;
}

AutoPtr<ISQLiteDatabase> Index::GetWritableDatabase()
{
    AutoPtr<ISQLiteDatabase> database;
    IndexDatabaseHelper::GetInstance(mContext)->GetWritableDatabase((ISQLiteDatabase**)&database);
    return database;
}

AutoPtr<IUri> Index::BuildUriForXmlResources(
    /* [in] */ const String& authority)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += "content://";
    builder += authority;
    builder += "/";

    AutoPtr<ISearchIndexablesContract> contract;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&contract);
    String path;
    contract->GetINDEXABLES_XML_RES_PATH(&path);

    builder += path;
    AutoPtr<IUri> uri;
    helper->Parse(builder.ToString(), (IUri**)&uri);
    return uri;
}

AutoPtr<IUri> Index::BuildUriForRawData(
    /* [in] */ const String& authority)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += "content://";
    builder += authority;
    builder += "/";

    AutoPtr<ISearchIndexablesContract> contract;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&contract);
    String path;
    contract->GetINDEXABLES_RAW_PATH(&path);

    builder += path;
    AutoPtr<IUri> uri;
    helper->Parse(builder.ToString(), (IUri**)&uri);
    return uri;
}

AutoPtr<IUri> Index::BuildUriForNonIndexableKeys(
    /* [in] */ const String& authority)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += "content://";
    builder += authority;
    builder += "/";

    AutoPtr<ISearchIndexablesContract> contract;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&contract);
    String path;
    contract->GetNON_INDEXABLES_KEYS_PATH(&path);

    builder += path;
    AutoPtr<IUri> uri;
    helper->Parse(builder.ToString(), (IUri**)&uri);
    return uri;
}

void Index::UpdateInternal()
{
    {
        AutoLock syncLock(mDataToProcess);
        AutoPtr<UpdateIndexTask> task = new UpdateIndexTask(this);
        AutoPtr<UpdateData> copy = mDataToProcess->Copy();

        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (IObject*)copy.Get());

        task->Execute(args);
        mDataToProcess->Clear();
    }
}

void Index::AddIndexablesForXmlResourceUri(
    /* [in] */ IContext* packageContext,
    /* [in] */ const String& packageName,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ Int32 baseRank)
{
    AutoPtr<IContentResolver> resolver;
    packageContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, projection, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    if (cursor == NULL) {
        Logger::W(TAG, "Cannot add index data for Uri: %s", TO_CSTR(uri));
        return;
    }

    // try {
    Int32 count;
    cursor->GetCount(&count);
    if (count > 0) {
        Boolean res;
        while (cursor->MoveToNext(&res), res) {
            Int32 providerRank;
            cursor->GetInt32(
                    ISearchIndexablesContract::COLUMN_INDEX_XML_RES_RANK,
                    &providerRank);
            Int32 rank = (providerRank > 0) ? baseRank + providerRank : baseRank;

            Int32 xmlResId;
            cursor->GetInt32(
                    ISearchIndexablesContract::COLUMN_INDEX_XML_RES_RESID,
                    &xmlResId);

            String className;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_XML_RES_CLASS_NAME,
                    &className);
            Int32 iconResId;
            cursor->GetInt32(
                    ISearchIndexablesContract::COLUMN_INDEX_XML_RES_ICON_RESID,
                    &iconResId);

            String action;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_ACTION,
                    &action);
            String targetPackage;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE,
                    &targetPackage);
            String targetClass;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS,
                    &targetClass);

            AutoPtr<ISearchIndexableResource> _sir;
            CSearchIndexableResource::New(packageContext, (ISearchIndexableResource**)&_sir);
            ISearchIndexableData* sir = ISearchIndexableData::Probe(_sir);
            sir->SetRank(rank);
            _sir->SetXmlResId(xmlResId);
            sir->SetClassName(className);
            sir->SetPackageName(packageName);
            sir->SetIconResId(iconResId);
            sir->SetIntentAction(action);
            sir->SetIntentTargetPackage(targetPackage);
            sir->SetIntentTargetClass(targetClass);

            AddIndexableData(sir);
        }
    }
    // } finally {
    ICloseable::Probe(cursor)->Close();
    // }
}

void Index::AddIndexablesForRawDataUri(
    /* [in] */ IContext* packageContext,
    /* [in] */ const String& packageName,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ Int32 baseRank)
{
    AutoPtr<IContentResolver> resolver;
    packageContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, projection, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    if (cursor == NULL) {
        Logger::W(TAG, "Cannot add index data for Uri: %s", TO_CSTR(uri));
        return;
    }

    // try {
    Int32 count;
    cursor->GetCount(&count);
    if (count > 0) {
        Boolean res;
        while (cursor->MoveToNext(&res), res) {
            Int32 providerRank;
            cursor->GetInt32(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_RANK, &providerRank);
            Int32 rank = (providerRank > 0) ? baseRank + providerRank : baseRank;

            String title;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_TITLE, &title);
            String summaryOn;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_SUMMARY_ON,
                    &summaryOn);
            String summaryOff;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_SUMMARY_OFF,
                    &summaryOff);
            String entries;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_ENTRIES, &entries);
            String keywords;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_KEYWORDS, &keywords);

            String screenTitle;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_SCREEN_TITLE,
                    &screenTitle);

            String className;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_CLASS_NAME,
                    &className);
            Int32 iconResId;
            cursor->GetInt32(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_ICON_RESID,
                    &iconResId);

            String action;
            cursor->GetString(ISearchIndexablesContract::COLUMN_INDEX_RAW_INTENT_ACTION,
                &action);
            String targetPackage;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_INTENT_TARGET_PACKAGE,
                    &targetPackage);
            String targetClass;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_INTENT_TARGET_CLASS,
                    &targetClass);

            String key;
            cursor->GetString(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_KEY, &key);
            Int32 userId;
            cursor->GetInt32(
                    ISearchIndexablesContract::COLUMN_INDEX_RAW_USER_ID, &userId);

            AutoPtr<SearchIndexableRaw> data = new SearchIndexableRaw();
            data->constructor(packageContext);
            data->mRank = rank;
            data->mTitle = title;
            data->mSummaryOn = summaryOn;
            data->mSummaryOff = summaryOff;
            data->mEntries = entries;
            data->mKeywords = keywords;
            data->mScreenTitle = screenTitle;
            data->mClassName = className;
            data->mPackageName = packageName;
            data->mIConResId = iconResId;
            data->mIntentAction = action;
            data->mIntentTargetPackage = targetPackage;
            data->mIntentTargetClass = targetClass;
            data->mKey = key;
            data->mUserId = userId;

            AddIndexableData((ISearchIndexableData*)data);
        }
    }
    // } finally {
    ICloseable::Probe(cursor)->Close();
    // }
}

String Index::BuildSearchSQL(
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* colums,
    /* [in] */ Boolean withOrderBy)
{
    StringBuilder sb;
    sb.Append(BuildSearchSQLForColumn(query, colums));
    if (withOrderBy) {
        sb.Append(" ORDER BY ");
        sb.Append(IIndexDatabaseHelperIndexColumns::DATA_RANK);
    }
    return sb.ToString();
}

String Index::BuildSearchSQLForColumn(
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* columnNames)
{
    StringBuilder sb;
    sb.Append("SELECT ");
    for (Int32 n = 0; n < SELECT_COLUMNS->GetLength(); n++) {
        sb.Append((*SELECT_COLUMNS)[n]);
        if (n < SELECT_COLUMNS->GetLength() - 1) {
            sb.Append(", ");
        }
    }
    sb.Append(" FROM ");
    sb.Append(IIndexDatabaseHelperTables::TABLE_PREFS_INDEX);
    sb.Append(" WHERE ");
    sb.Append(BuildSearchWhereStringForColumns(query, columnNames));

    return sb.ToString();
}

String Index::BuildSearchWhereStringForColumns(
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* columnNames)
{
    AutoPtr<StringBuilder> sb = new StringBuilder(IIndexDatabaseHelperTables::TABLE_PREFS_INDEX);
    sb->Append(" MATCH ");

    AutoPtr<IDatabaseUtils> utils;
    CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&utils);
    utils->AppendEscapedSQLString(sb, BuildSearchMatchStringForColumns(query, columnNames));

    sb->Append(" AND ");
    sb->Append(IIndexDatabaseHelperIndexColumns::LOCALE);
    sb->Append(" = ");

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    String localeStr;
    locale->ToString(&localeStr);

    utils->AppendEscapedSQLString(sb, localeStr);
    sb->Append(" AND ");
    sb->Append(IIndexDatabaseHelperIndexColumns::ENABLED);
    sb->Append(" = 1");
    return sb->ToString();
}

String Index::BuildSearchMatchStringForColumns(
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* columnNames)
{
    String value = query + String("*");
    StringBuilder sb;
    const Int32 count = columnNames->GetLength();
    for (Int32 n = 0; n < count; n++) {
        sb.Append((*columnNames)[n]);
        sb.Append(":");
        sb.Append(value);
        if (n < count - 1) {
            sb.Append(" OR ");
        }
    }
    return sb.ToString();
}

void Index::IndexOneSearchIndexableData(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& localeStr,
    /* [in] */ ISearchIndexableData* data,
    /* [in] */ IMap* nonIndexableKeys) //Map<String, List<String>>
{
    ISearchIndexableResource* resData = ISearchIndexableResource::Probe(data);
    if (resData != NULL) {
        IndexOneResource(database, localeStr, resData, nonIndexableKeys);
    }
    else if (ISearchIndexableRaw::Probe(data) != NULL) {
        IndexOneRaw(database, localeStr, ISearchIndexableRaw::Probe(data));
    }
}

void Index::IndexOneRaw(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& localeStr,
    /* [in] */ ISearchIndexableRaw* raw)
{
    // Should be the same locale as the one we are processing
    AutoPtr<SearchIndexableRaw> siRaw = (SearchIndexableRaw*)raw;
    String str;
    siRaw->mLocale->ToString(&str);
    if (!str.EqualsIgnoreCase(localeStr)) {
        return;
    }

    UpdateOneRowWithFilteredData(database, localeStr,
            siRaw->mTitle,
            siRaw->mSummaryOn,
            siRaw->mSummaryOff,
            siRaw->mEntries,
            siRaw->mClassName,
            siRaw->mScreenTitle,
            siRaw->mIConResId,
            siRaw->mRank,
            siRaw->mKeywords,
            siRaw->mIntentAction,
            siRaw->mIntentTargetPackage,
            siRaw->mIntentTargetClass,
            siRaw->mEnabled,
            siRaw->mKey,
            siRaw->mUserId);
}

Boolean Index::IsIndexableClass(
    /* [in] */ const String& className)
{
    if (className.Equals("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings")
            || className.Equals("Elastos.Droid.Settings.Notification.CNotificationSettings")
            || className.Equals("Elastos.Droid.Settings.Notification.COtherSoundSettings")
            || className.Equals("Elastos.Droid.Settings.Notification.CZenModeSettings")
            || className.Equals("Elastos.Droid.Settings.Wifi.CSavedAccessPointsWifiSettings")
            || className.Equals("Elastos.Droid.Settings.Wifi.CWifiSettings")
            || className.Equals("Elastos.Droid.Settings.CSecuritySettings")
            || className.Equals("Elastos.Droid.Settings.CWirelessSettings")) {
        return TRUE;
    }
    // TODO
    // else if....  other  class implements Indexable
    return FALSE;
}

void Index::IndexOneResource(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& localeStr,
    /* [in] */ ISearchIndexableResource* _sir,
    /* [in] */ IMap* nonIndexableKeysFromResource) //Map<String, List<String>>
{
    if (_sir == NULL) {
        Logger::E(TAG, "Cannot index a NULL resource!");
        return;
    }

    AutoPtr<IList> nonIndexableKeys;
    CArrayList::New((IList**)&nonIndexableKeys);

    String packageName, className;
    Int32 xmlResId, iconResId, rank;
    _sir->GetXmlResId(&xmlResId);
    AutoPtr<ISearchIndexableData> sir = ISearchIndexableData::Probe(_sir);
    AutoPtr<IContext> context;
    sir->GetContext((IContext**)&context);
    sir->GetPackageName(&packageName);
    sir->GetClassName(&className);
    sir->GetIconResId(&iconResId);
    sir->GetRank(&rank);

    // Logger::I(TAG, " >> IndexOneResource: localeStr: %s, packageName: %s, className: %s, xmlResId: %08x, iconResId: %08x",
    //     localeStr.string(), packageName.string(), className.string(), xmlResId, iconResId);

    if (xmlResId > SearchIndexableResources::NO_DATA_RES_ID) {
        AutoPtr<IInterface> obj;
        nonIndexableKeysFromResource->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        IList* resNonIndxableKeys = IList::Probe(obj);
        Int32 size;
        if (resNonIndxableKeys != NULL && (resNonIndxableKeys->GetSize(&size), size) > 0) {
            nonIndexableKeys->AddAll(ICollection::Probe(resNonIndxableKeys));
        }

        String intentAction;
        sir->GetIntentAction(&intentAction);
        String intentTargetPackage;
        sir->GetIntentTargetPackage(&intentTargetPackage);
        String intentTargetClass;
        sir->GetIntentTargetClass(&intentTargetClass);

        IndexFromResource(context, database, localeStr,
            xmlResId, className, iconResId, rank,
            intentAction, intentTargetPackage, intentTargetClass,
            nonIndexableKeys);
    }
    else {
        if (TextUtils::IsEmpty(className)) {
            Logger::W(TAG, "Cannot index an empty Search Provider name!");
            return;
        }

        if (!IsIndexableClass(className)) {
            // TODO, other class implements Indexable
            Logger::D(TAG, "other class implements Indexable is TODO");
            Logger::D(TAG, "SearchIndexableResource '%s' should implement the IIndexable interface!",
                    className.string());
            return;
        }

        // Will be non null only for a Local provider implementing a
        // SEARCH_INDEX_DATA_PROVIDER field
        AutoPtr<IIndexableSearchIndexProvider> provider = GetSearchIndexProvider(className);
        if (provider != NULL) {
            AutoPtr<IList> providerNonIndexableKeys;
            provider->GetNonIndexableKeys(context, (IList**)&providerNonIndexableKeys);
            Int32 size;
            if (providerNonIndexableKeys != NULL && (providerNonIndexableKeys->GetSize(&size), size) > 0) {
                nonIndexableKeys->AddAll(ICollection::Probe(providerNonIndexableKeys));
            }

            Boolean enabled;
            sir->GetEnabled(&enabled);

            IndexFromProvider(mContext, database, localeStr, provider, className,
                iconResId, rank, enabled, nonIndexableKeys);
        }
    }
}

AutoPtr<IIndexableSearchIndexProvider> Index::GetSearchIndexProvider(const String& className)
{
    static HashMap<String, AutoPtr<IIndexableSearchIndexProvider> > providerMap;
    if (providerMap.IsEmpty()) {
        providerMap[String("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings")]
            = CInputMethodAndLanguageSettings::GetSEARCH_INDEX_DATA_PROVIDER();
        providerMap[String("Elastos.Droid.Settings.Notification.CNotificationSettings")]
            = CNotificationSettings::GetSEARCH_INDEX_DATA_PROVIDER();
        providerMap[String("Elastos.Droid.Settings.Notification.COtherSoundSettings")]
            = COtherSoundSettings::GetSEARCH_INDEX_DATA_PROVIDER();
        providerMap[String("Elastos.Droid.Settings.Notification.CZenModeSettings")]
            = CZenModeSettings::GetSEARCH_INDEX_DATA_PROVIDER();
        providerMap[String("Elastos.Droid.Settings.Wifi.CWifiSettings")]
            = CWifiSettings::GetSEARCH_INDEX_DATA_PROVIDER();
        providerMap[String("Elastos.Droid.Settings.Wifi.CSavedAccessPointsWifiSettings")]
            = CSavedAccessPointsWifiSettings::GetSEARCH_INDEX_DATA_PROVIDER();
        providerMap[String("Elastos.Droid.Settings.CWirelessSettings")]
            = CWirelessSettings::GetSEARCH_INDEX_DATA_PROVIDER();
        providerMap[String("Elastos.Droid.Settings.CSecuritySettings")]
            = CSecuritySettings::GetSEARCH_INDEX_DATA_PROVIDER();
    }

    HashMap<String, AutoPtr<IIndexableSearchIndexProvider> >::Iterator it = providerMap.Find(className);
    if (it != providerMap.End()) {
        return it->mSecond;
    }

    // TODO
    // other  class implements Indexable
    Logger::W(TAG, " >> TODO: other class [%s] implements Indexable", className.string());
    return NULL;
}

ECode Index::IndexFromResource(
    /* [in] */ IContext* context,
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& localeStr,
    /* [in] */ Int32 xmlResId,
    /* [in] */ const String& fragmentName,
    /* [in] */ Int32 iconResId,
    /* [in] */ Int32 rank,
    /* [in] */ const String& intentAction,
    /* [in] */ const String& intentTargetPackage,
    /* [in] */ const String& intentTargetClass,
    /* [in] */ IList* nonIndexableKeys) //List<String>
{
    AutoPtr<IXmlResourceParser> parser;
    // try {
    AutoPtr<IResources> resource;
    context->GetResources((IResources**)&resource);
    resource->GetXml(xmlResId, (IXmlResourceParser**)&parser);

    IXmlPullParser* pullParser = IXmlPullParser::Probe(parser);

    Int32 type;
    while ((pullParser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
        // Parse next until start tag is found
    }

    String nodeName;
    pullParser->GetName(&nodeName);
    if (!NODE_NAME_PREFERENCE_SCREEN.Equals(nodeName)) {
        String pos;
        pullParser->GetPositionDescription(&pos);
        Logger::E(TAG, "XML document must start with <PreferenceScreen> tag; found %s at %s",
                nodeName.string(), pos.string());
        if (parser != NULL) ICloseable::Probe(parser)->Close();
        return E_RUNTIME_EXCEPTION;
        // throw new RuntimeException(
        //         "XML document must start with <PreferenceScreen> tag; found"
        //                 + nodeName + " at " + parser->GetPositionDescription());
    }

    Int32 outerDepth;
    pullParser->GetDepth(&outerDepth);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(pullParser);

    String screenTitle = GetDataTitle(context, attrs);

    String key = GetDataKey(context, attrs);

    String title;
    String summary;
    String keywords;

    // Insert rows for the main PreferenceScreen node. Rewrite the data for removing
    // hyphens.
    Boolean res;
    if (nonIndexableKeys->Contains(CoreUtils::Convert(key), &res), !res) {
        title = GetDataTitle(context, attrs);
        summary = GetDataSummary(context, attrs);
        keywords = GetDataKeywords(context, attrs);

        UpdateOneRowWithFilteredData(database, localeStr, title, summary,
                String(NULL), String(NULL),
                fragmentName, screenTitle, iconResId, rank,
                keywords, intentAction, intentTargetPackage, intentTargetClass, TRUE,
                key, -1 /* default user id */);
    }

    Int32 depth;
    while ((pullParser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (pullParser->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        pullParser->GetName(&nodeName);

        key = GetDataKey(context, attrs);
        Boolean result;
        if (nonIndexableKeys->Contains(CoreUtils::Convert(key), &result), result) {
            continue;
        }

        title = GetDataTitle(context, attrs);
        keywords = GetDataKeywords(context, attrs);

        if (!nodeName.Equals(NODE_NAME_CHECK_BOX_PREFERENCE)) {
            summary = GetDataSummary(context, attrs);

            String entries;

            if (nodeName.EndWith(NODE_NAME_LIST_PREFERENCE)) {
                entries = GetDataEntries(context, attrs);
            }

            // Insert rows for the child nodes of PreferenceScreen
            UpdateOneRowWithFilteredData(database, localeStr, title, summary,
                    String(NULL), entries,
                    fragmentName, screenTitle, iconResId, rank,
                    keywords, intentAction, intentTargetPackage, intentTargetClass,
                    TRUE, key, -1 /* default user id */);
        }
        else {
            String summaryOn = GetDataSummaryOn(context, attrs);
            String summaryOff = GetDataSummaryOff(context, attrs);

            if (TextUtils::IsEmpty(summaryOn) && TextUtils::IsEmpty(summaryOff)) {
                summaryOn = GetDataSummary(context, attrs);
            }

            UpdateOneRowWithFilteredData(database, localeStr, title, summaryOn,
                    summaryOff, String(NULL), fragmentName, screenTitle, iconResId, rank,
                    keywords, intentAction, intentTargetPackage, intentTargetClass,
                    TRUE, key, -1 /* default user id */);
        }
    }

    // } catch (XmlPullParserException e) {
    //     throw new RuntimeException("Error parsing PreferenceScreen", e);
    // } catch (IOException e) {
    //     throw new RuntimeException("Error parsing PreferenceScreen", e);
    // } finally {
    if (parser != NULL) ICloseable::Probe(parser)->Close();
    // }
    return NOERROR;
}

void Index::IndexFromProvider(
    /* [in] */ IContext* context,
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& localeStr,
    /* [in] */ IIndexableSearchIndexProvider* provider,
    /* [in] */ const String& className,
    /* [in] */ Int32 iconResId,
    /* [in] */ Int32 rank,
    /* [in] */ Boolean enabled,
    /* [in] */ IList* nonIndexableKeys) //List<String>
{
    if (provider == NULL) {
        Logger::W(TAG, "Cannot find provider: %s", className.string());
        return;
    }

    AutoPtr<IList> rawList;
    provider->GetRawDataToIndex(context, enabled, (IList**)&rawList);

    if (rawList != NULL) {
        Int32 rawSize;
        rawList->GetSize(&rawSize);
        for (Int32 i = 0; i < rawSize; i++) {
            AutoPtr<IInterface> obj;
            rawList->Get(i, (IInterface**)&obj);
            SearchIndexableRaw* raw = (SearchIndexableRaw*)ISearchIndexableRaw::Probe(obj);

            // Should be the same locale as the one we are processing
            String str;
            if (!(raw->mLocale->ToString(&str), str).EqualsIgnoreCase(localeStr)) {
                continue;
            }

            Boolean res;
            if (nonIndexableKeys->Contains(CoreUtils::Convert(raw->mKey), &res), res) {
                continue;
            }

            UpdateOneRowWithFilteredData(database, localeStr,
                    raw->mTitle,
                    raw->mSummaryOn,
                    raw->mSummaryOff,
                    raw->mEntries,
                    className,
                    raw->mScreenTitle,
                    iconResId,
                    rank,
                    raw->mKeywords,
                    raw->mIntentAction,
                    raw->mIntentTargetPackage,
                    raw->mIntentTargetClass,
                    raw->mEnabled,
                    raw->mKey,
                    raw->mUserId);
        }
    }

    AutoPtr<IList> resList;
    provider->GetXmlResourcesToIndex(context, enabled, (IList**)&resList);
    if (resList != NULL) {
        Int32 resSize;
        resList->GetSize(&resSize);
        for (Int32 i = 0; i < resSize; i++) {
            AutoPtr<IInterface> obj;
            resList->Get(i, (IInterface**)&obj);
            ISearchIndexableResource* item = ISearchIndexableResource::Probe(obj);

            // Should be the same locale as the one we are processing
            String str;
            AutoPtr<SearchIndexableData> siData = (SearchIndexableData*)(ISearchIndexableData::Probe(item));
            if (!(siData->mLocale->ToString(&str), str).EqualsIgnoreCase(localeStr)) {
                continue;
            }

            const Int32 itemIconResId = (siData->mIConResId == 0) ? iconResId : siData->mIConResId;
            const Int32 itemRank = (siData->mRank == 0) ? rank : siData->mRank;
            String itemClassName = (TextUtils::IsEmpty(siData->mClassName))
                    ? className : siData->mClassName;

            Int32 xmlResId;
            item->GetXmlResId(&xmlResId);

            if (xmlResId != 0) {
                IndexFromResource(context, database, localeStr,
                    xmlResId, itemClassName, itemIconResId, itemRank,
                    siData->mIntentAction, siData->mIntentTargetPackage,
                    siData->mIntentTargetClass, nonIndexableKeys);
            }
            else {
                Logger::W(TAG, " >> IndexFromProvider: %s, xmlResId:%08x", itemClassName.string(), xmlResId);
            }
        }
    }
}

void Index::UpdateOneRowWithFilteredData(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& locale,
    /* [in] */ const String& title,
    /* [in] */ const String& summaryOn,
    /* [in] */ const String& summaryOff,
    /* [in] */ const String& entries,
    /* [in] */ const String& className,
    /* [in] */ const String& screenTitle,
    /* [in] */ Int32 iconResId,
    /* [in] */ Int32 rank,
    /* [in] */ const String& keywords,
    /* [in] */ const String& intentAction,
    /* [in] */ const String& intentTargetPackage,
    /* [in] */ const String& intentTargetClass,
    /* [in] */ Boolean enabled,
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    const String updatedTitle = NormalizeHyphen(title);
    const String updatedSummaryOn = NormalizeHyphen(summaryOn);
    const String updatedSummaryOff = NormalizeHyphen(summaryOff);

    const String normalizedTitle = NormalizeString(updatedTitle);
    const String normalizedSummaryOn = NormalizeString(updatedSummaryOn);
    const String normalizedSummaryOff = NormalizeString(updatedSummaryOff);

    UpdateOneRow(database, locale,
            updatedTitle, normalizedTitle, updatedSummaryOn, normalizedSummaryOn,
            updatedSummaryOff, normalizedSummaryOff, entries,
            className, screenTitle, iconResId,
            rank, keywords, intentAction, intentTargetPackage, intentTargetClass, enabled,
            key, userId);
}

String Index::NormalizeHyphen(
    /* [in] */ const String& input)
{
    if (!input.IsNull()) {
        String str;
        StringUtils::ReplaceAll(input, NON_BREAKING_HYPHEN, HYPHEN, &str);
        return str;
    }
    return EMPTY;
}

String Index::NormalizeString(
    /* [in] */ const String& input)
{
    String nohyphen = EMPTY;
    if (!input.IsNull()) {
        StringUtils::ReplaceAll(input, HYPHEN, EMPTY, &nohyphen);
    }

    AutoPtr<INormalizer> helper;
    CNormalizerHelper::AcquireSingleton((INormalizer**)&helper);
    String normalized;
    helper->Normalize(CoreUtils::Convert(nohyphen), NormalizerForm_NFD, &normalized);

    AutoPtr<IMatcher> matcher;
    REMOVE_DIACRITICALS_PATTERN->Matcher(normalized, (IMatcher**)&matcher);
    String result;
    matcher->ReplaceAll(String(""), &result);
    return result.ToLowerCase();
}

void Index::UpdateOneRow(
    /* [in] */ ISQLiteDatabase* database,
    /* [in] */ const String& locale,
    /* [in] */ const String& updatedTitle,
    /* [in] */ const String& normalizedTitle,
    /* [in] */ const String& updatedSummaryOn,
    /* [in] */ const String& normalizedSummaryOn,
    /* [in] */ const String& updatedSummaryOff,
    /* [in] */ const String& normalizedSummaryOff,
    /* [in] */ const String& entries,
    /* [in] */ const String& className,
    /* [in] */ const String& screenTitle,
    /* [in] */ Int32 iconResId,
    /* [in] */ Int32 rank,
    /* [in] */ const String& keywords,
    /* [in] */ const String& intentAction,
    /* [in] */ const String& intentTargetPackage,
    /* [in] */ const String& intentTargetClass,
    /* [in] */ Boolean enabled,
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    if (TextUtils::IsEmpty(updatedTitle)) {
        return;
    }

    // The DocID should contains more than the title string itself (you may have two settings
    // with the same title). So we need to use a combination of the title and the screenTitle.
    AutoPtr<StringBuilder> sb = new StringBuilder(updatedTitle);
    sb->Append(screenTitle);
    String str;
    sb->ToString(&str);
    Int32 docId = str.GetHashCode();

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IIndexDatabaseHelperIndexColumns::DOCID, docId);
    values->Put(IIndexDatabaseHelperIndexColumns::LOCALE, locale);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_RANK, rank);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_TITLE, updatedTitle);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_TITLE_NORMALIZED, normalizedTitle);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_ON, updatedSummaryOn);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_ON_NORMALIZED, normalizedSummaryOn);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_OFF, updatedSummaryOff);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_OFF_NORMALIZED, normalizedSummaryOff);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_ENTRIES, entries);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_KEYWORDS, keywords);
    values->Put(IIndexDatabaseHelperIndexColumns::CLASS_NAME, className);
    values->Put(IIndexDatabaseHelperIndexColumns::SCREEN_TITLE, screenTitle);
    values->Put(IIndexDatabaseHelperIndexColumns::INTENT_ACTION, intentAction);
    values->Put(IIndexDatabaseHelperIndexColumns::INTENT_TARGET_PACKAGE, intentTargetPackage);
    values->Put(IIndexDatabaseHelperIndexColumns::INTENT_TARGET_CLASS, intentTargetClass);
    values->Put(IIndexDatabaseHelperIndexColumns::ICON, iconResId);
    values->Put(IIndexDatabaseHelperIndexColumns::ENABLED, enabled);
    values->Put(IIndexDatabaseHelperIndexColumns::DATA_KEY_REF, key);
    values->Put(IIndexDatabaseHelperIndexColumns::USER_ID, userId);

    Int64 result;
    database->ReplaceOrThrow(IIndexDatabaseHelperTables::TABLE_PREFS_INDEX, String(NULL), values, &result);
}

String Index::GetDataKey(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    using Elastos::Droid::R;
    AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Preference),
            ArraySize(R::styleable::Preference));
    return GetData(context, attrs, attrsId, R::styleable::Preference_key);
}

String Index::GetDataTitle(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    using Elastos::Droid::R;
    AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Preference),
            ArraySize(R::styleable::Preference));
    return GetData(context, attrs, attrsId, R::styleable::Preference_title);
}

String Index::GetDataSummary(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    using Elastos::Droid::R;
    AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Preference),
            ArraySize(R::styleable::Preference));
    return GetData(context, attrs, attrsId, R::styleable::Preference_summary);
}

String Index::GetDataSummaryOn(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    using Elastos::Droid::R;
    AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::CheckBoxPreference),
            ArraySize(R::styleable::CheckBoxPreference));
    return GetData(context, attrs, attrsId, R::styleable::CheckBoxPreference_summaryOn);
}

String Index::GetDataSummaryOff(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    using Elastos::Droid::R;
    AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::CheckBoxPreference),
            ArraySize(R::styleable::CheckBoxPreference));
    return GetData(context, attrs, attrsId, R::styleable::CheckBoxPreference_summaryOff);
}

String Index::GetDataEntries(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    using Elastos::Droid::R;
    AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ListPreference),
            ArraySize(R::styleable::ListPreference));
    return GetDataEntries(context, attrs, attrsId, R::styleable::ListPreference_entries);
}

String Index::GetDataKeywords(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Preference),
            ArraySize(R::styleable::Preference));
    return GetData(context, attrs, attrsId, R::styleable::Preference_keywords);
}

String Index::GetData(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 resId)
{
    AutoPtr<ITypedArray> sa;
    context->ObtainStyledAttributes(set, attrs, (ITypedArray**)&sa);
    AutoPtr<ITypedValue> tv;
    sa->PeekValue(resId, (ITypedValue**)&tv);

    AutoPtr<ICharSequence> data;
    Int32 type;
    if (tv != NULL && (tv->GetType(&type), type) == ITypedValue::TYPE_STRING) {
        Int32 resourceId;
        tv->GetResourceId(&resourceId);
        if (resourceId != 0) {
            context->GetText(resourceId, (ICharSequence**)&data);
        }
        else {
            tv->GetString((ICharSequence**)&data);
        }
    }
    String str;
    return (data != NULL) ? (data->ToString(&str), str) : String(NULL);
}

String Index::GetDataEntries(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 resId)
{
    AutoPtr<ITypedArray> sa;
    context->ObtainStyledAttributes(set, attrs, (ITypedArray**)&sa);
    AutoPtr<ITypedValue> tv;
    sa->PeekValue(resId, (ITypedValue**)&tv);

    AutoPtr< ArrayOf<String> > data;
    Int32 type;
    if (tv != NULL && (tv->GetType(&type), type) == ITypedValue::TYPE_REFERENCE) {
        Int32 resourceId;
        tv->GetResourceId(&resourceId);
        if (resourceId != 0) {
            AutoPtr<IResources> resource;
            context->GetResources((IResources**)&resource);
            resource->GetStringArray(resourceId, (ArrayOf<String>**)&data);
        }
    }
    Int32 count = (data == NULL ) ? 0 : data->GetLength();
    if (count == 0) {
        return String(NULL);
    }
    StringBuilder result;
    for (Int32 n = 0; n < count; n++) {
        result.Append((*data)[n]);
        result.Append(ENTRIES_SEPARATOR);
    }
    return result.ToString();
}

Int32 Index::GetResId(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 resId)
{
    AutoPtr<ITypedArray> sa;
    context->ObtainStyledAttributes(set, attrs, (ITypedArray**)&sa);
    AutoPtr<ITypedValue> tv;
    sa->PeekValue(resId, (ITypedValue**)&tv);

    Int32 type;
    if (tv != NULL && (tv->GetType(&type), type) == ITypedValue::TYPE_STRING) {
        Int32 resId;
        tv->GetResourceId(&resId);
        return resId;
    }
    else {
        return 0;
    }
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos
