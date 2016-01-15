
#ifndef __ELASTOS_DROID_WEBKIT_CCACHEMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_CCACHEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Webkit_CCacheManager.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Singleton.h>

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

CarClass(CCacheManager)
    , public Singleton
    , public ICacheManager
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
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCacheFileBaseDir(
        /* [out] */ IFile** dir);

    CARAPI CacheDisabled(
        /* [out] */ Boolean* disabled);

    CARAPI StartCacheTransaction(
        /* [out] */ Boolean* result);

    CARAPI EndCacheTransaction(
        /* [out] */ Boolean* result);

    CARAPI GetCacheFile(
            /* [in] */ const String& url,
            /* [in] */ IMap* headers,
            /* [out] */ ICacheManagerCacheResult** cacheResult);

    CARAPI SaveCacheFile(
        /* [in] */ const String& url,
        /* [in] */ ICacheManagerCacheResult* cacheResult);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __CCACHEMANAGERHELPER_H__
