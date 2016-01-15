#include "AsyncImageLoader.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
#include <unistd.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IBitmapDrawable;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String AsyncImageLoader::TAG("AsyncImageLoader");

Int32 AsyncImageLoader::sImgWidth = 320;
Int32 AsyncImageLoader::sImgHeight = 320;
AutoPtr<ImageCacheService> AsyncImageLoader::sCacheService;
HashMap<String, AutoPtr<IBitmapDrawable> > AsyncImageLoader::sImageCache;
HashMap<String, Boolean> AsyncImageLoader::sImageSampleStatus;
HashMap<String, AutoPtr<IWeakReference> > AsyncImageLoader::sHighImageCache;
// HashMap<String, AutoPtr<IBitmapDrawable> > AsyncImageLoader::sHighImageCache;
HashMap<String, AutoPtr< ArrayOf<Int32> > > AsyncImageLoader::sHighImageWH;
HashMap<String, AutoPtr<List<AutoPtr<ImageLoaderParam> > > > AsyncImageLoader::sImageLoaderParamCache;
Mutex AsyncImageLoader::sLock;
Mutex AsyncImageLoader::sStatusLock;
Mutex AsyncImageLoader::sImageWHLock;

AsyncImageLoader::ImageLoaderThread::ImageLoaderThread(
    /* [in] */ Boolean isHigh,
    /* [in] */ const String& imageUrl)
    : mIsHigh(isHigh)
    , mImageUrl(imageUrl)
{}

CAR_INTERFACE_IMPL(AsyncImageLoader::ImageLoaderThread, IRunnable)

ECode AsyncImageLoader::ImageLoaderThread::Run()
{
    Logger::D(TAG, "ImageLoaderThread::++++ tid:%d", gettid());
    AutoPtr<IBitmapDrawable> drawable = LoadImageFromUrl(mImageUrl);
    AsyncImageLoader::DrawableLoaded(mImageUrl, mIsHigh, drawable);

    Logger::D(TAG, "ImageLoaderThread::----- drawable:%p tid:%d", drawable.Get(), gettid());
    return NOERROR;
}

AutoPtr<IBitmapDrawable> AsyncImageLoader::ImageLoaderThread::LoadImageFromUrl(
    /* [in] */ const String& path)
{
    Logger::D(TAG, "LoadImageFromUrl()----path:%s tid:%d", path.string(), gettid());
    AutoPtr<ImageCacheService> cacheService = GetCacheService();
    AutoPtr<IBitmap> bitmap;
    Int64 last = 0;
    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    file->LastModified(&last);
    Int32 hashkey = path.GetHashCode();
    Int64 len = 0;
    file->GetLength(&len);
    cacheService->GetImageData(hashkey, mIsHigh, len, last, (IBitmap**)&bitmap);
    if (bitmap != NULL) {
        {
            AutoLock lock(sStatusLock);
            sImageSampleStatus[path] = TRUE;
        }
        AutoPtr<IBitmapDrawable> bitmapDrawable;
        CBitmapDrawable::New(bitmap, (IBitmapDrawable**)&bitmapDrawable);
        Logger::D(TAG, "LoadImageFromUrl()---1---bitmapDrawable:%p", bitmapDrawable.Get());
        return bitmapDrawable;
    }

    AutoPtr<IBitmapFactoryOptions> opt;
    Int32 inSampleSize = 0;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opt);
    opt->SetInJustDecodeBounds(TRUE);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    factory->DecodeFile(path, opt, (IBitmap**)&bitmap);

    if (mIsHigh) {
        inSampleSize = ComputeSampleSize(opt, -1, 1280 * 672 * 4);
    }
    else {
        inSampleSize = ComputeSampleSize(opt, -1, sImgWidth * sImgHeight);
    }

    Logger::D(TAG, "LoadImageFromUrl()----inSampleSize:%d tid:%d",
        inSampleSize, gettid());
    opt->SetInSampleSize(inSampleSize);
    opt->SetInJustDecodeBounds(FALSE);

    {
        AutoLock lock(sStatusLock);
        sImageSampleStatus[path] = (inSampleSize > 1 || mIsHigh);
    }

    bitmap = NULL;
    factory->DecodeFile(path, opt, (IBitmap**)&bitmap);
    AutoPtr<IBitmapDrawable> bitmapDrawable;
    CBitmapDrawable::New(bitmap, (IBitmapDrawable**)&bitmapDrawable);
    if (inSampleSize > 1 && bitmap != NULL) {
        cacheService->PutImageData(hashkey, mIsHigh, len, last, bitmap);
    }
    Logger::D(TAG, "LoadImageFromUrl()---2---bitmapDrawable:%p tid:%d",
        bitmapDrawable.Get(), gettid());
    return bitmapDrawable;
}

Int32 AsyncImageLoader::ImageLoaderThread::ComputeSampleSize(
    /* [in] */ IBitmapFactoryOptions* options,
    /* [in] */ Int32 minSideLength,
    /* [in] */ Int32 maxNumOfPixels)
{
    Int32 initialSize = ComputeInitialSampleSize(options, minSideLength, maxNumOfPixels);
    Int32 roundedSize = 0;

    if (initialSize <= 8 ) {
        roundedSize = 1;
        while (roundedSize < initialSize) {
            roundedSize <<= 1;
        }
    }
    else {
        roundedSize = (initialSize + 7) / 8 * 8;
    }

    return roundedSize;
}

Int32 AsyncImageLoader::ImageLoaderThread::ComputeInitialSampleSize(
    /* [in] */ IBitmapFactoryOptions* options,
    /* [in] */ Int32 minSideLength,
    /* [in] */ Int32 maxNumOfPixels)
{
    Int32 w = 0;
    Int32 h = 0;
    options->GetOutWidth(&w);
    options->GetOutHeight(&h);
    Logger::D(TAG, "ComputeInitialSampleSize----w:%d,h:%d, tid:%d", w, h, gettid());

    if (mIsHigh) {
        AutoLock lock(sImageWHLock);
        AutoPtr< ArrayOf<Int32> > origionWH = ArrayOf<Int32>::Alloc(2);
        (*origionWH)[0] = w;
        (*origionWH)[1] = h;
        sHighImageWH[mImageUrl] = origionWH;

        Logger::D(TAG, "ComputeInitialSampleSize----size:%d", sHighImageWH.GetSize());
    }

    Int32 lowerBound = (maxNumOfPixels == -1/*UNCONSTRAINED*/) ? 1 :
        (Int32) Elastos::Core::Math::Ceil(Elastos::Core::Math::Sqrt((Double)w * h / maxNumOfPixels));
    Int32 upperBound = (minSideLength == -1/*UNCONSTRAINED*/) ? 128 :
        (Int32) Elastos::Core::Math::Min(Elastos::Core::Math::Floor((Double)w / minSideLength),
        Elastos::Core::Math::Floor((Double)(h / minSideLength)));

    if (upperBound < lowerBound) {
        // return the larger one when there is no overlapping zone.
        return lowerBound;
    }

    if ((maxNumOfPixels == -1/*UNCONSTRAINED*/) && (minSideLength == -1/*UNCONSTRAINED*/)) {
        return 1;
    }
    else if (minSideLength == -1/*UNCONSTRAINED*/) {
        return lowerBound;
    }
    else {
        return upperBound;
    }
}

AutoPtr<ImageCacheService> AsyncImageLoader::GetCacheService()
{
    if (sCacheService == NULL) {
        sCacheService = new ImageCacheService(sImgWidth, sImgHeight);
    }
    return sCacheService;
}

String AsyncImageLoader::MakeImageParamKey(
    /* [in] */ const String& imageUrl,
    /* [in] */ Boolean isHigh)
{
    StringBuilder sb(imageUrl);
    sb +="isHigh=";
    sb += isHigh;
    return sb.ToString();
}

AutoPtr<IBitmapDrawable> AsyncImageLoader::LoadDrawable(
    /* [in] */ const String& imageUrl,
    /* [in] */ Boolean isHigh,
    /* [in] */ IImageView* imageView,
    /* [in] */ ILoadImageCallback* imageCallback)
{
    Logger::D(TAG, "===================================================");
    Logger::D(TAG, "LoadDrawable()--imageUrl:%s,isHigh:%d, tid:%d", imageUrl.string(), isHigh, gettid());

    AutoLock lock(sLock);

    if (isHigh) {
        HashMap<String, AutoPtr<IWeakReference> >::Iterator it = sHighImageCache.Find(imageUrl);
        // HashMap<String, AutoPtr<IBitmapDrawable> >::Iterator it = sHighImageCache.Find(imageUrl);
        if (it != sHighImageCache.End()) {
            AutoPtr<IBitmapDrawable> drawable;
            AutoPtr<IWeakReference> drawableRef = it->mSecond;
            drawableRef->Resolve(EIID_IBitmapDrawable, (IInterface**)&drawable);
            // AutoPtr<IBitmapDrawable> drawable = it->mSecond;
            if (drawable != NULL) {
                return drawable;
            }
        }
    }
    else {
        HashMap<String, AutoPtr<IBitmapDrawable> >::Iterator it = sImageCache.Find(imageUrl);
        if (it != sImageCache.End()) {
            AutoPtr<IBitmapDrawable> drawable = it->mSecond;
            if (drawable != NULL) {
                return drawable;
            }
        }
    }

    {
        String key = MakeImageParamKey(imageUrl, isHigh);
        HashMap<String, AutoPtr<List<AutoPtr<ImageLoaderParam> > > >::Iterator it = sImageLoaderParamCache.Find(key);
        if (it != sImageLoaderParamCache.End()) {
            Logger::D(TAG, "===========It's already on loading process...");
            Boolean needAdd = TRUE;
            AutoPtr<List<AutoPtr<ImageLoaderParam> > > list = it->mSecond;
            List<AutoPtr<ImageLoaderParam> >::Iterator pit = list->Begin();
            for (; pit != list->End(); ++pit) {
                ImageLoaderParam* param = *pit;
                if (param->mImageView.Get() == imageView) {
                    Logger::D(TAG, "===========It's loading image for same imageView...");
                    needAdd = FALSE;
                    break;
                }
            }

            if (needAdd) {
                Logger::D(TAG, "===========It's loading image for new imageView...");
                AutoPtr<ImageLoaderParam> param = new ImageLoaderParam();
                param->mImageView = imageView;
                param->mCallback = imageCallback;
                list->PushBack(param);
            }
        }
        else {
            Logger::D(TAG, "===========It's a new loading process..., tid: %d", gettid());

            AutoPtr< List<AutoPtr<ImageLoaderParam> >  > list = new List<AutoPtr<ImageLoaderParam> >();
            AutoPtr<ImageLoaderParam> param = new ImageLoaderParam();
            param->mImageView = imageView;
            param->mCallback = imageCallback;
            list->PushBack(param);
            sImageLoaderParamCache[key] = list;

            AutoPtr<IThread> loadThread;
            AutoPtr<ImageLoaderThread> loadRunnable = new ImageLoaderThread(isHigh, imageUrl);
            CThread::New(IRunnable::Probe(loadRunnable), (IThread**)&loadThread);
            assert(loadThread != NULL);
            loadThread->Start();
        }
    }

    return NULL;
}

void AsyncImageLoader::DrawableLoaded(
    /* [in] */ const String& imageUrl,
    /* [in] */ Boolean isHigh,
    /* [in] */ IBitmapDrawable* d)
{
    AutoLock lock(sLock);

    AutoPtr<IBitmapDrawable> drawable = d;
    if (isHigh) {
        AutoPtr<IWeakReference> drawableRef;
        IWeakReferenceSource::Probe(drawable)->GetWeakReference((IWeakReference**)&drawableRef);
        sHighImageCache[imageUrl] = drawableRef;
        // sHighImageCache[imageUrl] = drawable;
        Logger::D(TAG, "ImageLoaderThread::----sHighImageCache---size:%d tid:%d",
            sHighImageCache.GetSize(), gettid());
    }
    else {
        sImageCache[imageUrl] = drawable;
        Logger::D(TAG, "ImageLoaderThread::----sImageCache---size:%d tid:%d",
            sImageCache.GetSize(), gettid());
    }

    {
        String key = MakeImageParamKey(imageUrl, isHigh);
        HashMap<String, AutoPtr<List<AutoPtr<ImageLoaderParam> > > >::Iterator it = sImageLoaderParamCache.Find(key);
        if (it != sImageLoaderParamCache.End()) {
            AutoPtr<List<AutoPtr<ImageLoaderParam> > > list = it->mSecond;
            List<AutoPtr<ImageLoaderParam> >::Iterator pit = list->Begin();
            for (; pit != list->End(); ++pit) {
                AutoPtr<ImageLoaderParam> param = *pit;
                param->mCallback->ImageLoaded(drawable, param->mImageView);
            }

            list->Clear();
            sImageLoaderParamCache.Erase(it);
        }
    }
}

Boolean AsyncImageLoader::NeedLoadHighDrawable(
    /* [in] */ const String& imageUrl)
{
    AutoLock lock(sStatusLock);
    HashMap<String, Boolean>::Iterator it = sImageSampleStatus.Find(imageUrl);
    if (it != sImageSampleStatus.End()) {
        return it->mSecond;
    }

    return TRUE;
}

Boolean AsyncImageLoader::GetOrigionWidthAndHeight(
    /* [in] */ const String& imageUrl,
    /* [out] */ Int32* w,
    /* [out] */ Int32* h)
{
    // VALIDATE_NOT_NULL(w);
    *w = 0;
    // VALIDATE_NOT_NULL(h);
    *h = 0;

    AutoLock lock(sImageWHLock);
    HashMap<String, AutoPtr< ArrayOf<Int32> > >::Iterator it = sHighImageWH.Find(imageUrl);
    if (it != sHighImageWH.End()) {
        AutoPtr< ArrayOf<Int32> > tmpArray = it->mSecond;
        *w = (*tmpArray)[0];
        *h = (*tmpArray)[1];
        return TRUE;
    }
    AutoPtr<IBitmapFactoryOptions> opt;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opt);
    opt->SetInJustDecodeBounds(TRUE);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> bitmap;
    if (FAILED(factory->DecodeFile(imageUrl, opt, (IBitmap**)&bitmap)))
        return FALSE;
    opt->GetOutWidth(w);
    opt->GetOutHeight(h);
    return TRUE;
}

void AsyncImageLoader::ClearAll()
{
    Logger::D(TAG, "ClearAll---");
    sImageCache.Clear();
    sImageSampleStatus.Clear();
    sHighImageCache.Clear();
    sHighImageWH.Clear();
}

void AsyncImageLoader::UpdateImageCache(
    /* [in] */ ArrayOf<String>* albumArray)
{
    Logger::D(TAG, "UpdateImageCache()---");
    AutoPtr<ImageCacheService> cacheService = GetCacheService();
    AutoPtr< ArrayOf<String> > album = albumArray;
    cacheService->UpdateCache();
    String albumPath;
    String key;
    Int32 len = album->GetLength();
    Boolean isMatch = FALSE;
    HashMap<String, AutoPtr<IBitmapDrawable> >::Iterator it = sImageCache.Begin();
    for (; it != sImageCache.End();) {
        key = it->mFirst;
        isMatch = FALSE;
        for (Int32 i = 0; i != len; i++) {
            if (key.Equals((*album)[i])) {
                isMatch = TRUE;
                break;
            }
        }
        if (isMatch) {
            ++it;
        }
        else {
            sImageCache.Erase(it++);
        }
    }
}

} // namespace Gallery
