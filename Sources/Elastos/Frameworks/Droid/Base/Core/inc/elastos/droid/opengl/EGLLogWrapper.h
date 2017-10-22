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

#ifndef __ELASTOS_DROID_OPENGL_EGLLOGWRAPPER_H__
#define __ELASTOS_DROID_OPENGL_EGLLOGWRAPPER_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IWriter;
using Elastosx::Microedition::Khronos::Egl::IEGL;
using Elastosx::Microedition::Khronos::Egl::IEGL10;
using Elastosx::Microedition::Khronos::Egl::IEGL11;
using Elastosx::Microedition::Khronos::Egl::IEGLConfig;
using Elastosx::Microedition::Khronos::Egl::IEGLContext;
using Elastosx::Microedition::Khronos::Egl::IEGLDisplay;
using Elastosx::Microedition::Khronos::Egl::IEGLSurface;

namespace Elastos {
namespace Droid {
namespace Opengl {

class EGLLogWrapper
    : public Object
    , public IEGL
    , public IEGL10
    , public IEGL11
{
typedef Elastosx::Microedition::Khronos::Egl::IEGLConfig XIEGLConfig;
typedef Elastosx::Microedition::Khronos::Egl::IEGLContext XIEGLContext;
typedef Elastosx::Microedition::Khronos::Egl::IEGLDisplay XIEGLDisplay;
typedef Elastosx::Microedition::Khronos::Egl::IEGLSurface XIEGLSurface;

public:
    EGLLogWrapper(
        /* [in] */ IEGL* egl,
        /* [in] */ Int32 configFlags,
        /* [in] */ IWriter* log);

    CAR_INTERFACE_DECL()

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

    static CARAPI_(String) GetErrorString(
        /* [in] */ Int32 error);

private:
    CARAPI CheckError();

    CARAPI LogLine(
        /* [in] */ const String& message);

    CARAPI Log(
        /* [in] */ const String& message);

    CARAPI Begin(
        /* [in] */ const String& message);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI End();

    CARAPI Flush();

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 value);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ IInterface* obj);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ XIEGLDisplay* obj);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ XIEGLContext* obj);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ XIEGLSurface* obj);

    CARAPI Returns(
        /* [in] */ const String& result);

    CARAPI Returns(
        /* [in] */ Int32 result);

    CARAPI Returns(
        /* [in] */ Boolean result);

    CARAPI Returns(
        /* [in] */ IInterface* result);

    using Object::ToString;

    CARAPI_(String) ToString(
        /* [in] */ IInterface* obj);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Int32>* obj);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<IInterface*>* obj);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* obj,
        /* [in] */ Int32 offset);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<IInterface*>* obj,
        /* [in] */ Int32 offset);

    static CARAPI_(String) GetHex(
        /* [in] */ Int32 value);

private:
    AutoPtr<IEGL10> mEgl10;
    Int32 mArgCount;

public:
    AutoPtr<IWriter> mLog;
    Boolean mLogArgumentNames;
    Boolean mCheckError;


};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif
