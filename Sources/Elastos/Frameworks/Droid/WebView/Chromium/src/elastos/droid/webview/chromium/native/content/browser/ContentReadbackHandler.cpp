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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentReadbackHandler.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ContentReadbackHandler_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Creates a {@link ContentReadbackHandler}.
 */
ContentReadbackHandler::ContentReadbackHandler()
    : mNextReadbackId(1)
    , mNativeContentReadbackHandler(0)
{
    //mGetBitmapRequests = new SparseArray<GetBitmapCallback>();
    CSparseArray::New((ISparseArray**)&mGetBitmapRequests);
}

/**
 * Initialize the native object.
 */
void ContentReadbackHandler::InitNativeContentReadbackHandler()
{
    mNativeContentReadbackHandler = NativeInit();
}

/**
 * Should be called when the ContentReadackHandler is not needed anymore.
 */
void ContentReadbackHandler::Destroy()
{
    if (mNativeContentReadbackHandler != 0) {
        NativeDestroy(mNativeContentReadbackHandler);
    }
    mNativeContentReadbackHandler = 0;
}


//@CalledByNative
void ContentReadbackHandler::NotifyGetBitmapFinished(
    /* [in] */ Int32 readbackId,
    /* [in] */ Boolean success,
    /* [in] */ IBitmap* bitmap)
{
    AutoPtr<IInterface> obj;
    mGetBitmapRequests->Get(readbackId, (IInterface**)&obj);
    AutoPtr<GetBitmapCallback> callback = (GetBitmapCallback*)(IObject::Probe(obj));
    if (callback != NULL) {
        mGetBitmapRequests->Delete(readbackId);
        callback->OnFinishGetBitmap(success, bitmap);
    }
    else {
        // readback Id is unregistered.
        //assert false : "Readback finished for unregistered Id: " + readbackId;
        Logger::E("ContentReadbackHandler::NotifyGetBitmapFinished", "Readback finished for unregistered Id: %d", readbackId);
    }
}

/**
 * Asynchronously, generate and grab a bitmap representing what is currently on the screen
 * for {@code view}.
 *
 * @param scale The scale that should be applied to the content.
 * @param srcRect  A subrect of the original content to capture. If this is empty, it will grab
 *                 the whole surface.
 * @param view  The {@link ContentViewCore} to grab the bitmap from.
 * @param callback The callback to be executed after readback completes.
 */
void ContentReadbackHandler::GetContentBitmapAsync(
    /* [in] */ Float scale,
    /* [in] */ IRect* srcRect,
    /* [in] */ ContentViewCore* view,
    /* [in] */ GetBitmapCallback* callback)
{
    if (!ReadyForReadback()) {
        callback->OnFinishGetBitmap(FALSE, NULL);
        return;
    }

    ThreadUtils::AssertOnUiThread();

    Int32 readbackId = mNextReadbackId++;
    mGetBitmapRequests->Put(readbackId, TO_IINTERFACE(callback));
    Int32 width, height, top, left;
    srcRect->GetWidth(&width);
    srcRect->GetHeight(&height);
    srcRect->GetTop(&top);
    srcRect->GetLeft(&left);
    NativeGetContentBitmap(mNativeContentReadbackHandler, readbackId, scale,
            Elastos::Droid::Graphics::BitmapConfig_ARGB_8888, top, left, width,
            height, TO_IINTERFACE(view));
}

/**
 * Asynchronously, grab a bitmap of the current browser compositor root layer.
 *
 * @param windowAndroid The window that hosts the compositor.
 * @param callback      The callback to be executed after readback completes.
 */
void ContentReadbackHandler::GetCompositorBitmapAsync(
    /* [in] */ WindowElastos* windowAndroid,
    /* [in] */ GetBitmapCallback* callback)
{
    if (!ReadyForReadback()) {
        callback->OnFinishGetBitmap(FALSE, NULL);
        return;
    }

    ThreadUtils::AssertOnUiThread();

    Int32 readbackId = mNextReadbackId++;
    mGetBitmapRequests->Put(readbackId, TO_IINTERFACE(callback));
    NativeGetCompositorBitmap(mNativeContentReadbackHandler, readbackId,
            windowAndroid->GetNativePointer());
}

HANDLE ContentReadbackHandler::NativeInit()
{
    return Elastos_ContentReadbackHandler_nativeInit(TO_IINTERFACE(this));
}

void ContentReadbackHandler::NativeDestroy(
    /* [in] */ HANDLE nativeContentReadbackHandler)
{
    Elastos_ContentReadbackHandler_nativeDestroy(TO_IINTERFACE(this), nativeContentReadbackHandler);
}

void ContentReadbackHandler::NativeGetContentBitmap(
    /* [in] */ HANDLE nativeContentReadbackHandler,
    /* [in] */ Int32 readback_id,
    /* [in] */ Float scale,
    /* [in] */ /*BitmapConfig*/Int32 config,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float width,
    /* [in] */ Float height,
    /* [in] */ IInterface* contentViewCore)
{
    Elastos_ContentReadbackHandler_nativeGetContentBitmap(TO_IINTERFACE(this), nativeContentReadbackHandler, readback_id, scale, config, x, y, width, height, contentViewCore);
}

void ContentReadbackHandler::NativeGetCompositorBitmap(
    /* [in] */ HANDLE nativeContentReadbackHandler,
    /* [in] */ Int32 readback_id,
    /* [in] */ HANDLE nativeWindowAndroid)
{
    Elastos_ContentReadbackHandler_nativeGetCompositorBitmap(TO_IINTERFACE(this), nativeContentReadbackHandler, readback_id, nativeWindowAndroid);
}

void ContentReadbackHandler::NotifyGetBitmapFinished(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 readbackId,
    /* [in] */ Boolean success,
    /* [in] */ IInterface* bitmap)
{
    AutoPtr<ContentReadbackHandler> mObj = (ContentReadbackHandler*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ContentReadbackHandler", "ContentReadbackHandler::NotifyGetBitmapFinished, mObj is NULL");
        return;
    }
    AutoPtr<IBitmap> b = IBitmap::Probe(bitmap);
    mObj->NotifyGetBitmapFinished(readbackId, success, b);
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
