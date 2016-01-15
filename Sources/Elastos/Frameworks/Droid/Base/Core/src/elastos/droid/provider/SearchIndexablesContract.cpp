
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"

using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Provider {
//============================================================================
//              SearchIndexablesContract::BaseColumns
//============================================================================
const String SearchIndexablesContract::BaseColumns::COLUMN_RANK("rank");
const String SearchIndexablesContract::BaseColumns::COLUMN_CLASS_NAME("className");
const String SearchIndexablesContract::BaseColumns::COLUMN_ICON_RESID("iconResId");
const String SearchIndexablesContract::BaseColumns::COLUMN_INTENT_ACTION("intentAction");
const String SearchIndexablesContract::BaseColumns::COLUMN_INTENT_TARGET_PACKAGE("intentTargetPackage");
const String SearchIndexablesContract::BaseColumns::COLUMN_INTENT_TARGET_CLASS("intentTargetClass");

//============================================================================
//              SearchIndexablesContract::XmlResource
//============================================================================
static String initXmlResourceMIME_TYPE()
{
    return IContentResolver::CURSOR_DIR_BASE_TYPE + "/" +
            ISearchIndexablesContract::INDEXABLES_XML_RES;
}

const String SearchIndexablesContract::XmlResource::MIME_TYPE =
            initXmlResourceMIME_TYPE();

//============================================================================
//              SearchIndexablesContract::RawData
//============================================================================
static String initRawDataMIME_TYPE()
{
    return IContentResolver::CURSOR_DIR_BASE_TYPE + "/" +
            ISearchIndexablesContract::INDEXABLES_RAW;
}

const String SearchIndexablesContract::RawData::MIME_TYPE = initRawDataMIME_TYPE();

//============================================================================
//              SearchIndexablesContract::NonIndexableKey
//============================================================================
static String initNonIndexableKeyMIME_TYPE()
{
    return IContentResolver::CURSOR_DIR_BASE_TYPE + "/" +
            ISearchIndexablesContract::NON_INDEXABLES_KEYS;
}

const String SearchIndexablesContract::NonIndexableKey::MIME_TYPE =
            initNonIndexableKeyMIME_TYPE();

//============================================================================
//              SearchIndexablesContract
//============================================================================
const String SearchIndexablesContract::SETTINGS("settings");

static String initINDEXABLES_XML_RES_PATH()
{
    return SearchIndexablesContract::SETTINGS + "/" + ISearchIndexablesContract::INDEXABLES_XML_RES;
}

static String initINDEXABLES_RAW_PATH()
{
    return SearchIndexablesContract::SETTINGS + "/" + ISearchIndexablesContract::INDEXABLES_RAW;
}

static String initNON_INDEXABLES_KEYS_PATH()
{
    return SearchIndexablesContract::SETTINGS + "/" + ISearchIndexablesContract::NON_INDEXABLES_KEYS;
}

static AutoPtr<ArrayOf<String> > initINDEXABLES_XML_RES_COLUMNS()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(7);

    (*args)[0] = SearchIndexablesContract::BaseColumns::COLUMN_RANK;
    (*args)[1] = ISearchIndexablesContractXmlResource::COLUMN_XML_RESID;
    (*args)[2] = SearchIndexablesContract::BaseColumns::COLUMN_CLASS_NAME;
    (*args)[3] = SearchIndexablesContract::BaseColumns::COLUMN_ICON_RESID;
    (*args)[4] = SearchIndexablesContract::BaseColumns::COLUMN_INTENT_ACTION;
    (*args)[5] = SearchIndexablesContract::BaseColumns::COLUMN_INTENT_TARGET_PACKAGE;
    (*args)[6] = SearchIndexablesContract::BaseColumns::COLUMN_INTENT_TARGET_CLASS;

    return args;
}

static AutoPtr<ArrayOf<String> > initINDEXABLES_RAW_COLUMNS()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(14);

    (*args)[0] = SearchIndexablesContract::BaseColumns::COLUMN_RANK;
    (*args)[1] = ISearchIndexablesContractRawData::COLUMN_TITLE;
    (*args)[2] = ISearchIndexablesContractRawData::COLUMN_SUMMARY_ON;
    (*args)[3] = ISearchIndexablesContractRawData::COLUMN_SUMMARY_OFF;
    (*args)[4] = ISearchIndexablesContractRawData::COLUMN_ENTRIES;
    (*args)[5] = ISearchIndexablesContractRawData::COLUMN_KEYWORDS;
    (*args)[6] = ISearchIndexablesContractRawData::COLUMN_SCREEN_TITLE;
    (*args)[7] = SearchIndexablesContract::BaseColumns::COLUMN_CLASS_NAME;
    (*args)[8] = SearchIndexablesContract::BaseColumns::COLUMN_ICON_RESID;
    (*args)[9] = SearchIndexablesContract::BaseColumns::COLUMN_INTENT_ACTION;
    (*args)[10] = SearchIndexablesContract::BaseColumns::COLUMN_INTENT_TARGET_PACKAGE;
    (*args)[11] = SearchIndexablesContract::BaseColumns::COLUMN_INTENT_TARGET_CLASS;
    (*args)[12] = ISearchIndexablesContractRawData::COLUMN_KEY;
    (*args)[13] = ISearchIndexablesContractRawData::COLUMN_USER_ID;

    return args;
}

static AutoPtr<ArrayOf<String> > initNON_INDEXABLES_KEYS_COLUMNS()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);

    (*args)[0] = SearchIndexablesContract::BaseColumns::COLUMN_RANK;
}

const String SearchIndexablesContract::INDEXABLES_XML_RES_PATH = initINDEXABLES_XML_RES_PATH();

const String SearchIndexablesContract::INDEXABLES_RAW_PATH = initINDEXABLES_RAW_PATH();

const String SearchIndexablesContract::NON_INDEXABLES_KEYS_PATH = initNON_INDEXABLES_KEYS_PATH();

const AutoPtr<ArrayOf<String> > SearchIndexablesContract::INDEXABLES_XML_RES_COLUMNS =
        initINDEXABLES_XML_RES_COLUMNS();

const AutoPtr<ArrayOf<String> > SearchIndexablesContract::INDEXABLES_RAW_COLUMNS =
       initINDEXABLES_RAW_COLUMNS();

const AutoPtr<ArrayOf<String> > SearchIndexablesContract::NON_INDEXABLES_KEYS_COLUMNS =
        initNON_INDEXABLES_KEYS_COLUMNS();

} // namespace Provider
} // namespace Droid
} // namespace Elastos
