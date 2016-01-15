
#ifndef __ELASTOS_DROID_WEBKIT_CACHEMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_CACHEMANAGER_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::Http::IHeaders;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace Webkit {

class CacheManager
{
public:
    class CacheResult
        : public Object
        , public ICacheManagerCacheResult
    {
    public:
        CacheResult();

        CAR_INTERFACE_DECL()

        CARAPI GetHttpStatusCode(
            /* [out] */ Int32* statusCode);

        CARAPI GetContentLength(
            /* [out] */ Int64* length);

        CARAPI GetLocalPath(
            /* [out] */ String* path);

        CARAPI GetExpires(
            /* [out] */ Int64* expires);

        CARAPI GetExpiresString(
            /* [out] */ String* expires);

        CARAPI GetLastModified(
            /* [out] */ String* lastModified);

        CARAPI GetETag(
            /* [out] */ String* etag);

        CARAPI GetMimeType(
            /* [out] */ String* mimeType);

        CARAPI GetLocation(
            /* [out] */ String* location);

        CARAPI GetEncoding(
            /* [out] */ String* encoding);

        CARAPI GetContentDisposition(
            /* [out] */ String* contentdisposition);

        CARAPI GetInputStream(
            /* [out] */ IInputStream** inStream);

        CARAPI GetOutputStream(
            /* [out] */ IOutputStream** outStream);

        CARAPI SetInputStream(
            /* [in] */ IInputStream* stream);

        CARAPI SetEncoding(
            /* [in] */ const String& encoding);

        CARAPI SetContentLength(
            /* [in] */ Int64 contentLength);

    public:
        // these fields are saved to the database
        Int32 mHttpStatusCode;
        Int64 mContentLength;
        Int64 mExpires;
        String mExpiresString;
        String mLocalPath;
        String mLastModified;
        String mEtag;
        String mMimeType;
        String mLocation;
        String mEncoding;
        String mContentdisposition;
        String mCrossDomain;

        // these fields are NOT saved to the database
        AutoPtr<IInputStream> mInStream;
        AutoPtr<IOutputStream> mOutStream;
        AutoPtr<IFile> mOutFile;
    };

public:
    static CARAPI_(AutoPtr<IFile>) GetCacheFileBaseDir();

    static CARAPI_(Boolean) CacheDisabled();

    static CARAPI_(Boolean) StartCacheTransaction();

    static CARAPI_(Boolean) EndCacheTransaction();

    static CARAPI_(AutoPtr<ICacheManagerCacheResult>) GetCacheFile(
        /* [in] */ const String& url,
        /* [in] */ IMap* headers);

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
    static CARAPI_(AutoPtr<ICacheManagerCacheResult>) CreateCacheFile(
        /* [in] */ const String& url,
        /* [in] */ Int32 statusCode,
        /* [in] */ IHeaders* headers,
        /* [in] */ const String& mimeType,
        /* [in] */ Boolean forceCache);

    static CARAPI_(void) SaveCacheFile(
        /* [in] */ const String& url,
        /* [in] */ ICacheManagerCacheResult* cacheResult);

    static CARAPI_(void) SaveCacheFile(
        /* [in] */ const String& url,
        /* [in] */ Int64 postIdentifier,
        /* [in] */ ICacheManagerCacheResult* cacheRet);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __CCACHEMANAGERHELPER_H__
