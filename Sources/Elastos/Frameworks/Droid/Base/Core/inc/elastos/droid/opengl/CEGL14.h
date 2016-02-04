
#ifndef __ELASTOS_DROID_OPENGL_CEGL14_H__
#define __ELASTOS_DROID_OPENGL_CEGL14_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CEGL14.h"

#include <elastos/core/Singleton.h>
#include <gui/Surface.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CEGL14)
    , public Singleton
    , public IEGL14
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI EglGetError(
        /* [out] */ Int32* error);

    CARAPI EglGetDisplay(
        /* [in] */ Int32 display_id,
        /* [out] */ Elastos::Droid::Opengl::IEGLDisplay** display);

    CARAPI EglGetDisplay(
        /* [in] */ Int64 display_id,
        /* [out] */ Elastos::Droid::Opengl::IEGLDisplay** display);

    CARAPI EglGetDisplayInner(
        /* [in] */ Int64 display_id,
        /* [out] */ Elastos::Droid::Opengl::IEGLDisplay** display);

    CARAPI EglInitialize(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ ArrayOf<Int32>* major,
        /* [in] */ Int32 majorOffset,
        /* [in] */ ArrayOf<Int32>* minor,
        /* [in] */ Int32 minorOffset,
        /* [out] */ Boolean* result);

    CARAPI EglTerminate(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [out] */ Boolean* result);

    CARAPI EglQueryString(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI EglGetConfigs(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ ArrayOf<Elastos::Droid::Opengl::IEGLConfig *>* configs,
        /* [in] */ Int32 configsOffset,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [in] */ Int32 num_configOffset,
        /* [out] */ Boolean* result);

    CARAPI EglChooseConfig(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 attrib_listOffset,
        /* [in] */ ArrayOf<Elastos::Droid::Opengl::IEGLConfig *>* configs,
        /* [in] */ Int32 configsOffset,
        /* [in] */ Int32 config_size,
        /* [in] */ ArrayOf<Int32>* num_config,
        /* [in] */ Int32 num_configOffset,
        /* [out] */ Boolean* result);

    CARAPI EglGetConfigAttrib(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    CARAPI EglCreateWindowSurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ IInterface* win,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI EglCreatePbufferSurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI EglCreatePixmapSurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ Int32 pixmap,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI EglDestroySurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI EglQuerySurface(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    CARAPI EglBindAPI(
        /* [in] */ Int32 api,
        /* [out] */ Boolean* result);

    CARAPI EglQueryAPI(
        /* [out] */ Int32* value);

    CARAPI EglWaitClient(
        /* [out] */ Boolean* result);

    CARAPI EglReleaseThread(
        /* [out] */ Boolean* result);

    CARAPI EglCreatePbufferFromClientBuffer(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 buftype,
        /* [in] */ Int64 buffer,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI EglCreatePbufferFromClientBuffer(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 buftype,
        /* [in] */ Int32 buffer,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list_ref,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI EglCreatePbufferFromClientBufferInner(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 buftype,
        /* [in] */ Int64 buffer,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ ArrayOf<Int32>* attrib_list_ref,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI EglSurfaceAttrib(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 attribute,
        /* [in] */ Int32 value,
        /* [out] */ Boolean* result);

    CARAPI EglBindTexImage(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* result);

    CARAPI EglReleaseTexImage(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* result);

    CARAPI EglSwapInterval(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Int32 interval,
        /* [out] */ Boolean* result);

    CARAPI EglCreateContext(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLConfig* config,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* share_context,
        /* [in] */ ArrayOf<Int32>* attrib_list,
        /* [in] */ Int32 offset,
        /* [out] */ Elastos::Droid::Opengl::IEGLContext** context);

    CARAPI EglDestroyContext(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI EglMakeCurrent(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* draw,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* read,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI EglGetCurrentContext(
        /* [out] */ Elastos::Droid::Opengl::IEGLContext** context);

    CARAPI EglGetCurrentSurface(
        /* [in] */ Int32 readdraw,
        /* [out] */ Elastos::Droid::Opengl::IEGLSurface** surface);

    CARAPI EglGetCurrentDisplay(
        /* [out] */ Elastos::Droid::Opengl::IEGLDisplay** display);

    CARAPI EglQueryContext(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLContext* ctx,
        /* [in] */ Int32 attribute,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    CARAPI EglWaitGL(
        /* [out] */ Boolean* result);

    CARAPI EglWaitNative(
        /* [in] */ Int32 engine,
        /* [out] */ Boolean* result);

    CARAPI EglSwapBuffers(
        /* [in] */ Elastos::Droid::Opengl::IEGLDisplay* dpy,
        /* [in] */ Elastos::Droid::Opengl::IEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI EglCopyBuffers(
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

    static CARAPI_(Int64) FromEGLHandle(
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
