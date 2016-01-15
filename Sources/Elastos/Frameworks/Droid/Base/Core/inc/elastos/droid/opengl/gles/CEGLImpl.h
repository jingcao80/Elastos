#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLImpl.h"
#include <gui/Surface.h>
#include "gles/CEGLDisplayImpl.h"
#include "gles/CEGLSurfaceImpl.h"
#include "gles/CEGLContextImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CarClass(CEGLImpl)
{
typedef Elastosx::Microedition::Khronos::Egl::IEGLConfig XIEGLConfig;
typedef Elastosx::Microedition::Khronos::Egl::IEGLContext XIEGLContext;
typedef Elastosx::Microedition::Khronos::Egl::IEGLDisplay XIEGLDisplay;
typedef Elastosx::Microedition::Khronos::Egl::IEGLSurface XIEGLSurface;

public:
    CARAPI constructor();

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
        /* [out] */ Int32* rst);

    CARAPI_(Int32) _eglCreateWindowSurfaceTexture(
        /* [in] */ XIEGLDisplay* display,
        /* [in] */ XIEGLConfig* config,
        /* [in] */ IInterface* native_window,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [out] */ Int32* rst);

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
