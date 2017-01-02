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
#include "elastos/droid/webkit/CacheManager.h"
#include "elastos/droid/webkit/CCacheManager.h"
#include "elastos/droid/webkit/DebugFlags.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::CThread;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::IString;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {

//===============================================================
//              CCacheManager::CacheResult
//===============================================================

CAR_INTERFACE_IMPL(CCacheManager::CacheResult, Object, ICacheManagerCacheResult)

CCacheManager::CacheResult::CacheResult()
    : mHttpStatusCode(0)
    , mContentLength(0)
    , mExpires(0)
{
}

ECode CCacheManager::CacheResult::GetHttpStatusCode(
    /* [out] */ Int32* statusCode)
{
    VALIDATE_NOT_NULL(statusCode);
    *statusCode = mHttpStatusCode;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);
    *length = mContentLength;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetLocalPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = mLocalPath;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetExpires(
    /* [out] */ Int64* expires)
{
    VALIDATE_NOT_NULL(expires);
    *expires = mExpires;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetExpiresString(
    /* [out] */ String* expires)
{
    VALIDATE_NOT_NULL(expires);
    *expires = mExpiresString;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetLastModified(
    /* [out] */ String* lastModified)
{
    VALIDATE_NOT_NULL(lastModified);
    *lastModified = mLastModified;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetETag(
    /* [out] */ String* etag)
{
    VALIDATE_NOT_NULL(etag);
    *etag = mEtag;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetMimeType(
    /* [out] */ String* mimeType)
{
    VALIDATE_NOT_NULL(mimeType);
    *mimeType = mMimeType;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetLocation(
    /* [out] */ String* location)
{
    VALIDATE_NOT_NULL(location);
    *location = mLocation;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetEncoding(
    /* [out] */ String* encoding)
{
    VALIDATE_NOT_NULL(encoding);
    *encoding = mEncoding;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetContentDisposition(
    /* [out] */ String* contentdisposition)
{
    VALIDATE_NOT_NULL(contentdisposition);
    *contentdisposition = mContentdisposition;
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetInputStream(
    /* [out] */ IInputStream** inStream)
{
    VALIDATE_NOT_NULL(inStream);
    *inStream = mInStream;
    REFCOUNT_ADD(*inStream);
    return NOERROR;
}

ECode CCacheManager::CacheResult::GetOutputStream(
    /* [out] */ IOutputStream** outStream)
{
    VALIDATE_NOT_NULL(outStream);
    *outStream = mOutStream;
    REFCOUNT_ADD(*outStream);
    return NOERROR;
}

ECode CCacheManager::CacheResult::SetInputStream(
    /* [in] */ IInputStream* stream)
{
    mInStream = stream;
    return NOERROR;
}

ECode CCacheManager::CacheResult::SetEncoding(
    /* [in] */ const String& encoding)
{
    mEncoding = encoding;
    return NOERROR;
}

ECode CCacheManager::CacheResult::SetContentLength(
    /* [in] */ Int64 contentLength)
{
    mContentLength = contentLength;
    return NOERROR;
}

//===============================================================
//                       CCacheManager
//===============================================================

CAR_INTERFACE_IMPL(CCacheManager, Object, ICacheManager);

CAR_SINGLETON_IMPL(CCacheManager);

ECode CCacheManager::GetCacheFileBaseDir(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir);
    AutoPtr<IFile> _dir = CacheManager::GetCacheFileBaseDir();
    *dir = _dir;
    REFCOUNT_ADD(*dir);
    return NOERROR;
}

ECode CCacheManager::CacheDisabled(
    /* [out] */ Boolean* disabled)
{
    VALIDATE_NOT_NULL(disabled);
    *disabled = CacheManager::CacheDisabled();
    return NOERROR;
}

ECode CCacheManager::StartCacheTransaction(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CacheManager::StartCacheTransaction();
    return NOERROR;
}

ECode CCacheManager::EndCacheTransaction(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CacheManager::EndCacheTransaction();
    return NOERROR;
}

ECode CCacheManager::GetCacheFile(
    /* [in] */ const String& url,
    /* [in] */ IMap* headers,
    /* [out] */ ICacheManagerCacheResult** cacheResult)
{
    VALIDATE_NOT_NULL(cacheResult);
    AutoPtr<ICacheManagerCacheResult> _cacheResult = CacheManager::GetCacheFile(url,  headers);
    *cacheResult = _cacheResult;
    REFCOUNT_ADD(*cacheResult);
    return NOERROR;
}

ECode CCacheManager::SaveCacheFile(
    /* [in] */ const String& url,
    /* [in] */ ICacheManagerCacheResult* cacheResult)
{
    CacheManager::SaveCacheFile(url, cacheResult);
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
