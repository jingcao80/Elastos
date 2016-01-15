
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::Uri;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Provider {
//============================================================================
//              BrowserContract
//============================================================================
static AutoPtr<IUri> initAUTHORITY_URI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + IBrowserContract::AUTHORITY, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::AUTHORITY_URI = initAUTHORITY_URI();

//============================================================================
//              BrowserContract::ChromeSyncColumns
//============================================================================
static String initSERVER_UNIQUE()
{
    return IBrowserContractBaseSyncColumns::SYNC3;
}

static String initCLIENT_UNIQUE()
{
    return IBrowserContractBaseSyncColumns::SYNC4;
}

const String BrowserContract::ChromeSyncColumns::SERVER_UNIQUE = initCLIENT_UNIQUE();

const String BrowserContract::ChromeSyncColumns::CLIENT_UNIQUE = initCLIENT_UNIQUE();

//============================================================================
//              BrowserContract::Bookmarks
//============================================================================
static AutoPtr<IUri> initBookmarksCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), String("bookmarks"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::Bookmarks::CONTENT_URI = initBookmarksCONTENT_URI();

static AutoPtr<IUri> initCONTENT_URI_DEFAULT_FOLDER()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::Bookmarks::CONTENT_URI.Get(), String("folder"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::Bookmarks::CONTENT_URI_DEFAULT_FOLDER = initCONTENT_URI_DEFAULT_FOLDER();

ECode BrowserContract::Bookmarks::BuildFolderUri(
    /* [in] */ Int64 folderId,
    /* [out] */ IUri** uri)
{
    AutoPtr<IContentUris> icu;
    CContentUris::AcquireSingleton((IContentUris**)&icu);
    return icu->WithAppendedId(CONTENT_URI_DEFAULT_FOLDER.Get(), folderId, uri);
}

//============================================================================
//              BrowserContract::Accounts
//============================================================================
static AutoPtr<IUri> initAccountsCONTENT_URI()
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriBuilder> iub;
    BrowserContract::AUTHORITY_URI->BuildUpon((IUriBuilder**)&iub);;
    iub->AppendPath(String("accounts"));
    iub->Build((IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::Accounts::CONTENT_URI = initAccountsCONTENT_URI();

//============================================================================
//              History
//============================================================================
const AutoPtr<IUri> initHistotyCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), String("history"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::History::CONTENT_URI =  initHistotyCONTENT_URI();

//============================================================================
//              Searches
//============================================================================
static AutoPtr<IUri> initSearchesCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), String("searches"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::Searches::CONTENT_URI = initSearchesCONTENT_URI();

//============================================================================
//              BrowserContract::SyncState
//============================================================================
static String initCONTENT_DIRECTORY()
{
    return ISyncStateContractConstants::CONTENT_DIRECTORY;
}

const String BrowserContract::SyncState::CONTENT_DIRECTORY = initCONTENT_DIRECTORY();

static AutoPtr<IUri> initSyncStateCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), BrowserContract::SyncState::CONTENT_DIRECTORY, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::SyncState::CONTENT_URI = initSyncStateCONTENT_URI();

ECode BrowserContract::SyncState::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value);
    // return SyncStateContractHelpers::Get(provider, SyncState::CONTENT_URI.Get(), account, value);
}

ECode BrowserContract::SyncState::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    VALIDATE_NOT_NULL(value);
    // return SyncStateContractHelpers::GetWithUri(provider, SyncState::CONTENT_URI, account, value);
}

ECode BrowserContract::SyncState::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    // return SyncStateContractHelpers::Set(provider, BrowserContract::SyncState::CONTENT_URI.Get(), account, data);
}

ECode BrowserContract::SyncState::NewSetOperation(
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    VALIDATE_NOT_NULL(operation);
    // return SyncStateContractHelpers::NewSetOperation(BrowserContract::SyncState::CONTENT_URI.Get(), account, data, operation);
}

//============================================================================
//              BrowserContract::Images
//============================================================================
static AutoPtr<IUri> initImagesCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), String("images"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::Images::CONTENT_URI = initImagesCONTENT_URI();

//============================================================================
//              ImageMappings
//============================================================================
static AutoPtr<IUri> initImageMappingsCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), String("image_mappings"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::ImageMappings::CONTENT_URI = initImageMappingsCONTENT_URI();

//============================================================================
//              Combined
//============================================================================
static AutoPtr<IUri> initCombinedCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), String("combined"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::Combined::CONTENT_URI = initCombinedCONTENT_URI();

//============================================================================
//              Settings
//============================================================================
static AutoPtr<IUri> initSettingsCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(BrowserContract::AUTHORITY_URI.Get(), String("settings"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> BrowserContract::Settings::CONTENT_URI = initSettingsCONTENT_URI();

ECode BrowserContract::Settings::IsSyncEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(res);
    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<IContentResolver> icr;
    context->GetContentResolver((IContentResolver**)&icr);
    AutoPtr<ArrayOf<String> > values = ArrayOf<String>::Alloc(1);
    values->Set(0, IBrowserContractSettings::VALUE);
    AutoPtr<ArrayOf<String> > values_ = ArrayOf<String>::Alloc(1);
    values_->Set(0, IBrowserContractSettings::KEY_SYNC_ENABLED);
    icr->Query(Settings::CONTENT_URI.Get(), values, IBrowserContractSettings::KEY + "=?", values_, String(NULL), (ICursor**)&cursor);
    Boolean flag = FALSE;
    if (cursor == NULL || !(cursor->MoveToFirst(&flag), flag)) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 columnValue;
    cursor->GetInt32(0, &columnValue);
    *res = columnValue != 0;
    // } finally {
    // }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    return NOERROR;
}

ECode BrowserContract::Settings::SetSyncEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IBrowserContractSettings::KEY, IBrowserContractSettings::KEY_SYNC_ENABLED);
    values->Put(IBrowserContractSettings::VALUE, enabled ? 1 : 0);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> outUri;
    return resolver->Insert(Settings::CONTENT_URI.Get(), values, (IUri**)&outUri);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
