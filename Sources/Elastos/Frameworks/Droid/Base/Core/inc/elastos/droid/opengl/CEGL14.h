
#ifndef __ELASTOS_DROID_OPENGL_CEGL14_H__
#define __ELASTOS_DROID_OPENGL_CEGL14_H__

#include "_Elastos_Droid_Opengl_CEGL14.h"
#include <gui/Surface.h>

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CEGL14)
{
public:
    CARAPI eglGetError(
        /* [out] */ Int32* error);

    CARAPI eglGetDisplay(
        /* [in] */ Int32 display_id,
        /* [out] */ Elastos::Droid::Opengl::IEGLDisplay** display);

    CARAPI eglInitialize(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ ArrayOf<Int32>* major,
        /* [in] */ Int32 majorOffset,
        /* [in] */ ArrayOf<Int32>* minor,
        /* [in] */ Int32 minorOffset,
        /* [out] */ Boolean* result);

    CARAPI eglTerminate(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [out] */ Boolean* result);

    CARAPI eglQueryString(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI eglGetConfigs(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ ArrayOf<Elastos::Droid::Opengl::IEGLConfig *>* configs,
        /* [in] */ Int32 configsOffset,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [in] */ Int32 num_configOffset,
        /* [out] */ Boolean* result);

    CARAPI eglChooseConfig(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 attrib_listOffset,
        /* [in] */ ArrayOf<Elastos::Droid::Opengl::IEGLConfig *>* configs,
        /* [in] */ Int32 configsOffset,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [in] */ Int32 num_configOffset,
        /* [out] */ Boolean* result);

    CARAPI eglGetConfigAttrib(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    CARAPI eglCreateWindowSurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ IInterface* win,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI eglCreatePbufferSurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI eglCreatePixmapSurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ Int32 pixmap,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI eglDestroySurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI eglQuerySurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    CARAPI eglBindAPI(
        /* [in] */ Int32 api,
        /* [out] */ Boolean* result);

    CARAPI eglQueryAPI(
        /* [out] */ Int32* value);

    CARAPI eglWaitClient(
        /* [out] */ Boolean* result);

    CARAPI eglReleaseThread(
        /* [out] */ Boolean* result);

    CARAPI eglCreatePbufferFromClientBuffer(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 buftype,
        /* [in] */ Int32 buffer,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI eglSurfaceAttrib(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 attribute,
        /* [in] */ Int32 value,
        /* [out] */ Boolean* result);

    CARAPI eglBindTexImage(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* result);

    CARAPI eglReleaseTexImage(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* result);

    CARAPI eglSwapInterval(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 interval,
        /* [out] */ Boolean* result);

    CARAPI eglCreateContext(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* share_context,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLContext** context);

    CARAPI eglDestroyContext(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI eglMakeCurrent(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* draw,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* read,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI eglGetCurrentContext(
        /* [out] */ Elastos::Droid::Opengl::IEGLContext** context);

    CARAPI eglGetCurrentSurface(
        /* [in] */ Int32 readdraw,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI eglGetCurrentDisplay(
        /* [out] */ Elastos::Droid::Opengl::IEGLDisplay** display);

    CARAPI eglQueryContext(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* ctx,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    CARAPI eglWaitGL(
        /* [out] */ Boolean* result);

    CARAPI eglWaitNative(
        /* [in] */ Int32 engine,
        /* [out] */ Boolean* result);

    CARAPI eglSwapBuffers(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI eglCopyBuffers(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 target,
        /* [out] */ Boolean* result);

public:
    static CARAPI_(AutoPtr<IEGLContext>) InitStaticContext();
    static CARAPI_(AutoPtr<IEGLDisplay>) InitStaticDisplay();
    static CARAPI_(AutoPtr<IEGLSurface>) InitStaticSurface();

    CARAPI _EglCreateWindowSurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ IInterface* win,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI _EglCreateWindowSurfaceTexture(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ IInterface* win,
        /* [in] */ ArrayOf<Int32>* attrib_list_ref,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

private:
    static CARAPI_(AutoPtr<IInterface>) ToEGLHandle(
        /* [in] */ REIID riid,
        /* [in] */ void* handle);

    static CARAPI_(Int32) FromEGLHandle(
        /* [in] */ IEGLObjectHandle* obj);

    static CARAPI_(android::sp<ANativeWindow>) SurfaceGetNativeWindow(
        /* [in] */ IInterface* surfaceObj);

public:
    static AutoPtr<IEGLContext> eglNoContextObject;
    static AutoPtr<IEGLDisplay> eglNoDisplayObject;
    static AutoPtr<IEGLSurface> eglNoSurfaceObject;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CEGL14_H__
