// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_GL_SURFACETEXTURELISTENER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_GL_SURFACETEXTURELISTENER_H_

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.gl;
// import android.graphics.SurfaceTexture;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::Graphics::IOnFrameAvailableListener;
using Elastos::Droid::Graphics::ISurfaceTexture;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Gl {

/**
  * Listener to an android SurfaceTexture object for frame availability.
  */
// @JNINamespace("gfx")
class SurfaceTextureListener
    : public Object
    , public IOnFrameAvailableListener
{
public:
    CAR_INTERFACE_DECL()
    SurfaceTextureListener(
        /* [in] */ Handle64 nativeSurfaceTextureListener);

    // @Override
    CARAPI OnFrameAvailable(
        /* [in] */ ISurfaceTexture* surfaceTexture);

protected:
    // @Override
    CARAPI Finalize();

private:
    CARAPI NativeFrameAvailable(
        /* [in] */ Handle64 nativeSurfaceTextureListener);

    CARAPI NativeDestroy(
        /* [in] */ Handle64 nativeSurfaceTextureListener);

private:
    // Used to determine the class instance to dispatch the native call to.
    Handle64 mNativeSurfaceTextureListener;
};

} // namespace Gl
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_GL_SURFACETEXTURELISTENER_H_

