
#ifndef __IMAGECACHESERVICE_H__
#define __IMAGECACHESERVICE_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Utility::Etl::List;

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
    Object mLock;
};

} // namespace Gallery

#endif // __IMAGECACHESERVICE_H__
