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

#include "elastos/droid/providers/telephony/CTelephonyProvider.h"
#include "R.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Environment.h"
#include <elastos/droid/R.h>
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::CTelephonyCarriers;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ITelephonyCarriers;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileReader;
using Elastos::IO::IReader;
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

const String CTelephonyProvider::DATABASE_NAME("telephony.db");
const Boolean CTelephonyProvider::DBG = TRUE;
const Boolean CTelephonyProvider::VDBG = FALSE;
const Int32 CTelephonyProvider::DATABASE_VERSION = 16 << 16;
const Int32 CTelephonyProvider::URL_UNKNOWN = 0;
const Int32 CTelephonyProvider::URL_TELEPHONY = 1;
const Int32 CTelephonyProvider::URL_CURRENT = 2;
const Int32 CTelephonyProvider::URL_ID = 3;
const Int32 CTelephonyProvider::URL_RESTOREAPN = 4;
const Int32 CTelephonyProvider::URL_PREFERAPN = 5;
const Int32 CTelephonyProvider::URL_PREFERAPN_NO_UPDATE = 6;
const Int32 CTelephonyProvider::URL_SIMINFO = 7;
const Int32 CTelephonyProvider::URL_TELEPHONY_USING_SUBID = 8;
const Int32 CTelephonyProvider::URL_CURRENT_USING_SUBID = 9;
const Int32 CTelephonyProvider::URL_RESTOREAPN_USING_SUBID = 10;
const Int32 CTelephonyProvider::URL_PREFERAPN_USING_SUBID = 11;
const Int32 CTelephonyProvider::URL_PREFERAPN_NO_UPDATE_USING_SUBID = 12;
const Int32 CTelephonyProvider::URL_SIMINFO_USING_SUBID = 13;
const String CTelephonyProvider::TAG("TelephonyProvider");
const String CTelephonyProvider::CARRIERS_TABLE("carriers");
const String CTelephonyProvider::SIMINFO_TABLE("siminfo");
const String CTelephonyProvider::PREF_FILE("preferred-apn");
const String CTelephonyProvider::COLUMN_APN_ID("apn_id");
const String CTelephonyProvider::PARTNER_APNS_PATH("etc/apns-conf.xml");
AutoPtr<IUriMatcher> CTelephonyProvider::s_urlMatcher = InitUriMatcher();
AutoPtr<IContentValues> CTelephonyProvider::s_currentNullMap;
AutoPtr<IContentValues> CTelephonyProvider::s_currentSetMap;
Boolean CTelephonyProvider::s_Init = InitMember();

Boolean CTelephonyProvider::InitMember()
{
    assert(s_urlMatcher != NULL);
    s_urlMatcher->AddURI(String("telephony"), String("carriers"), URL_TELEPHONY);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/current"), URL_CURRENT);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/#"), URL_ID);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/restore"), URL_RESTOREAPN);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/preferapn"), URL_PREFERAPN);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/preferapn_no_update"), URL_PREFERAPN_NO_UPDATE);

    s_urlMatcher->AddURI(String("telephony"), String("siminfo"), URL_SIMINFO);

    s_urlMatcher->AddURI(String("telephony"), String("carriers/subId/*"), URL_TELEPHONY_USING_SUBID);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/current/subId/*"), URL_CURRENT_USING_SUBID);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/restore/subId/*"), URL_RESTOREAPN_USING_SUBID);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/preferapn/subId/*"), URL_PREFERAPN_USING_SUBID);
    s_urlMatcher->AddURI(String("telephony"), String("carriers/preferapn_no_update/subId/*"),
        URL_PREFERAPN_NO_UPDATE_USING_SUBID);

    CContentValues::New(1, (IContentValues**)&s_currentNullMap);
    s_currentNullMap->Put(String("current"), (IInteger64*)NULL/*(Long) null*/);

    CContentValues::New(1, (IContentValues**)&s_currentSetMap);
    s_currentSetMap->Put(String("current"), String("1"));
    return TRUE;
}


CTelephonyProvider::DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ CTelephonyProvider* host)
    : mHost(host)
{
    SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, GetVersion(context));
    mContext = context;
}

Int32 CTelephonyProvider::DatabaseHelper::GetVersion(
    /* [in] */ IContext* context)
{
    if (VDBG) Log(String("getVersion:+"));
    // Get the database version, combining a static schema version and the XML version
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<IXmlResourceParser> parser;
    r->GetXml(Elastos::Droid::R::xml::apns, (IXmlResourceParser**)&parser);
    ECode ec = NOERROR;
    do {
        ec = XmlUtils::BeginDocument(IXmlPullParser::Probe(parser), String("apns"));
        if (FAILED(ec)) {
            break;
        }
        String attr;
        ec = IXmlPullParser::Probe(parser)->GetAttributeValue(String(NULL), String("version"), &attr);
        if (FAILED(ec)) {
            break;
        }

        Int32 publicversion = StringUtils::ParseInt32(attr);
        Int32 version = DATABASE_VERSION | publicversion;
        if (VDBG) Log(String("getVersion:- version=0x") + StringUtils::ToHexString(version));
        parser->Close();
        return version;
    } while (0);

    parser->Close();

    if (FAILED(ec)) {
        Loge(String("Can't get version of APN database") + " return version=" +
                StringUtils::ToHexString(DATABASE_VERSION));
        return DATABASE_VERSION;
    }

    assert(0 && "Not Touch....");
    return 0;
}

ECode CTelephonyProvider::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    if (DBG) Log(String("dbh.onCreate:+ db=") + TO_CSTR(db));
    CreateSimInfoTable(db);
    CreateCarriersTable(db);
    InitDatabase(db);
    if (DBG) Log(String("dbh.onCreate:- db=") + TO_CSTR(db));
    return NOERROR;
}

ECode CTelephonyProvider::DatabaseHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    if (VDBG) Log(String("dbh.onOpen:+ db=") + TO_CSTR(db));
    // Try to access the table and create it if "no such table"
    AutoPtr<ICursor> c;
    if (FAILED(db->Query(SIMINFO_TABLE, NULL, String(NULL), NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c))) {
        Loge(String("Exception ") + SIMINFO_TABLE);
        // if (e.getMessage().startsWith("no such table")) {
        CreateSimInfoTable(db);
        // }
    }
    else {
        if (DBG) Log(String("dbh.onOpen: ok, queried table=") + SIMINFO_TABLE);
    }

    c = NULL;
    if (FAILED(db->Query(CARRIERS_TABLE, NULL, String(NULL), NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c))) {
        Loge(String("Exception ") + CARRIERS_TABLE);
        // if (e.getMessage().startsWith("no such table")) {
        CreateCarriersTable(db);
        // }
    }
    else {
        if (DBG) Log(String("dbh.onOpen: ok, queried table=") + CARRIERS_TABLE);
    }

    if (VDBG) Log(String("dbh.onOpen:- db=") + TO_CSTR(db));
    return NOERROR;
}

void CTelephonyProvider::DatabaseHelper::CreateSimInfoTable(
    /* [in] */ ISQLiteDatabase* db)
{
    if (DBG) Log(String("dbh.createSimInfoTable:+"));
    db->ExecSQL(String("CREATE TABLE ") + SIMINFO_TABLE + "("
            + "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            + ISubscriptionManager::ICC_ID + " TEXT NOT NULL,"
            + ISubscriptionManager::SIM_ID + " INTEGER DEFAULT "
            + StringUtils::ToString(ISubscriptionManager::SIM_NOT_INSERTED) + String(",")
            + ISubscriptionManager::DISPLAY_NAME + " TEXT,"
            + ISubscriptionManager::NAME_SOURCE + " INTEGER DEFAULT "
            + StringUtils::ToString(ISubscriptionManager::NAME_SOURCE_DEFAULT_SOURCE) + String(",")
            + ISubscriptionManager::COLOR + " INTEGER DEFAULT "
            + StringUtils::ToString(ISubscriptionManager::COLOR_DEFAULT) + String(",")
            + ISubscriptionManager::NUMBER + " TEXT,"
            + ISubscriptionManager::DISPLAY_NUMBER_FORMAT + " INTEGER NOT NULL DEFAULT "
            + StringUtils::ToString(ISubscriptionManager::DISLPAY_NUMBER_DEFAULT) + String(",")
            + ISubscriptionManager::DATA_ROAMING + " INTEGER DEFAULT "
            + StringUtils::ToString(ISubscriptionManager::DATA_ROAMING_DEFAULT) + String(",")
            + ISubscriptionManager::MCC + " INTEGER DEFAULT 0,"
            + ISubscriptionManager::MNC + " INTEGER DEFAULT 0" + String(",")
            + ISubscriptionManager::SUB_STATE + " INTEGER DEFAULT "
            + StringUtils::ToString(ISubscriptionManager::ACTIVE) + String(",")
            + ISubscriptionManager::NETWORK_MODE+ " INTEGER DEFAULT "
            + StringUtils::ToString(ISubscriptionManager::DEFAULT_NW_MODE)
            + ");");
    if (DBG) Log(String("dbh.createSimInfoTable:-"));
}

void CTelephonyProvider::DatabaseHelper::CreateCarriersTable(
    /* [in] */ ISQLiteDatabase* db)
{
    // Set up the database schema
    if (DBG) Log(String("dbh.createCarriersTable:+"));
    db->ExecSQL(String("CREATE TABLE ") + CARRIERS_TABLE +
        "(_id INTEGER PRIMARY KEY," +
            "name TEXT," +
            "numeric TEXT," +
            "mcc TEXT," +
            "mnc TEXT," +
            "apn TEXT," +
            "user TEXT," +
            "server TEXT," +
            "password TEXT," +
            "proxy TEXT," +
            "port TEXT," +
            "mmsproxy TEXT," +
            "mmsport TEXT," +
            "mmsc TEXT," +
            "authtype INTEGER," +
            "type TEXT," +
            "current INTEGER," +
            "protocol TEXT," +
            "roaming_protocol TEXT," +
            "carrier_enabled BOOLEAN," +
            "bearer INTEGER," +
            "mvno_type TEXT," +
            "mvno_match_data TEXT," +
            "sub_id LONG DEFAULT -1," +
            "profile_id INTEGER default 0," +
            "modem_cognitive BOOLEAN default 0," +
            "max_conns INTEGER default 0," +
            "wait_time INTEGER default 0," +
            "max_conns_time INTEGER default 0," +
            "mtu INTEGER);");
     /* FIXME Currenlty sub_id is column is not used for query purpose.
     This would be modified to more appropriate default value later. */
    if (DBG) Log(String("dbh.createCarriersTable:-"));
}

Int32 CTelephonyProvider::DatabaseHelper::GetDefaultPreferredApnId(
    /* [in] */ ISQLiteDatabase* db)
{
    Int32 id = -1;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String configPref;
    AutoPtr<ArrayOf<IInterface*> > os = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(String(""), (ICharSequence**)&cs);
    os->Set(0, cs);
    res->GetString(R::string::config_preferred_apn, os, &configPref);
    if (!TextUtils::IsEmpty(configPref)) {
        AutoPtr<ArrayOf<String> > s;
        StringUtils::Split(configPref, String(","), (ArrayOf<String>**)&s);
        if (s->GetLength() == 3 || s->GetLength() == 4) {
            AutoPtr<ICursor> c;
            AutoPtr<ArrayOf<String> > sv = ArrayOf<String>::Alloc(1);
            sv->Set(0, String("_id"));
            String selection = String("apn='") + (*s)[0] + "' AND mcc='" + (*s)[1] + "' AND mnc='"
                + (*s)[2] + (s->GetLength() == 4 ? String("' AND protocol='") + (*s)[3] : String("")) + "'";
            ECode ec = db->Query(String("carriers"), sv, selection ,
                    NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
            if (FAILED(ec)) {
                Logger::D(TAG, "[TODO] Query fail sql=[%s]");
            }
            Boolean tmp = FALSE;
            if (c->MoveToFirst(&tmp), tmp) {
                c->GetInt32(0, &id);
            }
            ICloseable::Probe(c)->Close();
        }
    }
    return id;
}

void CTelephonyProvider::DatabaseHelper::InitDatabase(
    /* [in] */ ISQLiteDatabase* db)
{
    if (VDBG) Log(String("dbh.initDatabase:+ db=") + TO_CSTR(db));
    // Read internal APNS data
    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    AutoPtr<IXmlResourceParser> parser;
    r->GetXml(Elastos::Droid::R::xml::apns, (IXmlResourceParser**)&parser);
    Int32 publicversion = -1;
    ECode ec = NOERROR;
    do {
        ec = XmlUtils::BeginDocument(IXmlPullParser::Probe(parser), String("apns"));
        if (FAILED(ec)) {
            break;
        }
        String attr;
        ec = IXmlPullParser::Probe(parser)->GetAttributeValue(String(NULL), String("version"), &attr);
        if (FAILED(ec)) {
            break;
        }
        publicversion = StringUtils::ParseInt32(attr);
        LoadApns(db, IXmlPullParser::Probe(parser));
    } while (0);

    if (FAILED(ec)) {
        Loge(String("Got exception while loading APN database."));
    }
    parser->Close();

    // Read external APNS data (partner-provided)
    AutoPtr<IXmlPullParser> confparser;
    // Environment.getRootDirectory() is a fancy way of saying ANDROID_ROOT or "/system".
    AutoPtr<IFile> confFile;
    CFile::New(Environment::GetRootDirectory(), PARTNER_APNS_PATH, (IFile**)&confFile);
    AutoPtr<IFileReader> confreader;
    do {
        ec = CFileReader::New(confFile, (IFileReader**)&confreader);
        if (FAILED(ec)) {
            break;
        }
        ec = Xml::NewPullParser((IXmlPullParser**)&confparser);
        if (FAILED(ec)) {
            break;
        }
        confparser->SetInput(IReader::Probe(confreader));
        ec = XmlUtils::BeginDocument(confparser, String("apns"));
        if (FAILED(ec)) {
            break;
        }

        // Sanity check. Force internal version and confidential versions to agree
        String attr;
        confparser->GetAttributeValue(String(NULL), String("version"), &attr);
        Int32 confversion = StringUtils::ParseInt32(attr);
        if (publicversion != confversion) {
            assert(0 && "Internal APNS file version doesn't match ");
            // throw new IllegalStateException("Internal APNS file version doesn't match "
            //         + confFile.getAbsolutePath());
        }

        LoadApns(db, confparser);
    } while (0);
    // } catch (FileNotFoundException e) {
    //     // It's ok if the file isn't found. It means there isn't a confidential file
    //     // Log.e(TAG, "File not found: '" + confFile.getAbsolutePath() + "'");
    // } catch (Exception e) {
    //     Loge("Exception while parsing '" + confFile.getAbsolutePath() + "'" + e);
    // } finally {
    //     try { if (confreader != NULL) confreader.close(); } catch (IOException e) { }
    // }
    if (FAILED(ec)) {
        String v;
        confFile->GetAbsolutePath(&v);
        Loge(String("Exception while parsing '") + v + "'");
    }
    if (confreader != NULL) {
        ICloseable::Probe(confreader)->Close();
    }
    if (VDBG) Log(String("dbh.initDatabase:- db=") + TO_CSTR(db));
}

ECode CTelephonyProvider::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    if (DBG) {
        Log(String("dbh.onUpgrade:+ db=") + TO_CSTR(db) + " oldV="
            + StringUtils::ToString(oldVersion) + " newV=" + StringUtils::ToString(newVersion));
    }

    if (oldVersion < (5 << 16 | 6)) {
        // 5 << 16 is the Database version and 6 in the xml version.

        // This change adds a new authtype column to the database.
        // The auth type column can have 4 values: 0 (None), 1 (PAP), 2 (CHAP)
        // 3 (PAP or CHAP). To avoid breaking compatibility, with already working
        // APNs, the unset value (-1) will be used. If the value is -1.
        // the authentication will default to 0 (if no user / password) is specified
        // or to 3. Currently, there have been no reported problems with
        // pre-configured APNs and hence it is set to -1 for them. Similarly,
        // if the user, has added a new APN, we set the authentication type
        // to -1.

        db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
                " ADD COLUMN authtype INTEGER DEFAULT -1;");

        oldVersion = 5 << 16 | 6;
    }
    if (oldVersion < (6 << 16 | 6)) {
        // Add protcol fields to the APN. The XML file does not change.
        db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
                " ADD COLUMN protocol TEXT DEFAULT IP;");
        db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
                " ADD COLUMN roaming_protocol TEXT DEFAULT IP;");
        oldVersion = 6 << 16 | 6;
    }
    if (oldVersion < (7 << 16 | 6)) {
        // Add carrier_enabled, bearer fields to the APN. The XML file does not change.
        db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
                " ADD COLUMN carrier_enabled BOOLEAN DEFAULT 1;");
        db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
                " ADD COLUMN bearer INTEGER DEFAULT 0;");
        oldVersion = 7 << 16 | 6;
    }
    if (oldVersion < (8 << 16 | 6)) {
        // Add mvno_type, mvno_match_data fields to the APN.
        // The XML file does not change.
        db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
                " ADD COLUMN mvno_type TEXT DEFAULT '';");
        db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
                " ADD COLUMN mvno_match_data TEXT DEFAULT '';");
        oldVersion = 8 << 16 | 6;
    }
    if (oldVersion < (9 << 16 | 6)) {
        UpgradeForSubIdIfNecessary(db);
        oldVersion = 9 << 16 | 6;
    }
    if (oldVersion < (10 << 16 | 6)) {
        UpgradeForProfileIdIfNecessary(db);
        oldVersion = 10 << 16 | 6;
    }
    if (oldVersion < (11 << 16 | 6)) {
        UpgradeForMtuIfNecessary(db);
        oldVersion = 11 << 16 | 6;
    }
    if (oldVersion < (12 << 16 | 6)) {
        UpgradeForSimInfoIfNecssary(db);
        oldVersion = 12 << 16 | 6;
    }
    if (oldVersion < (13 << 16 | 6)) {
        UpgradeForSubscriptionInfoIfNecessary(db);
        oldVersion = 13 << 16 | 6;
    }

    if (oldVersion < (14 << 16 | 6) || oldVersion < (15 << 16 | 6) ||
            oldVersion < (16 << 16 | 6)) {
        // Handle migration from current AOSP (by artificially bumping the version
        // and handle migration from cm-11.0, by checking for an existing column
        // and then forcing the introduction of new columns
        AutoPtr<ICursor> c;

        ECode ec = NOERROR;
        do {
            ec = db->RawQuery(String("SELECT preferred from ") + CARRIERS_TABLE, NULL, (ICursor**)&c);
            if (FAILED(ec)) {
                break;
            }
            // if this column exists, then we need to add all the columns previously
            // added by AOSP since version (9 << 16 | 6)
            if (c != NULL) {
                Boolean tmp = FALSE;
                if (c->MoveToNext(&tmp), tmp) {
                    UpgradeForSubIdIfNecessary(db);
                    UpgradeForProfileIdIfNecessary(db);
                    UpgradeForMtuIfNecessary(db);
                    UpgradeForSimInfoIfNecssary(db);
                    UpgradeForSubscriptionInfoIfNecessary(db);
                }
            }
        } while (0);
        if (FAILED(ec)) {
            if (DBG) {
                Log(String("onUpgrade skipping ") + CARRIERS_TABLE + " migrade. " +
                        " The table will get created in onOpen.");
            }
        }

        oldVersion = (16 << 16 | 6);
    }

    if (DBG) {
        Log(String("dbh.onUpgrade:- db=") + TO_CSTR(db) + " oldV="
            + StringUtils::ToString(oldVersion) + " newV=" + StringUtils::ToString(newVersion));
    }
    return NOERROR;
}

void CTelephonyProvider::DatabaseHelper::UpgradeForSubIdIfNecessary(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
            " ADD COLUMN sub_id LONG DEFAULT -1;");
}

void CTelephonyProvider::DatabaseHelper::UpgradeForProfileIdIfNecessary(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
            " ADD COLUMN profile_id INTEGER DEFAULT 0;");
    db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
            " ADD COLUMN modem_cognitive BOOLEAN DEFAULT 0;");
    db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
            " ADD COLUMN max_conns INTEGER DEFAULT 0;");
    db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
            " ADD COLUMN wait_time INTEGER DEFAULT 0;");
    db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
            " ADD COLUMN max_conns_time INTEGER DEFAULT 0;");
}

void CTelephonyProvider::DatabaseHelper::UpgradeForMtuIfNecessary(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("ALTER TABLE ") + CARRIERS_TABLE +
            " ADD COLUMN mtu INTEGER DEFAULT 0;");
}

void CTelephonyProvider::DatabaseHelper::UpgradeForSimInfoIfNecssary(
    /* [in] */ ISQLiteDatabase* db)
{
    ECode ec = NOERROR;
    do {
        // Try to update the siminfo table. It might not be there.
        ec = db->ExecSQL(String("ALTER TABLE ") + SIMINFO_TABLE +
                " ADD COLUMN " + ISubscriptionManager::MCC + " INTEGER DEFAULT 0;");
        if (FAILED(ec)) {
            break;
        }
        ec = db->ExecSQL(String("ALTER TABLE ") + SIMINFO_TABLE +
                " ADD COLUMN " + ISubscriptionManager::MNC + " INTEGER DEFAULT 0;");
    } while (0);
    if (FAILED(ec)) {
        if (DBG) {
            Log(String("onUpgrade skipping ") + SIMINFO_TABLE + " upgrade. " +
                    " The table will get created in onOpen.");
        }
    }
}

void CTelephonyProvider::DatabaseHelper::UpgradeForSubscriptionInfoIfNecessary(
    /* [in] */ ISQLiteDatabase* db)
{
    ECode ec = NOERROR;
    do {
        // Try to update the siminfo table. It might not be there.
        ec = db->ExecSQL(String("ALTER TABLE ") + SIMINFO_TABLE +
                " ADD COLUMN " + ISubscriptionManager::SUB_STATE
                + " INTEGER DEFAULT " + StringUtils::ToString(ISubscriptionManager::ACTIVE) + ";");
        if (FAILED(ec)) {
            break;
        }
        ec = db->ExecSQL(String("ALTER TABLE ") + SIMINFO_TABLE +
                " ADD COLUMN " + ISubscriptionManager::NETWORK_MODE
                + " INTEGER DEFAULT " + StringUtils::ToString(ISubscriptionManager::DEFAULT_NW_MODE) + ";");
    } while (0);
    if (FAILED(ec)) {
        if (DBG) {
            Log(String("onUpgrade skipping ") + SIMINFO_TABLE + " upgrade. " +
                    " The table will get created in onOpen.");
        }
    }
}

/**
 * Gets the next row of apn values.
 *
 * @param parser the parser
 * @return the row or NULL if it's not an apn
 */
AutoPtr<IContentValues> CTelephonyProvider::DatabaseHelper::GetRow(
    /* [in] */ IXmlPullParser* parser)
{
    String n;
    parser->GetName(&n);
    if (!String("apn").Equals(n)) {
        return NULL;
    }

    AutoPtr<IContentValues> map;
    CContentValues::New((IContentValues**)&map);

    String mcc;
    parser->GetAttributeValue(String(NULL), String("mcc"), &mcc);
    String mnc;
    parser->GetAttributeValue(String(NULL), String("mnc"), &mnc);
    String numeric = mcc + mnc;

    map->Put(ITelephonyCarriers::NUMERIC, numeric);
    map->Put(ITelephonyCarriers::MCC, mcc);
    map->Put(ITelephonyCarriers::MNC, mnc);
    String sv;
    parser->GetAttributeValue(String(NULL), String("carrier"), &sv);
    map->Put(ITelephonyCarriers::NAME, sv);
    parser->GetAttributeValue(String(NULL), String("apn"), &sv);
    map->Put(ITelephonyCarriers::APN, sv);
    parser->GetAttributeValue(String(NULL), String("user"), &sv);
    map->Put(ITelephonyCarriers::USER, sv);
    parser->GetAttributeValue(String(NULL), String("server"), &sv);
    map->Put(ITelephonyCarriers::SERVER, sv);
    parser->GetAttributeValue(String(NULL), String("password"), &sv);
    map->Put(ITelephonyCarriers::PASSWORD, sv);

    // do not add NULL to the map so that insert() will set the default value
    String proxy;
    parser->GetAttributeValue(String(NULL), String("proxy"), &proxy);
    if (proxy != NULL) {
        map->Put(ITelephonyCarriers::PROXY, proxy);
    }
    String port;
    parser->GetAttributeValue(String(NULL), String("port"), &port);
    if (port != NULL) {
        map->Put(ITelephonyCarriers::PORT, port);
    }
    String mmsproxy;
    parser->GetAttributeValue(String(NULL), String("mmsproxy"), &mmsproxy);
    if (mmsproxy != NULL) {
        map->Put(ITelephonyCarriers::MMSPROXY, mmsproxy);
    }
    String mmsport;
    parser->GetAttributeValue(String(NULL), String("mmsport"), &mmsport);
    if (mmsport != NULL) {
        map->Put(ITelephonyCarriers::MMSPORT, mmsport);
    }
    parser->GetAttributeValue(String(NULL), String("mmsc"), &sv);
    map->Put(ITelephonyCarriers::MMSC, sv);
    String type;
    parser->GetAttributeValue(String(NULL), String("type"), &type);
    if (type != NULL) {
        map->Put(ITelephonyCarriers::TYPE, type);
    }

    String auth;
    parser->GetAttributeValue(String(NULL), String("authtype"), &auth);
    if (auth != NULL) {
        map->Put(ITelephonyCarriers::AUTH_TYPE, StringUtils::ParseInt32(auth));
    }

    String protocol;
    parser->GetAttributeValue(String(NULL), String("protocol"), &protocol);
    if (protocol != NULL) {
        map->Put(ITelephonyCarriers::PROTOCOL, protocol);
    }

    String roamingProtocol;
    parser->GetAttributeValue(String(NULL), String("roaming_protocol"), &roamingProtocol);
    if (roamingProtocol != NULL) {
        map->Put(ITelephonyCarriers::ROAMING_PROTOCOL, roamingProtocol);
    }

    String carrierEnabled;
    parser->GetAttributeValue(String(NULL), String("carrier_enabled"), &carrierEnabled);
    if (carrierEnabled != NULL) {
        map->Put(ITelephonyCarriers::CARRIER_ENABLED, StringUtils::ParseBoolean(carrierEnabled));
    }

    String bearer;
    parser->GetAttributeValue(String(NULL), String("bearer"), &bearer);
    if (bearer != NULL) {
        map->Put(ITelephonyCarriers::BEARER, StringUtils::ParseInt32(bearer));
    }

    String mvno_type;
    parser->GetAttributeValue(String(NULL), String("mvno_type"), &mvno_type);
    if (mvno_type != NULL) {
        String mvno_match_data;
        parser->GetAttributeValue(String(NULL), String("mvno_match_data"), &mvno_match_data);
        if (mvno_match_data != NULL) {
            map->Put(ITelephonyCarriers::MVNO_TYPE, mvno_type);
            map->Put(ITelephonyCarriers::MVNO_MATCH_DATA, mvno_match_data);
        }
    }

    String profileId;
    parser->GetAttributeValue(String(NULL), String("profile_id"), &profileId);
    if (profileId != NULL) {
        map->Put(ITelephonyCarriers::PROFILE_ID, StringUtils::ParseInt32(profileId));
    }

    String modemCognitive;
    parser->GetAttributeValue(String(NULL), String("modem_cognitive"), &modemCognitive);
    if (modemCognitive != NULL) {
        map->Put(ITelephonyCarriers::MODEM_COGNITIVE, StringUtils::ParseBoolean(modemCognitive));
    }

    String maxConns;
    parser->GetAttributeValue(String(NULL), String("max_conns"), &maxConns);
    if (maxConns != NULL) {
        map->Put(ITelephonyCarriers::MAX_CONNS, StringUtils::ParseInt32(maxConns));
    }

    String waitTime;
    parser->GetAttributeValue(String(NULL), String("wait_time"), &waitTime);
    if (waitTime != NULL) {
        map->Put(ITelephonyCarriers::WAIT_TIME, StringUtils::ParseInt32(waitTime));
    }

    String maxConnsTime;
    parser->GetAttributeValue(String(NULL), String("max_conns_time"), &maxConnsTime);
    if (maxConnsTime != NULL) {
        map->Put(ITelephonyCarriers::MAX_CONNS_TIME, StringUtils::ParseInt32(maxConnsTime));
    }

    String mtu;
    parser->GetAttributeValue(String(NULL), String("mtu"), &mtu);
    if (mtu != NULL) {
        map->Put(ITelephonyCarriers::MTU, StringUtils::ParseInt32(mtu));
    }

    return map;
}

/*
 * Loads apns from xml file into the database
 *
 * @param db the sqlite database to write to
 * @param parser the xml parser
 *
 */
void CTelephonyProvider::DatabaseHelper::LoadApns(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IXmlPullParser* parser)
{
    if (parser != NULL) {
        ECode ec = NOERROR;
        do {
            ec = db->BeginTransaction();
            if (FAILED(ec)) {
                break;
            }
            ec = XmlUtils::NextElement(parser);
            if (FAILED(ec)) {
                break;
            }
            Int32 type = 0;
            while ((ec = parser->GetEventType(&type), type) != IXmlPullParser::END_DOCUMENT) {
                if (FAILED(ec)) {
                    break;
                }
                AutoPtr<IContentValues> row = GetRow(parser);
                if (row == NULL) {
                    // throw new XmlPullParserException("Expected 'apn' tag", parser, NULL);
                    assert(0 && "Expected 'apn' tag");
                }
                InsertAddingDefaults(db, CARRIERS_TABLE, row);
                ec = XmlUtils::NextElement(parser);
                if (FAILED(ec)) {
                    break;
                }
            }
            if (FAILED(ec)) {
                break;
            }

            ec = db->SetTransactionSuccessful();
        } while (0);
        if (FAILED(ec)) {
            Loge(String("Got IOException while loading apns."));
        }
        // } catch (XmlPullParserException e) {
        //     Loge("Got XmlPullParserException while loading apns." + e);
        // } catch (IOException e) {
        //     Loge("Got IOException while loading apns." + e);
        // } catch (SQLException e) {
        //     Loge("Got SQLException while loading apns." + e);
        // } finally {
        db->EndTransaction();
        // }
    }
}

AutoPtr<IContentValues> CTelephonyProvider::DatabaseHelper::SetDefaultValue(
    /* [in] */ IContentValues* values)
{
    Boolean contains = FALSE;
    if (values->ContainsKey(ITelephonyCarriers::NAME, &contains), contains) {
        values->Put(ITelephonyCarriers::NAME, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::APN, &contains), contains) {
        values->Put(ITelephonyCarriers::APN, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::PORT, &contains), contains) {
        values->Put(ITelephonyCarriers::PORT, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::PROXY, &contains), contains) {
        values->Put(ITelephonyCarriers::PROXY, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::USER, &contains), contains) {
        values->Put(ITelephonyCarriers::USER, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::SERVER, &contains), contains) {
        values->Put(ITelephonyCarriers::SERVER, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::PASSWORD, &contains), contains) {
        values->Put(ITelephonyCarriers::PASSWORD, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::MMSPORT, &contains), contains) {
        values->Put(ITelephonyCarriers::MMSPORT, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::MMSPROXY, &contains), contains) {
        values->Put(ITelephonyCarriers::MMSPROXY, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::AUTH_TYPE, &contains), contains) {
        values->Put(ITelephonyCarriers::AUTH_TYPE, -1);
    }
    if (values->ContainsKey(ITelephonyCarriers::PROTOCOL, &contains), contains) {
        values->Put(ITelephonyCarriers::PROTOCOL, String("IP"));
    }
    if (values->ContainsKey(ITelephonyCarriers::ROAMING_PROTOCOL, &contains), contains) {
        values->Put(ITelephonyCarriers::ROAMING_PROTOCOL, String("IP"));
    }
    if (values->ContainsKey(ITelephonyCarriers::CARRIER_ENABLED, &contains), contains) {
        values->Put(ITelephonyCarriers::CARRIER_ENABLED, TRUE);
    }
    if (values->ContainsKey(ITelephonyCarriers::BEARER, &contains), contains) {
        values->Put(ITelephonyCarriers::BEARER, 0);
    }
    if (values->ContainsKey(ITelephonyCarriers::MVNO_TYPE, &contains), contains) {
        values->Put(ITelephonyCarriers::MVNO_TYPE, String(""));
    }
    if (values->ContainsKey(ITelephonyCarriers::MVNO_MATCH_DATA, &contains), contains) {
        values->Put(ITelephonyCarriers::MVNO_MATCH_DATA, String(""));
    }

    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Int64 subId = 0;
    sm->GetDefaultDataSubId(&subId);
    if (values->ContainsKey(ITelephonyCarriers::SUB_ID, &contains), contains) {
        values->Put(ITelephonyCarriers::SUB_ID, subId);
    }

    if (values->ContainsKey(ITelephonyCarriers::PROFILE_ID, &contains), contains) {
        values->Put(ITelephonyCarriers::PROFILE_ID, 0);
    }
    if (values->ContainsKey(ITelephonyCarriers::MODEM_COGNITIVE, &contains), contains) {
        values->Put(ITelephonyCarriers::MODEM_COGNITIVE, FALSE);
    }
    if (values->ContainsKey(ITelephonyCarriers::MAX_CONNS, &contains), contains) {
        values->Put(ITelephonyCarriers::MAX_CONNS, 0);
    }
    if (values->ContainsKey(ITelephonyCarriers::WAIT_TIME, &contains), contains) {
        values->Put(ITelephonyCarriers::WAIT_TIME, 0);
    }
    if (values->ContainsKey(ITelephonyCarriers::MAX_CONNS_TIME, &contains), contains) {
        values->Put(ITelephonyCarriers::MAX_CONNS_TIME, 0);
    }

    return values;
}

void CTelephonyProvider::DatabaseHelper::InsertAddingDefaults(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ IContentValues* row)
{
    row = SetDefaultValue(row);
    Int64 count = 0;
    db->Insert(CARRIERS_TABLE, String(NULL), row, &count);
}

CAR_OBJECT_IMPL(CTelephonyProvider)
ECode CTelephonyProvider::constructor()
{
    return ContentProvider::constructor();
}

ECode CTelephonyProvider::OnCreate(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    if (VDBG) Log(String("onCreate:+"));
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mOpenHelper = new DatabaseHelper(ctx, this);
    if (VDBG) Log(String("onCreate:- ret TRUE"));
    *succeeded = TRUE;
    return NOERROR;
}

void CTelephonyProvider::SetPreferredApnId(
    /* [in] */ IInteger64* id,
    /* [in] */ Int64 subId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    ctx->GetSharedPreferences(PREF_FILE + StringUtils::ToString(subId),
            IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    Int64 v = -1;
    if (id != NULL) {
        id->GetValue(&v);
    }
    editor->PutInt64(COLUMN_APN_ID, v);
    editor->Apply();
}

Int64 CTelephonyProvider::GetPreferredApnId(
    /* [in] */ Int64 subId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ISharedPreferences> sp;
    ctx->GetSharedPreferences(PREF_FILE + StringUtils::ToString(subId),
            IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    Int64 id = 0;
    sp->GetInt64(COLUMN_APN_ID, -1, &id);
    if (id == -1) {
        id = GetDefaultPreferredApnId();
        if (id > -1) {
            AutoPtr<IInteger64> v;
            CInteger64::New(id, (IInteger64**)&v);
            SetPreferredApnId(v, subId);
        }
    }
    return id;
}

Int64 CTelephonyProvider::GetDefaultPreferredApnId()
{
    Int64 id = -1;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    String configPref;
    AutoPtr<ArrayOf<IInterface*> > os = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(String(""), (ICharSequence**)&cs);
    os->Set(0, cs);
    res->GetString(R::string::config_preferred_apn, os, &configPref);
    if (!TextUtils::IsEmpty(configPref)) {
        AutoPtr<ArrayOf<String> > s;
        StringUtils::Split(configPref, String(","), (ArrayOf<String>**)&s);
        Int32 len = s->GetLength();
        if (len == 3 || len == 4) {
            AutoPtr<ISQLiteDatabase> db;
            mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
            AutoPtr<ArrayOf<String> > sv = ArrayOf<String>::Alloc(1);
            sv->Set(0, String("_id"));
            String selection = String("apn='") + (*s)[0] + "' AND mcc='" + (*s)[1] + "' AND mnc='" + (*s)[2] +
                    (len == 4 ? String("' AND protocol='") + (*s)[3] : String("")) + "'";
            AutoPtr<ICursor> c;
            db->Query(String("carriers"), sv, selection, NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
            Boolean tmp = FALSE;
            if (c->MoveToFirst(&tmp), tmp) {
                c->GetInt64(0, &id);
            }
            ICloseable::Probe(c)->Close();
        }
    }
    Logger::D(TAG, "Preferred APN: %lld", id);
    return id;
}

// @Override
ECode CTelephonyProvider::Query(
    /* [in] */ IUri* url,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sort,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> mTelephonyManager = ITelephonyManager::Probe(obj);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Int64 subId = 0;
    sm->GetDefaultDataSubId(&subId);
    String subIdString;
    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
    qb->SetStrict(TRUE); // a little protection from injection attacks
    qb->SetTables(String("carriers"));

    Int32 match;
    s_urlMatcher->Match(url, &match);
    switch (match) {
        case URL_TELEPHONY_USING_SUBID: {
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     return NULL;
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            String on;
            mTelephonyManager->GetIccOperatorNumeric(subId, &on);
            AutoPtr<ICharSequence> cs;
            CString::New(String("numeric = '") + on + "'", (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            // FIXME alter the selection to pass subId
            // selection = selection + "and subId = "
        }
        //intentional fall through from above case
        // do nothing
        case URL_TELEPHONY: {
            break;
        }

        case URL_CURRENT_USING_SUBID: {
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     return NULL;
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            // FIXME alter the selection to pass subId
            // selection = selection + "and subId = "
        }
        //intentional fall through from above case
        case URL_CURRENT: {
            AutoPtr<ICharSequence> cs;
            CString::New(String("current IS NOT NULL"), (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            // do not ignore the selection since MMS may use it.
            //selection = NULL;
            break;
        }

        case URL_ID: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            String s;
            ICharSequence::Probe(obj)->ToString(&s);
            s = String("_id = ") + s;
            AutoPtr<ICharSequence> cs;
            CString::New(s, (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }

        case URL_PREFERAPN_USING_SUBID:
        case URL_PREFERAPN_NO_UPDATE_USING_SUBID: {
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     return NULL;
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
        }
        //intentional fall through from above case
        case URL_PREFERAPN:
        case URL_PREFERAPN_NO_UPDATE: {
            String s = String("_id = ") + StringUtils::ToString(GetPreferredApnId(subId));
            AutoPtr<ICharSequence> cs;
            CString::New(s, (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }

        case URL_SIMINFO: {
            qb->SetTables(SIMINFO_TABLE);
            break;
        }

        default: {
            *result = NULL;
            return NOERROR;
        }
    }

    if (match != URL_SIMINFO) {
        if (projectionIn != NULL) {
            Int32 len = projectionIn->GetLength();
            for (Int32 i = 0; i < len; i++) {
                String column = (*projectionIn)[i];
                if (ITelephonyCarriers::TYPE.Equals(column) ||
                        ITelephonyCarriers::MMSC.Equals(column) ||
                        ITelephonyCarriers::MMSPROXY.Equals(column) ||
                        ITelephonyCarriers::MMSPORT.Equals(column) ||
                        ITelephonyCarriers::APN.Equals(column)) {
                    // noop
                }
                else {
                    FAIL_RETURN(CheckPermission());
                    break;
                }
            }
        }
        else {
            // NULL returns all columns, so need permission check
            FAIL_RETURN(CheckPermission());
        }
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> ret;
    if (FAILED(qb->Query(db, projectionIn, selection, selectionArgs, String(NULL), String(NULL), sort, (ICursor**)&ret))) {
        Loge(String("got exception when querying: "));
    }
    if (ret != NULL) {
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);
        ret->SetNotificationUri(cr, url);
    }
    *result = ret;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override
ECode CTelephonyProvider::GetType(
    /* [in] */ IUri* url,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 match = 0;
    s_urlMatcher->Match(url, &match);
    switch (match) {
        case URL_TELEPHONY:
        case URL_TELEPHONY_USING_SUBID: {
            *result = String("vnd.android.cursor.dir/telephony-carrier");
            return NOERROR;
        }

        case URL_ID: {
            *result = String("vnd.android.cursor.item/telephony-carrier");
            return NOERROR;
        }

        case URL_PREFERAPN_USING_SUBID:
        case URL_PREFERAPN_NO_UPDATE_USING_SUBID:
        case URL_PREFERAPN:
        case URL_PREFERAPN_NO_UPDATE: {
            *result = String("vnd.android.cursor.item/telephony-carrier");
            return NOERROR;
        }

        default: {
            // throw new IllegalArgumentException("Unknown URL " + url);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

// @Override
ECode CTelephonyProvider::Insert(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    AutoPtr<IUri> result;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Int64 subId = 0;
    sm->GetDefaultDataSubId(&subId);

    FAIL_RETURN(CheckPermission());

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 match = 0;
    s_urlMatcher->Match(url, &match);
    Boolean notify = FALSE;
    switch (match) {
        case URL_TELEPHONY_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     return result;
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
        }
        //intentional fall through from above case

        case URL_TELEPHONY: {
            AutoPtr<IContentValues> values;
            if (initialValues != NULL) {
                CContentValues::New(initialValues, (IContentValues**)&values);
            }
            else {
                CContentValues::New((IContentValues**)&values);
            }

            values = DatabaseHelper::SetDefaultValue(values);

            Int64 rowID = 0;
            db->Insert(CARRIERS_TABLE, String(NULL), values, &rowID);
            if (rowID > 0) {
                AutoPtr<IContentUris> cu;
                CContentUris::AcquireSingleton((IContentUris**)&cu);
                AutoPtr<ITelephonyCarriers> tc;
                CTelephonyCarriers::AcquireSingleton((ITelephonyCarriers**)&tc);
                AutoPtr<IUri> uu;
                tc->GetCONTENT_URI((IUri**)&uu);
                cu->WithAppendedId(uu, rowID, (IUri**)&result);
                notify = TRUE;
            }

            if (VDBG) Log(String("inserted ") + TO_CSTR(values) + " rowID = " + StringUtils::ToString(rowID));
            break;
        }

        case URL_CURRENT_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     return result;
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            // FIXME use subId in the query
        }
        //intentional fall through from above case

        case URL_CURRENT: {
            // NULL out the previous operator
            Int32 tv = 0;
            db->Update(String("carriers"), s_currentNullMap, String("current IS NOT NULL"), NULL, &tv);

            String numeric;
            initialValues->GetAsString(String("numeric"), &numeric);
            Int32 updated = 0;
            db->Update(String("carriers"), s_currentSetMap,
                    String("numeric = '") + numeric + "'", NULL, &updated);

            if (updated > 0) {
                if (VDBG) Log(String("Setting numeric '") + numeric + "' to be the current operator");
            }
            else {
                Loge(String("Failed setting numeric '") + numeric + "' to the current operator");
            }
            break;
        }

        case URL_PREFERAPN_USING_SUBID:
        case URL_PREFERAPN_NO_UPDATE_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     return result;
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
        }
        //intentional fall through from above case

        case URL_PREFERAPN:
        case URL_PREFERAPN_NO_UPDATE: {
            if (initialValues != NULL) {
                Boolean tmp = FALSE;
                if (initialValues->ContainsKey(COLUMN_APN_ID, &tmp), tmp) {
                    AutoPtr<IInteger64> obj;
                    initialValues->GetAsInteger64(COLUMN_APN_ID, (IInteger64**)&obj);
                    SetPreferredApnId(obj, subId);
                }
            }
            break;
        }

        case URL_SIMINFO: {
            Int64 id = 0;
            db->Insert(SIMINFO_TABLE, String(NULL), initialValues, &id);
            AutoPtr<IContentUris> cu;
            CContentUris::AcquireSingleton((IContentUris**)&cu);

            AutoPtr<ISubscriptionManager> sm;
            CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
            AutoPtr<IUri> uu;
            sm->GetCONTENT_URI((IUri**)&uu);
            cu->WithAppendedId(uu, id, (IUri**)&result);
            break;
        }
    }

    if (notify) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ITelephonyCarriers> tc;
        CTelephonyCarriers::AcquireSingleton((ITelephonyCarriers**)&tc);
        AutoPtr<IUri> uu;
        tc->GetCONTENT_URI((IUri**)&uu);
        cr->NotifyChange(uu, NULL, TRUE, IUserHandle::USER_ALL);
    }

    *uri = result;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

// @Override
ECode CTelephonyProvider::Delete(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count = 0;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Int64 subId = 0;
    sm->GetDefaultDataSubId(&subId);

    FAIL_RETURN(CheckPermission());

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 match = 0;
    s_urlMatcher->Match(url, &match);
    switch (match) {
        case URL_TELEPHONY_USING_SUBID: {
             String subIdString;
             url->GetLastPathSegment(&subIdString);
             // try {
                 subId = StringUtils::ParseInt64(subIdString);
             // } catch (NumberFormatException e) {
             //     Loge("NumberFormatException" + e);
             //     throw new IllegalArgumentException("Invalid subId " + url);
             // }
             if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            // FIXME use subId in query
        }
        //intentional fall through from above case

        case URL_TELEPHONY: {
            db->Delete(CARRIERS_TABLE, where, whereArgs, &count);
            break;
        }

        case URL_CURRENT_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     throw new IllegalArgumentException("Invalid subId " + url);
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            // FIXME use subId in query
        }
        //intentional fall through from above case

        case URL_CURRENT: {
            db->Delete(CARRIERS_TABLE, where, whereArgs, &count);
            break;
        }

        case URL_ID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            AutoPtr<ArrayOf<String> > sv = ArrayOf<String>::Alloc(1);
            sv->Set(0, subIdString);
            db->Delete(CARRIERS_TABLE, IBaseColumns::ID + "=?", sv, &count);
            break;
        }

        case URL_RESTOREAPN_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     throw new IllegalArgumentException("Invalid subId " + url);
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            // FIXME use subId in query
        }
        case URL_RESTOREAPN: {
            count = 1;
            RestoreDefaultAPN(subId);
            break;
        }

        case URL_PREFERAPN_USING_SUBID:
        case URL_PREFERAPN_NO_UPDATE_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     throw new IllegalArgumentException("Invalid subId " + url);
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
        }
        //intentional fall through from above case

        case URL_PREFERAPN:
        case URL_PREFERAPN_NO_UPDATE: {
            AutoPtr<IInteger64> obj;
            CInteger64::New(-1, (IInteger64**)&obj);
            SetPreferredApnId(obj, subId);
            if ((match == URL_PREFERAPN) || (match == URL_PREFERAPN_USING_SUBID)) count = 1;
            break;
        }

        case URL_SIMINFO: {
            db->Delete(SIMINFO_TABLE, where, whereArgs, &count);
            break;
        }

        default: {
            *result = 0;
            // throw new UnsupportedOperationException("Cannot delete that URL: " + url);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    if (count > 0) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);

        AutoPtr<ITelephonyCarriers> tc;
        CTelephonyCarriers::AcquireSingleton((ITelephonyCarriers**)&tc);
        AutoPtr<IUri> uu;
        tc->GetCONTENT_URI((IUri**)&uu);
        cr->NotifyChange(uu, NULL, TRUE, IUserHandle::USER_ALL);
    }

    *result = count;
    return NOERROR;
}

// @Override
ECode CTelephonyProvider::Update(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count = 0;
    Int32 uriType = URL_UNKNOWN;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Int64 subId = 0;
    sm->GetDefaultDataSubId(&subId);

    FAIL_RETURN(CheckPermission());

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 match = 0;
    s_urlMatcher->Match(url, &match);
    switch (match) {
        case URL_TELEPHONY_USING_SUBID: {
             String subIdString;
             url->GetLastPathSegment(&subIdString);
             // try {
                 subId = StringUtils::ParseInt64(subIdString);
             // } catch (NumberFormatException e) {
             //     Loge("NumberFormatException" + e);
             //     throw new IllegalArgumentException("Invalid subId " + url);
             // }
             if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            //FIXME use subId in the query
        }
        //intentional fall through from above case

        case URL_TELEPHONY: {
            db->Update(CARRIERS_TABLE, values, where, whereArgs, &count);
            break;
        }

        case URL_CURRENT_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     throw new IllegalArgumentException("Invalid subId " + url);
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
            //FIXME use subId in the query
        }
        //intentional fall through from above case

        case URL_CURRENT: {
            db->Update(CARRIERS_TABLE, values, where, whereArgs, &count);
            break;
        }

        case URL_ID: {
            if (where != NULL || whereArgs != NULL) {
                // throw new UnsupportedOperationException(
                //         "Cannot update URL " + url + " with a where clause");
                *result = 0;
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            }

            String subIdString;
            url->GetLastPathSegment(&subIdString);
            AutoPtr<ArrayOf<String> > sv = ArrayOf<String>::Alloc(1);
            sv->Set(0, subIdString);
            db->Update(CARRIERS_TABLE, values, IBaseColumns::ID + "=?", sv, &count);
            break;
        }

        case URL_PREFERAPN_USING_SUBID:
        case URL_PREFERAPN_NO_UPDATE_USING_SUBID: {
            String subIdString;
            url->GetLastPathSegment(&subIdString);
            // try {
                subId = StringUtils::ParseInt64(subIdString);
            // } catch (NumberFormatException e) {
            //     Loge("NumberFormatException" + e);
            //     throw new IllegalArgumentException("Invalid subId " + url);
            // }
            if (DBG) Log(String("subIdString = ") + subIdString + " subId = " + StringUtils::ToString(subId));
        }

        case URL_PREFERAPN:
        case URL_PREFERAPN_NO_UPDATE: {
            if (values != NULL) {
                Boolean tmp = FALSE;
                if (values->ContainsKey(COLUMN_APN_ID, &tmp), tmp) {
                    AutoPtr<IInteger64> obj;
                    values->GetAsInteger64(COLUMN_APN_ID, (IInteger64**)&obj);
                    SetPreferredApnId(obj, subId);
                    if ((match == URL_PREFERAPN) ||
                            (match == URL_PREFERAPN_USING_SUBID)) {
                        count = 1;
                    }
                }
            }
            break;
        }

        case URL_SIMINFO: {
            db->Update(SIMINFO_TABLE, values, where, whereArgs, &count);
            uriType = URL_SIMINFO;
            break;
        }

        default: {
            *result = 0;
            // throw new UnsupportedOperationException("Cannot update that URL: " + url);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    if (count > 0) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);
        switch (uriType) {
            case URL_SIMINFO: {
                AutoPtr<ISubscriptionManager> sm;
                CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
                AutoPtr<IUri> uu;
                sm->GetCONTENT_URI((IUri**)&uu);
                cr->NotifyChange(uu, NULL, TRUE, IUserHandle::USER_ALL);
                break;
            }
            default: {
                AutoPtr<ITelephonyCarriers> tc;
                CTelephonyCarriers::AcquireSingleton((ITelephonyCarriers**)&tc);
                AutoPtr<IUri> uu;
                tc->GetCONTENT_URI((IUri**)&uu);
                cr->NotifyChange(uu, NULL, TRUE, IUserHandle::USER_ALL);
            }
        }
    }

    *result = count;
    return NOERROR;
}

ECode CTelephonyProvider::CheckPermission()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    Int32 status = 0;
    ctx->CheckCallingOrSelfPermission(String("android.permission.WRITE_APN_SETTINGS"), &status);
    if (status == IPackageManager::PERMISSION_GRANTED) {
        return NOERROR;
    }

    AutoPtr<IPackageManager> packageManager;
    ctx->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<ArrayOf<String> > packages;
    packageManager->GetPackagesForUid(Binder::GetCallingUid(), (ArrayOf<String>**)&packages);

    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> telephonyManager = ITelephonyManager::Probe(obj);
    Int32 len = packages->GetLength();
    for (Int32 i = 0; i < len; i++) {
        String pkg = (*packages)[i];
        Int32 flag = 0;
        telephonyManager->CheckCarrierPrivilegesForPackage(pkg, &flag);
        if (flag == ITelephonyManager::CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
            return NOERROR;
        }
    }
    // throw new SecurityException("No permission to write APN settings");
    return E_SECURITY_EXCEPTION;
}

void CTelephonyProvider::RestoreDefaultAPN(
    /* [in] */ Int64 subId)
{
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    Int32 count = 0;
    if (FAILED(db->Delete(CARRIERS_TABLE, String(NULL), NULL, &count))) {
        Loge(String("got exception when deleting to restore: "));
    }

    AutoPtr<IInteger64> obj;
    CInteger64::New(-1, (IInteger64**)&obj);
    SetPreferredApnId(obj, subId);
    mOpenHelper->InitDatabase(db);

    obj = NULL;
    CInteger64::New(GetDefaultPreferredApnId(), (IInteger64**)&obj);
    SetPreferredApnId(obj, subId);
}

/**
 * Log with debug
 *
 * @param s is string Log
 */
void CTelephonyProvider::Log(
    /* [in] */ const String& s)
{
    Logger::D(TAG, s);
}

void CTelephonyProvider::Loge(
    /* [in] */ const String& s)
{
    Logger::E(TAG, s);
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
