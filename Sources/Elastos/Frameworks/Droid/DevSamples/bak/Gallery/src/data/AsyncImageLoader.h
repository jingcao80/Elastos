
#ifndef __ASYNCIMAGELOADER_H__
#define __ASYNCIMAGELOADER_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/utility/etl/HashMap.h>
#include "ImageCacheService.h"

using Elastos::Core::IRunnable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::Etl::HashMap;

namespace Gallery {

class ILoadImageCallback
    : public ElRefBase
{
public:
    CARAPI virtual ImageLoaded(
        /* [in] */ IDrawable* imageDrawable,
        /* [in] */ IImageView* imageView) = 0;
};

class ImageLoaderParam
    : public ElRefBase
{
public:
    AutoPtr<IImageView> mImageView;
    AutoPtr<ILoadImageCallback> mCallback;
};

class AsyncImageLoader
    : public ElRefBase
{
private:
    class ImageLoaderThread
        : public ElRefBase
        , public IRunnable
    {
    public:
        ImageLoaderThread(
            /* [in] */ Boolean isHigh,
            /* [in] */ const String& imageUrl);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        AutoPtr<IBitmapDrawable> LoadImageFromUrl(
            /* [in] */ const String& path);

        CARAPI_(Int32) ComputeSampleSize(
            /* [in] */ IBitmapFactoryOptions* options,
            /* [in] */ Int32 minSideLength,
            /* [in] */ Int32 maxNumOfPixels);

        CARAPI_(Int32) ComputeInitialSampleSize(
            /* [in] */ IBitmapFactoryOptions* options,
            /* [in] */ Int32 minSideLength,
            /* [in] */ Int32 maxNumOfPixels);

    private:
        Boolean mIsHigh;
        String mImageUrl;
    };

friend class ImageLoaderThread;

public:
    static AutoPtr<IBitmapDrawable> LoadDrawable(
        /* [in] */ const String& imageUrl,
        /* [in] */ Boolean isHigh,
        /* [in] */ IImageView* imageView,
        /* [in] */ ILoadImageCallback* imageCallback);

    static void DrawableLoaded(
        /* [in] */ const String& imageUrl,
        /* [in] */ Boolean isHigh,
        /* [in] */ IBitmapDrawable* drawable);

    static CARAPI_(Boolean) NeedLoadHighDrawable(
        /* [in] */ const String& imageUrl);

    static CARAPI_(Boolean) GetOrigionWidthAndHeight(
        /* [in] */ const String& imageUrl,
        /* [out] */ Int32* w,
        /* [out] */ Int32* h);

    static CARAPI_(void) ClearAll();

    static String MakeImageParamKey(
        /* [in] */ const String& imageUrl,
        /* [in] */ Boolean isHigh);

    static CARAPI_(void) UpdateImageCache(
        /* [in] */ ArrayOf<String>* albumArray);

    static AutoPtr<ImageCacheService> GetCacheService();

private:
    static const String TAG;
    static Int32 sImgWidth;
    static Int32 sImgHeight;

    static AutoPtr<ImageCacheService> sCacheService;

    static HashMap<String, AutoPtr<IBitmapDrawable> > sImageCache;
    static HashMap<String, Boolean> sImageSampleStatus;
    static HashMap<String, AutoPtr<IWeakReference> > sHighImageCache;
    // static HashMap<String, AutoPtr<IBitmapDrawable> > sHighImageCache;
    static HashMap<String, AutoPtr< ArrayOf<Int32> > > sHighImageWH;
    static HashMap<String, AutoPtr<List<AutoPtr<ImageLoaderParam> > > > sImageLoaderParamCache;

    static Object sLock;
    static Object sStatusLock;
    static Object sImageWHLock;
};

} // namespace Gallery

#endif // __ASYNCIMAGELOADER_H__
