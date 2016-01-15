#include "gles/CEGLImpl.h"
#include "gles/CEGLConfigImpl.h"
#include "gles/CEGL10Helper.h"

#include <EGL/egl_display.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <gui/SurfaceTexture.h>
#include <gui/SurfaceTextureClient.h>

#include <skia/core/SkBitmap.h>
#include <skia/core/SkPixelRef.h>
#include <ui/ANativeObjectBase.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/logging/Slogger.h>

using Elastosx::Microedition::Khronos::egl::IEGL10;
using Elastosx::Microedition::Khronos::egl::IEGL11;
using Elastosx::Microedition::Khronos::egl::IEGL10Helper;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::Graphics::ISurfaceTexture;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

static const Int32 gNull_attrib_base[] = {EGL_NONE};

static inline EGLDisplay getDisplay(Elastosx::Microedition::Khronos::egl::IEGLDisplay* o) {
    if (!o) return EGL_NO_DISPLAY;
    return (EGLDisplay)(((CEGLDisplayImpl*)o)->GetEGLDisplay());
}
static inline EGLSurface getSurface(Elastosx::Microedition::Khronos::egl::IEGLSurface* o) {
    if (!o) return EGL_NO_SURFACE;
    return (EGLSurface)(((CEGLSurfaceImpl*)o)->GetEGLSurface());
}
static inline EGLContext getContext(Elastosx::Microedition::Khronos::egl::IEGLContext* o) {
    if (!o) return EGL_NO_CONTEXT;
    return (EGLContext)(((CEGLContextImpl*)o)->GetEGLContext());
}
static inline EGLConfig getConfig(Elastosx::Microedition::Khronos::egl::IEGLConfig* o) {
    if (!o) return 0;
    return (EGLConfig)(((CEGLConfigImpl*)o)->GetEGLConfig());
}

static Int32* beginNativeAttribList(ArrayOf<Int32>* attrib_list) {
    if (attrib_list != NULL) {
        return attrib_list->GetPayload();
    } else {
        return(Int32*) gNull_attrib_base;
    }
}

static Boolean validAttribList(ArrayOf<Int32>* attrib_list) {
    if (attrib_list == NULL) {
        return TRUE;
    }
    Int32 len = attrib_list->GetLength();
    if (len < 1) {
        return FALSE;
    }
    Int32 item = 0;
    item = (*attrib_list)[len-1];
    return item == EGL_NONE;
}

static android::PixelFormat convertPixelFormat(SkBitmap::Config format)
{
    switch (format) {
    case SkBitmap::kARGB_8888_Config:   return android::PIXEL_FORMAT_RGBA_8888;
    case SkBitmap::kARGB_4444_Config:   return android::PIXEL_FORMAT_RGBA_4444;
    case SkBitmap::kRGB_565_Config:     return android::PIXEL_FORMAT_RGB_565;
    case SkBitmap::kA8_Config:          return android::PIXEL_FORMAT_A_8;
    default:                            return android::PIXEL_FORMAT_NONE;
    }
}

ECode CEGLImpl::constructor()
{
    CEGLContextImpl::New(-1, (XIEGLContext**)&mContext);
    CEGLDisplayImpl::New(-1, (XIEGLDisplay**)&mDisplay);
    CEGLSurfaceImpl::New(-1, (XIEGLSurface**)&mSurface);
    return NOERROR;
}

ECode CEGLImpl::eglChooseConfig(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [in] */ ArrayOf<XIEGLConfig*>* configs,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config,
    /* [out] */ Boolean* result)
{
    if (display == NULL
        || !validAttribList(attrib_list)
        || (configs != NULL && configs->GetLength() < config_size)
        || (num_config != NULL && num_config->GetLength() < 1)) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    Boolean success = FALSE;

    if (configs == NULL) {
        config_size = 0;
    }
    EGLConfig nativeConfigs[config_size];

    Int32 num = 0;
    Int32* attrib_base = beginNativeAttribList(attrib_list);
    success = eglChooseConfig(dpy, attrib_base, configs ? nativeConfigs : 0, config_size, &num);

    if (num_config != NULL) {
        (*num_config)[0] = num;
    }

    if (success && configs!=NULL) {
        for (Int32 i=0 ; i<num ; i++) {
            AutoPtr<XIEGLConfig> obj;
            CEGLConfigImpl::New((Int32)nativeConfigs[i], (XIEGLConfig**)&obj);
            configs->Set(i, obj);
        }
    }
    *result = success;
    return NOERROR;
}

ECode CEGLImpl::eglCopyBuffers(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [in] */ IInterface* native_pixmap,
    /* [out] */ Boolean* result)
{
    if (display == NULL || surface == NULL || native_pixmap == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // TODO: Implement this
    *result = FALSE;
    return NOERROR;
}

ECode CEGLImpl::eglCreateContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ XIEGLContext* share_context,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLContext** context)
{
    if (display == NULL || config == NULL || share_context == NULL
        || !validAttribList(attrib_list)) {
        *context = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLConfig  cnf = getConfig(config);
    EGLContext shr = getContext(share_context);
    Int32* base = beginNativeAttribList(attrib_list);
    EGLContext ctx = eglCreateContext(dpy, cnf, shr, base);
    Int32 eglContextId = (Int32)ctx;
    if (eglContextId == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLContext> noContext;
        helper->GetNoContext((XIEGLContext**)&noContext);
        *context = noContext;
        REFCOUNT_ADD(*context);
        return NOERROR;
    }
    CEGLContextImpl::New(eglContextId, context);
    return NOERROR;
}

ECode CEGLImpl::eglCreatePbufferSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    if (display == NULL || config == NULL
        || !validAttribList(attrib_list)) {
        *surface = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLConfig  cnf = getConfig(config);
    Int32* base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreatePbufferSurface(dpy, cnf, base);
    Int32 eglSurfaceId =(Int32)sur;
    if (eglSurfaceId == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLSurface> noSurface;
        helper->GetNoSurface((XIEGLSurface**)&noSurface);
        *surface = noSurface;
        REFCOUNT_ADD(*surface);
        return NOERROR;
    }
    CEGLSurfaceImpl::New(eglSurfaceId, surface);
    return NOERROR;
}

ECode CEGLImpl::eglCreatePixmapSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_pixmap,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    AutoPtr<CEGLSurfaceImpl> out_sur;
    CEGLSurfaceImpl::NewByFriend((CEGLSurfaceImpl**)&out_sur);
    if (display == NULL || config == NULL || native_pixmap == NULL
        || !validAttribList(attrib_list)) {
        *surface = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLConfig  cnf = getConfig(config);
    Int32* base = 0;

    SkBitmap const * nativeBitmap = NULL;
    if (IBitmap::Probe(native_pixmap) != NULL) {
        AutoPtr<IBitmap> bt = IBitmap::Probe(native_pixmap);
        Handle32 nbt;
        bt->GetNativeBitmap(&nbt);
        nativeBitmap = (SkBitmap const *)nbt;
    }
    SkPixelRef* ref = nativeBitmap ? nativeBitmap->pixelRef() : 0;
    if (ref == NULL) {
        SLOGGERE("CEGLImpl", "Bitmap has no PixelRef")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SkSafeRef(ref);
    ref->lockPixels();

    egl_native_pixmap_t pixmap;
    pixmap.version = sizeof(pixmap);
    pixmap.width  = nativeBitmap->width();
    pixmap.height = nativeBitmap->height();
    pixmap.stride = nativeBitmap->rowBytes() / nativeBitmap->bytesPerPixel();
    pixmap.format = convertPixelFormat(nativeBitmap->config());
    pixmap.data   = (uint8_t*)ref->pixels();

    base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreatePixmapSurface(dpy, cnf, &pixmap, base);

    if (sur != EGL_NO_SURFACE) {
        out_sur->mEGLSurface = (Int32)sur;
        out_sur->mNativePixelRef = (Int32)ref;
    } else {
        ref->unlockPixels();
        SkSafeUnref(ref);
    }

    if (out_sur->mEGLSurface == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLSurface> noSurface;
        helper->GetNoSurface((XIEGLSurface**)&noSurface);
        *surface = noSurface;
        REFCOUNT_ADD(*surface);
        return NOERROR;
    }
    *surface = out_sur;
    REFCOUNT_ADD(*surface)
    return NOERROR;
}

ECode CEGLImpl::eglCreateWindowSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_window,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    AutoPtr<ISurface> sur;
    if (ISurfaceView::Probe(native_window) != NULL) {
        AutoPtr<ISurfaceView> surfaceView = ISurfaceView::Probe(native_window);
        AutoPtr<ISurfaceHolder> holder;
        surfaceView->GetHolder((ISurfaceHolder**)&holder);
        holder->GetSurface((ISurface**)&sur);
    } else if (ISurfaceHolder::Probe(native_window) != NULL) {
        AutoPtr<ISurfaceHolder> holder = ISurfaceHolder::Probe(native_window);
        holder->GetSurface((ISurface**)&sur);
    } else if (ISurface::Probe(native_window) != NULL) {
        sur = ISurface::Probe(native_window);
    }

    Int32 eglSurfaceId;
    if (sur != NULL) {
        FAIL_RETURN(_eglCreateWindowSurface(display, config, sur, attrib_list, &eglSurfaceId));
    } else if (ISurfaceTexture::Probe(native_window) != NULL) {
        FAIL_RETURN(_eglCreateWindowSurfaceTexture(display, config,
                native_window, attrib_list, &eglSurfaceId));
    } else {
        SLOGGERE("CEGLImpl", String("eglCreateWindowSurface() can only be called with an instance of ") +
            "Surface, SurfaceView, SurfaceHolder or SurfaceTexture at the moment.")
    }

    if (eglSurfaceId == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLSurface> noSurface;
        helper->GetNoSurface((XIEGLSurface**)&noSurface);
        *surface = noSurface;
        REFCOUNT_ADD(*surface);
        return NOERROR;
    }
    CEGLSurfaceImpl::New(eglSurfaceId, surface);
    return NOERROR;
}

ECode CEGLImpl::eglDestroyContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLContext* context,
    /* [out] */ Boolean* result)
{
    if (display == NULL || context == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext ctx = getContext(context);
    *result = eglDestroyContext(dpy, ctx);
    return NOERROR;
}

ECode CEGLImpl::eglDestroySurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    if (display == NULL || surface == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLSurface sur = getSurface(surface);

    if (sur) {
        SkPixelRef* ref = (SkPixelRef*)(((CEGLSurfaceImpl*)surface)->mNativePixelRef);
        if (ref) {
            ref->unlockPixels();
            SkSafeUnref(ref);
        }
    }
    *result = eglDestroySurface(dpy, sur);
    return NOERROR;
}

ECode CEGLImpl::eglGetConfigAttrib(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    if (display == NULL || config == NULL
        || (value == NULL || value->GetLength() < 1)) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext cnf = getConfig(config);
    Boolean success = FALSE;
    Int32 localValue;
    success = eglGetConfigAttrib(dpy, cnf, attribute, &localValue);
    if (success) {
       (*value)[0] = localValue;
    }
    *result = success;
    return NOERROR;
}

ECode CEGLImpl::eglGetConfigs(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<XIEGLConfig*>* configs,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config,
    /* [out] */ Boolean* result)
{
    if (display == NULL || (configs != NULL && configs->GetLength() < config_size)
        || (num_config != NULL && num_config->GetLength() < 1)) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    Boolean success = FALSE;
    if (configs == NULL) {
        config_size = 0;
    }
    EGLConfig nativeConfigs[config_size];
    Int32 num;
    success = eglGetConfigs(dpy, configs ? nativeConfigs : 0, config_size, &num);
    if (num_config != NULL) {
        (*num_config)[0] = num;
    }
    if (success && configs) {
        for (Int32 i=0; i < num; i++) {
            AutoPtr<XIEGLConfig> cfg;
            CEGLConfigImpl::New((Int32)nativeConfigs[i], (XIEGLConfig**)&cfg);
            configs->Set(i, cfg);
        }
    }
    *result = success;
    return NOERROR;
}

ECode CEGLImpl::eglGetCurrentContext(
    /* [out] */ XIEGLContext** context)
{
    Int32 value = (Int32)eglGetCurrentContext();
    if (value == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLContext> noContext;
        helper->GetNoContext((XIEGLContext**)&noContext);
        *context = noContext;
        REFCOUNT_ADD(*context);
        return NOERROR;
    }
    if (mContext->mEGLContext != value) {
        mContext = NULL;
        CEGLContextImpl::New(value, (XIEGLContext**)&mContext);
    }
    *context = mContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CEGLImpl::eglGetCurrentDisplay(
    /* [out] */ XIEGLDisplay** display)
{
    Int32 value = (Int32)eglGetCurrentDisplay();
    if (value == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLDisplay> noDisplay;
        helper->GetNoDisplay((XIEGLDisplay**)&noDisplay);
        *display = noDisplay;
        REFCOUNT_ADD(*display);
        return NOERROR;
    }
    if (mDisplay->mEGLDisplay != value){
        mDisplay = NULL;
        CEGLDisplayImpl::New(value, (XIEGLDisplay**)&mDisplay);
    }
    *display = mDisplay;
    REFCOUNT_ADD(*display);
    return NOERROR;
}

ECode CEGLImpl::eglGetCurrentSurface(
    /* [in] */ Int32 readdraw,
    /* [out] */ XIEGLSurface** surface)
{
    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
    AutoPtr<XIEGLSurface> noSurface;
    helper->GetNoSurface((XIEGLSurface**)&noSurface);

    if ((readdraw != EGL_READ) && (readdraw != EGL_DRAW)) {
        *surface = noSurface;
        REFCOUNT_ADD(*surface)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 value = (Int32)eglGetCurrentSurface(readdraw);
    if (value == 0) {
        *surface = noSurface;
        REFCOUNT_ADD(*surface)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mSurface->mEGLSurface != value){
        mSurface = NULL;
        CEGLSurfaceImpl::New(value, (XIEGLSurface**)&mSurface);
    }
    *surface = mSurface;
    REFCOUNT_ADD(*surface);
    return NOERROR;
}

ECode CEGLImpl::eglGetDisplay(
    /* [in] */ IInterface* native_display,
    /* [out] */ XIEGLDisplay** display)
{
    Int32 value = (Int32)eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (value == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        AutoPtr<XIEGLDisplay> noDisplay;
        helper->GetNoDisplay((XIEGLDisplay**)&noDisplay);
        *display = noDisplay;
        REFCOUNT_ADD(*display);
        return NOERROR;
    }
    if (mDisplay->mEGLDisplay != value) {
        mDisplay = NULL;
        CEGLDisplayImpl::New(value, (XIEGLDisplay**)&mDisplay);
    }
    *display = mDisplay;
    REFCOUNT_ADD(*display);
    return NOERROR;
}

ECode CEGLImpl::eglGetError(
    /* [out] */ Int32* eRst)
{
    EGLint error = eglGetError();
    *eRst = error;
    return NOERROR;
}

ECode CEGLImpl::eglInitialize(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<Int32>* major_minor,
    /* [out] */ Boolean* result)
{
    if (display == NULL || (major_minor != NULL &&
            major_minor->GetLength() < 2)) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    EGLDisplay dpy = getDisplay(display);
    Boolean success = eglInitialize(dpy, NULL, NULL);
    if (success && major_minor) {
        Int32 len = major_minor->GetLength();
        if (len) {
            // we're exposing only EGL 1.0
            Int32* base = major_minor->GetPayload();
            if (len >= 1) base[0] = 1;
            if (len >= 2) base[1] = 0;
        }
    }
    *result = success;
    return NOERROR;
}

ECode CEGLImpl::eglMakeCurrent(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* draw,
    /* [in] */ XIEGLSurface* read,
    /* [in] */ XIEGLContext* context,
    /* [out] */ Boolean* result)
{
    if (display == NULL || draw == NULL || read == NULL || context == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLSurface sdr = getSurface(draw);
    EGLSurface srd = getSurface(read);
    EGLContext ctx = getContext(context);
    *result = eglMakeCurrent(dpy, sdr, srd, ctx);
    return NOERROR;
}

ECode CEGLImpl::eglQueryContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLContext* context,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    if (display == NULL || context == NULL || value == NULL
        || value->GetLength() < 1) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext ctx = getContext(context);
    Boolean success = FALSE;
    Int32 len = value->GetLength();
    if (len) {
        Int32* base = value->GetPayload();
        success = eglQueryContext(dpy, ctx, attribute, base);
    }
    *result = success;
    return NOERROR;
}

ECode CEGLImpl::eglQueryString(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    if (display == NULL) {
        *str = String(NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    const char* chars = eglQueryString(dpy, name);
    *str = String(chars);
    return NOERROR;
}

ECode CEGLImpl::eglQuerySurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    if (display == NULL || surface == NULL || value == NULL
        || value->GetLength() < 1) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext sur = getSurface(surface);

    Boolean success = FALSE;
    Int32 len = value->GetLength();
    if (len) {
        Int32* base = value->GetPayload();
        success = eglQuerySurface(dpy, sur, attribute, base);
    }
    *result = success;
    return NOERROR;
}

/** @hide **/
ECode CEGLImpl::eglReleaseThread(
    /* [out] */ Boolean* result)
{
    *result = eglReleaseThread();
    return NOERROR;
}

ECode CEGLImpl::eglSwapBuffers(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    if (display == NULL || surface == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLSurface sur = getSurface(surface);
    *result = eglSwapBuffers(dpy, sur);
    return NOERROR;
}

ECode CEGLImpl::eglTerminate(
    /* [in] */ XIEGLDisplay* display,
    /* [out] */ Boolean* result)
{
    if (display == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    *result = eglTerminate(dpy);
    return NOERROR;
}

ECode CEGLImpl::eglWaitGL(
    /* [out] */ Boolean* result)
{
    *result = eglWaitGL();
    return NOERROR;
}

ECode CEGLImpl::eglWaitNative(
    /* [in] */ Int32 engine,
    /* [in] */ IInterface* bindTarget,
    /* [out] */ Boolean* result)
{
    *result = eglWaitNative(engine);
    return NOERROR;
}

Int32 CEGLImpl::GetInitCount(
    /* [in] */ XIEGLDisplay* display)
{
    EGLDisplay dpy = getDisplay(display);
    android::egl_display_t* eglDisplay = android::get_display_nowake(dpy);
    return eglDisplay ? eglDisplay->getRefsCount() : 0;
}

android::sp<ANativeWindow> CEGLImpl::SurfaceGetNativeWindow(
    /* [in] */ IInterface* surfaceObj)
{
    AutoPtr<ISurface> surface = ISurface::Probe(surfaceObj);
    if (surface == NULL) {
        SLOGGERE("CEGLImpl", "CEGLImpl::SurfaceGetNativeWindow")
        return NULL;
    }
    Handle32 tmp;
    surface->GetSurface(&tmp);
    android::sp<android::Surface> result =reinterpret_cast<android::Surface*>(tmp);
    if (result == NULL) {
        /*
         * if this method is called from the WindowManager's process, it means
         * the client is is not remote, and therefore is allowed to have
         * a Surface (data), so we create it here.
         * If we don't have a SurfaceControl, it means we're in a different
         * process.
         */
        surface->GetNativeSurfaceControl(&tmp);
        android::SurfaceControl* const control = reinterpret_cast<android::SurfaceControl*>(tmp);
        if (control) {
            result = control->getSurface();
            if (result != NULL) {
                result->incStrong(surfaceObj);
                surface->SetSurface(reinterpret_cast<Handle32>(result.get()));
            }
        }
    }

    return result;
}

ECode CEGLImpl::_eglCreateWindowSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_window,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ Int32* rst)
{
    if (display == NULL || config == NULL
        || !validAttribList(attrib_list)) {
        *rst = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext cnf = getConfig(config);
    android::sp<ANativeWindow> window;
    if (native_window == NULL) {
        SLOGGERE("CEGLImpl", "_eglCreateWindowSurface native_window == NULL");
not_valid_surface:
        SLOGGERE("CEGLImpl", "Make sure the SurfaceView or associated SurfaceHolder has a valid Surface")
        *rst = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    window = SurfaceGetNativeWindow(native_window);
    if (window == NULL) {
        SLOGGERE("CEGLImpl", "_eglCreateWindowSurface window == NULL");
        goto not_valid_surface;
    }

    Int32* base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreateWindowSurface(dpy, cnf, window.get(), base);
    *rst = (Int32)sur;
    return NOERROR;
}


ECode CEGLImpl::_eglCreateWindowSurfaceTexture(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_window,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ Int32* rst)
{
    if (display == NULL || config == NULL
        || !validAttribList(attrib_list)) {
        *rst = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext cnf = getConfig(config);
    android::sp<ANativeWindow> window;
    if (native_window == 0) {
not_valid_surface:
        SLOGGERE("CEGLImpl", "Make sure the SurfaceTexture is valid")
        *rst = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 surTexture;
    (ISurfaceTexture::Probe(native_window))->GetSurfaceTexture(&surTexture);
    android::sp<android::SurfaceTexture> surfaceTexture((android::SurfaceTexture*)surTexture);

    window = new android::SurfaceTextureClient(surfaceTexture);
    if (window == NULL)
        goto not_valid_surface;

    Int32* base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreateWindowSurface(dpy, cnf, window.get(), base);
    *rst = (Int32)sur;
    return NOERROR;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos