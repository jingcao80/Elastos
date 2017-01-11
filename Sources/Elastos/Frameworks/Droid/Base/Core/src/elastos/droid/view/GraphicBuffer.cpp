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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/GraphicBuffer.h"
#include "elastos/droid/graphics/NativeCanvas.h"
#include "elastos/droid/graphics/Canvas.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/view/CGraphicBuffer.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

#include <binder/Parcel.h>

#include <ui/GraphicBuffer.h>
#include <ui/PixelFormat.h>

#include <gui/IGraphicBufferAlloc.h>
#include <gui/ISurfaceComposer.h>

#include <SkCanvas.h>
#include <SkBitmap.h>

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <private/gui/ComposerService.h>

#define LOCK_CANVAS_USAGE GraphicBuffer::USAGE_SW_READ_OFTEN | GraphicBuffer::USAGE_SW_WRITE_OFTEN
#define NativeGraphicBuffer android::GraphicBuffer
#define sp android::sp
#define ISurfaceComposer android::ISurfaceComposer
#define ComposerService android::ComposerService
#define IGraphicBufferAlloc android::IGraphicBufferAlloc

using Elastos::Droid::Graphics::Canvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::NativeCanvas;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

static inline SkColorType convertPixelFormat(int32_t format) {
    switch (format) {
        case android::PIXEL_FORMAT_RGBA_8888:
            return kN32_SkColorType;
        case android::PIXEL_FORMAT_RGBX_8888:
            return kN32_SkColorType;
        case android::PIXEL_FORMAT_RGB_565:
            return kRGB_565_SkColorType;
        default:
            return kUnknown_SkColorType;
    }
}

namespace Elastos {
namespace Droid {
namespace View {

class GraphicBufferWrapper {
public:
    GraphicBufferWrapper(const sp<NativeGraphicBuffer>& buffer): buffer(buffer) {
    }

    sp<NativeGraphicBuffer> buffer;
};

CAR_INTERFACE_IMPL_2(GraphicBuffer, Object, IGraphicBuffer, IParcelable)

GraphicBuffer::GraphicBuffer()
    : mWidth(0)
    , mHeight(0)
    , mFormat(0)
    , mUsage(0)
    , mNativeObject(0)
    , mSaveCount(0)
    , mDestroyed(0)
    , mInit(FALSE)
{
}

GraphicBuffer::~GraphicBuffer()
{
    Destroy();
}

ECode GraphicBuffer::constructor()
{
    return NOERROR;
}

ECode GraphicBuffer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CGraphicBuffer{");
    sb += StringUtils::ToHexString((Int32)this);
    if (mNativeObject != 0) {
        sb += ", nativeObject=";
        GraphicBufferWrapper* wrapper = reinterpret_cast<GraphicBufferWrapper*>(mNativeObject);
        Int64 handle = reinterpret_cast<Int64>(wrapper->buffer.get());
        sb += StringUtils::ToHexString(handle);
    }
    sb += ", width=";
    sb += mWidth;
    sb += ", height=";
    sb += mHeight;
    sb += ", format=";
    sb += mFormat;
    sb += ", usage=";
    sb += mUsage;
    sb += ", destroyed=";
    sb += mDestroyed;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode GraphicBuffer::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mWidth);
    source->ReadInt32(&mHeight);
    source->ReadInt32(&mFormat);
    source->ReadInt32(&mUsage);
    FAIL_RETURN(nReadGraphicBufferFromParcel(source, &mNativeObject))
    if (mNativeObject == 0) {
        Slogger::E("GraphicBuffer",
            "This GraphicBuffer has no nativeObject when read from a parcel.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mInit = TRUE;
    return NOERROR;
}

ECode GraphicBuffer::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mDestroyed) {
        Slogger::E("GraphicBuffer",
            "This GraphicBuffer has been destroyed and cannot be written to a parcel.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (!mInit) {
        Slogger::E("GraphicBuffer",
            "This GraphicBuffer has not been initiated and cannot be written to a parcel.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    dest->WriteInt32(mWidth);
    dest->WriteInt32(mHeight);
    dest->WriteInt32(mFormat);
    dest->WriteInt32(mUsage);
    nWriteGraphicBufferToParcel(mNativeObject, dest);
    return NOERROR;
}

/**
 * Returns the width of this buffer in pixels.
 */
ECode GraphicBuffer::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = 0;
    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *width = mWidth;
    return NOERROR;
}

/**
 * Returns the height of this buffer in pixels.
 */
ECode GraphicBuffer::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = 0;
    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *height = mHeight;
    return NOERROR;
}

/**
 * Returns the pixel format of this buffer. The pixel format must be one of
 * the formats defined in {@link PixelFormat}.
 */
ECode GraphicBuffer::GetFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format)
    *format = 0;
    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *format = mFormat;
    return NOERROR;
}

/**
 * Returns the usage hint set on this buffer.
 */
ECode GraphicBuffer::GetUsage(
    /* [out] */ Int32* usage)
{
    VALIDATE_NOT_NULL(usage)
    *usage = 0;
    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *usage = mUsage;
    return NOERROR;
}

/**
 * <p>Start editing the pixels in the buffer. A null is returned if the buffer
 * cannot be locked for editing.</p>
 *
 * <p>The content of the buffer is preserved between unlockCanvas()
 * and lockCanvas().</p>
 *
 * <p>If this method is called after {@link #destroy()}, the return value will
 * always be null.</p>
 *
 * @return A Canvas used to draw into the buffer, or null.
 *
 * @see #lockCanvas(android.graphics.Rect)
 * @see #unlockCanvasAndPost(android.graphics.Canvas)
 * @see #isDestroyed()
 */
ECode GraphicBuffer::LockCanvas(
    /* [out] */ ICanvas** canvas)
{
    return LockCanvas(NULL, canvas);
}

/**
 * Just like {@link #lockCanvas()} but allows specification of a dirty
 * rectangle.
 *
 * <p>If this method is called after {@link #destroy()}, the return value will
 * always be null.</p>
 *
 * @param dirty Area of the buffer that may be modified.

 * @return A Canvas used to draw into the surface, or null.
 *
 * @see #lockCanvas()
 * @see #unlockCanvasAndPost(android.graphics.Canvas)
 * @see #isDestroyed()
 */
ECode GraphicBuffer::LockCanvas(
    /* [in] */ IRect* dirty,
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas)
    *canvas = NULL;

    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mDestroyed) {
        return NOERROR;
    }

    if (mCanvas == NULL) {
        CCanvas::New((ICanvas**)&mCanvas);
    }

    if (nLockCanvas(mNativeObject, mCanvas, dirty)) {
        mCanvas->Save(&mSaveCount);
        *canvas = mCanvas;
        REFCOUNT_ADD(*canvas);
        return NOERROR;
    }

    *canvas = mCanvas;
    REFCOUNT_ADD(*canvas);
    return NOERROR;
}

/**
 * Finish editing pixels in the buffer.
 *
 * <p>This method doesn't do anything if {@link #destroy()} was
 * previously called.</p>
 *
 * @param canvas The Canvas previously returned by lockCanvas()
 *
 * @see #lockCanvas()
 * @see #lockCanvas(android.graphics.Rect)
 * @see #isDestroyed()
 */
ECode GraphicBuffer::UnlockCanvasAndPost(
    /* [in] */ ICanvas* canvas)
{
    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!mDestroyed && mCanvas != NULL && canvas == mCanvas) {
        canvas->RestoreToCount(mSaveCount);
        mSaveCount = 0;

        nUnlockCanvasAndPost(mNativeObject, mCanvas);
    }
    return NOERROR;
}

/**
 * Destroyes this buffer immediately. Calling this method frees up any
 * underlying resources. After calling this method, this buffer
 * must not be used in any way ({@link #lockCanvas()} must not be called,
 * etc.)
 *
 * @see #isDestroyed()
 */
ECode GraphicBuffer::Destroy()
{
    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!mDestroyed) {
        mDestroyed = TRUE;
        nDestroyGraphicBuffer(mNativeObject);
        mNativeObject = 0;
    }
    return NOERROR;
}
/**
 * Indicates whether this buffer has been destroyed. A destroyed buffer
 * cannot be used in any way: locking a Canvas will return null, the buffer
 * cannot be written to a parcel, etc.
 *
 * @return True if this <code>GraphicBuffer</code> is in a destroyed state,
 *         false otherwise.
 *
 * @see #destroy()
 */
ECode GraphicBuffer::IsDestroyed(
    /* [out] */ Boolean* isDestroyed)
{
    VALIDATE_NOT_NULL(isDestroyed)
    *isDestroyed = TRUE;
    if (!mInit) {
        Slogger::E("GraphicBuffer", "This GraphicBuffer has not been initiated");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *isDestroyed = mDestroyed;
    return NOERROR;
}

ECode GraphicBuffer::GetNativeObject(
    /* [out] */ Int64* handle)
{
    VALIDATE_NOT_NULL(handle)
    if (mNativeObject != 0) {
        GraphicBufferWrapper* wrapper = reinterpret_cast<GraphicBufferWrapper*>(mNativeObject);
        *handle = reinterpret_cast<Int64>(wrapper->buffer.get());
    }
    else {
        *handle = 0;
    }
    return NOERROR;
}

ECode GraphicBuffer::Create(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 usage,
    /* [out] */ IGraphicBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    Int64 nativeObject = nCreateGraphicBuffer(width, height, format, usage);
    if (nativeObject != 0) {
        CGraphicBuffer::New(width, height, format, usage, nativeObject, buf);
        return NOERROR;
    }
    *buf = NULL;
    return NOERROR;
}

Int64 GraphicBuffer::nCreateGraphicBuffer(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 usage)
{
    sp<ISurfaceComposer> composer(ComposerService::getComposerService());
    sp<IGraphicBufferAlloc> alloc(composer->createGraphicBufferAlloc());
    if (alloc == NULL) {
        Slogger::E("GraphicBuffer", "createGraphicBufferAlloc() failed in GraphicBuffer.create()");
        return 0;
    }

    android::status_t error;
    sp<NativeGraphicBuffer> buffer(alloc->createGraphicBuffer(width, height, format, usage, &error));
    if (buffer == NULL) {
        Slogger::E("GraphicBuffer", "createGraphicBuffer() failed in GraphicBuffer.create()");
        return 0;
    }

    GraphicBufferWrapper* wrapper = new GraphicBufferWrapper(buffer);
    return reinterpret_cast<Int64>(wrapper);
}

void GraphicBuffer::nDestroyGraphicBuffer(
    /* [in] */ Int64 nativeObject)
{
    if (nativeObject != 0) {
        GraphicBufferWrapper* wrapper = reinterpret_cast<GraphicBufferWrapper*>(nativeObject);
        delete wrapper;
    }
}

ECode GraphicBuffer::nWriteGraphicBufferToParcel(
    /* [in] */ Int64 nativeObject,
    /* [in] */ IParcel* dest)
{
    GraphicBufferWrapper* wrapper = reinterpret_cast<GraphicBufferWrapper*>(nativeObject);
    android::Parcel* parcel;
    dest->GetDataPayload((Handle32*)&parcel);
    if (parcel) {
        parcel->write(*wrapper->buffer);
    }

    return NOERROR;
}

ECode GraphicBuffer::nReadGraphicBufferFromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ Int64* nativeObject)
{
    VALIDATE_NOT_NULL(nativeObject)
    *nativeObject = 0;

    if (source == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    }
    android::Parcel* parcel;
    source->GetDataPayload((Handle32*)&parcel);

    if (parcel) {
        sp<android::GraphicBuffer> buffer = new android::GraphicBuffer();
        parcel->read(*buffer);
        *nativeObject = reinterpret_cast<Int64>(new GraphicBufferWrapper(buffer));
    }

    return NOERROR;
}

Boolean GraphicBuffer::nLockCanvas(
    /* [in] */ Int64 nativeObject,
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* dirtyRect)
{
    GraphicBufferWrapper* wrapper = reinterpret_cast<GraphicBufferWrapper*>(nativeObject);
    if (!wrapper) {
        return FALSE;
    }

    sp<NativeGraphicBuffer> buffer(wrapper->buffer);

    android::Rect rect;
    if (dirtyRect) {
        dirtyRect->GetLeft(&(rect.left));
        dirtyRect->GetTop(&(rect.top));
        dirtyRect->GetRight(&(rect.right));
        dirtyRect->GetBottom(&(rect.bottom));
    } else {
        rect.set(android::Rect(buffer->getWidth(), buffer->getHeight()));
    }

    void* bits = NULL;
    android::status_t status = buffer->lock(LOCK_CANVAS_USAGE, rect, &bits);

    if (status) return FALSE;
    if (!bits) {
        buffer->unlock();
        return FALSE;
    }

    ssize_t bytesCount = buffer->getStride() * android::bytesPerPixel(buffer->getPixelFormat());

    SkBitmap bitmap;
    bitmap.setInfo(SkImageInfo::Make(buffer->getWidth(), buffer->getHeight(),
                                     convertPixelFormat(buffer->getPixelFormat()),
                                     kPremul_SkAlphaType), bytesCount);

    if (buffer->getWidth() > 0 && buffer->getHeight() > 0) {
        bitmap.setPixels(bits);
    } else {
        bitmap.setPixels(NULL);
    }

    Canvas* impl = (Canvas*)canvas;
    canvas->SetSurfaceFormat(buffer->getPixelFormat());
    impl->SetNativeBitmap(reinterpret_cast<Int64>(&bitmap));

    SkRect clipRect;
    clipRect.set(rect.left, rect.top, rect.right, rect.bottom);
    SkCanvas* nativeCanvas = reinterpret_cast<NativeCanvas*>(impl->mNativeCanvas)->getSkCanvas();
    nativeCanvas->clipRect(clipRect);

    if (dirtyRect) {
        dirtyRect->Set(rect.left, rect.top, rect.right, rect.bottom);
    }

    return FALSE;
}

Boolean GraphicBuffer::nUnlockCanvasAndPost(
    /* [in] */ Int64 nativeObject,
    /* [in] */ ICanvas* canvas)
{
    GraphicBufferWrapper* wrapper = reinterpret_cast<GraphicBufferWrapper*>(nativeObject);
    Canvas* impl = (Canvas*)canvas;
    impl->SetNativeBitmap((Int64)0);

    if (wrapper) {
        android::status_t status = wrapper->buffer->unlock();
        return status == 0 ? TRUE : FALSE;
    }

    return FALSE;
}

ECode GraphicBuffer::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 usage,
    /* [in] */ Int64 nativeObject)
{
    mWidth = width;
    mHeight = height;
    mFormat = format;
    mUsage = usage;
    mNativeObject = nativeObject;
    mInit = TRUE;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
