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
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/app/CWallpaperManager.h"
#include "elastos/droid/app/CFastBitmapDrawable.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/BitmapRegionDecoder.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseOutputStream.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::BitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::BitmapRegionDecoder;
using Elastos::Droid::Graphics::IBitmapRegionDecoder;
using Elastos::Droid::Graphics::MatrixScaleToFit_FILL;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;

using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String CWallpaperManager::TAG("WallpaperManager");
Boolean CWallpaperManager::DEBUG = FALSE;
Object CWallpaperManager::sSync;
AutoPtr<CGlobalsWallpaperManagerCallback> CWallpaperManager::sGlobals;

/** {@hide} */
const String CWallpaperManager::PROP_WALLPAPER("ro.config.wallpaper");
/** {@hide} */
const String CWallpaperManager::PROP_WALLPAPER_COMPONENT("ro.config.wallpaper_component");

CAR_INTERFACE_IMPL(CWallpaperManager, Object, IWallpaperManager)

CAR_OBJECT_IMPL(CWallpaperManager)

CWallpaperManager::CWallpaperManager()
    : mWallpaperXStep(-1)
    , mWallpaperYStep(-1)
{}

ECode CWallpaperManager::constructor(
    /* [in] */ IContext *context,
    /* [in] */ IHandler *handler)
{
    mContext = context;
    AutoPtr<ILooper> looper;
    FAIL_RETURN(context->GetMainLooper((ILooper**)&looper));
    return InitGlobals(looper);
}

ECode CWallpaperManager::InitGlobals(
    /* [in] */ ILooper* looper)
{
    AutoLock lock(sSync);
    if (sGlobals == NULL) {
        return CGlobalsWallpaperManagerCallback::NewByFriend(
            looper, (CGlobalsWallpaperManagerCallback**)&sGlobals);
    }
    return NOERROR;
}

ECode CWallpaperManager::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IWallpaperManager** manager)
{
    VALIDATE_NOT_NULL(manager);
    *manager = NULL;
    AutoPtr<IInterface> service;
    FAIL_RETURN(context->GetSystemService(IContext::WALLPAPER_SERVICE, (IInterface**)&service))
    *manager = IWallpaperManager::Probe(service);
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

ECode CWallpaperManager::GetIWallpaperManager(
    /* [out] */ IIWallpaperManager **manager)
{
    VALIDATE_NOT_NULL(manager);
    *manager = sGlobals->mService;
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

ECode CWallpaperManager::GetDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IBitmap> bm = sGlobals->PeekWallpaperBitmap(mContext, TRUE);
    if (bm != NULL) {
        AutoPtr<IDrawable> dr;
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        CBitmapDrawable::New(res, bm, (IDrawable**)&dr);
        dr->SetDither(FALSE);
        *drawable = dr;
        REFCOUNT_ADD(*drawable);
        return NOERROR;
    }
    *drawable = NULL;
    return NOERROR;
}

ECode CWallpaperManager::GetBuiltInDrawable(
    /* [out] */ IDrawable** drawable)
{
    return GetBuiltInDrawable(0, 0, FALSE, 0.0, 0.0, drawable);
}

ECode CWallpaperManager::GetBuiltInDrawable(
    /* [in] */ Int32 outWidth,
    /* [in] */ Int32 outHeight,
    /* [in] */ Boolean scaleToFit,
    /* [in] */ Float horizontalAlignment,
    /* [in] */ Float verticalAlignment,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;

    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        return NOERROR;
    }

    using Elastos::Core::Math;

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    horizontalAlignment = (Float)Math::Max(0.0, (Double)Math::Min(1.0, (Double)horizontalAlignment));
    verticalAlignment = (Float)Math::Max(0.0, (Double)Math::Min(1.0, (Double)verticalAlignment));

    AutoPtr<IInputStream> is;
    CBufferedInputStream::New(OpenDefaultWallpaper(mContext), (IInputStream**)&is);

    if (is == NULL) {
        Logger::E(TAG, "default wallpaper input stream is NULL");
        return NOERROR;
    }
    else {
        if (outWidth <= 0 || outHeight <= 0) {
            AutoPtr<IBitmap> fullSize;
            BitmapFactory::DecodeStream(is, NULL, NULL, (IBitmap**)&fullSize);
            return CBitmapDrawable::New(resources, fullSize, drawable);
        }
        else {
            Int32 inWidth;
            Int32 inHeight;
            {
                AutoPtr<IBitmapFactoryOptions> options;
                CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
                options->SetInJustDecodeBounds(TRUE);
                AutoPtr<IBitmap> tmp;
                BitmapFactory::DecodeStream(is, NULL, options, (IBitmap**)&tmp);
                Int32 ow, oh;
                options->GetOutWidth(&ow);
                options->GetOutHeight(&oh);
                if (ow != 0 && oh != 0) {
                    inWidth = ow;
                    inHeight = oh;
                } else {
                    Logger::E(TAG, "default wallpaper dimensions are 0");
                    return NOERROR;
                }
            }

            CBufferedInputStream::New(OpenDefaultWallpaper(mContext), (IInputStream**)&is);

            AutoPtr<IRectF> cropRectF;

            outWidth = Math::Min(inWidth, outWidth);
            outHeight = Math::Min(inHeight, outHeight);
            if (scaleToFit) {
                cropRectF = GetMaxCropRect(inWidth, inHeight, outWidth, outHeight,
                    horizontalAlignment, verticalAlignment);
            }
            else {
                Float left = (inWidth - outWidth) * horizontalAlignment;
                Float right = left + outWidth;
                Float top = (inHeight - outHeight) * verticalAlignment;
                Float bottom = top + outHeight;
                CRectF::New(left, top, right, bottom, (IRectF**)&cropRectF);
            }
            AutoPtr<IRect> roundedTrueCrop;
            CRect::New((IRect**)&roundedTrueCrop);
            cropRectF->RoundOut(roundedTrueCrop);

            Int32 w, h;
            roundedTrueCrop->GetWidth(&w);
            roundedTrueCrop->GetHeight(&h);
            if (w <= 0 || h <= 0) {
                Logger::W(TAG, "crop has bad values for full size image");
                return NOERROR;
            }

            // See how much we're reducing the size of the image
            Int32 scaleDownSampleSize = Math::Min(w / outWidth, h / outHeight);

            // Attempt to open a region decoder
            AutoPtr<IBitmapRegionDecoder> decoder;
            // try {
            ECode ec = BitmapRegionDecoder::NewInstance(is, TRUE, (IBitmapRegionDecoder**)&decoder);
            // } catch (IOException e) {
            if (ec == (ECode)E_IO_EXCEPTION) {
                Logger::W(TAG, "cannot open region decoder for default wallpaper");
            }
            // }

            AutoPtr<IBitmap> crop;
            if (decoder != NULL) {
                // Do region decoding to get crop bitmap
                AutoPtr<IBitmapFactoryOptions> options;
                CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
                if (scaleDownSampleSize > 1) {
                    options->SetInSampleSize(scaleDownSampleSize);
                }
                decoder->DecodeRegion(roundedTrueCrop, options, (IBitmap**)&crop);
                decoder->Recycle();
            }

            if (crop == NULL) {
                // BitmapRegionDecoder has failed, try to crop in-memory
                is = NULL;
                CBufferedInputStream::New(OpenDefaultWallpaper(mContext), (IInputStream**)&is);
                AutoPtr<IBitmap> fullSize;
                if (is != NULL) {
                    AutoPtr<IBitmapFactoryOptions> options;
                    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
                    if (scaleDownSampleSize > 1) {
                        options->SetInSampleSize(scaleDownSampleSize);
                    }
                    BitmapFactory::DecodeStream(is, NULL, options, (IBitmap**)&fullSize);
                }
                if (fullSize != NULL) {
                    Int32 l, t, w, h;
                    roundedTrueCrop->GetLeft(&l);
                    roundedTrueCrop->GetTop(&t);
                    roundedTrueCrop->GetWidth(&w);
                    roundedTrueCrop->GetHeight(&h);
                    CBitmap::CreateBitmap(fullSize, l, t, w, h, (IBitmap**)&crop);
                }
            }

            if (crop == NULL) {
                Logger::W(TAG, "cannot decode default wallpaper");
                return NOERROR;
            }

            // Scale down if necessary
            crop->GetWidth(&w);
            crop->GetHeight(&h);
            if (outWidth > 0 && outHeight > 0 && (w != outWidth || h != outHeight)) {
                AutoPtr<IMatrix> m;
                CMatrix::New((IMatrix**)&m);
                AutoPtr<IRectF> cropRect, returnRect;
                CRectF::New(0, 0, w, h, (IRectF**)&cropRect);
                CRectF::New(0, 0, outWidth, outHeight, (IRectF**)&returnRect);
                Boolean bval;
                m->SetRectToRect(cropRect, returnRect, MatrixScaleToFit_FILL, &bval);
                Float fw, fh;
                returnRect->GetWidth(&fw);
                returnRect->GetHeight(&fh);
                AutoPtr<IBitmap> tmp;
                CBitmap::CreateBitmap((Int32)fw, (Int32)fh, BitmapConfig_ARGB_8888, (IBitmap**)&tmp);
                if (tmp != NULL) {
                    AutoPtr<ICanvas> c;
                    CCanvas::New(tmp, (ICanvas**)&c);
                    AutoPtr<IPaint> p;
                    CPaint::New((IPaint**)&p);
                    p->SetFilterBitmap(true);
                    c->DrawBitmap(crop, m, p);
                    crop = tmp;
                }
            }

            return CBitmapDrawable::New(resources, crop, drawable);
        }
    }

    return NOERROR;
}

AutoPtr<IRectF> CWallpaperManager::GetMaxCropRect(
    /* [in] */ Int32 inWidth,
    /* [in] */ Int32 inHeight,
    /* [in] */ Int32 outWidth,
    /* [in] */ Int32 outHeight,
    /* [in] */ Float horizontalAlignment,
    /* [in] */ Float verticalAlignment)
{
    AutoPtr<IRectF> cropRect;
    CRectF::New((IRectF**)&cropRect);

    // Get a crop rect that will fit this
    if (inWidth / (Float) inHeight > outWidth / (Float) outHeight) {
         cropRect->SetTop(0);
         cropRect->SetBottom(inHeight);
         Float cropWidth = outWidth * (inHeight / (Float) outHeight);
         Float left = (inWidth - cropWidth) * horizontalAlignment;
         cropRect->SetLeft(left);
         cropRect->SetRight(left + cropWidth);
    }
    else {
        cropRect->SetLeft(0);
        cropRect->SetRight(inWidth);
        Float cropHeight = outHeight * (inWidth / (Float) outWidth);
        Float top = (inHeight - cropHeight) * verticalAlignment;
        cropRect->SetTop(top);
        cropRect->SetBottom(top + cropHeight);
    }
    return cropRect;
}

ECode CWallpaperManager::PeekDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IBitmap> bm = sGlobals->PeekWallpaperBitmap(mContext, FALSE);
    if (bm != NULL) {
        AutoPtr<IDrawable> dr;
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        CBitmapDrawable::New(res, bm, (IDrawable**)&dr);
        dr->SetDither(FALSE);
        *drawable = dr;
        REFCOUNT_ADD(*drawable);
        return NOERROR;
    }
    *drawable = NULL;
    return NOERROR;
}

ECode CWallpaperManager::GetFastDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IBitmap> bm = sGlobals->PeekWallpaperBitmap(mContext, TRUE);
    if (bm != NULL) {
        AutoPtr<IFastBitmapDrawable> fd;
        CFastBitmapDrawable::New(bm, (IFastBitmapDrawable**)&fd);
        *drawable = IDrawable::Probe(fd.Get());
        REFCOUNT_ADD(*drawable);
        return NOERROR;
    }
    *drawable = NULL;
    return NOERROR;
}

ECode CWallpaperManager::GetFastKeyguardDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IBitmap> bm = sGlobals->PeekKeyguardWallpaperBitmap(mContext);
    if (bm != NULL) {
        return CFastBitmapDrawable::New(bm, drawable);
    }
    *drawable = NULL;
    return NOERROR;
}

ECode CWallpaperManager::PeekFastDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IBitmap> bm = sGlobals->PeekWallpaperBitmap(mContext, FALSE);
    if (bm != NULL) {
        AutoPtr<IFastBitmapDrawable> fd;
        CFastBitmapDrawable::New(bm, (IFastBitmapDrawable**)&fd);
        *drawable = IDrawable::Probe(fd.Get());
        REFCOUNT_ADD(*drawable);
        return NOERROR;
    }
    *drawable = NULL;
    return NOERROR;
}

ECode CWallpaperManager::GetBitmap(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    AutoPtr<IBitmap> bm = sGlobals->PeekWallpaperBitmap(mContext, TRUE);
    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CWallpaperManager::GetKeyguardBitmap(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    AutoPtr<IBitmap> bm = sGlobals->PeekKeyguardWallpaperBitmap(mContext);
    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CWallpaperManager::ForgetLoadedWallpaper()
{
    sGlobals->ForgetLoadedWallpaper();
    return NOERROR;
}

ECode CWallpaperManager::ForgetLoadedKeyguardWallpaper()
{
    sGlobals->ForgetLoadedKeyguardWallpaper();
    return NOERROR;
}

ECode CWallpaperManager::GetWallpaperInfo(
    /* [out] */ IWallpaperInfo** info)
{
    VALIDATE_NOT_NULL(info);
    // try {
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        *info = NULL;
        return NOERROR;
    }
    else {
        return sGlobals->mService->GetWallpaperInfo(info);
    }
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CWallpaperManager::GetCropAndSetWallpaperIntent(
    /* [in] */ IUri* imageUri,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;

    if (imageUri == NULL) {
        Logger::E(TAG, "Image URI must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String scheme;
    imageUri->GetScheme(&scheme);
    if (!IContentResolver::SCHEME_CONTENT.Equals(scheme)) {
        Logger::E(TAG, "Image URI must be of the %s scheme type",
            IContentResolver::SCHEME_CONTENT.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IIntent> cropAndSetWallpaperIntent;
    CIntent::New(ACTION_CROP_AND_SET_WALLPAPER, imageUri, (IIntent**)&cropAndSetWallpaperIntent);
    cropAndSetWallpaperIntent->AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);

    // Find out if the default HOME activity supports CROP_AND_SET_WALLPAPER
    AutoPtr<IIntent> homeIntent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&homeIntent);
    homeIntent->AddCategory(IIntent::CATEGORY_HOME);
    AutoPtr<IResolveInfo> resolvedHome;
    packageManager->ResolveActivity(homeIntent,
        IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&resolvedHome);
    if (resolvedHome != NULL) {
        AutoPtr<IActivityInfo> ai;
        resolvedHome->GetActivityInfo((IActivityInfo**)&ai);
        String packageName;
        IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
        cropAndSetWallpaperIntent->SetPackage(packageName);

        AutoPtr<IList> cropAppList;
        packageManager->QueryIntentActivities(cropAndSetWallpaperIntent, 0, (IList**)&cropAppList);
        Int32 size;
        cropAppList->GetSize(&size);
        if (size > 0) {
            *intent = cropAndSetWallpaperIntent;
            REFCOUNT_ADD(*intent)
            return NOERROR;
        }
    }

    // fallback crop activity
    cropAndSetWallpaperIntent->SetPackage(String("com.android.wallpapercropper"));
    AutoPtr<IList> cropAppList;
    packageManager->QueryIntentActivities(cropAndSetWallpaperIntent, 0, (IList**)&cropAppList);
    Int32 size;
    cropAppList->GetSize(&size);
    if (size > 0) {
        *intent = cropAndSetWallpaperIntent;
        REFCOUNT_ADD(*intent)
        return NOERROR;
    }

    // If the URI is not of the right type, or for some reason the system wallpaper
    // cropper doesn't exist, return NULL
    // Logger::E(TAG, "Cannot use passed URI to set wallpaper; " +
    //     "check that the type returned by ContentProvider matches image/*");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CWallpaperManager::SetResource(
    /* [in] */ Int32 resid)
{
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        return NOERROR;
    }
    // try {
    AutoPtr<IResources> resources;
    ASSERT_SUCCEEDED(mContext->GetResources((IResources**)&resources));
    String name;
    resources->GetResourceName(resid, &name);
    /* Set the wallpaper to the default values */
    AutoPtr<IParcelFileDescriptor> fd;
    FAIL_RETURN(sGlobals->mService->SetWallpaper(
            String("res:") + name, (IParcelFileDescriptor**)&fd));
    if (fd != NULL) {
        AutoPtr<IFileOutputStream> fos;
        // try {
        // fos = new ParcelFileDescriptor.AutoCloseOutputStream(fd);
        AutoPtr<IFileDescriptor> des;
        fd->GetFileDescriptor((IFileDescriptor**)&des);
        ASSERT_SUCCEEDED(CFileOutputStream::New(des, (IFileOutputStream**)&fos));
        AutoPtr<IInputStream> res;
        ASSERT_SUCCEEDED(resources->OpenRawResource(resid, (IInputStream**)&res));
        SetWallpaper(res, fos);
        if (fos != NULL) {
            ICloseable::Probe(fos)->Close();
        }
        // } finally {
        //     if (fos != NULL) {
        //         fos.close();
        //     }
        // }
    }
    // } catch (RemoteException e) {
    //     // Ignore
    // }
    return NOERROR;
}

ECode CWallpaperManager::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        return NOERROR;
    }
    // try {
    AutoPtr<IParcelFileDescriptor> fd;
    FAIL_RETURN(sGlobals->mService->SetWallpaper(String(NULL), (IParcelFileDescriptor**)&fd));
    if (fd == NULL) {
        return NOERROR;
    }
    AutoPtr<IFileOutputStream> fos;
    // try {
    AutoPtr<IFileDescriptor> des;
    fd->GetFileDescriptor((IFileDescriptor**)&des);
    ASSERT_SUCCEEDED(CFileOutputStream::New(des, (IFileOutputStream**)&fos));
    // fos = new ParcelFileDescriptor.AutoCloseOutputStream(fd);
    Boolean result;
    bitmap->Compress(BitmapCompressFormat_PNG, 90, IOutputStream::Probe(fos), &result);
    // } finally {
    if (fos != NULL) {
        ICloseable::Probe(fos)->Close();
    }
    return NOERROR;
    // }
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

ECode CWallpaperManager::SetKeyguardBitmap(
    /* [in] */ IBitmap* bitmap)
{
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        return NOERROR;
    }
    // try {
    AutoPtr<IParcelFileDescriptor> fd;
    ECode ec = sGlobals->mService->SetKeyguardWallpaper(String(NULL), (IParcelFileDescriptor**)&fd);
    if (FAILED(ec) || fd == NULL) {
        return NOERROR;
    }
    AutoPtr<IFileOutputStream> fos;
    // try {
    CParcelFileDescriptorAutoCloseOutputStream::New(fd, (IFileOutputStream**)&fos);
    Boolean result;
    bitmap->Compress(BitmapCompressFormat_PNG, 90, IOutputStream::Probe(fos), &result);
    if (fos != NULL) {
        ICloseable::Probe(fos)->Close();
    }
    return NOERROR;
    // } finally {
    //     if (fos != null) {
    //         fos.close();
    //     }
    // }
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

ECode CWallpaperManager::SetStream(
    /* [in] */ IInputStream* data)
{
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        return NOERROR;
    }
    // try {
    AutoPtr<IParcelFileDescriptor> fd;
    FAIL_RETURN(sGlobals->mService->SetWallpaper(String(NULL), (IParcelFileDescriptor**)&fd));
    if (fd == NULL) {
        return NOERROR;
    }
    AutoPtr<IFileOutputStream> fos;
    // try {
    AutoPtr<IFileDescriptor> des;
    fd->GetFileDescriptor((IFileDescriptor**)&des);
    ASSERT_SUCCEEDED(CFileOutputStream::New(des, (IFileOutputStream**)&fos));
    // fos = new ParcelFileDescriptor.AutoCloseOutputStream(fd);
    SetWallpaper(data, fos);
    // } finally {
    if (fos != NULL) {
        ICloseable::Probe(fos)->Close();
    }
    return NOERROR;
    // }
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

ECode CWallpaperManager::SetKeyguardStream(
    /* [in] */ IInputStream* data)
{
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "CWallpaperService not running");
        return NOERROR;
    }
    // try {
    AutoPtr<IParcelFileDescriptor> fd;
    ECode ec = sGlobals->mService->SetKeyguardWallpaper(String(NULL), (IParcelFileDescriptor**)&fd);
    if (FAILED(ec) || fd == NULL) return NOERROR;
    AutoPtr<IFileOutputStream> fos;
    // try {
    CParcelFileDescriptorAutoCloseOutputStream::New(fd, (IFileOutputStream**)&fos);
    SetWallpaper(data, fos);
    if (fos != NULL) {
        ICloseable::Probe(fos)->Close();
    }
    return NOERROR;
    // } finally {
    //     if (fos != null) {
    //         fos.close();
    //     }
    // }
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

void CWallpaperManager::SetWallpaper(
    /* [in] */ IInputStream* data,
    /* [in] */ IFileOutputStream* fos)
{
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(32768);
    IOutputStream* os = IOutputStream::Probe(fos);
    Int32 amt;
    while (data->Read(buffer, &amt), amt > 0) {
        os->Write(buffer, 0, amt);
    }
    return;
}

ECode CWallpaperManager::HasResourceWallpaper(
    /* [in] */ Int32 resid,
    /* [out] */ Boolean* hasResource)
{
    VALIDATE_NOT_NULL(hasResource);
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        *hasResource = FALSE;
        return NOERROR;
    }
    // try {
    AutoPtr<IResources> resources;
    ASSERT_SUCCEEDED(mContext->GetResources((IResources**)&resources));
    String resName;
    resources->GetResourceName(resid, &resName);
    String name = String("res:") + resName;
    return sGlobals->mService->HasNamedWallpaper(name, hasResource);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWallpaperManager::GetDesiredMinimumWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        *width = 0;
        return NOERROR;
    }
    // try {
    return sGlobals->mService->GetWidthHint(width);
    // } catch (RemoteException e) {
    //     // Shouldn't happen!
    //     return 0;
    // }
}

ECode CWallpaperManager::GetDesiredMinimumHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        *height = 0;
        return NOERROR;
    }
    // try {
    return sGlobals->mService->GetHeightHint(height);
    // } catch (RemoteException e) {
    //     // Shouldn't happen!
    //     return 0;
    // }
}

ECode CWallpaperManager::SuggestDesiredDimensions(
    /* [in] */ Int32 minimumWidth,
    /* [in] */ Int32 minimumHeight)
{
    // try {
    /**
     * The framework makes no attempt to limit the window size
     * to the maximum texture size. Any window larger than this
     * cannot be composited.
     *
     * Read maximum texture size from system property and scale down
     * minimumWidth and minimumHeight accordingly.
     */
    Int32 maximumTextureSize = 0;
    SystemProperties::GetInt32(String("sys.max_texture_size"), 0, &maximumTextureSize);

    if (maximumTextureSize > 0) {
        if ((minimumWidth > maximumTextureSize) ||
            (minimumHeight > maximumTextureSize)) {
            Float aspect = (Float)minimumHeight / (Float)minimumWidth;
            if (minimumWidth > minimumHeight) {
                minimumWidth = maximumTextureSize;
                minimumHeight = (Int32)((minimumWidth * aspect) + 0.5);
            } else {
                minimumHeight = maximumTextureSize;
                minimumWidth = (Int32)((minimumHeight / aspect) + 0.5);
            }
        }
    }


    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
        return NOERROR;
    }
    else {
        return sGlobals->mService->SetDimensionHints(minimumWidth, minimumHeight);
    }
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

ECode CWallpaperManager::SetDisplayPadding(
    /* [in] */ IRect* padding)
{
    if (sGlobals->mService == NULL) {
        Logger::W(TAG, "WallpaperService not running");
    }
    else {
        sGlobals->mService->SetDisplayPadding(padding);
    }
    return NOERROR;
}

ECode CWallpaperManager::SetDisplayOffset(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    //Log.v(TAG, "Sending new wallpaper display offsets from app...");
    return CWindowManagerGlobal::GetWindowSession()->SetWallpaperDisplayOffset(
            windowToken, x, y);
}

ECode CWallpaperManager::SetWallpaperOffsets(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Float xOffset,
    /* [in] */ Float yOffset)
{
    //Log.v(TAG, "Sending new wallpaper offsets from app...");
    return CWindowManagerGlobal::GetWindowSession()->SetWallpaperPosition(
            windowToken, xOffset, yOffset, mWallpaperXStep, mWallpaperYStep);
}

ECode CWallpaperManager::SetWallpaperOffsetSteps(
    /* [in] */ Float xStep,
    /* [in] */ Float yStep)
{
    mWallpaperXStep = xStep;
    mWallpaperYStep = yStep;
    return NOERROR;
}

/** @hide */
ECode CWallpaperManager::GetLastWallpaperX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);
    // try {
    ECode ec = CWindowManagerGlobal::GetWindowSession()->GetLastWallpaperX(x);
    if (SUCCEEDED(ec)) return NOERROR;
    // } catch (RemoteException e) {
    //     // Ignore.
    // }

    *x = -1;
    return NOERROR;
}

/** @hide */
ECode CWallpaperManager::GetLastWallpaperY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    // try {
    ECode ec = CWindowManagerGlobal::GetWindowSession()->GetLastWallpaperY(y);
    if (SUCCEEDED(ec)) return NOERROR;
    // } catch (RemoteException e) {
    //     // Ignore.
    // }

    *y = -1;
    return NOERROR;
}

ECode CWallpaperManager::SendWallpaperCommand(
    /* [in] */ IBinder *windowToken,
    /* [in] */ const String &action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras)
{
    // try {
    //Log.v(TAG, "Sending new wallpaper offsets from app...");
    AutoPtr<IBundle> b;
    return CWindowManagerGlobal::GetWindowSession()->SendWallpaperCommand(
            windowToken, action, x, y, z, extras, FALSE, (IBundle**)&b);
    //Log.v(TAG, "...app returning after sending offsets!");
    // } catch (RemoteException e) {
    //     // Ignore.
    // }
}

ECode CWallpaperManager::ClearWallpaperOffsets(
    /* [in] */ IBinder* windowToken)
{
    // try {
    return CWindowManagerGlobal::GetWindowSession()->SetWallpaperPosition(
            windowToken, -1, -1, -1, -1);
    // } catch (RemoteException e) {
    //     // Ignore.
    // }
}

ECode CWallpaperManager::Clear()
{
    return Clear(TRUE);
}

/** @hide */
ECode CWallpaperManager::Clear(
    /* [in] */ Boolean setToDefault)
{
    if (setToDefault) {
        return SetStream(OpenDefaultWallpaper(mContext));
    }
    else {
        AutoPtr<IBitmap> blackBmp;
        CBitmap::CreateBitmap(1, 1, BitmapConfig_RGB_565, (IBitmap**)&blackBmp);
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Int32 color;
        res->GetColor(R::color::black, &color);
        blackBmp->SetPixel(0, 0, color);
        return SetBitmap(blackBmp);
    }
}

/**
 * @hide
 */
ECode CWallpaperManager::ClearKeyguardWallpaper()
{
    sGlobals->ClearKeyguardWallpaper();
    return NOERROR;
}

AutoPtr<IInputStream> CWallpaperManager::OpenDefaultWallpaper(
    /* [in] */ IContext* context)
{
    String path;
    SystemProperties::Get(PROP_WALLPAPER, &path);
    if (!TextUtils::IsEmpty(path)) {
        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        Boolean exist;
        if (file->Exists(&exist), exist) {
            // try {
        AutoPtr<IInputStream> is;
        CFileInputStream::New(file, (IInputStream**)&is);
        return is;
            // } catch (IOException e) {
            //     // Ignored, fall back to platform default below
            // }
        }
    }
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IInputStream> is;
    res->OpenRawResource(R::drawable::default_wallpaper, (IInputStream**)&is);
    return is;
}

AutoPtr<IComponentName> CWallpaperManager::GetDefaultWallpaperComponent(
    /* [in] */ IContext* context)
{
    String flat;
    SystemProperties::Get(PROP_WALLPAPER_COMPONENT, &flat);
    if (!TextUtils::IsEmpty(flat)) {
        AutoPtr<IComponentName> cn;
        CComponentName::UnflattenFromString(flat, (IComponentName**)&cn);
        if (cn != NULL) {
            return cn;
        }
    }

    context->GetString(R::string::default_wallpaper_component, &flat);
    if (!TextUtils::IsEmpty(flat)) {
        AutoPtr<IComponentName> cn;
        CComponentName::UnflattenFromString(flat, (IComponentName**)&cn);
        if (cn != NULL) {
            return cn;
        }
    }

    return NULL;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
