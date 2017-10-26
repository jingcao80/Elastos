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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Sql.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/providers/downloads/CDownloadProvider.h"
#include "elastos/droid/providers/downloads/RealSystemFacade.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/providers/downloads/CHelpers.h"
#include "elastos/droid/providers/downloads/CDownloadStorageProvider.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CDownloadManager;
using Elastos::Droid::App::IDownloadManagerRequest;
using Elastos::Droid::App::IDownloadManagerHelper;
using Elastos::Droid::App::CDownloadManagerHelper;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::EIID_IParcelFileDescriptorOnCloseListener;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Provider::IDownloadsImplRequestHeaders;
using Elastos::Droid::Provider::IOpenableColumns;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Droid::Manifest;
using Elastos::Core::CString;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IWriter;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

//===============================================================
// CDownloadProvider::
//===============================================================
String CDownloadProvider::DB_NAME("downloads.db");

Int32 CDownloadProvider::DB_VERSION = 109;

String CDownloadProvider::DB_TABLE("downloads");

String CDownloadProvider::DOWNLOAD_LIST_TYPE("vnd.android.cursor.dir/download");

String CDownloadProvider::DOWNLOAD_TYPE("vnd.android.cursor.item/download");

const Int32 CDownloadProvider::MY_DOWNLOADS = 1;

const Int32 CDownloadProvider::MY_DOWNLOADS_ID = 2;

const Int32 CDownloadProvider::ALL_DOWNLOADS = 3;

const Int32 CDownloadProvider::ALL_DOWNLOADS_ID = 4;

Int32 CDownloadProvider::REQUEST_HEADERS_URI = 5;

const Int32 CDownloadProvider::PUBLIC_DOWNLOAD_ID = 6;

static AutoPtr<IUriMatcher> InitMatcher()
{
    AutoPtr<IUriMatcher> p;
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&p);

    p->AddURI(String("downloads"), String("my_downloads"), CDownloadProvider::MY_DOWNLOADS);
    p->AddURI(String("downloads"), String("my_downloads/#"), CDownloadProvider::MY_DOWNLOADS_ID);
    p->AddURI(String("downloads"), String("all_downloads"), CDownloadProvider::ALL_DOWNLOADS);
    p->AddURI(String("downloads"), String("all_downloads/#"), CDownloadProvider::ALL_DOWNLOADS_ID);
    p->AddURI(String("downloads"),
            String("my_downloads/#/") + IDownloadsImplRequestHeaders::URI_SEGMENT,
            CDownloadProvider::REQUEST_HEADERS_URI);
    p->AddURI(String("downloads"),
            String("all_downloads/#/") + IDownloadsImplRequestHeaders::URI_SEGMENT,
            CDownloadProvider::REQUEST_HEADERS_URI);
    // temporary, for backwards compatibility
    p->AddURI(String("downloads"), String("download"), CDownloadProvider::MY_DOWNLOADS);
    p->AddURI(String("downloads"), String("download/#"), CDownloadProvider::MY_DOWNLOADS_ID);
    p->AddURI(String("downloads"),
            String("download/#/") + IDownloadsImplRequestHeaders::URI_SEGMENT,
            CDownloadProvider::REQUEST_HEADERS_URI);
    p->AddURI(String("downloads"),
            IDownloadsImpl::PUBLICLY_ACCESSIBLE_DOWNLOADS_URI_SEGMENT + String("/#"),
            CDownloadProvider::PUBLIC_DOWNLOAD_ID);

    return p;
}

AutoPtr<IUriMatcher> CDownloadProvider::sURIMatcher = InitMatcher();

AutoPtr<ArrayOf<IUri*> > CDownloadProvider::BASE_URIS = ArrayOf<IUri*>::Alloc(2);

AutoPtr<ArrayOf<String> > CDownloadProvider::sAppReadableColumnsArray = ArrayOf<String>::Alloc(22);

static AutoPtr<IHashSet> InitSet()
{
    AutoPtr<IHashSet> p;
    CHashSet::New((IHashSet**)&p);
    for (Int32 i = 0; i < CDownloadProvider::sAppReadableColumnsArray->GetLength(); ++i) {
        AutoPtr<ICharSequence> pContent;
        CString::New((*CDownloadProvider::sAppReadableColumnsArray)[i], (ICharSequence**)&pContent);
        p->Add(pContent);
    }
    return p;
}

AutoPtr<IHashSet> CDownloadProvider::sAppReadableColumnsSet = InitSet();

static AutoPtr<IHashMap> InitMap()
{
    AutoPtr<IHashMap> mp;
    CHashMap::New((IHashMap**)&mp);
    AutoPtr<ICharSequence> pDN;
    CString::New(IOpenableColumns::DISPLAY_NAME, (ICharSequence**)&pDN);
    AutoPtr<ICharSequence> pCT;
    CString::New(IDownloadsImpl::COLUMN_TITLE + " AS " + IOpenableColumns::DISPLAY_NAME, (ICharSequence**)&pCT);
    mp->Put(pDN, pCT);
    AutoPtr<ICharSequence> pCS;
    CString::New(IOpenableColumns::SIZE, (ICharSequence**)&pCS);
    AutoPtr<ICharSequence> pCTB;
    CString::New(IDownloadsImpl::COLUMN_TOTAL_BYTES + " AS " + IOpenableColumns::SIZE, (ICharSequence**)&pCTB);
    mp->Put(pCS, pCTB);
    return mp;
}

AutoPtr<IHashMap> CDownloadProvider::sColumnsMap = InitMap();

static AutoPtr<IList> InitList()
{
    AutoPtr<IList> p;
    AutoPtr<IDownloadManagerHelper> hlp;
    CDownloadManagerHelper::AcquireSingleton((IDownloadManagerHelper**)&hlp);
    AutoPtr<ArrayOf<String> > arr;
    hlp->GetUNDERLYINGCOLUMNS((ArrayOf<String>**)&arr);
    Arrays::AsList(arr, (IList**)&p);
    return p;
}

AutoPtr<IList> CDownloadProvider::downloadManagerColumnsList = InitList();

//===============================================================
// CDownloadProvider::SqlSelection::
//===============================================================

CDownloadProvider::SqlSelection::SqlSelection()
{
    CArrayList::New((IList**)&mParameters);
}

void CDownloadProvider::SqlSelection::AppendClause(
    /* [in] */ String newClause,
    /* [in] */ ArrayOf<String>* parameters)
{
    if (newClause == NULL || newClause.IsEmpty()) {
        return;
    }
    if (mWhereClause.GetLength() != 0) {
        mWhereClause.Append(" AND ");
    }
    mWhereClause.Append("(");
    mWhereClause.Append(newClause);
    mWhereClause.Append(")");
    if (parameters != NULL) {
        for (Int32 i = 0;i < parameters->GetLength(); i++) {
            String parameter = (*parameters)[i];
            AutoPtr<ICharSequence> pPram;
            CString::New(parameter, (ICharSequence**)&pPram);
            mParameters->Add(pPram);
        }
    }
}

String CDownloadProvider::SqlSelection::GetSelection()
{
    return mWhereClause.ToString();
}

AutoPtr<ArrayOf<String> > CDownloadProvider::SqlSelection::GetParameters()
{
    Int32 size = 0;
    mParameters->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > res;
    mParameters->ToArray(inArray, (ArrayOf<IInterface*>**)&res);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; i++)
        (*array)[i] = TO_STR((*res)[i]);
    return array;
}

//===============================================================
// CDownloadProvider::DatabaseHelper::
//===============================================================

ECode CDownloadProvider::DatabaseHelper::constructor(
    /* [in] */ IContext* context)
{
    return SQLiteOpenHelper::constructor(context, DB_NAME, NULL, DB_VERSION);
}

ECode CDownloadProvider::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    if (Constants::LOGVV) {
        Logger::V(Constants::TAG, "populating new database");
    }
    OnUpgrade(db, 0, DB_VERSION);
    return NOERROR;
}

ECode CDownloadProvider::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldV,
    /* [in] */ Int32 newV)
{
    if (oldV == 31) {
        // 31 and 100 are identical, just in different codelines. Upgrading from 31 is the
        // same as upgrading from 100.
        oldV = 100;
    }
    else if (oldV < 100) {
        // no logic to upgrade from these older version, just recreate the DB
        Logger::I(Constants::TAG, "Upgrading downloads database from version %d"
                                    " to version %d"
                                    ", which will destroy all old data", oldV, newV);
        oldV = 99;
    }
    else if (oldV > newV) {
        // user must have downgraded software; we have no way to know how to downgrade the
        // DB, so just recreate it
        Logger::I(Constants::TAG, "Downgrading downloads database from version %d"
                                    " (current version is %d"
                                    "), destroying all old data", oldV, newV);
        oldV = 99;
    }

    for (Int32 version = oldV + 1; version <= newV; version++) {
        UpgradeTo(db, version);
    }
    return NOERROR;
}

void CDownloadProvider::DatabaseHelper::UpgradeTo(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 version)
{
    switch (version) {
        case 100:
            CreateDownloadsTable(db);
            break;

        case 101:
            CreateHeadersTable(db);
            break;

        case 102:
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_IS_PUBLIC_API,
                        String("INTEGER NOT NULL DEFAULT 0"));
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_ALLOW_ROAMING,
                        String("INTEGER NOT NULL DEFAULT 0"));
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_ALLOWED_NETWORK_TYPES,
                        String("INTEGER NOT NULL DEFAULT 0"));
            break;

        case 103:
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI,
                        String("INTEGER NOT NULL DEFAULT 1"));
            MakeCacheDownloadsInvisible(db);
            break;

        case 104:
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_BYPASS_RECOMMENDED_SIZE_LIMIT,
                    String("INTEGER NOT NULL DEFAULT 0"));
            break;

        case 105:
            FillNullValues(db);
            break;

        case 106:
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_MEDIAPROVIDER_URI, String("TEXT"));
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_DELETED,
                    String("BOOLEAN NOT NULL DEFAULT 0"));
            break;

        case 107:
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_ERROR_MSG, String("TEXT"));
            break;

        case 108:
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_ALLOW_METERED,
                    String("INTEGER NOT NULL DEFAULT 1"));
            break;

        case 109:
            AddColumn(db, DB_TABLE, IDownloadsImpl::COLUMN_ALLOW_WRITE,
                    String("BOOLEAN NOT NULL DEFAULT 0"));
            break;

        default:
            // throw new IllegalStateException("Don't know how to upgrade to " + version);
            return;
    }
}

void CDownloadProvider::DatabaseHelper::FillNullValues(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IDownloadsImpl::COLUMN_CURRENT_BYTES, 0);
    FillNullValuesForColumn(db, values);
    values->Put(IDownloadsImpl::COLUMN_TOTAL_BYTES, -1);
    FillNullValuesForColumn(db, values);
    values->Put(IDownloadsImpl::COLUMN_TITLE, String(""));
    FillNullValuesForColumn(db, values);
    values->Put(IDownloadsImpl::COLUMN_DESCRIPTION, String(""));
    FillNullValuesForColumn(db, values);
}

void CDownloadProvider::DatabaseHelper::FillNullValuesForColumn(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values)
{
    AutoPtr<ISet> s;
    values->GetValueSet((ISet**)&s);
    AutoPtr<IIterator> it;
    s->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> nxt;
    it->GetNext((IInterface**)&nxt);
    AutoPtr<IInterface> pColm;
    IMapEntry::Probe(nxt)->GetKey((IInterface**)&pColm);
    String column;
    ICharSequence::Probe(pColm)->ToString(&column);
    Int32 res = 0;
    db->Update(DB_TABLE, values, column + " is NULL", NULL, &res);
    values->Clear();
}

void CDownloadProvider::DatabaseHelper::MakeCacheDownloadsInvisible(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI, FALSE);
    StringBuilder cacheSelection(IDownloadsImpl::COLUMN_DESTINATION);
    cacheSelection += " != ";
    cacheSelection += IDownloadsImpl::DESTINATION_EXTERNAL;
    Int32 res = 0;
    db->Update(DB_TABLE, values, cacheSelection.ToString(), NULL, &res);
}

void CDownloadProvider::DatabaseHelper::AddColumn(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ String dbTable,
    /* [in] */ String columnName,
    /* [in] */ String columnDefinition)
{
    String sql;
    sql.AppendFormat("ALTER TABLE %s ADD COLUMN %s %s", dbTable.string(), columnName.string(), columnDefinition.string());
    db->ExecSQL(sql);
}

void CDownloadProvider::DatabaseHelper::CreateDownloadsTable(
    /* [in] */ ISQLiteDatabase* db)
{
    // try {
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + DB_TABLE);
    StringBuilder sb("CREATE TABLE ");
    sb += DB_TABLE;
    sb += "(";
    sb += IBaseColumns::ID;
    sb += " INTEGER PRIMARY KEY AUTOINCREMENT,";
    sb += IDownloadsImpl::COLUMN_URI;
    sb += " TEXT, ";
    sb += Constants::RETRY_AFTER_X_REDIRECT_COUNT;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_APP_DATA;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_NO_INTEGRITY;
    sb += " BOOLEAN, ";
    sb += IDownloadsImpl::COLUMN_FILE_NAME_HINT;
    sb += " TEXT, ";
    sb += Constants::OTA_UPDATE;
    sb += " BOOLEAN, ";
    sb += IDownloadsImpl::_DATA;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_MIME_TYPE;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_DESTINATION;
    sb += " INTEGER, ";
    sb += Constants::NO_SYSTEM_FILES;
    sb += " BOOLEAN, ";
    sb += IDownloadsImpl::COLUMN_VISIBILITY;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_CONTROL;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_STATUS;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_FAILED_CONNECTIONS;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_LAST_MODIFICATION;
    sb += " BIGINT, ";
    sb += IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_NOTIFICATION_CLASS;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_NOTIFICATION_EXTRAS;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_COOKIE_DATA;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_USER_AGENT;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_REFERER;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_TOTAL_BYTES;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_CURRENT_BYTES;
    sb += " INTEGER, ";
    sb += Constants::ETAG;
    sb += " TEXT, ";
    sb += Constants::UID;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_OTHER_UID;
    sb += " INTEGER, ";
    sb += IDownloadsImpl::COLUMN_TITLE;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_DESCRIPTION;
    sb += " TEXT, ";
    sb += IDownloadsImpl::COLUMN_MEDIA_SCANNED;
    sb += " BOOLEAN)";
    db->ExecSQL(sb.ToString());
    // } catch (SQLException ex) {
    //     Logger::E(Constants.TAG, "couldn't create table in downloads database");
    //     throw ex;
    // }
}

void CDownloadProvider::DatabaseHelper::CreateHeadersTable(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IDownloadsImplRequestHeaders::HEADERS_DB_TABLE);
    StringBuilder sb("CREATE TABLE ");
    sb += IDownloadsImplRequestHeaders::HEADERS_DB_TABLE;
    sb += "(";
    sb += "id INTEGER PRIMARY KEY AUTOINCREMENT,";
    sb += IDownloadsImplRequestHeaders::COLUMN_DOWNLOAD_ID;
    sb += " INTEGER NOT NULL,";
    sb += IDownloadsImplRequestHeaders::COLUMN_HEADER;
    sb += " TEXT NOT NULL,";
    sb += IDownloadsImplRequestHeaders::COLUMN_VALUE;
    sb += " TEXT NOT NULL";
    sb += ");";
    db->ExecSQL(sb.ToString());
}

//===============================================================
// CDownloadProvider::OnCloseListener::
//===============================================================
CAR_INTERFACE_IMPL(CDownloadProvider::OnCloseListener, Object, IParcelFileDescriptorOnCloseListener)

CDownloadProvider::OnCloseListener::OnCloseListener(
    /* [in] */ IFile* f,
    /* [in] */ IUri* u,
    /* [in] */ Boolean b,
    /* [in] */ CDownloadProvider* host)
{
    mFile = f;
    mUri = u;
    mShouldScan = b;
    mHost = host;
}

ECode CDownloadProvider::OnCloseListener::OnClose(
    /* [in] */ ECode ioe)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    Int64 lenth = 0;
    mFile->GetLength(&lenth);
    values->Put(IDownloadsImpl::COLUMN_TOTAL_BYTES, lenth);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 mils = 0;
    sys->GetCurrentTimeMillis(&mils);
    values->Put(IDownloadsImpl::COLUMN_LAST_MODIFICATION, mils);
    Int32 res = 0;
    mHost->Update(mUri, values, String(NULL), NULL, &res);

    if (mShouldScan) {
        AutoPtr<IIntent> intent;
        CIntent::New(
                IIntent::ACTION_MEDIA_SCANNER_SCAN_FILE, (IIntent**)&intent);
        AutoPtr<IUriHelper> hlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
        AutoPtr<IUri> uri;
        hlp->FromFile(mFile, (IUri**)&uri);
        intent->SetData(uri);
        AutoPtr<IContext> cxt;
        mHost->GetContext((IContext**)&cxt);
        cxt->SendBroadcast(intent);
    }
    return NOERROR;
}

//===============================================================
// CDownloadProvider::
//===============================================================
CAR_OBJECT_IMPL(CDownloadProvider)

CAR_INTERFACE_IMPL(CDownloadProvider, ContentProvider, IDownloadProvider)

CDownloadProvider::CDownloadProvider()
    : mSystemUid(-1)
    , mDefContainerUid(-1)
{
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> uri;
    imp->GetCONTENT_URI((IUri**)&uri);
    BASE_URIS->Set(0, uri);
    AutoPtr<IUri> allUri;
    imp->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&allUri);
    BASE_URIS->Set(1, allUri);

    (*sAppReadableColumnsArray)[0] = IBaseColumns::ID,
    (*sAppReadableColumnsArray)[1] = IDownloadsImpl::COLUMN_APP_DATA;
    (*sAppReadableColumnsArray)[2] = IDownloadsImpl::_DATA;
    (*sAppReadableColumnsArray)[3] = IDownloadsImpl::COLUMN_MIME_TYPE;
    (*sAppReadableColumnsArray)[4] = IDownloadsImpl::COLUMN_VISIBILITY;
    (*sAppReadableColumnsArray)[5] = IDownloadsImpl::COLUMN_DESTINATION;
    (*sAppReadableColumnsArray)[6] = IDownloadsImpl::COLUMN_CONTROL;
    (*sAppReadableColumnsArray)[7] = IDownloadsImpl::COLUMN_STATUS;
    (*sAppReadableColumnsArray)[8] = IDownloadsImpl::COLUMN_LAST_MODIFICATION;
    (*sAppReadableColumnsArray)[9] = IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE;
    (*sAppReadableColumnsArray)[10] = IDownloadsImpl::COLUMN_NOTIFICATION_CLASS;
    (*sAppReadableColumnsArray)[11] = IDownloadsImpl::COLUMN_TOTAL_BYTES;
    (*sAppReadableColumnsArray)[12] = IDownloadsImpl::COLUMN_CURRENT_BYTES;
    (*sAppReadableColumnsArray)[13] = IDownloadsImpl::COLUMN_TITLE;
    (*sAppReadableColumnsArray)[14] = IDownloadsImpl::COLUMN_DESCRIPTION;
    (*sAppReadableColumnsArray)[15] = IDownloadsImpl::COLUMN_URI;
    (*sAppReadableColumnsArray)[16] = IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI;
    (*sAppReadableColumnsArray)[17] = IDownloadsImpl::COLUMN_FILE_NAME_HINT;
    (*sAppReadableColumnsArray)[18] = IDownloadsImpl::COLUMN_MEDIAPROVIDER_URI;
    (*sAppReadableColumnsArray)[19] = IDownloadsImpl::COLUMN_DELETED;
    (*sAppReadableColumnsArray)[20] = IOpenableColumns::DISPLAY_NAME;
    (*sAppReadableColumnsArray)[21] = IOpenableColumns::SIZE;
}

ECode CDownloadProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    if (mSystemFacade == NULL) {
        mSystemFacade = new RealSystemFacade(cxt);
    }

    CHandler::New((IHandler**)&mHandler);

    mOpenHelper = new DatabaseHelper();
    mOpenHelper->constructor(cxt);

    // Initialize the system uid
    mSystemUid = IProcess::SYSTEM_UID;
    // Initialize the default container uid. Package name hardcoded
    // for now.
    AutoPtr<IApplicationInfo> appInfo;
    // try {
    AutoPtr<IPackageManager> pm;
    cxt->GetPackageManager((IPackageManager**)&pm);
    if (FAILED(pm->GetApplicationInfo(String("Elastos.Droid.DefContainer"), 0, (IApplicationInfo**)&appInfo)))
        Logger::W(Constants::TAG, "Could not get ApplicationInfo for Elastos.Droid.DefContainer");
    // } catch (NameNotFoundException e) {
    //     Log.wtf(Constants.TAG, "Could not get ApplicationInfo for com.android.defconatiner", e);
    // }
    if (appInfo != NULL) {
        appInfo->GetUid(&mDefContainerUid);
    }
    // start the DownloadService class. don't wait for the 1st download to be issued.
    // saves us by getting some initialization code in DownloadService out of the way.
    AutoPtr<IIntent> p;
    CIntent::New(cxt, ECLSID_CDownloadService, (IIntent**)&p);
    AutoPtr<IComponentName> cn;
    cxt->StartService(p, (IComponentName**)&cn);
    *result = TRUE;
    return NOERROR;
}

ECode CDownloadProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 match = 0;
    sURIMatcher->Match(uri, &match);
    switch (match) {
        case MY_DOWNLOADS:
        case ALL_DOWNLOADS: {
            *result = DOWNLOAD_LIST_TYPE;
            return NOERROR;
        }
        case MY_DOWNLOADS_ID:
        case ALL_DOWNLOADS_ID:
        case PUBLIC_DOWNLOAD_ID: {
            // return the mimetype of this id from the database
            String id = GetDownloadIdFromUri(uri);
            AutoPtr<ISQLiteDatabase> db;
            mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
            AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
            (*arr)[0] = id;
            AutoPtr<IDatabaseUtils> du;
            CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&du);
            StringBuilder str("SELECT ");
            str += IDownloadsImpl::COLUMN_MIME_TYPE;
            str += " FROM ";
            str += DB_TABLE;
            str += " WHERE ";
            str += IBaseColumns::ID;
            str += " = ?";
            String mimeType;
            du->StringForQuery(db, str.ToString(), arr, &mimeType);
            AutoPtr<ITextUtils> tu;
            CTextUtils::AcquireSingleton((ITextUtils**)&tu);
            Boolean bEmp = FALSE;
            tu->IsEmpty(mimeType, &bEmp);
            if (bEmp) {
                *result = DOWNLOAD_TYPE;
                return NOERROR;
            }
            else {
                *result = mimeType;
                return NOERROR;
            }
        }
        default: {
            if (Constants::LOGV) {
                Logger::V(Constants::TAG, "calling getType on an unknown URI: %p", uri);
            }
            // throw new IllegalArgumentException("Unknown URI: %p", uri);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CDownloadProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    FAIL_RETURN(CheckInsertPermissions(values))
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    // note we disallow inserting into ALL_DOWNLOADS
    Int32 match = 0;
    sURIMatcher->Match(uri, &match);
    if (match != MY_DOWNLOADS) {
        Logger::D(Constants::TAG, "calling insert on an unknown/invalid URI: %s", TO_CSTR(uri));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // copy some of the input values as it
    AutoPtr<IContentValues> filteredValues;
    CContentValues::New((IContentValues**)&filteredValues);
    CopyString(IDownloadsImpl::COLUMN_URI, values, filteredValues);
    CopyString(IDownloadsImpl::COLUMN_APP_DATA, values, filteredValues);
    CopyBoolean(IDownloadsImpl::COLUMN_NO_INTEGRITY, values, filteredValues);
    CopyString(IDownloadsImpl::COLUMN_FILE_NAME_HINT, values, filteredValues);
    CopyString(IDownloadsImpl::COLUMN_MIME_TYPE, values, filteredValues);
    CopyBoolean(IDownloadsImpl::COLUMN_IS_PUBLIC_API, values, filteredValues);

    AutoPtr<IBoolean> bobj;
    values->GetAsBoolean(IDownloadsImpl::COLUMN_IS_PUBLIC_API, (IBoolean**)&bobj);
    Boolean bval;
    bobj->GetValue(&bval);
    Boolean isPublicApi = bval == TRUE;

    // validate the destination column
    Int32 dest = 0;
    AutoPtr<IInteger32> destObj;
    values->GetAsInteger32(IDownloadsImpl::COLUMN_DESTINATION, (IInteger32**)&destObj);
    if (destObj != NULL) {
        destObj->GetValue(&dest);
        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        Int32 per = 0;
        cxt->CheckCallingOrSelfPermission(IDownloadsImpl::PERMISSION_ACCESS_ADVANCED, &per);
        if (per != IPackageManager::PERMISSION_GRANTED
                && (dest == IDownloadsImpl::DESTINATION_CACHE_PARTITION
                        || dest == IDownloadsImpl::DESTINATION_CACHE_PARTITION_NOROAMING
                        || dest == IDownloadsImpl::DESTINATION_SYSTEMCACHE_PARTITION)) {
            // throw new SecurityException("setting destination to : " + dest +
            //         " not allowed, unless PERMISSION_ACCESS_ADVANCED is granted");
            return E_SECURITY_EXCEPTION;
        }
        // for public API behavior, if an app has CACHE_NON_PURGEABLE permission, automatically
        // switch to non-purgeable download
        cxt->CheckCallingOrSelfPermission(
                        IDownloadsImpl::PERMISSION_CACHE_NON_PURGEABLE, &per);
        Boolean hasNonPurgeablePermission = per == IPackageManager::PERMISSION_GRANTED;
        if (isPublicApi && dest == IDownloadsImpl::DESTINATION_CACHE_PARTITION_PURGEABLE
                && hasNonPurgeablePermission) {
            dest = IDownloadsImpl::DESTINATION_CACHE_PARTITION;
        }
        AutoPtr<IBinderHelper> hlp;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
        Int32 pid = 0, uid = 0;
        hlp->GetCallingPid(&pid);
        hlp->GetCallingUid(&uid);
        if (dest == IDownloadsImpl::DESTINATION_FILE_URI) {
            cxt->EnforcePermission(
                    Manifest::permission::WRITE_EXTERNAL_STORAGE,
                    pid, uid,
                    String("need WRITE_EXTERNAL_STORAGE permission to use DESTINATION_FILE_URI"));
            FAIL_RETURN(CheckFileUriDestination(values))
        }
        else if (dest == IDownloadsImpl::DESTINATION_SYSTEMCACHE_PARTITION) {
            cxt->EnforcePermission(
                    Manifest::permission::ACCESS_CACHE_FILESYSTEM,
                    pid, uid,
                    String("need ACCESS_CACHE_FILESYSTEM permission to use system cache"));
        }
        filteredValues->Put(IDownloadsImpl::COLUMN_DESTINATION, dest);
    }

    // validate the visibility column
    AutoPtr<IInteger32> vis;
    values->GetAsInteger32(IDownloadsImpl::COLUMN_VISIBILITY, (IInteger32**)&vis);
    if (vis == NULL) {
        if (dest == IDownloadsImpl::DESTINATION_EXTERNAL) {
            filteredValues->Put(IDownloadsImpl::COLUMN_VISIBILITY,
                    IDownloadsImpl::VISIBILITY_VISIBLE_NOTIFY_COMPLETED);
        }
        else {
            filteredValues->Put(IDownloadsImpl::COLUMN_VISIBILITY,
                    IDownloadsImpl::VISIBILITY_HIDDEN);
        }
    }
    else {
        Int32 ival = 0;
        vis->GetValue(&ival);
        filteredValues->Put(IDownloadsImpl::COLUMN_VISIBILITY, ival);
    }
    // copy the control column as is
    CopyInteger(IDownloadsImpl::COLUMN_CONTROL, values, filteredValues);

    /*
     * requests coming from
     * DownloadManager.addCompletedDownload(String, String, String,
     * Boolean, String, String, long) need special treatment
     */
    AutoPtr<IInteger32> cdObj;
    values->GetAsInteger32(IDownloadsImpl::COLUMN_DESTINATION, (IInteger32**)&cdObj);
    Int32 cd = 0; cdObj->GetValue(&cd);
    if (cd == IDownloadsImpl::DESTINATION_NON_DOWNLOADMANAGER_DOWNLOAD) {
        // these requests always are marked as 'completed'
        filteredValues->Put(IDownloadsImpl::COLUMN_STATUS, IDownloadsImpl::STATUS_SUCCESS);
        AutoPtr<IInteger64> lobj;
        values->GetAsInteger64(IDownloadsImpl::COLUMN_TOTAL_BYTES, (IInteger64**)&lobj);
        Int64 longVal = 0; lobj->GetValue(&longVal);
        filteredValues->Put(IDownloadsImpl::COLUMN_TOTAL_BYTES, longVal);
        filteredValues->Put(IDownloadsImpl::COLUMN_CURRENT_BYTES, 0);
        CopyInteger(IDownloadsImpl::COLUMN_MEDIA_SCANNED, values, filteredValues);
        CopyString(IDownloadsImpl::_DATA, values, filteredValues);
        CopyBoolean(IDownloadsImpl::COLUMN_ALLOW_WRITE, values, filteredValues);
    }
    else {
        filteredValues->Put(IDownloadsImpl::COLUMN_STATUS, IDownloadsImpl::STATUS_PENDING);
        filteredValues->Put(IDownloadsImpl::COLUMN_TOTAL_BYTES, -1);
        filteredValues->Put(IDownloadsImpl::COLUMN_CURRENT_BYTES, 0);
    }

    // set lastupdate to current time
    Int64 lastMod = 0;
    mSystemFacade->GetCurrentTimeMillis(&lastMod);
    filteredValues->Put(IDownloadsImpl::COLUMN_LAST_MODIFICATION, lastMod);

    // use packagename of the caller to set the notification columns
    String pckg;
    values->GetAsString(IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE, &pckg);
    String clazz;
    values->GetAsString(IDownloadsImpl::COLUMN_NOTIFICATION_CLASS, &clazz);
    if (pckg != NULL && (clazz != NULL || isPublicApi)) {
        AutoPtr<IBinderHelper> hlp;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
        Int32 uid = 0;
        hlp->GetCallingUid(&uid);
        // try {
        Boolean bOP = FALSE;
        if (uid == 0 ||
                (mSystemFacade->UserOwnsPackage(uid, pckg, &bOP), bOP)) {
            filteredValues->Put(IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE, pckg);
            if (clazz != NULL) {
                filteredValues->Put(IDownloadsImpl::COLUMN_NOTIFICATION_CLASS, clazz);
            }
        }
        // } catch (PackageManager.NameNotFoundException ex) {
        //     /* ignored for now */
        // }
    }

    // copy some more columns as is
    CopyString(IDownloadsImpl::COLUMN_NOTIFICATION_EXTRAS, values, filteredValues);
    CopyString(IDownloadsImpl::COLUMN_COOKIE_DATA, values, filteredValues);
    CopyString(IDownloadsImpl::COLUMN_USER_AGENT, values, filteredValues);
    CopyString(IDownloadsImpl::COLUMN_REFERER, values, filteredValues);

    // UID, PID columns
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    Int32 permission = 0;
    cxt->CheckCallingOrSelfPermission(IDownloadsImpl::PERMISSION_ACCESS_ADVANCED, &permission);
    if (permission == IPackageManager::PERMISSION_GRANTED) {
        CopyInteger(IDownloadsImpl::COLUMN_OTHER_UID, values, filteredValues);
    }
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int32 uid = 0;
    hlp->GetCallingUid(&uid);
    filteredValues->Put(Constants::UID, uid);
    if (uid == 0) {
        CopyInteger(Constants::UID, values, filteredValues);
    }

    // copy some more columns as is
    CopyStringWithDefault(IDownloadsImpl::COLUMN_TITLE, values, filteredValues, String(""));
    CopyStringWithDefault(IDownloadsImpl::COLUMN_DESCRIPTION, values, filteredValues, String(""));

    // is_visible_in_downloads_ui column
    Boolean bCK = FALSE;
    if ((values->ContainsKey(IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI, &bCK), bCK)) {
        CopyBoolean(IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI, values, filteredValues);
    }
    else {
        // by default, make external downloads visible in the UI
        Boolean isExternal = (dest == 0 || dest == IDownloadsImpl::DESTINATION_EXTERNAL);
        filteredValues->Put(IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI, isExternal);
    }

    // public api requests and networktypes/roaming columns
    if (isPublicApi) {
        CopyInteger(IDownloadsImpl::COLUMN_ALLOWED_NETWORK_TYPES, values, filteredValues);
        CopyBoolean(IDownloadsImpl::COLUMN_ALLOW_ROAMING, values, filteredValues);
        CopyBoolean(IDownloadsImpl::COLUMN_ALLOW_METERED, values, filteredValues);
    }

    if (Constants::LOGVV) {
        AutoPtr<IInteger32> cuid;
        filteredValues->GetAsInteger32(Constants::UID, (IInteger32**)&cuid);
        Logger::V(Constants::TAG, "initiating download with UID %s", TO_CSTR(cuid));
        Boolean bCUID = FALSE;
        if ((filteredValues->ContainsKey(IDownloadsImpl::COLUMN_OTHER_UID, &bCUID), bCUID)) {
            AutoPtr<IInteger32> cou;
            filteredValues->GetAsInteger32(IDownloadsImpl::COLUMN_OTHER_UID, (IInteger32**)&cou);
            Logger::V(Constants::TAG, "other UID %s", TO_CSTR(cou));
        }
    }

    Int64 rowID = 0;
    db->Insert(DB_TABLE, String(NULL), filteredValues, &rowID);
    if (rowID == -1) {
        Logger::D(Constants::TAG, "couldn't insert into downloads database");
        *result = NULL;
        return NOERROR;
    }

    FAIL_RETURN(InsertRequestHeaders(db, rowID, values));
    NotifyContentChanged(uri, match);

    // Always start service to handle notifications and/or scanning
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIntent> p;
    CIntent::New(context, ECLSID_CDownloadService, (IIntent**)&p);
    AutoPtr<IComponentName> cn;
    context->StartService(p, (IComponentName**)&cn);

    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> content_uri;
    imp->GetCONTENT_URI((IUri**)&content_uri);
    return cu->WithAppendedId(content_uri, rowID, result);
}

ECode CDownloadProvider::CheckFileUriDestination(
    /* [in] */ IContentValues* values)
{
    String fileUri;
    values->GetAsString(IDownloadsImpl::COLUMN_FILE_NAME_HINT, &fileUri);
    if (fileUri == NULL) {
        Logger::E(Constants::TAG,
            "DESTINATION_FILE_URI must include a file URI under COLUMN_FILE_NAME_HINT");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IUriHelper> hlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
    AutoPtr<IUri> uri;
    hlp->Parse(fileUri, (IUri**)&uri);
    String scheme;
    uri->GetScheme(&scheme);
    if (scheme == NULL || !scheme.Equals("file")) {
        Logger::E(Constants::TAG, "Not a file URI: %s", TO_CSTR(uri));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String path;
    uri->GetPath(&path);
    if (path == NULL) {
        Logger::E(Constants::TAG, "Invalid file URI: %s", TO_CSTR(uri));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IFile> f;
    CFile::New(path, (IFile**)&f);
    String canonicalPath;
    f->GetCanonicalPath(&canonicalPath);
    String externalPath;
    Environment::GetExternalStorageDirectory()->GetAbsolutePath(&externalPath);
    if (!canonicalPath.StartWith(externalPath)) {
        Logger::E(Constants::TAG, "Destination must be on external storage: %s", TO_CSTR(uri));
        return E_SECURITY_EXCEPTION;
    }
    // } catch (IOException e) {
    //     throw new SecurityException("Problem resolving path: " + uri);
    // }
    return NOERROR;
}

ECode CDownloadProvider::CheckInsertPermissions(
    /* [in] */ IContentValues* values)
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    Int32 per = 0;
    cxt->CheckCallingOrSelfPermission(IDownloadsImpl::PERMISSION_ACCESS, &per);
    if (per == IPackageManager::PERMISSION_GRANTED) {
        return NOERROR;
    }

    FAIL_RETURN(cxt->EnforceCallingOrSelfPermission(Manifest::permission::INTERNET,
            String("INTERNET permission is required to use the download manager")))

    // ensure the request fits within the bounds of a public API request
    // first copy so we can remove values
    CContentValues::New(values, (IContentValues**)&values);

    // check columns whose values are restricted
    AutoPtr<IBoolean> bo;
    CBoolean::New(TRUE, (IBoolean**)&bo);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
    (*arr)[0] = bo;
    FAIL_RETURN(EnforceAllowedValues(values, IDownloadsImpl::COLUMN_IS_PUBLIC_API, arr))

    // validate the destination column
    AutoPtr<IInteger32> cdObj;
    values->GetAsInteger32(IDownloadsImpl::COLUMN_DESTINATION, (IInteger32**)&cdObj);
    Int32 cd = 0; cdObj->GetValue(&cd);
    if (cd == IDownloadsImpl::DESTINATION_NON_DOWNLOADMANAGER_DOWNLOAD) {
        /* this row is inserted by
         * DownloadManager.addCompletedDownload(String, String, String,
         * Boolean, String, String, long)
         */
        values->Remove(IDownloadsImpl::COLUMN_TOTAL_BYTES);
        values->Remove(IDownloadsImpl::_DATA);
        values->Remove(IDownloadsImpl::COLUMN_STATUS);
    }
    AutoPtr<IInteger32> pCachePartition;
    CInteger32::New(IDownloadsImpl::DESTINATION_CACHE_PARTITION_PURGEABLE, (IInteger32**)&pCachePartition);
    AutoPtr<IInteger32> pFileUri;
    CInteger32::New(IDownloadsImpl::DESTINATION_FILE_URI, (IInteger32**)&pFileUri);
    AutoPtr<IInteger32> pDownload;
    CInteger32::New(IDownloadsImpl::DESTINATION_NON_DOWNLOADMANAGER_DOWNLOAD, (IInteger32**)&pDownload);
    AutoPtr<ArrayOf<IInterface*> > arrVals = ArrayOf<IInterface*>::Alloc(3);
    arrVals->Set(0, pCachePartition);
    arrVals->Set(1, pFileUri);
    arrVals->Set(2, pDownload);
    FAIL_RETURN(EnforceAllowedValues(values, IDownloadsImpl::COLUMN_DESTINATION, arr))

    AutoPtr<IInteger32> pVis;
    CInteger32::New(IDownloadManagerRequest::VISIBILITY_VISIBLE, (IInteger32**)&pVis);
    AutoPtr<IInteger32> pVis_Com;
    CInteger32::New(IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_COMPLETED, (IInteger32**)&pVis_Com);
    AutoPtr<IInteger32> pVis_only;
    CInteger32::New(IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_ONLY_COMPLETION, (IInteger32**)&pVis_only);
    Int32 perNo = 0;
    cxt->CheckCallingOrSelfPermission(IDownloadsImpl::PERMISSION_NO_NOTIFICATION, &perNo);
    if (perNo == IPackageManager::PERMISSION_GRANTED) {
        AutoPtr<IInteger32> pVis_hid;
        CInteger32::New(IDownloadManagerRequest::VISIBILITY_HIDDEN, (IInteger32**)&pVis_hid);
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(4);
        arr->Set(0, pVis_hid); arr->Set(1, pVis);
        arr->Set(2, pVis_Com); arr->Set(3, pVis_only);
        FAIL_RETURN(EnforceAllowedValues(values, IDownloadsImpl::COLUMN_VISIBILITY, arr))
    }
    else {
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(3);
        arr->Set(0, pVis);
        arr->Set(1, pVis_Com);
        arr->Set(2, pVis_only);
        FAIL_RETURN(EnforceAllowedValues(values, IDownloadsImpl::COLUMN_VISIBILITY, arr))
    }

    // remove the rest of the columns that are allowed (with any value)
    values->Remove(IDownloadsImpl::COLUMN_URI);
    values->Remove(IDownloadsImpl::COLUMN_TITLE);
    values->Remove(IDownloadsImpl::COLUMN_DESCRIPTION);
    values->Remove(IDownloadsImpl::COLUMN_MIME_TYPE);
    values->Remove(IDownloadsImpl::COLUMN_FILE_NAME_HINT); // checked later in insert()
    values->Remove(IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE); // checked later in insert()
    values->Remove(IDownloadsImpl::COLUMN_ALLOWED_NETWORK_TYPES);
    values->Remove(IDownloadsImpl::COLUMN_ALLOW_ROAMING);
    values->Remove(IDownloadsImpl::COLUMN_ALLOW_METERED);
    values->Remove(IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI);
    values->Remove(IDownloadsImpl::COLUMN_MEDIA_SCANNED);
    values->Remove(IDownloadsImpl::COLUMN_ALLOW_WRITE);
    AutoPtr<ISet> s;
    values->GetValueSet((ISet**)&s);
    AutoPtr<IIterator> iterator;
    s->GetIterator((IIterator**)&iterator);
    Boolean bHasNxt = FALSE;
    while ((iterator->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> nxt;
        iterator->GetNext((IInterface**)&nxt);
        AutoPtr<IMapEntry> me = IMapEntry::Probe(nxt);
        AutoPtr<IInterface> pKey;
        me->GetKey((IInterface**)&pKey);
        AutoPtr<ICharSequence> _pKey = ICharSequence::Probe(pKey);
        String key;
        _pKey->ToString(&key);
        if (key.StartWith(IDownloadsImplRequestHeaders::INSERT_KEY_PREFIX)) {
            iterator->Remove();
        }
    }

    // any extra columns are extraneous and disallowed
    Int32 valSize = 0;
    values->GetSize(&valSize);
    if (valSize > 0) {
        StringBuilder error("Invalid columns in request: ");
        Boolean first = TRUE;
        AutoPtr<IIterator> it;
        s->GetIterator((IIterator**)&it);
        Boolean bNext = FALSE;
        while ((it->HasNext(&bNext), bNext)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
            if (!first) {
                error.Append(", ");
            }
            AutoPtr<IInterface> k;
            entry->GetKey((IInterface**)&k);
            error.Append(k);
        }
        Logger::E(Constants::TAG, error.ToString());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CDownloadProvider::EnforceAllowedValues(
    /* [in] */ IContentValues* values,
    /* [in] */ String column,
    /* [in] */ ArrayOf<IInterface*>* allowedValues)
{
    AutoPtr<IInterface> value;
    values->Get(column, (IInterface**)&value);
    values->Remove(column);
    for (Int32 i = 0; i < allowedValues->GetLength(); i++) {
        AutoPtr<IInterface> allowedValue = (*allowedValues)[i];
        if (value == NULL && allowedValue == NULL) {
            return NOERROR;
        }
        if (value != NULL && Object::Equals(value, allowedValue)) {
            return NOERROR;
        }
    }
    Logger::E(Constants::TAG, "Invalid value for %s: %s", column.string(), TO_CSTR(value));
    return E_SECURITY_EXCEPTION;
}

AutoPtr<ICursor> CDownloadProvider::QueryCleared(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ String selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ String sort)
{
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int64 token = 0;
    hlp->ClearCallingIdentity(&token);
    AutoPtr<ICursor> res;
    Query(uri, projection, selection, selectionArgs, sort, (ICursor**)&res);
    hlp->RestoreCallingIdentity(token);
    return res;
}

ECode CDownloadProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sort,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IHelpers> hlps;
    CHelpers::AcquireSingleton((IHelpers**)&hlps);
    hlps->ValidateSelection(selection, ISet::Probe(sAppReadableColumnsSet));

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);

    Int32 match = 0;
    sURIMatcher->Match(uri, &match);
    if (match == -1) {
        if (Constants::LOGV) {
            Logger::V(Constants::TAG, "querying unknown URI: %p", uri);
        }
        // throw new IllegalArgumentException("Unknown URI: " + uri);
        *result = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (match == REQUEST_HEADERS_URI) {
        if (projection != NULL || selection != NULL || sort != NULL) {
            // throw new UnsupportedOperationException("Request header queries do not support "
            //                                         + "projections, selections or sorting");
            *result = NULL;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
        AutoPtr<ICursor> res = QueryRequestHeaders(db, uri);
        *result = res;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<SqlSelection> fullSelection = GetWhereClause(uri, selection, selectionArgs, match);

    if (ShouldRestrictVisibility()) {
        if (projection == NULL) {
            projection = sAppReadableColumnsArray->Clone();
        }
        else {
            // check the validity of the columns in projection
            for (Int32 i = 0; i < projection->GetLength(); ++i) {
                AutoPtr<ICharSequence> pPro;
                CString::New((*projection)[i], (ICharSequence**)&pPro);
                Boolean bSetContains = FALSE, bListContains = FALSE;
                sAppReadableColumnsSet->Contains(pPro, &bSetContains);
                downloadManagerColumnsList->Contains(pPro, &bListContains);
                if (!bSetContains && !bListContains) {
                    // throw new IllegalArgumentException(
                    //         "column " + projection[i] + " is not allowed in queries");
                    *result = NULL;
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }
        }

        for (Int32 i = 0; i < projection->GetLength(); i++) {
            AutoPtr<ICharSequence> pPro;
            CString::New((*projection)[i], (ICharSequence**)&pPro);
            AutoPtr<IInterface> pCol;
            sColumnsMap->Get(pPro, (IInterface**)&pCol);
            AutoPtr<ICharSequence> _pCol = ICharSequence::Probe(pCol);
            String newColumn;
            _pCol->ToString(&newColumn);
            if (newColumn != NULL) {
                (*projection)[i] = newColumn;
            }
        }
    }

    if (Constants::LOGVV) {
        LogVerboseQueryInfo(projection, selection, selectionArgs, sort, db);
    }

    AutoPtr<ICursor> ret;
    db->Query(DB_TABLE, projection, fullSelection->GetSelection(),
            fullSelection->GetParameters(), String(NULL), String(NULL), sort, (ICursor**)&ret);

    if (ret != NULL) {
        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        AutoPtr<IContentResolver> cr;
        cxt->GetContentResolver((IContentResolver**)&cr);
        ret->SetNotificationUri(cr, uri);
        if (Constants::LOGVV) {
            AutoPtr<IBinderHelper> hlp;
            CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
            Int32 pid = 0;
            hlp->GetCallingPid(&pid);
            Logger::V(Constants::TAG,
                    "created cursor %p on behalf of %d", ret.Get(), pid);
        }
    }
    else {
        if (Constants::LOGV) {
            Logger::V(Constants::TAG, "query failed in downloads database");
        }
    }

    *result = ret;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CDownloadProvider::LogVerboseQueryInfo(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ String selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ String sort,
    /* [in] */ ISQLiteDatabase* db)
{
    StringBuilder sb;
    sb.Append("starting query, database is ");
    if (db != NULL) {
        sb.Append("not ");
    }
    sb.Append("null; ");
    if (projection == NULL) {
        sb.Append("projection is null; ");
    }
    else if (projection->GetLength() == 0) {
        sb.Append("projection is empty; ");
    }
    else {
        for (Int32 i = 0; i < projection->GetLength(); ++i) {
            sb.Append("projection[");
            sb.Append(i);
            sb.Append("] is ");
            sb.Append((*projection)[i]);
            sb.Append("; ");
        }
    }
    sb.Append("selection is ");
    sb.Append(selection);
    sb.Append("; ");
    if (selectionArgs == NULL) {
        sb.Append("selectionArgs is null; ");
    }
    else if (selectionArgs->GetLength() == 0) {
        sb.Append("selectionArgs is empty; ");
    }
    else {
        for (Int32 i = 0; i < selectionArgs->GetLength(); ++i) {
            sb.Append("selectionArgs[");
            sb.Append(i);
            sb.Append("] is ");
            sb.Append((*selectionArgs)[i]);
            sb.Append("; ");
        }
    }
    sb.Append("sort is ");
    sb.Append(sort);
    sb.Append(".");
    Logger::V(Constants::TAG, sb.ToString());
}

String CDownloadProvider::GetDownloadIdFromUri(
    /* [in] */ IUri* uri)
{
    AutoPtr<IList> l;
    uri->GetPathSegments((IList**)&l);
    AutoPtr<IInterface> p;
    l->Get(1, (IInterface**)&p);
    AutoPtr<ICharSequence> res = ICharSequence::Probe(p);
    String result;
    res->ToString(&result);
    return result;
}

ECode CDownloadProvider::InsertRequestHeaders(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 downloadId,
    /* [in] */ IContentValues* values)
{
    AutoPtr<IContentValues> rowValues;
    CContentValues::New((IContentValues**)&rowValues);
    rowValues->Put(IDownloadsImplRequestHeaders::COLUMN_DOWNLOAD_ID, downloadId);
    AutoPtr<ISet> s;
    values->GetValueSet((ISet**)&s);
    AutoPtr<IIterator> iterator;
    s->GetIterator((IIterator**)&iterator);
    Boolean bHasNxt = FALSE;
    while ((iterator->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> nxt;
        iterator->GetNext((IInterface**)&nxt);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(nxt);
        AutoPtr<IInterface> pKey;
        entry->GetKey((IInterface**)&pKey);
        AutoPtr<ICharSequence> _pKey = ICharSequence::Probe(pKey);
        String key;
        _pKey->ToString(&key);
        if (key.StartWith(IDownloadsImplRequestHeaders::INSERT_KEY_PREFIX)) {
            AutoPtr<IInterface> pVal;
            entry->GetValue((IInterface**)&pVal);
            AutoPtr<ICharSequence> _pVal = ICharSequence::Probe(pVal);
            String headerLine;
            _pVal->ToString(&headerLine);
            if (!headerLine.Contains(":")) {
                Logger::E(Constants::TAG, "Invalid HTTP header line: %s", headerLine.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<ArrayOf<String> > parts;
            StringUtils::Split(headerLine, String(":"), 2, (ArrayOf<String>**)&parts);
            rowValues->Put(IDownloadsImplRequestHeaders::COLUMN_HEADER, (*parts)[0].Trim());
            rowValues->Put(IDownloadsImplRequestHeaders::COLUMN_VALUE, (*parts)[1].Trim());
            Int64 val = 0;
            db->Insert(IDownloadsImplRequestHeaders::HEADERS_DB_TABLE, String(NULL), rowValues, &val);
        }
    }
    return NOERROR;
}

AutoPtr<ICursor> CDownloadProvider::QueryRequestHeaders(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IUri* uri)
{
    String where = IDownloadsImplRequestHeaders::COLUMN_DOWNLOAD_ID + "="
                   + GetDownloadIdFromUri(uri);
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(2);
    (*projection)[0] = IDownloadsImplRequestHeaders::COLUMN_HEADER;
    (*projection)[1] = IDownloadsImplRequestHeaders::COLUMN_VALUE;
    AutoPtr<ICursor> res;
    db->Query(IDownloadsImplRequestHeaders::HEADERS_DB_TABLE, projection, where,
                    NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&res);
    return res;
}

void CDownloadProvider::DeleteRequestHeaders(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ String where,
    /* [in] */ ArrayOf<String>* whereArgs)
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = IBaseColumns::ID;
    AutoPtr<ICursor> cursor;
    db->Query(DB_TABLE, projection, where, whereArgs,
                String(NULL), String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
    Boolean result;
    for (cursor->MoveToFirst(&result); (cursor->IsAfterLast(&result), !result); cursor->MoveToNext(&result)) {
        Int64 id = 0;
        cursor->GetInt64(0, &id);
        StringBuilder sb(IDownloadsImplRequestHeaders::COLUMN_DOWNLOAD_ID);
        sb += "="; sb +=id;
        Int32 val = 0;
        db->Delete(IDownloadsImplRequestHeaders::HEADERS_DB_TABLE, sb.ToString(), NULL, &val);
    }
    ICloseable::Probe(cursor)->Close();
}

Boolean CDownloadProvider::ShouldRestrictVisibility()
{
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int32 pid = 0, callingUid = 0;
    hlp->GetCallingPid(&pid);
    hlp->GetCallingUid(&callingUid);
    return pid != Process::MyPid() &&
            callingUid != mSystemUid &&
            callingUid != mDefContainerUid;
}

ECode CDownloadProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IHelpers> hlps;
    CHelpers::AcquireSingleton((IHelpers**)&hlps);
    hlps->ValidateSelection(where, ISet::Probe(sAppReadableColumnsSet));

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    Int32 count = 0;
    Boolean startService = FALSE;

    Boolean bCK = FALSE;
    if ((values->ContainsKey(IDownloadsImpl::COLUMN_DELETED, &bCK), bCK)) {
        AutoPtr<IInteger32> cdObj;
        values->GetAsInteger32(IDownloadsImpl::COLUMN_DELETED, (IInteger32**)&cdObj);
        Int32 cd = 0; cdObj->GetValue(&cd);
        if (cd == 1) {
            // some rows are to be 'deleted'. need to start DownloadService.
            startService = TRUE;
        }
    }

    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int32 pid = 0;
    hlp->GetCallingPid(&pid);
    AutoPtr<IContentValues> filteredValues;
    if (pid != Process::MyPid()) {
        CContentValues::New((IContentValues**)&filteredValues);
        CopyString(IDownloadsImpl::COLUMN_APP_DATA, values, filteredValues);
        CopyInteger(IDownloadsImpl::COLUMN_VISIBILITY, values, filteredValues);
        AutoPtr<IInteger32> cc;
        values->GetAsInteger32(IDownloadsImpl::COLUMN_CONTROL, (IInteger32**)&cc);
        if (cc != NULL) {
            Int32 i; cc->GetValue(&i);
            filteredValues->Put(IDownloadsImpl::COLUMN_CONTROL, i);
            startService = TRUE;
        }

        CopyInteger(IDownloadsImpl::COLUMN_CONTROL, values, filteredValues);
        CopyString(IDownloadsImpl::COLUMN_TITLE, values, filteredValues);
        CopyString(IDownloadsImpl::COLUMN_MEDIAPROVIDER_URI, values, filteredValues);
        CopyString(IDownloadsImpl::COLUMN_DESCRIPTION, values, filteredValues);
        CopyInteger(IDownloadsImpl::COLUMN_DELETED, values, filteredValues);
    }
    else {
        filteredValues = values;
        String filename;
        values->GetAsString(IDownloadsImpl::_DATA, &filename);
        if (filename != NULL) {
            AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
            (*arr)[0] = IDownloadsImpl::COLUMN_TITLE;
            AutoPtr<ICursor> c;
            Query(uri, arr, String(NULL), NULL, String(NULL), (ICursor**)&c);
            String cStr;
            c->GetString(0, &cStr);
            Boolean bMF = FALSE;
            if (!(c->MoveToFirst(&bMF), bMF) || cStr.IsEmpty()) {
                AutoPtr<IFile> f;
                CFile::New(filename, (IFile**)&f);
                String strName;
                f->GetName(&strName);
                values->Put(IDownloadsImpl::COLUMN_TITLE, strName);
            }
            AutoPtr<IIoUtils> iou;
            CIoUtils::AcquireSingleton((IIoUtils**)&iou);
            iou->CloseQuietly(ICloseable::Probe(c));
        }

        Boolean isRestart = FALSE;
        AutoPtr<IInteger32> status;
        values->GetAsInteger32(IDownloadsImpl::COLUMN_STATUS, (IInteger32**)&status);
        if (status != NULL) {
            Int32 val; status->GetValue(&val);
            isRestart = val == IDownloadsImpl::STATUS_PENDING;
        }
        Boolean bval = FALSE;
        values->ContainsKey(IDownloadsImpl::COLUMN_BYPASS_RECOMMENDED_SIZE_LIMIT, &bval);
        if (isRestart || bval) {
            startService = TRUE;
        }
    }

    Int32 match = 0;
    sURIMatcher->Match(uri, &match);
    switch (match) {
        case MY_DOWNLOADS:
        case MY_DOWNLOADS_ID:
        case ALL_DOWNLOADS:
        case ALL_DOWNLOADS_ID:
        {
            AutoPtr<SqlSelection> selection = GetWhereClause(uri, where, whereArgs, match);
            Int32 s = 0;
            filteredValues->GetSize(&s);
            if (s > 0) {
                db->Update(DB_TABLE, filteredValues, selection->GetSelection(),
                        selection->GetParameters(), &count);
            }
            else {
                count = 0;
            }
        }
            break;

        default:
            Logger::D(Constants::TAG, "updating unknown/invalid URI: %p", uri);
            // throw new UnsupportedOperationException("Cannot update URI: " + uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    NotifyContentChanged(uri, match);
    if (startService) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IIntent> p;
        CIntent::New(context, ECLSID_CDownloadService, (IIntent**)&p);
        AutoPtr<IComponentName> cn;
        context->StartService(p, (IComponentName**)&cn);
    }
    *result = count;
    return NOERROR;
}

void CDownloadProvider::NotifyContentChanged(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 uriMatch)
{
    Int64 downloadId = 0;
    if (uriMatch == MY_DOWNLOADS_ID || uriMatch == ALL_DOWNLOADS_ID) {
        downloadId = StringUtils::ParseInt64(GetDownloadIdFromUri(uri));
    }
    for (Int32 i = 0; i < BASE_URIS->GetLength(); i++) {
        AutoPtr<IUri> uriToNotify = (*BASE_URIS)[i];

        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        cu->WithAppendedId(uriToNotify, downloadId, (IUri**)&uriToNotify);

        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        AutoPtr<IContentResolver> cr;
        cxt->GetContentResolver((IContentResolver**)&cr);
        cr->NotifyChange(uriToNotify, NULL);
    }
}

AutoPtr<CDownloadProvider::SqlSelection> CDownloadProvider::GetWhereClause(
    /* [in] */ IUri* uri,
    /* [in] */ String where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [in] */ Int32 uriMatch)
{
    AutoPtr<SqlSelection> selection = new SqlSelection();
    selection->AppendClause(where, whereArgs);
    if (uriMatch == MY_DOWNLOADS_ID || uriMatch == ALL_DOWNLOADS_ID ||
            uriMatch == PUBLIC_DOWNLOAD_ID) {
        AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
        (*arr)[0] = GetDownloadIdFromUri(uri);
        selection->AppendClause(IBaseColumns::ID + " = ?", arr);
    }
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    Int32 per = 0;
    cxt->CheckCallingOrSelfPermission(IDownloadsImpl::PERMISSION_ACCESS_ALL, &per);
    if ((uriMatch == MY_DOWNLOADS || uriMatch == MY_DOWNLOADS_ID)
            && per != IPackageManager::PERMISSION_GRANTED) {
        AutoPtr<IBinderHelper> hlp;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
        Int32 uid = 0;
        hlp->GetCallingUid(&uid);
        String strID = StringUtils::ToString(uid);
        AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
        (*arr)[0] = strID;
        (*arr)[1] = strID;
        selection->AppendClause(
                Constants::UID + "= ? OR " + IDownloadsImpl::COLUMN_OTHER_UID + "= ?",
                arr);
    }
    return selection;
}

ECode CDownloadProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (ShouldRestrictVisibility()) {
        AutoPtr<IHelpers> hlps;
        CHelpers::AcquireSingleton((IHelpers**)&hlps);
        hlps->ValidateSelection(where, ISet::Probe(sAppReadableColumnsSet));
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 count = 0;
    Int32 match = 0;
    sURIMatcher->Match(uri, &match);
    switch (match) {
        case MY_DOWNLOADS:
        case MY_DOWNLOADS_ID:
        case ALL_DOWNLOADS:
        case ALL_DOWNLOADS_ID:
        {
            AutoPtr<SqlSelection> selection = GetWhereClause(uri, where, whereArgs, match);
            DeleteRequestHeaders(db, selection->GetSelection(), selection->GetParameters());

            AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
            (*arr)[0] = IBaseColumns::ID;
            AutoPtr<ICursor> cursor;
            db->Query(DB_TABLE, arr, selection->GetSelection(), selection->GetParameters(),
                    String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
            Boolean bMN = FALSE;
            while ((cursor->MoveToNext(&bMN), bMN)) {
                Int64 id = 0;
                cursor->GetInt64(0, &id);
                AutoPtr<IContext> cxt;
                GetContext((IContext**)&cxt);
                CDownloadStorageProvider::OnDownloadProviderDelete(cxt, id);
            }
            AutoPtr<IIoUtils> iou;
            CIoUtils::AcquireSingleton((IIoUtils**)&iou);
            iou->CloseQuietly(ICloseable::Probe(cursor));

            db->Delete(DB_TABLE, selection->GetSelection(), selection->GetParameters(), &count);
            break;
        }
        default:
        {
            Logger::D(Constants::TAG, "deleting unknown/invalid URI: %p", uri);
            // throw new UnsupportedOperationException("Cannot delete URI: %p", uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }
    NotifyContentChanged(uri, match);
    *result = count;
    return NOERROR;
}

ECode CDownloadProvider::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    if (Constants::LOGVV) {
        LogVerboseOpenFileInfo(uri, mode);
    }

    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(4);
    (*arr)[0] = IDownloadsImpl::_DATA;
    (*arr)[1] = IDownloadsImpl::COLUMN_STATUS;
    (*arr)[2] = IDownloadsImpl::COLUMN_DESTINATION;
    (*arr)[3] = IDownloadsImpl::COLUMN_MEDIA_SCANNED;
    AutoPtr<ICursor> cursor = QueryCleared(uri, arr, String(NULL), NULL, String(NULL));
    String path;
    Boolean shouldScan = FALSE;
    Int32 cc = 0;
    Int32 count = (cursor != NULL) ? (cursor->GetCount(&cc), cc) : 0;
    if (count != 1) {
        // If there is not exactly one result, throw an appropriate exception.
        if (count == 0) {
            // throw new FileNotFoundException("No entry for " + uri);
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
        // throw new FileNotFoundException("Multiple items at " + uri);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    Boolean bMF = FALSE;
    if ((cursor->MoveToFirst(&bMF), bMF)) {
        Int32 status = 0;
        cursor->GetInt32(1, &status);
        Int32 destination = 0;
        cursor->GetInt32(2, &destination);
        Int32 mediaScanned = 0;
        cursor->GetInt32(3, &mediaScanned);

        cursor->GetString(0, &path);
        Boolean bSS = FALSE;
        AutoPtr<IDownloadsImpl> impl;
        CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
        impl->IsStatusSuccess(status, &bSS);
        shouldScan = bSS && (
                destination == IDownloadsImpl::DESTINATION_EXTERNAL
                || destination == IDownloadsImpl::DESTINATION_FILE_URI
                || destination == IDownloadsImpl::DESTINATION_NON_DOWNLOADMANAGER_DOWNLOAD)
                && mediaScanned != 2;
    }
    else {
        // throw new FileNotFoundException("Failed moveToFirst");
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    AutoPtr<IIoUtils> iou;
    CIoUtils::AcquireSingleton((IIoUtils**)&iou);
    iou->CloseQuietly(ICloseable::Probe(cursor));

    if (path == NULL) {
        // throw new FileNotFoundException("No filename found.");
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IHelpers> hlps;
    CHelpers::AcquireSingleton((IHelpers**)&hlps);
    Boolean bValid = FALSE;
    if (!(hlps->IsFilenameValid(cxt, file, &bValid), bValid)) {
        // throw new FileNotFoundException("Invalid file: " + file);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<IParcelFileDescriptorHelper> hlp;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&hlp);
    Int32 pfdMode = 0;
    hlp->ParseMode(mode, &pfdMode);
    if (pfdMode == IParcelFileDescriptor::MODE_READ_ONLY) {
        return hlp->Open(file, pfdMode, result);
    }
    else {
        // try {
            // When finished writing, update size and timestamp
        AutoPtr<OnCloseListener> p = new OnCloseListener(file, uri, shouldScan, this);
        return hlp->Open(file, pfdMode, mHandler,
                        IParcelFileDescriptorOnCloseListener::Probe(p), result);
        // } catch (IOException e) {
        //     throw new FileNotFoundException("Failed to open for writing: " + e);
        // }
    }
}

ECode CDownloadProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IIndentingPrintWriter> pw;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), 120, (IIndentingPrintWriter**)&pw);

    IPrintWriter::Probe(pw)->Println(String("Downloads updated in last hour:"));
    pw->IncreaseIndent();

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    Int64 mils = 0;
    mSystemFacade->GetCurrentTimeMillis(&mils);
    Int64 modifiedAfter = mils - IDateUtils::HOUR_IN_MILLIS;
    StringBuilder sb(IDownloadsImpl::COLUMN_LAST_MODIFICATION); sb += ">"; sb += modifiedAfter;
    AutoPtr<ICursor> cursor;
    db->Query(DB_TABLE, NULL,
            sb.ToString(), NULL, String(NULL), String(NULL),
            IBaseColumns::ID + " ASC", (ICursor**)&cursor);
    AutoPtr<ArrayOf<String> > cols;
    cursor->GetColumnNames((ArrayOf<String>**)&cols);
    Int32 idCol = 0;
    cursor->GetColumnIndex(IBaseColumns::ID, &idCol);
    Boolean bHasNxt = FALSE;
    while ((cursor->MoveToNext(&bHasNxt), bHasNxt)) {
        Int32 id = 0;
        cursor->GetInt32(idCol, &id);
        StringBuilder sb("Download #"); sb += id; sb += ":";
        IPrintWriter::Probe(pw)->Println(sb.ToString());
        pw->IncreaseIndent();
        for (Int32 i = 0; i < cols->GetLength(); i++) {
            // Omit sensitive data when dumping
            if (IDownloadsImpl::COLUMN_COOKIE_DATA.Equals((*cols)[i])) {
                continue;
            }
            String cont;
            cursor->GetString(i, &cont);
            AutoPtr<ICharSequence> cs;
            CString::New(cont, (ICharSequence**)&cs);
            pw->PrintPair((*cols)[i], cs);
        }
        IPrintWriter::Probe(pw)->Println();
        pw->DecreaseIndent();
    }
    ICloseable::Probe(cursor)->Close();

    pw->DecreaseIndent();
    return NOERROR;
}

void CDownloadProvider::LogVerboseOpenFileInfo(
    /* [in] */ IUri* uri,
    /* [in] */ String mode)
{
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int32 uid = 0;
    hlp->GetCallingUid(&uid);
    Logger::V(Constants::TAG, "openFile uri: %p, mode: %s, uid: %d",
                uri, (const char*)mode, uid);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = "_id";
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> content_uri;
    imp->GetCONTENT_URI((IUri**)&content_uri);
    AutoPtr<ICursor> cursor;
    Query(content_uri,
            arr, String(NULL), NULL, String("_id"), (ICursor**)&cursor);
    if (cursor == NULL) {
        Logger::V(Constants::TAG, "null cursor in openFile");
    }
    else {
        Boolean bMF = FALSE, bMN = FALSE;
        if (!(cursor->MoveToFirst(&bMF), bMF)) {
            Logger::V(Constants::TAG, "empty cursor in openFile");
        }
        else {
            do {
                Int32 i = 0;
                cursor->GetInt32(0, &i);
                Logger::V(Constants::TAG, "row %d available", i);
            } while((cursor->MoveToNext(&bMN), bMN));
        }
        ICloseable::Probe(cursor)->Close();
    }
    AutoPtr<ArrayOf<String> > arrDT = ArrayOf<String>::Alloc(1);
    (*arr)[0] = "_data";
    Query(uri, arrDT, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor == NULL) {
        Logger::V(Constants::TAG, "null cursor in openFile");
    }
    else {
        Boolean bMF = FALSE;
        if (!(cursor->MoveToFirst(&bMF), bMF)) {
            Logger::V(Constants::TAG, "empty cursor in openFile");
        }
        else {
            String filename;
            cursor->GetString(0, &filename);
            Logger::V(Constants::TAG, "filename in openFile: %s", (const char*)filename);
            AutoPtr<IFile> f;
            CFile::New(filename, (IFile**)&f);
            Boolean bIsFile = FALSE;
            if ((f->IsFile(&bIsFile), bIsFile)) {
                Logger::V(Constants::TAG, "file exists in openFile");
            }
        }
        ICloseable::Probe(cursor)->Close();
    }
}

void CDownloadProvider::CopyInteger(
    /* [in] */ String key,
    /* [in] */ IContentValues* from,
    /* [in] */ IContentValues* to)
{
    AutoPtr<IInteger32> i;
    from->GetAsInteger32(key, (IInteger32**)&i);
    if (i != NULL) {
        to->Put(key, i);
    }
}

void CDownloadProvider::CopyBoolean(
    /* [in] */ String key,
    /* [in] */ IContentValues* from,
    /* [in] */ IContentValues* to)
{
    AutoPtr<IBoolean> b;
    from->GetAsBoolean(key, (IBoolean**)&b);
    if (b != NULL) {
        to->Put(key, b);
    }
}

void CDownloadProvider::CopyString(
    /* [in] */ String key,
    /* [in] */ IContentValues* from,
    /* [in] */ IContentValues* to)
{
    String s;
    from->GetAsString(key, &s);
    if (s != NULL) {
        to->Put(key, s);
    }
}

void CDownloadProvider::CopyStringWithDefault(
    /* [in] */ String key,
    /* [in] */ IContentValues* from,
    /* [in] */ IContentValues* to,
    /* [in] */ String defaultValue)
{
    CopyString(key, from, to);
    Boolean bCK = FALSE;
    if (!(to->ContainsKey(key, &bCK), bCK)) {
        to->Put(key, defaultValue);
    }
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
