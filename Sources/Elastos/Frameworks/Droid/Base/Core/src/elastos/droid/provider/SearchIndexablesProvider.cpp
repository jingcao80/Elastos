#include "elastos/droid/provider/SearchIndexablesContract.h"
#include "elastos/droid/provider/SearchIndexablesProvider.h"
#include "elastos/droid/content/CUriMatcher.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::EIID_IContentProvider;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::IContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Manifest;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Provider {

const String SearchIndexablesProvider::TAG("IndexablesProvider");
const Int32 SearchIndexablesProvider::MATCH_RES_CODE = 1;
const Int32 SearchIndexablesProvider::MATCH_RAW_CODE = 2;
const Int32 SearchIndexablesProvider::MATCH_NON_INDEXABLE_KEYS_CODE = 3;

SearchIndexablesProvider::SearchIndexablesProvider()
{}

SearchIndexablesProvider::~SearchIndexablesProvider()
{}

CAR_INTERFACE_IMPL(SearchIndexablesProvider, ContentProvider, ISearchIndexablesProvider)

ECode SearchIndexablesProvider::constructor()
{
    return ContentProvider::constructor();
}

//@Override
ECode SearchIndexablesProvider::AttachInfo(
    /* [in] */ IContext* context,
    /* [in] */ IProviderInfo* info)
{
    VALIDATE_NOT_NULL(info);
    String authority;
    info->GetAuthority(&authority);
    mAuthority = authority;

    mMatcher = NULL;
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&mMatcher);

    mMatcher->AddURI(mAuthority, SearchIndexablesContract::INDEXABLES_XML_RES_PATH,
            MATCH_RES_CODE);
    mMatcher->AddURI(mAuthority, SearchIndexablesContract::INDEXABLES_RAW_PATH,
            MATCH_RAW_CODE);
    mMatcher->AddURI(mAuthority, SearchIndexablesContract::NON_INDEXABLES_KEYS_PATH,
            MATCH_NON_INDEXABLE_KEYS_CODE);

    // Sanity check our setup
    Boolean flag = FALSE;
    IComponentInfo::Probe(info)->GetExported(&flag);
    if (!flag) {
        // throw new SecurityException("Provider must be exported");
        Slogger::E(TAG, "Provider must be exported");
        return E_SECURITY_EXCEPTION;
    }

    if (!(info->GetGrantUriPermissions(&flag), flag)) {
        // throw new SecurityException("Provider must grantUriPermissions");
        Slogger::E(TAG, "Provider must grantUriPermissions");
        return E_SECURITY_EXCEPTION;
    }
    String readPermission;
    info->GetReadPermission(&readPermission);
    if (!readPermission.Equals(Manifest::permission::READ_SEARCH_INDEXABLES)) {
        // throw new SecurityException("Provider must be protected by READ_SEARCH_INDEXABLES");
        Slogger::E(TAG, "Provider must be protected by READ_SEARCH_INDEXABLES");
        return E_SECURITY_EXCEPTION;
    }

    return IContentProvider::Probe(this)->AttachInfo(context, info);
}

ECode SearchIndexablesProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    Int32 matchCode;
    mMatcher->Match(uri, &matchCode);
    switch (matchCode) {
        case MATCH_RES_CODE:
            return QueryXmlResources(NULL, cursor);
        case MATCH_RAW_CODE:
            return QueryRawData(NULL, cursor);
        case MATCH_NON_INDEXABLE_KEYS_CODE:
            return QueryNonIndexableKeys(NULL, cursor);
        default:
            // throw new UnsupportedOperationException("Unknown Uri " + uri);
            Slogger::E(TAG, "Unknown Uri %p", uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

//@Override
ECode SearchIndexablesProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 matchCode;
    mMatcher->Match(uri, &matchCode);
    switch (matchCode) {
            case MATCH_RES_CODE:
                {
                    *result = SearchIndexablesContract::NonIndexableKey::MIME_TYPE;
                    return NOERROR;
                }

            case MATCH_RAW_CODE:
                {
                    *result = SearchIndexablesContract::RawData::MIME_TYPE;
                    return NOERROR;
                }
            case MATCH_NON_INDEXABLE_KEYS_CODE:
                {
                    *result = SearchIndexablesContract::NonIndexableKey::MIME_TYPE;
                    return NOERROR;
                }

            default:
                // throw new IllegalArgumentException("Unknown URI " + uri);
                Slogger::E(TAG, "Unknown URI %p", uri);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;;
        }
}

//@Override
ECode SearchIndexablesProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** insertedUri)
{
    VALIDATE_NOT_NULL(insertedUri);
    // throw new UnsupportedOperationException("Insert not supported");
    Slogger::E(TAG, "Insert not supported");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//@Override
ECode SearchIndexablesProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected);
    // throw new UnsupportedOperationException("Delete not supported");
    Slogger::E(TAG, "Delete not supported");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//@Override
ECode SearchIndexablesProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected);
    // throw new UnsupportedOperationException("Update not supported");
    Slogger::E(TAG, "Update not supported");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos