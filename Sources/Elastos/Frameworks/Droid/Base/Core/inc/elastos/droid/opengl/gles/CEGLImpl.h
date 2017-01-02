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

#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLImpl.h"
#include "elastos/droid/opengl/gles/CEGLDisplayImpl.h"
#include "elastos/droid/opengl/gles/CEGLSurfaceImpl.h"
#include "elastos/droid/opengl/gles/CEGLContextImpl.h"

#include <elastos/core/Object.h>
#include <gui/Surface.h>

using Elastos::Core::Object;
using Elastosx::Microedition::Khronos::Egl::IEGL;
using Elastosx::Microedition::Khronos::Egl::IEGL10;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CarClass(CEGLImpl)
    , public Object
    , public IEGL10
    , public IEGL
{
typedef Elastosx::Microedition::Khronos::Egl::IEGLConfig XIEGLConfig;
typedef Elastosx::Microedition::Khronos::Egl::IEGLContext XIEGLContext;
typedef Elastosx::Microedition::Khronos::Egl::IEGLDisplay XIEGLDisplay;
typedef Elastosx::Microedition::Khronos::Egl::IEGLSurface XIEGLSurface;

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI EglChooseConfig(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ ArrayOf<XIEGLConfig*>* configs,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [out] */ Boolean* result);

    CARAPI EglCopyBuffers(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [in] */ IInterface* native_pixmap,
        /* [out] */ Boolean* result);

    CARAPI EglCreateContext(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ XIEGLContext* share_context,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLContext** context);

    CARAPI EglCreatePbufferSurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLSurface** surface);

    CARAPI EglCreatePixmapSurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ IInterface* native_pixmap,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLSurface** surface);

    CARAPI EglCreateWindowSurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ IInterface* native_window,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLSurface** surface);

    CARAPI EglDestroyContext(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLContext* context,
        /* [out] */ Boolean* result);

    CARAPI EglDestroySurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI EglGetConfigAttrib(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Boolean* result);

    CARAPI EglGetConfigs(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ ArrayOf<XIEGLConfig*>* configs,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [out] */ Boolean* result);

    CARAPI EglGetCurrentContext(
        /* [out] */ XIEGLContext** context);

    CARAPI EglGetCurrentDisplay(
        /* [out] */ XIEGLDisplay** display);

    CARAPI EglGetCurrentSurface(
        /* [in] */ Int32 readdraw,
        /* [out] */ XIEGLSurface** surface);

    CARAPI EglGetDisplay(
        /* [in] */ IInterface* native_display,
        /* [out] */ XIEGLDisplay** display);

    CARAPI EglGetError(
        /* [out] */ Int32* error);

    CARAPI EglInitialize(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ ArrayOf<Int32>* major_minor,
        /* [out] */ Boolean* result);

    CARAPI EglMakeCurrent(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* draw,
        /* [in] */ XIEGLSurface* read,
        /* [in] */ XIEGLContext* context,
        /* [out] */ Boolean* result);

    CARAPI EglQueryContext(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLContext* context,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Boolean* result);

    CARAPI EglQueryString(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI EglQuerySurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Boolean* result);

    /** @hide **/
    CARAPI EglReleaseThread(
        /* [out] */ Boolean* result);

    CARAPI EglSwapBuffers(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI EglTerminate(
        /* [in] */ XIEGLDisplay* display,
        /* [out] */ Boolean* result);

    CARAPI EglWaitGL(
        /* [out] */ Boolean* result);

    CARAPI EglWaitNative(
        /* [in] */ Int32 engine,
        /* [in] */ IInterface* bindTarget,
        /* [out] */ Boolean* result);

public:
    static CARAPI_(Int32) GetInitCount(
        /* [in] */ XIEGLDisplay* display);

private:
    static CARAPI_(android::sp<ANativeWindow>) SurfaceGetNativeWindow(
        /* [in] */ IInterface* surfaceObj);

    CARAPI_(Int32) _eglCreateWindowSurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ IInterface* native_window,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ Int64* rst);

    CARAPI_(Int32) _eglCreateWindowSurfaceTexture(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ IInterface* native_window,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ Int64* rst);

private:
    AutoPtr<CEGLContextImpl> mContext;
    AutoPtr<CEGLDisplayImpl> mDisplay;
    AutoPtr<CEGLSurfaceImpl> mSurface;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif
