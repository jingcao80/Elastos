#ifndef __ELASTOS_DROID_OPENGL_EGLLOGWRAPPER_H__
#define __ELASTOS_DROID_OPENGL_EGLLOGWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"

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
    : public IEGL11
    , public ElRefBase
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

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI eglChooseConfig(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ ArrayOf<XIEGLConfig*>* configs,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [out] */ Boolean* result);

    CARAPI eglCopyBuffers(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [in] */ IInterface* native_pixmap,
        /* [out] */ Boolean* result);

    CARAPI eglCreateContext(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ XIEGLContext* share_context,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLContext** context);

    CARAPI eglCreatePbufferSurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLSurface** surface);

    CARAPI eglCreatePixmapSurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ IInterface* native_pixmap,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLSurface** surface);

    CARAPI eglCreateWindowSurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ IInterface* native_window,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ XIEGLSurface** surface);

    CARAPI eglDestroyContext(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLContext* context,
        /* [out] */ Boolean* result);

    CARAPI eglDestroySurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI eglGetConfigAttrib(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Boolean* result);

    CARAPI eglGetConfigs(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ ArrayOf<XIEGLConfig*>* configs,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [out] */ Boolean* result);

    CARAPI eglGetCurrentContext(
        /* [out] */ XIEGLContext** context);

    CARAPI eglGetCurrentDisplay(
        /* [out] */ XIEGLDisplay** display);

    CARAPI eglGetCurrentSurface(
        /* [in] */ Int32 readdraw,
        /* [out] */ XIEGLSurface** surface);

    CARAPI eglGetDisplay(
        /* [in] */ IInterface* native_display,
        /* [out] */ XIEGLDisplay** display);

    CARAPI eglGetError(
        /* [out] */ Int32* error);

    CARAPI eglInitialize(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ ArrayOf<Int32>* major_minor,
        /* [out] */ Boolean* result);

    CARAPI eglMakeCurrent(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* draw,
        /* [in] */ XIEGLSurface* read,
        /* [in] */ XIEGLContext* context,
        /* [out] */ Boolean* result);

    CARAPI eglQueryContext(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLContext* context,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Boolean* result);

    CARAPI eglQueryString(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI eglQuerySurface(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Boolean* result);

    /** @hide **/
    CARAPI eglReleaseThread(
        /* [out] */ Boolean* result);

    CARAPI eglSwapBuffers(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI eglTerminate(
        /* [in] */ XIEGLDisplay* display,
        /* [out] */ Boolean* result);

    CARAPI eglWaitGL(
        /* [out] */ Boolean* result);

    CARAPI eglWaitNative(
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
