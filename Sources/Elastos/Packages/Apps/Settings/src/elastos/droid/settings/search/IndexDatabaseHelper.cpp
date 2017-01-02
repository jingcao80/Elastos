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

#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/search/IndexDatabaseHelper.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "_Elastos.Droid.Settings.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::Build;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

const String IndexDatabaseHelper::TAG("IndexDatabaseHelper");

const String IndexDatabaseHelper::DATABASE_NAME("search_index.db");
const Int32 IndexDatabaseHelper::DATABASE_VERSION = 115;

static String InitCREATE_INDEX_TABLE()
{
    StringBuilder builder;
    builder += "CREATE VIRTUAL TABLE ";
    builder += IIndexDatabaseHelperTables::TABLE_PREFS_INDEX;
    builder += " USING fts4";
    builder += "(";
    builder += IIndexDatabaseHelperIndexColumns::LOCALE;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_RANK;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_TITLE;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_TITLE_NORMALIZED;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_ON;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_ON_NORMALIZED;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_OFF;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_SUMMARY_OFF_NORMALIZED;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_ENTRIES;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_KEYWORDS;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::SCREEN_TITLE;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::CLASS_NAME;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::ICON;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::INTENT_ACTION;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::INTENT_TARGET_PACKAGE;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::INTENT_TARGET_CLASS;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::ENABLED;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::DATA_KEY_REF;
    builder += ", ";
    builder += IIndexDatabaseHelperIndexColumns::USER_ID;
    builder += ");";

    return builder.ToString();
}

const String IndexDatabaseHelper::CREATE_INDEX_TABLE = InitCREATE_INDEX_TABLE();

static String InitCREATE_META_TABLE()
{
    StringBuilder builder;
    builder += "CREATE TABLE ";
    builder += IIndexDatabaseHelperTables::TABLE_META_INDEX;
    builder += "(";
    builder += IIndexDatabaseHelperMetaColumns::BUILD;
    builder += " VARCHAR(32) NOT NULL";
    builder += ")";

    return builder.ToString();
}

const String IndexDatabaseHelper::CREATE_META_TABLE = InitCREATE_META_TABLE();

static String InitCREATE_SAVED_QUERIES_TABLE()
{
    StringBuilder builder;
    builder += "CREATE TABLE ";
    builder += IIndexDatabaseHelperTables::TABLE_SAVED_QUERIES;
    builder += "(";
    builder += IIndexDatabaseHelperSavedQueriesColums::QUERY;
    builder += " VARCHAR(64) NOT NULL";
    builder += ", ";
    builder += IIndexDatabaseHelperSavedQueriesColums::TIME_STAMP;
    builder += " INTEGER";
    builder += ")";

    return builder.ToString();
}

const String IndexDatabaseHelper::CREATE_SAVED_QUERIES_TABLE = InitCREATE_SAVED_QUERIES_TABLE();

static String InitINSERT_BUILD_VERSION()
{
    StringBuilder builder;
    builder += "INSERT INTO ";
    builder += IIndexDatabaseHelperTables::TABLE_META_INDEX;
    builder += " VALUES ('";
    builder += Build::VERSION::INCREMENTAL;
    builder += "');";

    return builder.ToString();
}

const String IndexDatabaseHelper::INSERT_BUILD_VERSION = InitINSERT_BUILD_VERSION();

static String InitSELECT_BUILD_VERSION()
{
    StringBuilder builder;
    builder += "SELECT ";
    builder += IIndexDatabaseHelperMetaColumns::BUILD;
    builder += " FROM ";
    builder += IIndexDatabaseHelperTables::TABLE_META_INDEX;
    builder += " LIMIT 1;";

    return builder.ToString();
}

const String IndexDatabaseHelper::SELECT_BUILD_VERSION = InitSELECT_BUILD_VERSION();

AutoPtr<IndexDatabaseHelper> IndexDatabaseHelper::sSingleton;

Object IndexDatabaseHelper::sLock;

IndexDatabaseHelper::IndexDatabaseHelper()
{}

IndexDatabaseHelper::~IndexDatabaseHelper()
{}

ECode IndexDatabaseHelper::constructor(
    /* [in] */ IContext* context)
{
    return SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, DATABASE_VERSION);
}

AutoPtr<IndexDatabaseHelper> IndexDatabaseHelper::GetInstance(
    /* [in] */ IContext* context)
{
    {    AutoLock syncLock(sLock);
        if (sSingleton == NULL) {
            sSingleton = new IndexDatabaseHelper();
            sSingleton->constructor(context);
        }
        return sSingleton;
    }
    return sSingleton;
}

ECode IndexDatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    BootstrapDB(db);
    return NOERROR;
}

void IndexDatabaseHelper::BootstrapDB(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(CREATE_INDEX_TABLE);
    db->ExecSQL(CREATE_META_TABLE);
    db->ExecSQL(CREATE_SAVED_QUERIES_TABLE);
    db->ExecSQL(INSERT_BUILD_VERSION);
    Logger::I(TAG, "Bootstrapped database");
}

ECode IndexDatabaseHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    SQLiteOpenHelper::OnOpen(db);

    Int32 version;
    db->GetVersion(&version);
    Logger::I(TAG, "Using schema version: %d", version);

    String str;
    GetBuildVersion(db, &str);
    if (!Build::VERSION::INCREMENTAL.Equals(str)) {
        Logger::W(TAG, "Index needs to be rebuilt as build-version is not the same");
        // We need to drop the tables and recreate them
        Reconstruct(db);
    }
    else {
        Logger::I(TAG, "Index is fine");
    }
    return NOERROR;
}

ECode IndexDatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    if (oldVersion < DATABASE_VERSION) {
        Logger::W(TAG, "Detected schema version '%d'. Index needs to be rebuilt for schema version '%d'.",
                oldVersion, newVersion);
        // We need to drop the tables and recreate them
        Reconstruct(db);
    }
    return NOERROR;
}

ECode IndexDatabaseHelper::OnDowngrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    Logger::W(TAG, "Detected schema version '%d'. Index needs to be rebuilt for schema version '%d'.",
            oldVersion, newVersion);
    // We need to drop the tables and recreate them
    Reconstruct(db);
    return NOERROR;
}

void IndexDatabaseHelper::Reconstruct(
    /* [in] */ ISQLiteDatabase* db)
{
    DropTables(db);
    BootstrapDB(db);
}

ECode IndexDatabaseHelper::GetBuildVersion(
    /* [in] */ ISQLiteDatabase* db,
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version);
    *version = String(NULL);

    AutoPtr<ICursor> cursor;
    // try {
    ECode ec = db->RawQuery(SELECT_BUILD_VERSION, NULL, (ICursor**)&cursor);
    if (FAILED(ec)) {
        Logger::E(TAG, "Cannot get build version from Index metadata");
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
        return NOERROR;
    }
    Boolean res;
    if (cursor->MoveToFirst(&res), res) {
        cursor->GetString(0, version);
    }
    // }
    // catch (Exception e) {
    //     Logger::E(TAG, "Cannot get build version from Index metadata");
    // }
    // finally {
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    // }
    return NOERROR;
}

void IndexDatabaseHelper::DropTables(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IIndexDatabaseHelperTables::TABLE_META_INDEX);
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IIndexDatabaseHelperTables::TABLE_PREFS_INDEX);
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IIndexDatabaseHelperTables::TABLE_SAVED_QUERIES);
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos
