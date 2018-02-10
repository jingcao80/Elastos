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

#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/media/CMediaHTTPService.h"
#include "elastos/droid/media/CMediaMetadataRetriever.h"
#include "elastos/droid/media/Media_Utils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/NativeBinder.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <assert.h>
#include <media/IMediaHTTPService.h>
#include <private/media/VideoFrame.h>
#include <skia/core/SkBitmap.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include <utils/threads.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::GraphicsNative;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::IBinderForDroidObject;
using Elastos::Core::AutoLock;
using Elastos::IO::ICloseable;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {

template<typename T>
static void rotate0(T* dst, const T* src, size_t width, size_t height)
{
    memcpy(dst, src, width * height * sizeof(T));
}

template<typename T>
static void rotate90(T* dst, const T* src, size_t width, size_t height)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            dst[j * height + height - 1 - i] = src[i * width + j];
        }
    }
}

template<typename T>
static void rotate180(T* dst, const T* src, size_t width, size_t height)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            dst[(height - 1 - i) * width + width - 1 - j] = src[i * width + j];
        }
    }
}

template<typename T>
static void rotate270(T* dst, const T* src, size_t width, size_t height)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            dst[(width - 1 - j) * height + i] = src[i * width + j];
        }
    }
}

template<typename T>
static void rotate(T *dst, const T *src, size_t width, size_t height, int angle)
{
    switch (angle) {
        case 0:
            rotate0(dst, src, width, height);
            break;
        case 90:
            rotate90(dst, src, width, height);
            break;
        case 180:
            rotate180(dst, src, width, height);
            break;
        case 270:
            rotate270(dst, src, width, height);
            break;
    }
}

static ECode process_media_retriever_call(android::status_t opStatus, ECode exception, const char *message)
{
    if (opStatus == (android::status_t) android::INVALID_OPERATION) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else if (opStatus != (android::status_t) android::OK) {
        if (strlen(message) > 230) {
            // If the message is too long, don't bother displaying the status code.
            Slogger::E("CMediaMetadataRetriever", message);
            return exception;
        }
        else {
            Slogger::E("CMediaMetadataRetriever: status = 0x%X", message, opStatus);
            return exception;
        }
    }
    return NOERROR;
}

const Int32 CMediaMetadataRetriever::EMBEDDED_PICTURE_TYPE_ANY = 0xFFFF;
const String CMediaMetadataRetriever::TAG("CMediaMetadataRetriever");

CAR_INTERFACE_IMPL(CMediaMetadataRetriever, Object, IMediaMetadataRetriever)

CAR_OBJECT_IMPL(CMediaMetadataRetriever)

CMediaMetadataRetriever::CMediaMetadataRetriever()
    : mContext(NULL)
{
}

CMediaMetadataRetriever::~CMediaMetadataRetriever()
{
    Finalize();
}

ECode CMediaMetadataRetriever::constructor()
{
    return NativeSetup();
}

android::MediaMetadataRetriever* CMediaMetadataRetriever::GetRetriever()
{
    return mContext;
}

void CMediaMetadataRetriever::SetRetriever(android::MediaMetadataRetriever* retriever)
{
    mContext = retriever;
}

ECode CMediaMetadataRetriever::SetDataSource(
    /* [in] */ const String& path)
{
    if (path == NULL) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IFileInputStream> is;
    // try {
    CFileInputStream::New(path, (IFileInputStream**)&is);
    AutoPtr<IFileDescriptor> fd;
    is->GetFD((IFileDescriptor**)&fd);
    SetDataSource(fd, 0, 0x7ffffffffffffffL);
    // } catch (FileNotFoundException fileEx) {
    //     throw new IllegalArgumentException();
    // } catch (IOException ioEx) {
    //     throw new IllegalArgumentException();
    // }

    // try {
    if (is != NULL) {
        ICloseable::Probe(is)->Close();
    }
    // } catch (Exception e) {}
    return NOERROR;
}

ECode CMediaMetadataRetriever::SetDataSource(
    /* [in] */ const String& uri,
    /* [in] */ IMap* headers)
{
    AutoPtr<ArrayOf<String> > keys;
    AutoPtr<ArrayOf<String> > values;

    Int32 size;
    headers->GetSize(&size);
    keys = ArrayOf<String>::Alloc(size);
    values = ArrayOf<String>::Alloc(size);

    AutoPtr<ISet> set;
    headers->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Int32 i = 0;
    Boolean b;
    while (it->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        obj = NULL;
        entry->GetKey((IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        keys->Set(i, str);
        obj = NULL;
        entry->GetValue((IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&str);
        values->Set(i, str);
        ++i;
    }

    AutoPtr<IBinder> binder;
    CMediaHTTPService::CreateHttpServiceBinderIfNecessary(uri,
            (IBinder**)&binder);
    return NativeSetDataSource(binder, uri, keys, values);
}

ECode CMediaMetadataRetriever::NativeSetDataSource(
    /* [in] */ IBinder* httpServiceBinder,
    /* [in] */ const String& path,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<String>* values)
{
    Slogger::V(TAG, "setDataSource Path");

    android::MediaMetadataRetriever* retriever = GetRetriever();
    if (retriever == 0) {
        Slogger::E(TAG, "No retriever available");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (path.IsNull()) {
        Slogger::E(TAG, "Null path");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const char *tmp = path.string();
    if (!tmp) {  // OutOfMemoryError exception already thrown
        return NOERROR;
    }

    android::String8 pathStr(tmp);

    // Don't let somebody trick us in to reading some random block of memory
    if (strncmp("mem://", pathStr.string(), 6) == 0) {
        Slogger::E(TAG, "Invalid pathname");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // We build a similar KeyedVector out of it.
    android::KeyedVector<android::String8, android::String8> headersVector;
    FAIL_RETURN(Media_Utils::ConvertKeyValueArraysToKeyedVector(keys, values, &headersVector));

    android::sp<android::IMediaHTTPService> httpService;
    if (httpServiceBinder != NULL) {
        android::sp<android::IBinder> binder = IBinderForDroidObject(httpServiceBinder);
        httpService = android::interface_cast<android::IMediaHTTPService>(binder);
    }

    android::status_t status = retriever->setDataSource(
        httpService, pathStr.string(), headersVector.size() > 0 ? &headersVector : NULL);
    return process_media_retriever_call(
        status, E_RUNTIME_EXCEPTION, "setDataSource failed");
}

ECode CMediaMetadataRetriever::SetDataSource(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    Slogger::V(TAG, "setDataSource Fd");

    android::MediaMetadataRetriever* retriever = GetRetriever();
    if (retriever == NULL) {
        Slogger::E(TAG, "No retriever available");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!fileDescriptor) {
        Slogger::E(TAG, "IllegalArgumentException: IFileDescriptor");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 fd = 0;
    fileDescriptor->GetDescriptor(&fd);

    if (offset < 0 || length < 0 || fd < 0) {
        if (offset < 0) {
            Slogger::E(TAG, "negative offset (%lld)", offset);
        }
        if (length < 0) {
            Slogger::E(TAG, "negative length (%lld)", length);
        }
        if (fd < 0) {
            Slogger::E(TAG, "invalid file descriptor");
        }

        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::status_t status = retriever->setDataSource(fd, offset, length);
    return process_media_retriever_call(status, E_RUNTIME_EXCEPTION, "setDataSource failed");
}

ECode CMediaMetadataRetriever::SetDataSource(
    /* [in] */ IFileDescriptor* fd)
{
    // intentionally less than LONG_MAX
    return SetDataSource(fd, 0, 0x7ffffffffffffffL);
}

ECode CMediaMetadataRetriever::SetDataSource(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        //throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String scheme;
    String path;
    uri->GetScheme(&scheme);
    if(scheme == NULL || scheme.Equals("file")) {
       uri->GetPath(&path);
       return SetDataSource(path);
    }

    AutoPtr<IAssetFileDescriptor> fd = NULL;
    // try {
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    String u;
    Int64 startoffset, declaredlength;
    Boolean tmp;
    AutoPtr<IFileDescriptor> descriptor;

    ECode ec = resolver->OpenAssetFileDescriptor(uri, String("r"), (IAssetFileDescriptor**)&fd);
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        ec = E_ILLEGAL_ARGUMENT_EXCEPTION;
        FAIL_GOTO(ec, _EXIT_);
    }

    if (fd == NULL) {
        ec = E_ILLEGAL_ARGUMENT_EXCEPTION;
        FAIL_GOTO(ec, _EXIT_);
    }

    ec = fd->GetFileDescriptor((IFileDescriptor**)&descriptor);
    FAIL_GOTO(ec, _EXIT_)

    if (!descriptor->Valid(&tmp)) {
        ec = E_ILLEGAL_ARGUMENT_EXCEPTION;
        FAIL_GOTO(ec, _EXIT_)
    }

    // Note: using getDeclaredLength so that our behavior is the same
    // as previous versions when the content provider is returning
    // a full file.

    ec = fd->GetDeclaredLength(&declaredlength);
    FAIL_GOTO(ec, _EXIT_)

    if (declaredlength < 0) {
       ec = SetDataSource(descriptor);
    }
    else {
       ec = fd->GetStartOffset(&startoffset);
       FAIL_GOTO(ec, _EXIT_)

       ec = SetDataSource(descriptor, startoffset, declaredlength);
    }

_EXIT_:
    if (fd != NULL) {
        fd->Close();
        fd = NULL;
    }

    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        ec = NOERROR;
    }

    if (SUCCEEDED(ec)) {
        return NOERROR;
    }

    ((Uri*)uri)->ToString(&u);
    return SetDataSource(u);
}

ECode CMediaMetadataRetriever::ExtractMetadata(
    /* [in] */ Int32 keyCode,
    /* [out] */ String* data)
{
    VALIDATE_NOT_NULL(data);
    *data = String("");
    return NOERROR;
}

ECode CMediaMetadataRetriever::GetFrameAtTime(
    /* [in] */ Int64 timeUs,
    /* [in] */ Int32 option,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    if (option < OPTION_PREVIOUS_SYNC || option > OPTION_CLOSEST) {
        //throw new IllegalArgumentException("Unsupported option: " + option);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NativeGetFrameAtTime(timeUs, option, bitmap);
}

ECode CMediaMetadataRetriever::GetFrameAtTime(
    /* [in] */ Int64 timeUs,
    /* [out] */ IBitmap** bitmap)
{
    return GetFrameAtTime(timeUs, OPTION_CLOSEST_SYNC, bitmap);
}

ECode CMediaMetadataRetriever::GetFrameAtTime(
    /* [out] */ IBitmap** bitmap)
{
    return GetFrameAtTime(-1, OPTION_CLOSEST_SYNC, bitmap);
}

ECode CMediaMetadataRetriever::NativeGetFrameAtTime(
    /* [in] */ Int64 timeUs,
    /* [in] */ Int32 option,
    /* [out] */ IBitmap** result)
{
    assert(result != NULL);
    *result = NULL;

    Slogger::V(TAG, "getFrameAtTime: %lld us option: %d", timeUs, option);

    android::MediaMetadataRetriever* retriever = GetRetriever();
    if (retriever == NULL) {
        Slogger::E(TAG, "No retriever available");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Call native method to retrieve a video frame
    android::VideoFrame* videoFrame = NULL;
    android::sp<android::IMemory> frameMemory = retriever->getFrameAtTime(timeUs, option);
    if (frameMemory != 0) {  // cast the shared structure to a VideoFrame object
        videoFrame = static_cast<android::VideoFrame *>(frameMemory->pointer());
    }
    if (videoFrame == NULL) {
        Slogger::E(TAG, "getFrameAtTime: videoFrame is a NULL pointer");
        return NOERROR;
    }

    Slogger::V(TAG, "Dimension = %dx%d and bytes = %d",
            videoFrame->mDisplayWidth,
            videoFrame->mDisplayHeight,
            videoFrame->mSize);

    BitmapConfig config = GraphicsNative::ColorTypeToLegacyBitmapConfig(kRGB_565_SkColorType);

    size_t width, height;
    Boolean swapWidthAndHeight = FALSE;
    if (videoFrame->mRotationAngle == 90 || videoFrame->mRotationAngle == 270) {
        width = videoFrame->mHeight;
        height = videoFrame->mWidth;
        swapWidthAndHeight = TRUE;
    }
    else {
        width = videoFrame->mWidth;
        height = videoFrame->mHeight;
    }

    AutoPtr<IBitmap> jBitmap;
    CBitmap::CreateBitmap(width, height, config, (IBitmap**)&jBitmap);

    HANDLE nativeBitmap;
    jBitmap->GetNativeBitmap(&nativeBitmap);

    SkBitmap *bitmap = (SkBitmap *)nativeBitmap;
    bitmap->lockPixels();
    rotate((uint16_t*)bitmap->getPixels(),
           (uint16_t*)((char*)videoFrame + sizeof(android::VideoFrame)),
           videoFrame->mWidth,
           videoFrame->mHeight,
           videoFrame->mRotationAngle);
    bitmap->unlockPixels();

    if (videoFrame->mDisplayWidth  != videoFrame->mWidth ||
        videoFrame->mDisplayHeight != videoFrame->mHeight) {
        size_t displayWidth = videoFrame->mDisplayWidth;
        size_t displayHeight = videoFrame->mDisplayHeight;
        if (swapWidthAndHeight) {
            displayWidth = videoFrame->mDisplayHeight;
            displayHeight = videoFrame->mDisplayWidth;
        }
        Slogger::V(TAG, "Bitmap dimension is scaled from %dx%d to %dx%d",
                width, height, displayWidth, displayHeight);

        AutoPtr<IBitmap> scaledBitmap;
        CBitmap::CreateScaledBitmap(
                jBitmap, displayWidth, displayHeight, TRUE, (IBitmap**)&scaledBitmap);

        *result = scaledBitmap;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    *result = jBitmap;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaMetadataRetriever::GetEmbeddedPicture(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return GetEmbeddedPicture(EMBEDDED_PICTURE_TYPE_ANY, result);
}

ECode CMediaMetadataRetriever::GetEmbeddedPicture(
    /* [in] */ Int32 pictureType,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    Slogger::V(TAG, "GetEmbeddedPicture: %d", pictureType);

    VALIDATE_NOT_NULL(result);
    *result = NULL;

    android::MediaMetadataRetriever* retriever = GetRetriever();
    if (retriever == NULL) {
        Slogger::E(TAG, "No retriever available");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    assert(0 && "TODO");
    // android::MediaAlbumArt* mediaAlbumArt = NULL;

    // // FIXME:
    // // Use pictureType to retrieve the intended embedded picture and also change
    // // the method name to getEmbeddedPicture().
    // android::sp<android::IMemory> albumArtMemory = retriever->extractAlbumArt();
    // if (albumArtMemory != 0) {  // cast the shared structure to a MediaAlbumArt object
    //     mediaAlbumArt = static_cast<android::MediaAlbumArt *>(albumArtMemory->pointer());
    // }
    // if (mediaAlbumArt == NULL) {
    //     Slogger::E(TAG, "getEmbeddedPicture: Call to getEmbeddedPicture failed.");
    //     return NOERROR;
    // }

    // unsigned int len = mediaAlbumArt->mSize;
    // char* data = (char*) mediaAlbumArt + sizeof(MediaAlbumArt);
    // AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(len);
    // if (!array) {  // OutOfMemoryError exception has already been thrown.
    //     Slogger::E(TAG, "getEmbeddedPicture: OutOfMemoryError is thrown.");
    //     return E_OUT_OF_MEMORY;
    // }
    // else {
    //     memcpy(array->GetPayload(), data, len);
    // }

    // // No need to delete mediaAlbumArt here
    // *result = array;
    // REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaMetadataRetriever::ReleaseResources()
{
    {
        AutoLock syncLock(sLock);
        android::MediaMetadataRetriever* retriever = GetRetriever();
        if (retriever != NULL) {
            delete retriever;
            SetRetriever(NULL);
        }
    }
    return NOERROR;
}

ECode CMediaMetadataRetriever::NativeSetup()
{
    android::MediaMetadataRetriever* retriever = new android::MediaMetadataRetriever();
    if (retriever == NULL) {
        return E_RUNTIME_EXCEPTION;
    }

    SetRetriever(retriever);
    return NOERROR;
}

void CMediaMetadataRetriever::NativeFinalize()
{
    ReleaseResources();
}

ECode CMediaMetadataRetriever::Finalize()
{
    //try {
        NativeFinalize();
    /*} finally {
        super.finalize();
    }*/
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
