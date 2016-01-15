
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/webkit/CacheManager.h"
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
//              CacheManager::CacheResult
//===============================================================

CAR_INTERFACE_IMPL(CacheManager::CacheResult, Object, ICacheManagerCacheResult)

CacheManager::CacheResult::CacheResult()
    : mHttpStatusCode(0)
    , mContentLength(0)
    , mExpires(0)
{
}

ECode CacheManager::CacheResult::GetHttpStatusCode(
    /* [out] */ Int32* statusCode)
{
    VALIDATE_NOT_NULL(statusCode);
    *statusCode = mHttpStatusCode;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);
    *length = mContentLength;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetLocalPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = mLocalPath;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetExpires(
    /* [out] */ Int64* expires)
{
    VALIDATE_NOT_NULL(expires);
    *expires = mExpires;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetExpiresString(
    /* [out] */ String* expires)
{
    VALIDATE_NOT_NULL(expires);
    *expires = mExpiresString;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetLastModified(
    /* [out] */ String* lastModified)
{
    VALIDATE_NOT_NULL(lastModified);
    *lastModified = mLastModified;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetETag(
    /* [out] */ String* etag)
{
    VALIDATE_NOT_NULL(etag);
    *etag = mEtag;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetMimeType(
    /* [out] */ String* mimeType)
{
    VALIDATE_NOT_NULL(mimeType);
    *mimeType = mMimeType;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetLocation(
    /* [out] */ String* location)
{
    VALIDATE_NOT_NULL(location);
    *location = mLocation;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetEncoding(
    /* [out] */ String* encoding)
{
    VALIDATE_NOT_NULL(encoding);
    *encoding = mEncoding;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetContentDisposition(
    /* [out] */ String* contentdisposition)
{
    VALIDATE_NOT_NULL(contentdisposition);
    *contentdisposition = mContentdisposition;
    return NOERROR;
}

ECode CacheManager::CacheResult::GetInputStream(
    /* [out] */ IInputStream** inStream)
{
    VALIDATE_NOT_NULL(inStream);
    *inStream = mInStream;
    REFCOUNT_ADD(*inStream);
    return NOERROR;
}

ECode CacheManager::CacheResult::GetOutputStream(
    /* [out] */ IOutputStream** outStream)
{
    VALIDATE_NOT_NULL(outStream);
    *outStream = mOutStream;
    REFCOUNT_ADD(*outStream);
    return NOERROR;
}

ECode CacheManager::CacheResult::SetInputStream(
    /* [in] */ IInputStream* stream)
{
    mInStream = stream;
    return NOERROR;
}

ECode CacheManager::CacheResult::SetEncoding(
    /* [in] */ const String& encoding)
{
    mEncoding = encoding;
    return NOERROR;
}

ECode CacheManager::CacheResult::SetContentLength(
    /* [in] */ Int64 contentLength)
{
    mContentLength = contentLength;
    return NOERROR;
}

//===============================================================
//                       CCacheManager
//===============================================================

AutoPtr<IFile> CacheManager::GetCacheFileBaseDir()
{
    return NULL;
}

Boolean CacheManager::CacheDisabled()
{
    return FALSE;
}

Boolean CacheManager::StartCacheTransaction()
{
    return FALSE;
}

Boolean CacheManager::EndCacheTransaction()
{
    return FALSE;
}

AutoPtr<ICacheManagerCacheResult> CacheManager::GetCacheFile(
    /* [in] */ const String& url,
    /* [in] */ IMap* headers)
{
    return NULL;
}

/**
 * Given a URL and its full headers, gets a CacheResult if a local cache
 * can be stored. Otherwise returns null. The mimetype is passed in so that
 * the function can use the mimetype that will be passed to WebCore which
 * could be different from the mimetype defined in the headers.
 * forceCache is for out-of-package callers to force creation of a
 * CacheResult, and is used to supply surrogate responses for URL
 * interception.
 *
 * @return a CacheResult for a given URL
 */
AutoPtr<ICacheManagerCacheResult> CacheManager::CreateCacheFile(
    /* [in] */ const String& url,
    /* [in] */ Int32 statusCode,
    /* [in] */ IHeaders* headers,
    /* [in] */ const String& mimeType,
    /* [in] */ Boolean forceCache)
{
    // This method is public but hidden. We break functionality.
    return NULL;
}

void CacheManager::SaveCacheFile(
    /* [in] */ const String& url,
    /* [in] */ ICacheManagerCacheResult* cacheResult)
{
    SaveCacheFile(url, 0, cacheResult);
}

void CacheManager::SaveCacheFile(
    /* [in] */ const String& url,
    /* [in] */ Int64 postIdentifier,
    /* [in] */ ICacheManagerCacheResult* cacheRet)
{
    //try {
    AutoPtr<IOutputStream> outStream;
    cacheRet->GetOutputStream((IOutputStream**)&outStream);
    outStream->Close();
    //} catch (IOException e) {
    //    return;
    //}

    // This method is exposed in the public API but the API provides no
    // way to obtain a new CacheResult object with a non-null output
    // stream ...
    // - CacheResult objects returned by getCacheFile() have a null
    //   output stream.
    // - new CacheResult objects have a null output stream and no
    //   setter is provided.
    // Since this method throws a null pointer exception in this case,
    // it is effectively useless from the point of view of the public
    // API.
    //
    // With the Chromium HTTP stack we continue to throw the same
    // exception for 'backwards compatibility' with the Android HTTP
    // stack.
    //
    // This method is not used from within this package, and for public API
    // use, we should already have thrown an exception above.
    assert(FALSE);
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
