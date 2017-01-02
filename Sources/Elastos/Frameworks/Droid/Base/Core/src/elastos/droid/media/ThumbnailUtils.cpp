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

#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/ThumbnailUtils.h"
#include "elastos/droid/media/CMediaFile.h"
#include "elastos/droid/media/CMediaMetadataRetriever.h"
#include "elastos/droid/media/CExifInterface.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.CoreLibrary.IO.h>


using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Provider::IMediaStoreImagesThumbnails;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Media {

String ThumbnailUtils::TAG("ThumbnailUtils");

/* Maximum pixels size for created bitmap. */
const Int32 ThumbnailUtils::MAX_NUM_PIXELS_THUMBNAIL = 512 * 384;
const Int32 ThumbnailUtils::MAX_NUM_PIXELS_MICRO_THUMBNAIL = 160 * 120;
const Int32 ThumbnailUtils::UNCONSTRAINED = -1;

/* Options used internally. */
const Int32 ThumbnailUtils::OPTIONS_NONE = 0x0;
const Int32 ThumbnailUtils::OPTIONS_SCALE_UP = 0x1;

ECode ThumbnailUtils::CreateImageThumbnail(
    /* [in] */ const String& filePath,
    /* [in] */ Int32 kind,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    Boolean wantMini = (kind == IMediaStoreImagesThumbnails::MINI_KIND);

    Int32 targetSize = wantMini
            ? IThumbnailUtils::TARGET_SIZE_MINI_THUMBNAIL
            : IThumbnailUtils::TARGET_SIZE_MICRO_THUMBNAIL;
    Int32 maxPixels = wantMini
            ? MAX_NUM_PIXELS_THUMBNAIL
            : MAX_NUM_PIXELS_MICRO_THUMBNAIL;

    AutoPtr<SizedThumbnailBitmap> sizedThumbnailBitmap = new SizedThumbnailBitmap();

    AutoPtr<IBitmap> bitmap, temp;

    AutoPtr<IMediaFile> mediaFile;
    CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);

    AutoPtr<IMediaFileType> fileType;
    mediaFile->GetFileType(filePath, (IMediaFileType**)&fileType);
    Int32 type = 0;
    if (fileType != NULL) {
        fileType->GetFileType(&type);
    }

    if (type == IMediaFile::FILE_TYPE_JPEG) {
        CreateThumbnailFromEXIF(filePath, targetSize, maxPixels, sizedThumbnailBitmap);
        bitmap = sizedThumbnailBitmap->mBitmap;
    }

    if (bitmap == NULL) {
        //try {
        ECode ec = NOERROR;
        AutoPtr<IFileInputStream> stream;
        AutoPtr<IFileDescriptor> fd;
        AutoPtr<IBitmapFactoryOptions> options;
        Boolean isCancel;
        Int32 outWidth, outHeight;

        AutoPtr<IBitmapFactory> bitmapFactory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bitmapFactory);

        ec = CFileInputStream::New(filePath, (IFileInputStream**)&stream);
        FAIL_GOTO(ec, _EXIT_);

        ec = stream->GetFD((IFileDescriptor**)&fd);
        FAIL_GOTO(ec, _EXIT_);

        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
        options->SetInSampleSize(1);
        options->SetInJustDecodeBounds(TRUE);
        ec = bitmapFactory->DecodeFileDescriptor(fd, NULL, options, (IBitmap**)&temp);
        FAIL_GOTO(ec, _EXIT_);

        options->GetCancel(&isCancel);
        options->GetOutWidth(&outWidth);
        options->GetOutHeight(&outHeight);
        if (isCancel|| outWidth == -1 || outHeight -1) {
            *result = NULL;
            return NOERROR;
        }

        options->SetInSampleSize(ComputeSampleSize(options, targetSize, maxPixels));
        options->SetInJustDecodeBounds(FALSE);

        options->SetInDither(FALSE);
        options->SetInPreferredConfig(BitmapConfig_ARGB_8888);
        ec = bitmapFactory->DecodeFileDescriptor(fd, NULL, options, (IBitmap**)&bitmap);
        FAIL_GOTO(ec, _EXIT_);
        // } catch (IOException ex) {
        //     Log.e(TAG, "", ex);
        //     } catch (OutOfMemoryError oom) {
        //     Log.e(TAG, "Unable to decode file " + filePath + ". OutOfMemoryError.", oom);
        // } finally {
_EXIT_:
        if (ec == (ECode)E_IO_EXCEPTION || ec == (ECode)E_OUT_OF_MEMORY_ERROR) {
            Slogger::E(TAG, "Unable to decode file %s", filePath.string());
        }
        // try {
        if (stream != NULL) {
            ICloseable::Probe(stream)->Close();
            stream = NULL;
        }
        // } catch (IOException ex) {
        //     Log.e(TAG, "", ex);
        // }
    }

    if (kind == IMediaStoreImagesThumbnails::MICRO_KIND) {
        // now we make it a "square thumbnail" for MICRO_KIND thumbnail
        return ExtractThumbnail(bitmap,
            IThumbnailUtils::TARGET_SIZE_MICRO_THUMBNAIL,
            IThumbnailUtils::TARGET_SIZE_MICRO_THUMBNAIL,
            IThumbnailUtils::OPTIONS_RECYCLE_INPUT, result);
    }

    *result = bitmap.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ThumbnailUtils::CreateVideoThumbnail(
    /* [in] */ const String& filePath,
    /* [in] */ Int32 kind,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBitmap> bitmap;
    AutoPtr<IMediaMetadataRetriever> retriever;
    CMediaMetadataRetriever::New((IMediaMetadataRetriever**)&retriever);

    AutoPtr<IBitmapFactory> bitmapFactory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bitmapFactory);

    Int32 width, height, max, w, h;
    Float scale;
    AutoPtr<IBitmap> temp;

    //try {
    ECode ec = retriever->SetDataSource(filePath);
    FAIL_GOTO(ec, _EXIT_)

    ec = retriever->GetFrameAtTime(-1, (IBitmap**)&bitmap);
    FAIL_GOTO(ec, _EXIT_)

_EXIT_:
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        // Assume this is a corrupt video file
        ec = NOERROR;
    }
    else if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        // Assume this is a corrupt video file.
        ec = NOERROR;
    }

    if (retriever) {
        retriever->ReleaseResources();
    }

    if (FAILED(ec)) {
        *result = NULL;
        return NOERROR;
    }

    if (bitmap == NULL) {
        *result = NULL;
        return NOERROR;
    }

    if (kind == IMediaStoreImagesThumbnails::MINI_KIND) {
        // Scale down the bitmap if it's too large.
        bitmap->GetWidth(&width);
        bitmap->GetHeight(&height);

        max = Elastos::Core::Math::Max(width, height);
        if (max > 512) {
            scale = 512.0f / max;
            w = Elastos::Core::Math::Round(scale * width);
            h = Elastos::Core::Math::Round(scale * height);
            CBitmap::CreateScaledBitmap(bitmap.Get(), w, h, true, (IBitmap**)&temp);
            bitmap = temp;
        }
    }
    else if (kind == IMediaStoreImagesThumbnails::MICRO_KIND) {
        return ExtractThumbnail(bitmap,
           IThumbnailUtils::TARGET_SIZE_MICRO_THUMBNAIL,
           IThumbnailUtils::TARGET_SIZE_MICRO_THUMBNAIL,
           IThumbnailUtils::OPTIONS_RECYCLE_INPUT, result);
    }
    *result = bitmap.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ThumbnailUtils::ExtractThumbnail(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    return ExtractThumbnail(source, width, height, OPTIONS_NONE, result);
}

ECode ThumbnailUtils::ExtractThumbnail(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 options,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    if (source == NULL) {
        *result = NULL;
        return NOERROR;
    }

    Float scale;

    Int32 w, h;
    source->GetWidth(&w);
    source->GetHeight(&h);

    if (w < h) {
        scale = width / (Float) w;
    }
    else {
        scale = height / (Float) h;
    }

    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);

    matrix->SetScale(scale, scale);
    AutoPtr<IBitmap> thumbnail = Transform(matrix, source, width, height,
        OPTIONS_SCALE_UP | options);
    *result = thumbnail.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Int32 ThumbnailUtils::ComputeSampleSize(
    /* [in] */ IBitmapFactoryOptions* options,
    /* [in] */ Int32 minSideLength,
    /* [in] */ Int32 maxNumOfPixels)
{
    Int32 initialSize = ComputeInitialSampleSize(options, minSideLength, maxNumOfPixels);
    Int32 roundedSize;
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

Int32 ThumbnailUtils::ComputeInitialSampleSize(
    /* [in] */ IBitmapFactoryOptions* options,
    /* [in] */ Int32 minSideLength,
    /* [in] */ Int32 maxNumOfPixels)
{
    Int32 ow, oh;
    options->GetOutWidth(&ow);
    options->GetOutWidth(&oh);
    Double w = ow;
    Double h = oh;

    using Elastos::Core::Math;
    Int32 lowerBound = (maxNumOfPixels == UNCONSTRAINED) ? 1 :
        (Int32) Math::Ceil(Math::Sqrt(w * h / maxNumOfPixels));
    Int32 upperBound = (minSideLength == UNCONSTRAINED) ? 128 :
        (Int32) Math::Min(Math::Floor(w / minSideLength),
        Math::Floor(h / minSideLength));

    if (upperBound < lowerBound) {
        // return the larger one when there is no overlapping zone.
        return lowerBound;
    }

    if ((maxNumOfPixels == UNCONSTRAINED) &&
            (minSideLength == UNCONSTRAINED)) {
        return 1;
    }
    else if (minSideLength == UNCONSTRAINED) {
        return lowerBound;
    }

    return upperBound;
}

AutoPtr<IBitmap> ThumbnailUtils::MakeBitmap(
    /* [in] */ Int32 minSideLength,
    /* [in] */ Int32 maxNumOfPixels,
    /* [in] */ IUri* uri,
    /* [in] */ IContentResolver* cr,
    /* [in] */ IParcelFileDescriptor* pfd,
    /* [in] */ IBitmapFactoryOptions* inOptions)
{
    AutoPtr<IBitmapFactoryOptions> options = inOptions;
    AutoPtr<IBitmap> b;
    //try {
    if (pfd == NULL) {
        pfd = MakeInputStream(uri, cr);
    }
    if (pfd == NULL) {
        return NULL;
    }
    if (options == NULL) {
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
    }

    AutoPtr<IFileDescriptor> fd;
    pfd->GetFileDescriptor((IFileDescriptor**)&fd);
    options->SetInSampleSize(1);
    options->SetInJustDecodeBounds(TRUE);

    AutoPtr<IBitmapFactory> bf;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bf);
    AutoPtr<IBitmap> bitmap;
    bf->DecodeFileDescriptor(fd, NULL, options, (IBitmap**)&bitmap);

    Boolean isCancel;
    Int32 outWidth, outHeight;
    options->GetCancel(&isCancel);
    options->GetOutWidth(&outWidth);
    options->GetOutHeight(&outHeight);
    if (isCancel|| outWidth == -1 || outHeight -1) {
        return NULL;
    }

    options->SetInSampleSize(ComputeSampleSize(options, minSideLength, maxNumOfPixels));
    options->SetInJustDecodeBounds(FALSE);

    options->SetInDither(FALSE);
    options->SetInPreferredConfig(BitmapConfig_ARGB_8888);
    bf->DecodeFileDescriptor(fd, NULL, options, (IBitmap**)&b);
    /*} catch (OutOfMemoryError ex) {
        Log.e(TAG, "Got oom exception ", ex);
        return NULL;
    } finally {*/
    CloseSilently(pfd);
    //}
    return b;
}

void ThumbnailUtils::CloseSilently(
    /* [in] */ IParcelFileDescriptor* c)
{
  if (c == NULL) return;
  //try {
      c->Close();
  //} catch (Throwable t) {
  //    // do nothing
  //}
}

AutoPtr<IParcelFileDescriptor> ThumbnailUtils::MakeInputStream(
    /* [in] */ IUri* uri,
    /* [in] */ IContentResolver* cr)
{
    AutoPtr<IParcelFileDescriptor> result;
    cr->OpenFileDescriptor(uri, String("r") ,(IParcelFileDescriptor**)&result);
    return result;
}

AutoPtr<IBitmap> ThumbnailUtils::Transform(
    /* [in] */ IMatrix* inScaler,
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ Int32 options)
{
    using Elastos::Core::Math;

    AutoPtr<IMatrix> scaler = inScaler;
    Boolean scaleUp = (options & OPTIONS_SCALE_UP) != 0;
    Boolean recycle = (options & IThumbnailUtils::OPTIONS_RECYCLE_INPUT) != 0;

    Int32 w, h;
    source->GetWidth(&w);
    source->GetHeight(&h);

    Int32 deltaX = w - targetWidth;
    Int32 deltaY = h - targetHeight;
    if (!scaleUp && (deltaX < 0 || deltaY < 0)) {
        /*
        * In this case the bitmap is smaller, at least in one dimension,
        * than the target.  Transform it by placing as much of the image
        * as possible into the target and leaving the top/bottom or
        * left/right (or both) black.
        */
        AutoPtr<IBitmap> b2;
        CBitmap::CreateBitmap(targetWidth, targetHeight, BitmapConfig_ARGB_8888, (IBitmap**)&b2);
        AutoPtr<ICanvas> c;
        CCanvas::New(b2, (ICanvas**)&c);

        Int32 deltaXHalf = Math::Max(0, deltaX / 2);
        Int32 deltaYHalf = Math::Max(0, deltaY / 2);

        AutoPtr<IRect> src;
        CRect::New(
        deltaXHalf,
        deltaYHalf,
        deltaXHalf + Math::Min(targetWidth, w),
        deltaYHalf + Math::Min(targetHeight, h), (IRect**)&src);

        Int32 srcW, srcH;
        src->GetWidth(&srcW);
        src->GetHeight(&srcH);
        Int32 dstX = (targetWidth  - srcW)  / 2;
        Int32 dstY = (targetHeight - srcH) / 2;
        AutoPtr<IRect> dst;
        CRect::New(dstX, dstY, targetWidth - dstX, targetHeight - dstY, (IRect**)&dst);
        c->DrawBitmap(source, src, dst, NULL);
        if (recycle) {
            source->Recycle();
        }
        c->SetBitmap(NULL);
        return b2;
    }

    Float bitmapWidthF = w;
    Float bitmapHeightF = h;

    Float bitmapAspect = bitmapWidthF / bitmapHeightF;
    Float viewAspect   = (Float) targetWidth / targetHeight;

    if (bitmapAspect > viewAspect) {
        Float scale = targetHeight / bitmapHeightF;
        if (scale < 0.9f || scale > 1.0f) {
            scaler->SetScale(scale, scale);
        }
        else {
            scaler = NULL;
        }
    }
    else {
        Float scale = targetWidth / bitmapWidthF;
        if (scale < 0.9f || scale > 1.0f) {
            scaler->SetScale(scale, scale);
        }
        else {
            scaler = NULL;
        }
    }

    AutoPtr<IBitmap> b1;
    if (scaler != NULL) {
        source->GetHeight(&h);
        // this is used for minithumb and crop, so we want to filter here.
        CBitmap::CreateBitmap(source, 0, 0, w, h, scaler, TRUE, (IBitmap**)&b1);
    }
    else {
        b1 = source;
    }

    if (recycle && b1.Get() != source) {
        source->Recycle();
    }

    b1->GetWidth(&w);
    b1->GetHeight(&h);

    Int32 dx1 = Math::Max(0, w - targetWidth);
    Int32 dy1 = Math::Max(0, h - targetHeight);

    AutoPtr<IBitmap> b2;
    CBitmap::CreateBitmap(b1, dx1 / 2, dy1 / 2, targetWidth, targetHeight, (IBitmap**)&b2);

    if (b2 != b1) {
        if (recycle || b1.Get() != source) {
            b1->Recycle();
        }
    }

    return b2;
}

void ThumbnailUtils::CreateThumbnailFromEXIF(
    /* [in] */ const String& filePath,
    /* [in] */ Int32 targetSize,
    /* [in] */ Int32 maxPixels,
    /* [in] */ SizedThumbnailBitmap* sizedThumbBitmap)
{
    if (filePath == NULL) return;

    AutoPtr<IExifInterface> exif;
    AutoPtr<ArrayOf<Byte> > thumbData;
    //try {
    ECode ec = CExifInterface::New(filePath, (IExifInterface**)&exif);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Failed to create ExifInterface %08x", ec);
    }
    if (exif != NULL) {
        exif->GetThumbnail((ArrayOf<Byte>**)&thumbData);
    }
    /*} catch (IOException ex) {
        Log.w(TAG, ex);
    }*/

    AutoPtr<IBitmapFactoryOptions> fullOptions;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&fullOptions);
    AutoPtr<IBitmapFactoryOptions> exifOptions;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&exifOptions);
    Int32 exifThumbWidth = 0, fullThumbWidth = 0;

    AutoPtr<IBitmapFactory> bf;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bf);
    AutoPtr<IBitmap> tempBitmap;
    Int32 inSampleSize, outWidth;

    // Compute exifThumbWidth.
    if (thumbData != NULL) {
        exifOptions->SetInJustDecodeBounds(TRUE);

        bf->DecodeByteArray(thumbData.Get(), 0, thumbData->GetLength(), exifOptions, (IBitmap**)&tempBitmap);
        inSampleSize = ComputeSampleSize(exifOptions, targetSize, maxPixels);
        exifOptions->SetInSampleSize(inSampleSize);
        exifOptions->GetOutWidth(&outWidth);
        exifThumbWidth = outWidth / inSampleSize;
    }

    // Compute fullThumbWidth.
    fullOptions->SetInJustDecodeBounds(TRUE);
    tempBitmap = NULL;
    bf->DecodeFile(filePath, fullOptions, (IBitmap**)&tempBitmap);
    inSampleSize = ComputeSampleSize(fullOptions, targetSize, maxPixels);
    fullOptions->SetInSampleSize(inSampleSize);
    fullOptions->GetOutWidth(&outWidth);
    fullThumbWidth = outWidth / inSampleSize;

    // Choose the larger thumbnail as the returning sizedThumbBitmap.
    if (thumbData != NULL && exifThumbWidth >= fullThumbWidth) {
        Int32 width, height;
        exifOptions->GetOutWidth(&width);
        exifOptions->GetOutHeight(&height);
        exifOptions->SetInJustDecodeBounds(FALSE);
        sizedThumbBitmap->mBitmap = NULL;
        bf->DecodeByteArray(thumbData.Get(), 0, thumbData->GetLength(), exifOptions,
            (IBitmap**)&sizedThumbBitmap->mBitmap);
        if (sizedThumbBitmap->mBitmap != NULL) {
            sizedThumbBitmap->mThumbnailData = thumbData;
            sizedThumbBitmap->mThumbnailWidth = width;
            sizedThumbBitmap->mThumbnailHeight = height;
        }
    }
    else {
        fullOptions->SetInJustDecodeBounds(FALSE);
        sizedThumbBitmap->mBitmap = NULL;
        bf->DecodeFile(filePath, fullOptions, (IBitmap**)&sizedThumbBitmap->mBitmap);
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
