#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CWallpaperManager.h"
#include "elastos/droid/content/pm/ThemeUtils.h"
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/internal/utility/cm/CmImageUtils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ThemesContract.h"
#include "elastos/droid/webkit/URLUtil.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CWallpaperManager;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::BitmapFactory;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::IThemesContractThemesColumns;
using Elastos::Droid::Provider::ThemesContract;
using Elastos::Droid::Webkit::URLUtil;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

const String CmImageUtils::TAG("CmImageUtils"); // = CmImageUtils.class.getSimpleName();
const String CmImageUtils::ASSET_URI_PREFIX("file:///android_asset/");
const Int32 CmImageUtils::DEFAULT_IMG_QUALITY = 100;

ECode CmImageUtils::GetImageDimension(
    /* [in] */ IInputStream* inputStream,
    /* [out] */ IPoint** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (inputStream == NULL) {
        // throw new IllegalArgumentException("'inputStream' cannot be null!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IBitmapFactoryOptions> options;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
    options->SetInJustDecodeBounds(TRUE);
    AutoPtr<IBitmap> tmp;
    BitmapFactory::DecodeStream(inputStream, NULL, options, (IBitmap**)&tmp);
    Int32 width, height;
    options->GetOutWidth(&width);
    options->GetOutHeight(&height);
    AutoPtr<IPoint> point;
    CPoint::New(width, height, (IPoint**)&point);
    *result = point;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CmImageUtils::CropImage(
    /* [in] */ IInputStream* inputStream,
    /* [in] */ Int32 imageWidth,
    /* [in] */ Int32 imageHeight,
    /* [in] */ Int32 outWidth,
    /* [in] */ Int32 outHeight,
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (inputStream == NULL){
        // throw new IllegalArgumentException("inputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (imageWidth <= 0 || imageHeight <= 0) {
        // throw new IllegalArgumentException(
        //         String.format("imageWidth and imageHeight must be > 0: imageWidth=%d" +
        //                 " imageHeight=%d", imageWidth, imageHeight));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (outWidth <= 0 || outHeight <= 0) {
        // throw new IllegalArgumentException(
        //         String.format("outWidth and outHeight must be > 0: outWidth=%d" +
        //                 " outHeight=%d", imageWidth, outHeight));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 scaleDownSampleSize = Elastos::Core::Math::Min(imageWidth / outWidth, imageHeight / outHeight);
    if (scaleDownSampleSize > 0) {
        imageWidth /= scaleDownSampleSize;
        imageHeight /= scaleDownSampleSize;
    }
    else {
        Float ratio = (Float) outWidth / outHeight;
        if (imageWidth < imageHeight * ratio) {
            outWidth = imageWidth;
            outHeight = (Int32) (outWidth / ratio);
        }
        else {
            outHeight = imageHeight;
            outWidth = (Int32) (outHeight * ratio);
        }
    }
    Int32 left = (imageWidth - outWidth) / 2;
    Int32 top = (imageHeight - outHeight) / 2;
    AutoPtr<IInputStream> compressed;
    // try {
    AutoPtr<IBitmapFactoryOptions> options;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
    if (scaleDownSampleSize > 1) {
        options->SetInSampleSize(scaleDownSampleSize);
    }
    AutoPtr<IBitmap> bitmap;
    BitmapFactory::DecodeStream(inputStream, NULL, options, (IBitmap**)&bitmap);
    if (bitmap == NULL) {
        return NOERROR;
    }
    AutoPtr<IBitmap> cropped;
    CBitmap::CreateBitmap(bitmap, left, top, outWidth, outHeight, (IBitmap**)&cropped);
    AutoPtr<IByteArrayOutputStream> tmpOut;
    CByteArrayOutputStream::New(2048, (IByteArrayOutputStream**)&tmpOut);
    Boolean b;
    if (cropped->Compress(Elastos::Droid::Graphics::BitmapCompressFormat_PNG,
            DEFAULT_IMG_QUALITY, IOutputStream::Probe(tmpOut), &b), b) {
        AutoPtr<ArrayOf<Byte> > outByteArray;
        tmpOut->ToByteArray((ArrayOf<Byte>**)&outByteArray);
        CByteArrayInputStream::New(outByteArray, (IByteArrayInputStream**)&compressed);
    }
    // } catch (Exception e) {
    //     Log.e(TAG, "Exception " + e);
    // }
    *result = compressed;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CmImageUtils::GetCroppedKeyguardStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context,
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (pkgName.IsEmpty()) {
        // throw new IllegalArgumentException("'pkgName' cannot be null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        // throw new IllegalArgumentException("'context' cannot be null!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputStream> cropped;
    AutoPtr<IInputStream> stream;
    // try {
    stream = GetOriginalKeyguardStream(pkgName, context);
    if (stream == NULL) {
        return NOERROR;
    }
    AutoPtr<IPoint> point;
    GetImageDimension(stream, (IPoint**)&point);
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(stream));
    Int32 x, y;
    point->GetX(&x);
    point->GetY(&y);
    if (point == NULL || x == 0 || y == 0) {
        return NOERROR;
    }
    AutoPtr<IWallpaperManager> wm;
    CWallpaperManager::GetInstance(context, (IWallpaperManager**)&wm);
    Int32 outWidth;
    wm->GetDesiredMinimumWidth(&outWidth);
    Int32 outHeight;
    wm->GetDesiredMinimumHeight(&outHeight);
    stream = GetOriginalKeyguardStream(pkgName, context);
    if (stream == NULL) {
        return NOERROR;
    }
    CropImage(stream, x, y, outWidth, outHeight, (IInputStream**)&cropped);
    // } catch (Exception e) {
    //     Log.e(TAG, "Exception " + e);
    // } finally {
    ioUtils->CloseQuietly(ICloseable::Probe(stream));
    // }
    *result = cropped;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CmImageUtils::GetCroppedWallpaperStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context,
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (pkgName.IsEmpty()) {
        // throw new IllegalArgumentException("'pkgName' cannot be null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        // throw new IllegalArgumentException("'context' cannot be null!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputStream> cropped;
    AutoPtr<IInputStream> stream;
    // try {
    stream = GetOriginalWallpaperStream(pkgName, context);
    if (stream == NULL) {
        return NOERROR;
    }
    AutoPtr<IPoint> point;
    GetImageDimension(stream, (IPoint**)&point);
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(stream));
    Int32 x, y;
    point->GetX(&x);
    point->GetY(&y);
    if (point == NULL || x == 0 || y == 0) {
        return NOERROR;
    }
    AutoPtr<IWallpaperManager> wm;
    CWallpaperManager::GetInstance(context, (IWallpaperManager**)&wm);
    Int32 outWidth;
    wm->GetDesiredMinimumWidth(&outWidth);
    Int32 outHeight;
    wm->GetDesiredMinimumHeight(&outHeight);
    stream = GetOriginalWallpaperStream(pkgName, context);
    if (stream == NULL) {
        return NOERROR;
    }
    CropImage(stream, x, y, outWidth, outHeight, (IInputStream**)&cropped);
    // } catch (Exception e) {
    //     Log.e(TAG, "Exception " + e);
    // } finally {
    ioUtils->CloseQuietly(ICloseable::Probe(stream));
    // }
    *result = cropped;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IInputStream> CmImageUtils::GetOriginalKeyguardStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context)
{
    if (pkgName.IsEmpty() || context == NULL) {
        return NULL;
    }

    AutoPtr<IInputStream> inputStream;
    // try {
    //Get input WP stream from the theme
    AutoPtr<IContext> themeCtx;
    context->CreatePackageContext(pkgName,
            IContext::CONTEXT_IGNORE_SECURITY, (IContext**)&themeCtx);
    AutoPtr<IAssetManager> assetManager;
    themeCtx->GetAssets((IAssetManager**)&assetManager);
    String wpPath;
    ThemeUtils::GetLockscreenWallpaperPath(assetManager, &wpPath);
    if (wpPath.IsNull()) {
        Logger::W(TAG, "Not setting lockscreen wp because wallpaper file was not found.");
    }
    else {
        ThemeUtils::GetInputStreamFromAsset(themeCtx,
                ASSET_URI_PREFIX + wpPath, (IInputStream**)&inputStream);
    }
    // } catch (Exception e) {
    //     Log.e(TAG, "There was an error setting lockscreen wp for pkg " + pkgName, e);
    // }
    return inputStream;
}

AutoPtr<IInputStream> CmImageUtils::GetOriginalWallpaperStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context)
{
    if (pkgName.IsEmpty() || context == NULL) {
        return NULL;
    }

    AutoPtr<IInputStream> inputStream;
    String selection = IThemesContractThemesColumns::PKG_NAME + "= ?";
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    selectionArgs->Set(0, pkgName);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> c;
    cr->Query(ThemesContract::ThemesColumns::CONTENT_URI, NULL, selection, selectionArgs, String(NULL), (ICursor**)&c);
    Int32 count;
    if (c == NULL || (c->GetCount(&count), count) < 1) {
        if (c != NULL) ICloseable::Probe(c)->Close();
        return NULL;
    }
    else {
        Boolean b;
        c->MoveToFirst(&b);
    }

    // try {
    AutoPtr<IContext> themeContext;
    context->CreatePackageContext(pkgName,
            IContext::CONTEXT_IGNORE_SECURITY, (IContext**)&themeContext);

    Int32 index;
    c->GetColumnIndex(IThemesContractThemesColumns::IS_LEGACY_THEME, &index);
    Int32 val;
    c->GetInt32(index, &val);
    Boolean isLegacyTheme = val == 1;

    c->GetColumnIndex(IThemesContractThemesColumns::WALLPAPER_URI, &index);
    String wallpaper;
    c->GetString(index, &wallpaper);

    if (!wallpaper.IsNull()) {
        if (URLUtil::IsAssetUrl(wallpaper)) {
            ThemeUtils::GetInputStreamFromAsset(themeContext, wallpaper, (IInputStream**)&inputStream);
        }
        else {
            AutoPtr<IUri> uri;
            Uri::Parse(wallpaper, (IUri**)&uri);
            cr->OpenInputStream(uri, (IInputStream**)&inputStream);
        }
    }
    else {
        // try and get the wallpaper directly from the apk if the URI was null
        AutoPtr<IContext> themeCtx;
        context->CreatePackageContext(pkgName,
                IContext::CONTEXT_IGNORE_SECURITY, (IContext**)&themeCtx);
        AutoPtr<IAssetManager> assetManager;
        themeCtx->GetAssets((IAssetManager**)&assetManager);
        String wpPath;
        ThemeUtils::GetWallpaperPath(assetManager, &wpPath);
        if (wpPath.IsNull()) {
            Logger::E(TAG, "Not setting wp because wallpaper file was not found.");
        }
        else {
            ThemeUtils::GetInputStreamFromAsset(themeCtx,
                    ASSET_URI_PREFIX + wpPath, (IInputStream**)&inputStream);
        }
    }
    // } catch (Exception e) {
    //     Log.e(TAG, "getWallpaperStream: " + e);
    // } finally {
    ICloseable::Probe(c)->Close();
    // }

    return inputStream;
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
