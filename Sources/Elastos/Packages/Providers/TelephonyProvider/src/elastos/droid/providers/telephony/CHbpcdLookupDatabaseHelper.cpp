#include "elastos/droid/providers/telephony/CHbpcdLookupDatabaseHelper.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupArbitraryMccSidMatch;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccIdd;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccLookup;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccSidConflicts;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccSidRange;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupNanpAreaCode;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

const Boolean CHbpcdLookupDatabaseHelper::DEBUG = TRUE;

const String CHbpcdLookupDatabaseHelper::TAG("HbpcdLookupDatabaseHelper");
const String CHbpcdLookupDatabaseHelper::DATABASE_NAME("HbpcdLookup.db");
const Int32 CHbpcdLookupDatabaseHelper::DATABASE_VERSION;

CAR_OBJECT_IMPL(CHbpcdLookupDatabaseHelper)

CHbpcdLookupDatabaseHelper::CHbpcdLookupDatabaseHelper()
{}

ECode CHbpcdLookupDatabaseHelper::constructor()
{
    return NOERROR;
}

ECode CHbpcdLookupDatabaseHelper::constructor(
        /* [in] */ IContext* context)
{
    SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, DATABASE_VERSION);

    mContext = context;
    return NOERROR;
}

ECode CHbpcdLookupDatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    //set up the database schema
    // 1 MCC may has more IDDs
    db->ExecSQL(String("CREATE TABLE ") + IHbpcdLookupProvider::TABLE_MCC_IDD +
        "(_id INTEGER PRIMARY KEY," +
            "MCC INTEGER," +
            "IDD TEXT);");

    db->ExecSQL(String("CREATE TABLE ") + IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE +
        "(_id INTEGER PRIMARY KEY," +
            "MCC INTEGER," +
            "Country_Code TEXT," +
            "Country_Name TEXT," +
            "NDD TEXT," +
            "NANPS BOOLEAN," +
            "GMT_Offset_Low REAL," +
            "GMT_Offset_High REAL," +
            "GMT_DST_Low REAL," +
            "GMT_DST_High REAL);");

    db->ExecSQL(String("CREATE TABLE ") + IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT +
        "(_id INTEGER PRIMARY KEY," +
            "MCC INTEGER," +
            "SID_Conflict INTEGER);");

    db->ExecSQL(String("CREATE TABLE ") + IHbpcdLookupProvider::TABLE_MCC_SID_RANGE +
        "(_id INTEGER PRIMARY KEY," +
            "MCC INTEGER," +
            "SID_Range_Low INTEGER," +
            "SID_Range_High INTEGER);");

    db->ExecSQL(String("CREATE TABLE ") + IHbpcdLookupProvider::TABLE_NANP_AREA_CODE +
        "(_id INTEGER PRIMARY KEY," +
            "AREA_CODE INTEGER UNIQUE);");

    db->ExecSQL(String("CREATE TABLE ") + IHbpcdLookupProvider::TABLE_ARBITRARY_MCC_SID_MATCH +
        "(_id INTEGER PRIMARY KEY," +
            "MCC INTEGER," +
            "SID INTEGER UNIQUE);");

    InitDatabase(db);
    return NOERROR;
}

ECode CHbpcdLookupDatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    // do nothing
    return NOERROR;
}

void CHbpcdLookupDatabaseHelper::InitDatabase(
    /* [in] */ ISQLiteDatabase* db)
{
    // Read internal data from xml
    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    AutoPtr<IXmlResourceParser> parser;
    r->GetXml(R::xml::hbpcd_lookup_tables, (IXmlResourceParser**)&parser);

    if (parser == NULL) {
       Logger::E (TAG, "error to load the HBPCD resource");
    }
    else {
        // try {
        db->BeginTransaction();
        XmlUtils::BeginDocument(IXmlPullParser::Probe(parser), String("hbpcd_info"));

        Int32 eventType;
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        String tagName;
        IXmlPullParser::Probe(parser)->GetName(&tagName);

        while (eventType != IXmlPullParser::END_DOCUMENT) {
            if (eventType == IXmlPullParser::START_TAG
                    && tagName.EqualsIgnoreCase("table")) {
                String tableName;
                IXmlPullParser::Probe(parser)->GetAttributeValue(String(NULL), String("name"), &tableName);
                LoadTable(db, IXmlPullParser::Probe(parser), tableName);
            }
            Int32 val;
            IXmlPullParser::Probe(parser)->Next(&val);
            IXmlPullParser::Probe(parser)->GetEventType(&eventType);
            IXmlPullParser::Probe(parser)->GetName(&tagName);
        }
        db->SetTransactionSuccessful();
        // } catch (XmlPullParserException e) {
        //     Logger::E (TAG, "Got XmlPullParserException when load hbpcd info");
        // } catch (IOException e) {
        //     Logger::E (TAG, "Got IOException when load hbpcd info");
        // } catch (SQLException e) {
        //     Logger::E (TAG, "Got SQLException when load hbpcd info");
        // } finally {
        db->EndTransaction();
        ICloseable::Probe(parser)->Close();
        // }
    }
}

void CHbpcdLookupDatabaseHelper::LoadTable(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& tableName)
{
    Int32 eventType;
    IXmlPullParser::Probe(parser)->GetEventType(&eventType);
    String tagName;
    IXmlPullParser::Probe(parser)->GetName(&tagName);
    while (!(eventType == IXmlPullParser::END_TAG
            && tagName.EqualsIgnoreCase("table"))) {
        AutoPtr<IContentValues> row;
        if (tableName.EqualsIgnoreCase(IHbpcdLookupProvider::TABLE_MCC_IDD)) {
            row = GetTableMccIddRow(parser);
        }
        else if (tableName.EqualsIgnoreCase(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE)) {
            row = GetTableMccLookupTableRow(parser);
        }
        else if (tableName.EqualsIgnoreCase(IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT)) {
            row = GetTableMccSidConflictRow(parser);
        }
        else if (tableName.EqualsIgnoreCase(IHbpcdLookupProvider::TABLE_MCC_SID_RANGE)) {
            row = GetTableMccSidRangeRow(parser);
        }
        else if (tableName.EqualsIgnoreCase(IHbpcdLookupProvider::TABLE_NANP_AREA_CODE)) {
            row = GetTableNanpAreaCodeRow(parser);
        }
        else if (tableName.EqualsIgnoreCase(
                IHbpcdLookupProvider::TABLE_ARBITRARY_MCC_SID_MATCH)) {
            row = GetTableArbitraryMccSidMatch(parser);
        }
        else {
            Logger::E(TAG, "unrecognized table name %s", tableName.string());
            break;
        }
        if (row != NULL) {
            Int64 val;
            db->Insert(tableName, String(NULL), row, &val);
        }
        Int32 val;
        IXmlPullParser::Probe(parser)->Next(&val);
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        IXmlPullParser::Probe(parser)->GetName(&tagName);
    }
}

AutoPtr<IContentValues> CHbpcdLookupDatabaseHelper::GetTableMccIddRow(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 eventType;
    IXmlPullParser::Probe(parser)->GetEventType(&eventType);
    String tagName;
    IXmlPullParser::Probe(parser)->GetName(&tagName);
    AutoPtr<IContentValues> row;
    CContentValues::New((IContentValues**)&row);

    while (!(eventType == IXmlPullParser::END_TAG && tagName.EqualsIgnoreCase("row"))) {
        if (eventType == IXmlPullParser::START_TAG) {
            String nextText;
            parser->NextText(&nextText);
            if (tagName.EqualsIgnoreCase(IHbpcdLookupMccIdd::MCC)) {
                row->Put(IHbpcdLookupMccIdd::MCC, StringUtils::ParseInt32(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccIdd::IDD)) {
                row->Put(IHbpcdLookupMccIdd::IDD, nextText);
            }
        }
        Int32 val;
        IXmlPullParser::Probe(parser)->Next(&val);
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        IXmlPullParser::Probe(parser)->GetName(&tagName);
    }
    return row;
}

AutoPtr<IContentValues> CHbpcdLookupDatabaseHelper::GetTableMccLookupTableRow(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 eventType;
    IXmlPullParser::Probe(parser)->GetEventType(&eventType);
    String tagName;
    IXmlPullParser::Probe(parser)->GetName(&tagName);
    AutoPtr<IContentValues> row;
    CContentValues::New((IContentValues**)&row);

    while (!(eventType == IXmlPullParser::END_TAG && tagName.EqualsIgnoreCase("row"))) {
        if (eventType == IXmlPullParser::START_TAG) {
            String nextText;
            parser->NextText(&nextText);
            if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::MCC)) {
                row->Put(IHbpcdLookupMccLookup::MCC, StringUtils::ParseInt32(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::COUNTRY_CODE)) {
                row->Put(IHbpcdLookupMccLookup::COUNTRY_CODE, StringUtils::ParseInt32(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::COUNTRY_NAME)) {
                row->Put(IHbpcdLookupMccLookup::COUNTRY_NAME, nextText);
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::NDD)) {
                row->Put(IHbpcdLookupMccLookup::NDD, nextText);
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::NANPS)) {
                row->Put(IHbpcdLookupMccLookup::NANPS, StringUtils::ParseBoolean(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::GMT_OFFSET_LOW)) {
                row->Put(IHbpcdLookupMccLookup::GMT_OFFSET_LOW, StringUtils::ParseFloat(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::GMT_OFFSET_HIGH)) {
                row->Put(IHbpcdLookupMccLookup::GMT_OFFSET_HIGH, StringUtils::ParseFloat(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::GMT_DST_LOW)) {
                row->Put(IHbpcdLookupMccLookup::GMT_DST_LOW, StringUtils::ParseFloat(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccLookup::GMT_DST_HIGH)) {
                row->Put(IHbpcdLookupMccLookup::GMT_DST_HIGH, StringUtils::ParseFloat(nextText));
            }
        }
        Int32 val;
        IXmlPullParser::Probe(parser)->Next(&val);
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        IXmlPullParser::Probe(parser)->GetName(&tagName);
    }
    return row;
}

AutoPtr<IContentValues> CHbpcdLookupDatabaseHelper::GetTableMccSidConflictRow(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 eventType;
    IXmlPullParser::Probe(parser)->GetEventType(&eventType);
    String tagName;
    IXmlPullParser::Probe(parser)->GetName(&tagName);
    AutoPtr<IContentValues> row;
    CContentValues::New((IContentValues**)&row);

    while (!(eventType == IXmlPullParser::END_TAG && tagName.EqualsIgnoreCase("row"))) {
        if (eventType == IXmlPullParser::START_TAG) {
            String nextText;
            parser->NextText(&nextText);
            if (tagName.EqualsIgnoreCase(IHbpcdLookupMccSidConflicts::MCC)) {
                row->Put(IHbpcdLookupMccSidConflicts::MCC, StringUtils::ParseInt32(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccSidConflicts::SID_CONFLICT)) {
                row->Put(IHbpcdLookupMccSidConflicts::SID_CONFLICT, StringUtils::ParseInt32(nextText));
            }
        }
        Int32 val;
        IXmlPullParser::Probe(parser)->Next(&val);
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        IXmlPullParser::Probe(parser)->GetName(&tagName);
    }
    return row;
}

AutoPtr<IContentValues> CHbpcdLookupDatabaseHelper::GetTableMccSidRangeRow(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 eventType;
    IXmlPullParser::Probe(parser)->GetEventType(&eventType);
    String tagName;
    IXmlPullParser::Probe(parser)->GetName(&tagName);
    AutoPtr<IContentValues> row;
    CContentValues::New((IContentValues**)&row);

    while (!(eventType == IXmlPullParser::END_TAG && tagName.EqualsIgnoreCase("row"))) {
        if (eventType == IXmlPullParser::START_TAG) {
           String nextText;
           parser->NextText(&nextText);
            if (tagName.EqualsIgnoreCase(IHbpcdLookupMccSidRange::MCC)) {
                row->Put(IHbpcdLookupMccSidRange::MCC, StringUtils::ParseInt32(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccSidRange::RANGE_LOW)) {
                row->Put(IHbpcdLookupMccSidRange::RANGE_LOW, StringUtils::ParseInt32(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupMccSidRange::RANGE_HIGH)) {
                row->Put(IHbpcdLookupMccSidRange::RANGE_HIGH, StringUtils::ParseInt32(nextText));
            }
        }
        Int32 val;
        IXmlPullParser::Probe(parser)->Next(&val);
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        IXmlPullParser::Probe(parser)->GetName(&tagName);
    }
    return row;
}

AutoPtr<IContentValues> CHbpcdLookupDatabaseHelper::GetTableNanpAreaCodeRow(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 eventType;
    IXmlPullParser::Probe(parser)->GetEventType(&eventType);
    String tagName;
    IXmlPullParser::Probe(parser)->GetName(&tagName);
    AutoPtr<IContentValues> row;
    CContentValues::New((IContentValues**)&row);

    while (!(eventType == IXmlPullParser::END_TAG && tagName.EqualsIgnoreCase("row"))) {
        if (eventType == IXmlPullParser::START_TAG) {
            if (tagName.EqualsIgnoreCase(IHbpcdLookupNanpAreaCode::AREA_CODE)) {
                String nextText;
                parser->NextText(&nextText);
                row->Put(IHbpcdLookupNanpAreaCode::AREA_CODE, StringUtils::ParseInt32(nextText));
            }
        }
        Int32 val;
        IXmlPullParser::Probe(parser)->Next(&val);
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        IXmlPullParser::Probe(parser)->GetName(&tagName);
    }
    return row;
}

AutoPtr<IContentValues> CHbpcdLookupDatabaseHelper::GetTableArbitraryMccSidMatch(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 eventType;
    IXmlPullParser::Probe(parser)->GetEventType(&eventType);
    String tagName;
    IXmlPullParser::Probe(parser)->GetName(&tagName);
    AutoPtr<IContentValues> row;
    CContentValues::New((IContentValues**)&row);

    while (!(eventType == IXmlPullParser::END_TAG && tagName.EqualsIgnoreCase("row"))) {
        if (eventType == IXmlPullParser::START_TAG) {
            String nextText;
            parser->NextText(&nextText);
            if (tagName.EqualsIgnoreCase(IHbpcdLookupArbitraryMccSidMatch::MCC)) {
                row->Put(IHbpcdLookupArbitraryMccSidMatch::MCC, StringUtils::ParseInt32(nextText));
            }
            else if (tagName.EqualsIgnoreCase(IHbpcdLookupArbitraryMccSidMatch::SID)) {
                row->Put(IHbpcdLookupArbitraryMccSidMatch::SID, StringUtils::ParseInt32(nextText));
            }
        }
        Int32 val;
        IXmlPullParser::Probe(parser)->Next(&val);
        IXmlPullParser::Probe(parser)->GetEventType(&eventType);
        IXmlPullParser::Probe(parser)->GetName(&tagName);
    }
    return row;
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
