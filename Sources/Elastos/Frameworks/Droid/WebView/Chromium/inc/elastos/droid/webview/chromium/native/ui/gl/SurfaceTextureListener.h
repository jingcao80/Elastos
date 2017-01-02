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

