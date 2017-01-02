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

#include "elastos/droid/contacts/common/list/DirectoryListLoader.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "R.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::CContactsContractDirectory;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// DirectoryListLoader::DirectoryQuery
//=================================================================

AutoPtr<IUri> InitUri()
{
    AutoPtr<IContactsContractDirectory> directory;
    CContactsContractDirectory::AcquireSingleton((IContactsContractDirectory**)&directory);
    AutoPtr<IUri> uri;
    directory->GetCONTENT_URI((IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> DirectoryListLoader::DirectoryQuery::URI = InitUri();
const String DirectoryListLoader::DirectoryQuery::ORDER_BY = IBaseColumns::ID;

AutoPtr<ArrayOf<String> > InitProjection()
{
    AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(5);
    (*projections)[0] = IBaseColumns::ID;
    (*projections)[1] = IContactsContractDirectory::PACKAGE_NAME;
    (*projections)[2] = IContactsContractDirectory::TYPE_RESOURCE_ID;
    (*projections)[3] = IContactsContractDirectory::DISPLAY_NAME;
    (*projections)[4] = IContactsContractDirectory::PHOTO_SUPPORT;
    return projections;
}
const AutoPtr<ArrayOf<String> > DirectoryListLoader::DirectoryQuery::PROJECTION = InitProjection();
const Int32 DirectoryListLoader::DirectoryQuery::ID;
const Int32 DirectoryListLoader::DirectoryQuery::PACKAGE_NAME;
const Int32 DirectoryListLoader::DirectoryQuery::TYPE_RESOURCE_ID;
const Int32 DirectoryListLoader::DirectoryQuery::DISPLAY_NAME;
const Int32 DirectoryListLoader::DirectoryQuery::PHOTO_SUPPORT;


//=================================================================
// DirectoryListLoader::MyObserver
//=================================================================

ECode DirectoryListLoader::MyObserver::constructor(
    /* [in] */ DirectoryListLoader* host)
{
    mHost = host;
    return ContentObserver::constructor();
}

ECode DirectoryListLoader::MyObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->ForceLoad();
    return NOERROR;
}


//=================================================================
// DirectoryListLoader
//=================================================================

const String DirectoryListLoader::TAG("ContactEntryListAdapter");
const Int32 DirectoryListLoader::SEARCH_MODE_NONE;
const Int32 DirectoryListLoader::SEARCH_MODE_DEFAULT;
const Int32 DirectoryListLoader::SEARCH_MODE_CONTACT_SHORTCUT;
const Int32 DirectoryListLoader::SEARCH_MODE_DATA_SHORTCUT;
const String DirectoryListLoader::DIRECTORY_TYPE("directoryType");

static AutoPtr<ArrayOf<String> > InitResultProjection()
{
    AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(4);
    (*projections)[0] = IBaseColumns::ID;
    (*projections)[1] = DirectoryListLoader::DIRECTORY_TYPE;
    (*projections)[2] = IContactsContractDirectory::DISPLAY_NAME;
    (*projections)[3] = IContactsContractDirectory::PHOTO_SUPPORT;
    return projections;
}
const AutoPtr<ArrayOf<String> > DirectoryListLoader::RESULT_PROJECTION = InitResultProjection();

DirectoryListLoader::DirectoryListLoader(
    /* [in] */ IContext* ctx)
    : mDirectorySearchMode(0)
    , mLocalInvisibleDirectoryEnabled(FALSE)
{
    assert(0 && "TODO using constructor");
    AsyncTaskLoader::constructor(ctx);
    mObserver = new MyObserver();
    mObserver->constructor(this);
}

void DirectoryListLoader::SetDirectorySearchMode(
    /* [in] */ Int32 mode)
{
    mDirectorySearchMode = mode;
}

void DirectoryListLoader::SetLocalInvisibleDirectoryEnabled(
    /* [in] */ Boolean flag)
{
    mLocalInvisibleDirectoryEnabled = flag;
}

ECode DirectoryListLoader::OnStartLoading()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IContactsContractDirectory> directory;
    CContactsContractDirectory::AcquireSingleton((IContactsContractDirectory**)&directory);
    AutoPtr<IUri> uri;
    directory->GetCONTENT_URI((IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, mObserver);
    ForceLoad();
    return NOERROR;
}

ECode DirectoryListLoader::OnStopLoading()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(mObserver);
    return NOERROR;
}

ECode DirectoryListLoader::LoadInBackground(
    /* [out] */ IInterface** _cursor)
{
    VALIDATE_NOT_NULL(_cursor)
    if (mDirectorySearchMode == SEARCH_MODE_NONE) {
        AutoPtr<ICursor> c = GetDefaultDirectories();
        *_cursor = c;
        REFCOUNT_ADD(*_cursor)
        return NOERROR;
    }

    AutoPtr<IMatrixCursor> result;
    CMatrixCursor::New(RESULT_PROJECTION, (IMatrixCursor**)&result);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    String selection;
    switch (mDirectorySearchMode) {
        case SEARCH_MODE_DEFAULT:
            selection = mLocalInvisibleDirectoryEnabled ?
                    String(NULL) : (IBaseColumns::ID + "!=" + StringUtils::ToString(IContactsContractDirectory::LOCAL_INVISIBLE));
            break;

        case SEARCH_MODE_CONTACT_SHORTCUT:
            selection = IContactsContractDirectory::SHORTCUT_SUPPORT + "="
                    + StringUtils::ToString(IContactsContractDirectory::SHORTCUT_SUPPORT_FULL)
                    + (mLocalInvisibleDirectoryEnabled ?
                            String("") : (String(" AND ") + IBaseColumns::ID + "!=" +
                                    StringUtils::ToString(IContactsContractDirectory::LOCAL_INVISIBLE)));
            break;

        case SEARCH_MODE_DATA_SHORTCUT:
            selection = IContactsContractDirectory::SHORTCUT_SUPPORT + " IN ("
                    + StringUtils::ToString(IContactsContractDirectory::SHORTCUT_SUPPORT_FULL) + ", "
                    + StringUtils::ToString(IContactsContractDirectory::SHORTCUT_SUPPORT_DATA_ITEMS_ONLY) + ")"
                    + (mLocalInvisibleDirectoryEnabled ?
                            String("") : (String(" AND ") + IBaseColumns::ID + String("!=") +
                                    StringUtils::ToString(IContactsContractDirectory::LOCAL_INVISIBLE)));
            break;

        default:
            Logger::E("DirectoryListLoader", "Unsupported directory search mode: %d", mDirectorySearchMode);
            return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> cursor;
    cr->Query(DirectoryQuery::URI, DirectoryQuery::PROJECTION, selection,
            NULL, DirectoryQuery::ORDER_BY, (ICursor**)&cursor);
    if (cursor == NULL) {
        *_cursor = IInterface::Probe(result);
        REFCOUNT_ADD(*_cursor)
        return NOERROR;
    }
    // try {
    Boolean hasNext;
    while(cursor->MoveToNext(&hasNext), hasNext) {
        Int64 directoryId;
        cursor->GetInt64(DirectoryQuery::ID, &directoryId);
        String directoryType(NULL);

        String packageName;
        cursor->GetString(DirectoryQuery::PACKAGE_NAME, &packageName);
        Int32 typeResourceId;
        cursor->GetInt32(DirectoryQuery::TYPE_RESOURCE_ID, &typeResourceId);
        if (!TextUtils::IsEmpty(packageName) && typeResourceId != 0) {
            // try {
            AutoPtr<IResources> res;
            if (FAILED(pm->GetResourcesForApplication(packageName, (IResources**)&res))) {
                Logger::E("DirectoryListLoader", "Cannot obtain directory type from package: %s", packageName.string());
            }
            else {
                res->GetString(typeResourceId, &directoryType);
            }
            // } catch (Exception e) {
            //     Log.e(TAG, "Cannot obtain directory type from package: " + packageName);
            // }
        }
        String displayName;
        cursor->GetString(DirectoryQuery::DISPLAY_NAME, &displayName);
        Int32 photoSupport;
        cursor->GetInt32(DirectoryQuery::PHOTO_SUPPORT, &photoSupport);
        AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(4);
        AutoPtr<IInteger64> id;
        CInteger64::New(directoryId, (IInteger64**)&id);
        attrs->Set(0, id);
        AutoPtr<ICharSequence> type;
        CString::New(directoryType, (ICharSequence**)&type);
        attrs->Set(1, type);
        AutoPtr<ICharSequence> name;
        CString::New(displayName, (ICharSequence**)&name);
        attrs->Set(2, name);
        AutoPtr<IInteger32> supprot;
        CInteger32::New(photoSupport, (IInteger32**)&supprot);
        attrs->Set(3, supprot);
        result->AddRow(*attrs);
    }
    // } finally {
    //     cursor.close();
    // }
    ICloseable::Probe(cursor)->Close();

    *_cursor = IInterface::Probe(result);
    REFCOUNT_ADD(*_cursor)
    return NOERROR;
}

AutoPtr<ICursor> DirectoryListLoader::GetDefaultDirectories()
{
    if (mDefaultDirectoryList == NULL) {
        CMatrixCursor::New(RESULT_PROJECTION, (IMatrixCursor**)&mDefaultDirectoryList);
        AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(3);
        AutoPtr<IInteger64> def;
        CInteger64::New(IContactsContractDirectory::DEFAULT, (IInteger64**)&def);
        attrs->Set(0, def);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String str;
        context->GetString(Elastos::Droid::Dialer::R::string::contactsList, &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        attrs->Set(1, cs);
        attrs->Set(2, NULL);
        mDefaultDirectoryList->AddRow(*attrs);

        AutoPtr<ArrayOf<IInterface*> > attrs1 = ArrayOf<IInterface*>::Alloc(3);
        AutoPtr<IInteger64> v;
        CInteger64::New(IContactsContractDirectory::LOCAL_INVISIBLE, (IInteger64**)&v);
        attrs1->Set(0, v);
        String directory;
        context->GetString(Elastos::Droid::Dialer::R::string::local_invisible_directory, &directory);
        AutoPtr<ICharSequence> cs1;
        CString::New(directory, (ICharSequence**)&cs1);
        attrs1->Set(1, cs1);
        attrs1->Set(2, NULL);
        mDefaultDirectoryList->AddRow(*attrs1);
    }
    return ICursor::Probe(mDefaultDirectoryList);
}

ECode DirectoryListLoader::OnReset()
{
    return StopLoading();
}

} // namespace List
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
