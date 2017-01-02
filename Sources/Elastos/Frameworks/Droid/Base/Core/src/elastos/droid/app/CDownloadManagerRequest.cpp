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
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/app/CDownloadManagerRequest.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/os/Environment.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::IDownloadManagerRequest;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::IDownloadsImplRequestHeaders;

namespace Elastos {
namespace Droid {
namespace App {

const Int32 CDownloadManagerRequest::SCANNABLE_VALUE_YES;
const Int32 CDownloadManagerRequest::SCANNABLE_VALUE_NO;

CAR_INTERFACE_IMPL(CDownloadManagerRequest, Object, IDownloadManagerRequest)

CAR_OBJECT_IMPL(CDownloadManagerRequest)

CDownloadManagerRequest::CDownloadManagerRequest()
    : mAllowedNetworkTypes(~0)
    , mRoamingAllowed(TRUE)
    , mMeteredAllowed(TRUE)
    , mIsVisibleInDownloadsUi(TRUE)
    , mScannable(FALSE)
    , mUseSystemCache(FALSE)
    , mNotificationVisibility(IDownloadManagerRequest::VISIBILITY_VISIBLE)
{}

/**
 * @param uri the HTTP URI to download.
 */
ECode CDownloadManagerRequest::constructor(
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    String scheme;
    uri->GetScheme(&scheme);
    if (scheme.IsNull() || (!scheme.Equals("http") && !scheme.Equals("https"))) {
        // throw new IllegalArgumentException("Can only download HTTP/HTTPS URIs: " + uri);
        Logger::E("CDownloadManagerRequest", "Can only download HTTP/HTTPS URIs: %p", uri);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mUri = uri;
    return NOERROR;
}

ECode CDownloadManagerRequest::constructor(
    /* [in] */ const String& uriString)
{
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    uriHelper->Parse(uriString, (IUri**)&mUri);
    return NOERROR;
}

ECode CDownloadManagerRequest::SetDestinationUri(
    /* [in] */ IUri* uri)
{
    mDestinationUri = uri;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetDestinationToSystemCache()
{
    mUseSystemCache = TRUE;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetDestinationInExternalFilesDir(
    /* [in] */ IContext* context,
    /* [in] */ const String& dirType,
    /* [in] */ const String& subPath)
{
    AutoPtr<IFile> file;
    context->GetExternalFilesDir(dirType, (IFile**)&file);

    Boolean exists;
    if (file == NULL) {
        Logger::E("CDownloadManagerRequest", "Failed to get external storage files directory");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else if (file->Exists(&exists), exists) {
        Boolean isDir;
        file->IsDirectory(&isDir);
        if (!isDir) {
            String path;
            file->GetAbsolutePath(&path);
            Logger::E("CDownloadManagerRequest", "%s already exists and is not a directory", path.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        Boolean bval;
        file->Mkdirs(&bval);
        if (!bval) {
            String path;
            file->GetAbsolutePath(&path);
            Logger::E("CDownloadManagerRequest", "Unable to create directory: %s", path.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    SetDestinationFromBase(file, subPath);
    return NOERROR;
}

ECode CDownloadManagerRequest::SetDestinationInExternalPublicDir(
    /* [in] */ const String& dirType,
    /* [in] */ const String& subPath)
{
    AutoPtr<IFile> file = Environment::GetExternalStoragePublicDirectory(dirType);

    Boolean exists;
    if (file == NULL) {
        Logger::E("CDownloadManagerRequest", "Failed to get external storage public directory");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else if (file->Exists(&exists), exists) {
        Boolean isDir;
        file->IsDirectory(&isDir);
        if (!isDir) {
            String path;
            file->GetAbsolutePath(&path);
            Logger::E("CDownloadManagerRequest", "%s already exists and is not a directory", path.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        Boolean bval;
        file->Mkdirs(&bval);
        if (!bval) {
            String path;
            file->GetAbsolutePath(&path);
            Logger::E("CDownloadManagerRequest", "Unable to create directory: %s", path.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    SetDestinationFromBase(file, subPath);
    return NOERROR;
}

ECode CDownloadManagerRequest::SetDestinationFromBase(
    /* [in] */ IFile* base,
    /* [in] */ const String& subPath)
{
    if (subPath.IsNull()) {
        // throw new NullPointerException("subPath cannot be null");
        Logger::E("CDownloadManagerRequest", "subPath cannot be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> furi;
    helper->FromFile(base, (IUri**)&furi);
    mDestinationUri = NULL;
    helper->WithAppendedPath(furi, subPath, (IUri**)&mDestinationUri);
    return NOERROR;
}

ECode CDownloadManagerRequest::AllowScanningByMediaScanner()
{
    mScannable = TRUE;
    return NOERROR;
}

ECode CDownloadManagerRequest::AddRequestHeader(
    /* [in] */ const String& header,
    /* [in] */ const String& value)
{
    if (header.IsNull()) {
        // throw new NullPointerException("header cannot be null");
        Logger::E("CDownloadManagerRequest", "header cannot be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (header.Contains(":")) {
        // throw new IllegalArgumentException("header may not contain ':'");
        Logger::E("CDownloadManagerRequest", "header may not contain ':'");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (value.IsNull()) {
        mRequestHeaders.PushBack(Pair<String, String>(header, String("")));
        return NOERROR;
    }
    mRequestHeaders.PushBack(Pair<String, String>(header, value));
    return NOERROR;
}

ECode CDownloadManagerRequest::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetDescription(
    /* [in] */ ICharSequence* description)
{
    mDescription = description;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetMimeType(
    /* [in] */ const String& mimeType)
{
    mMimeType = mimeType;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetShowRunningNotification(
    /* [in] */ Boolean show)
{
    return (show) ? SetNotificationVisibility(IDownloadManagerRequest::VISIBILITY_VISIBLE) :
        SetNotificationVisibility(IDownloadManagerRequest::VISIBILITY_HIDDEN);
}

ECode CDownloadManagerRequest::SetNotificationVisibility(
    /* [in] */ Int32 visibility)
{
    mNotificationVisibility = visibility;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetAllowedNetworkTypes(
    /* [in] */ Int32 flags)
{
    mAllowedNetworkTypes = flags;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetAllowedOverRoaming(
    /* [in] */ Boolean allowed)
{
    mRoamingAllowed = allowed;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetAllowedOverMetered(
    /* [in] */ Boolean allow)
{
    mMeteredAllowed = allow;
    return NOERROR;
}

ECode CDownloadManagerRequest::SetVisibleInDownloadsUi(
    /* [in] */ Boolean isVisible)
{
    mIsVisibleInDownloadsUi = isVisible;
    return NOERROR;
}

/**
 * @return ContentValues to be passed to DownloadProvider.insert()
 */
AutoPtr<IContentValues> CDownloadManagerRequest::ToContentValues(
    /* [in] */ const String& packageName)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    assert(mUri != NULL);
    String uriStr = Object::ToString(mUri);
    AutoPtr<ICharSequence> cs;
    CString::New(uriStr, (ICharSequence**)&cs);
    values->Put(IDownloadsImpl::COLUMN_URI, cs);
    AutoPtr<IBoolean> bvalue;
    CBoolean::New(TRUE, (IBoolean**)&bvalue);
    values->Put(IDownloadsImpl::COLUMN_IS_PUBLIC_API, bvalue);
    AutoPtr<ICharSequence> pkgcs;
    CString::New(packageName, (ICharSequence**)&pkgcs);
    values->Put(IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE, pkgcs);

    if (mDestinationUri != NULL) {
        AutoPtr<IInteger32> furi;
        CInteger32::New(IDownloadsImpl::DESTINATION_FILE_URI, (IInteger32**)&furi);
        values->Put(IDownloadsImpl::COLUMN_DESTINATION, furi);
        String desStr = Object::ToString(mDestinationUri);
        AutoPtr<ICharSequence> descs;
        CString::New(desStr, (ICharSequence**)&descs);
        values->Put(IDownloadsImpl::COLUMN_FILE_NAME_HINT, descs);
    } else {
        AutoPtr<IInteger32> partition, purgeable;
        CInteger32::New(IDownloadsImpl::DESTINATION_SYSTEMCACHE_PARTITION, (IInteger32**)&partition);
        CInteger32::New(IDownloadsImpl::DESTINATION_CACHE_PARTITION_PURGEABLE, (IInteger32**)&purgeable);
        values->Put(IDownloadsImpl::COLUMN_DESTINATION,
                mUseSystemCache ? partition : purgeable);
    }
    // is the file supposed to be media-scannable?
    AutoPtr<IInteger32> yes, no;
    CInteger32::New(SCANNABLE_VALUE_YES, (IInteger32**)&yes);
    CInteger32::New(SCANNABLE_VALUE_NO, (IInteger32**)&no);
    values->Put(IDownloadsImpl::COLUMN_MEDIA_SCANNED, mScannable ? yes : no);

    if (!mRequestHeaders.IsEmpty()) {
        EncodeHttpHeaders(values);
    }

    PutIfNonNull(values, IDownloadsImpl::COLUMN_TITLE, mTitle);
    PutIfNonNull(values, IDownloadsImpl::COLUMN_DESCRIPTION, mDescription);
    AutoPtr<ICharSequence> type;
    CString::New(mMimeType, (ICharSequence**)&type);
    PutIfNonNull(values, IDownloadsImpl::COLUMN_MIME_TYPE, type);

    AutoPtr<IInteger32> visibility, types;
    CInteger32::New(mNotificationVisibility, (IInteger32**)&visibility);
    CInteger32::New(mAllowedNetworkTypes, (IInteger32**)&types);
    values->Put(IDownloadsImpl::COLUMN_VISIBILITY, visibility);
    values->Put(IDownloadsImpl::COLUMN_ALLOWED_NETWORK_TYPES, types);
    AutoPtr<IBoolean> value;
    CBoolean::New(mRoamingAllowed, (IBoolean**)&value);
    values->Put(IDownloadsImpl::COLUMN_ALLOW_ROAMING, value);
    value = NULL;
    CBoolean::New(mMeteredAllowed, (IBoolean**)&value);
    values->Put(IDownloadsImpl::COLUMN_ALLOW_METERED, value);
    value = NULL;
    CBoolean::New(mIsVisibleInDownloadsUi, (IBoolean**)&value);
    values->Put(IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI, value);

    return values;
}

void CDownloadManagerRequest::EncodeHttpHeaders(
    /* [in] */ IContentValues* values)
{
    Int32 index = 0;
    List< Pair<String, String> >::Iterator it;
    for (it = mRequestHeaders.Begin(); it != mRequestHeaders.End(); ++it) {
        Pair<String, String> header = *it;
        String headerString = header.mFirst + ": " + header.mSecond;
        AutoPtr<ICharSequence> headercs;
        CString::New(headerString, (ICharSequence**)&headercs);
        StringBuilder sb(IDownloadsImplRequestHeaders::INSERT_KEY_PREFIX);
        sb += index;
        values->Put(sb.ToString(), headercs);
        index++;
    }
}

void CDownloadManagerRequest::PutIfNonNull(
    /* [in] */ IContentValues* contentValues,
    /* [in] */ const String& key,
    /* [in] */ IInterface* value)
{
    if (value != NULL) {
        contentValues->Put(key, ICharSequence::Probe(value));
    }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
