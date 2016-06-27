
#ifndef __IMAGECACHESERVICE_H__
#define __IMAGECACHESERVICE_H__

#include "app/Activity.h"
#include <elastos/List.h>
#include <elastos/Mutex.h>

using Elastos::Core::Threading::Mutex;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Utility::List;

namespace Gallery {

class ImageCacheService
    : public ElRefBase
{
public:
    ImageCacheService(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI GetImageData(
        /* [in] */ Int32 key,
        /* [in] */ Boolean isHigh,
        /* [in] */ Int64 len,
        /* [in] */ Int64 modify,
        /* [out] */ IBitmap** bitmap);

    CARAPI PutImageData(
        /* [in] */ Int32 key,
        /* [in] */ Boolean isHigh,
        /* [in] */ Int64 len,
        /* [in] */ Int64 modify,
        /* [in] */ IBitmap* bitmap);

    CARAPI UpdateCache();

private:
    CARAPI_(void) UpdateCacheFilesIfNecessary();

    CARAPI_(void) DeleteFile(
        /* [in] */ const String& filePath);

    CARAPI_(void) CreateConfigFile(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI_(void) ClearCacheFiles();

private:
    static const String TAG;
    static const String CACHE_PATH;

private:
    Int32 mImgWidth;
    Int32 mImgHeight;
    List<String> mCachePathList;
    Mutex mLock;
};

} // namespace Gallery

#endif // __IMAGECACHESERVICE_H__
