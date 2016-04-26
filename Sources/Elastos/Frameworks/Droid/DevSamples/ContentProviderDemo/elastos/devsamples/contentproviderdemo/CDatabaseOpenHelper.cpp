#include "CDatabaseOpenHelper.h"
#include "Utils.h"
#include <Elastos.Droid.Content.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

static const String TAG("ContentProviderDemo::CDatabaseOpenHelper");

CAR_OBJECT_IMPL(CDatabaseOpenHelper)

ECode CDatabaseOpenHelper::constructor(
    /* [in] */ IContext* context)
{
    return SQLiteOpenHelper::constructor(context, Utils::DB_NAME, NULL, Utils::VERSION);
}

ECode CDatabaseOpenHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    Logger::I(TAG, "OnCreate");
    assert(db != NULL && "CDatabaseOpenHelper::OnCreate db is NULL.");

    StringBuilder sb("create table ");
    sb += Utils::TABLE_NAME;
    sb += "(";
    sb += Utils::TAG_ID;
    sb += " integer primary key autoincrement not null,";
    sb += Utils::USERNAME;
    sb += " text not null,";
    sb += Utils::SEX;
    sb += " text not null,";
    sb += Utils::EMAIL;
    sb += " text not null);";

    String sql = sb.ToString();
    ECode ec = db->ExecSQL(sql);
    Logger::I(TAG, "db ExecSQL %s, ec=%08x", sql.string(), ec);
    return ec;
}

ECode CDatabaseOpenHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    Logger::I(TAG, "OnUpgrade");
    return NOERROR;
}

ECode CDatabaseOpenHelper::Add(
    /* [in] */ const String& username,
    /* [in] */ const String& sex,
    /* [in] */ const String& email)
{
    Logger::I(TAG, "Add [%s, %s, %s]", username.string(), sex.string(), email.string());

    AutoPtr<ISQLiteDatabase> db;
    GetWritableDatabase((ISQLiteDatabase**)&db);
    assert(db != NULL);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(Utils::USERNAME, username);
    values->Put(Utils::SEX, sex);
    values->Put(Utils::EMAIL, email);

    String nullColumnHack;
    Int64 ival;
    ECode ec = db->Insert(Utils::TABLE_NAME, nullColumnHack, values, &ival);
    Logger::I(TAG, "db Insert %s, result:%lld, ec=%08x", TO_CSTR(values), ival, ec);
    return ec;
}


} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos