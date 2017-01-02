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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"

#include "elastos/droid/graphics/CSurfaceTexture.h"
#include "elastos/droid/view/Surface.h"
#include "elastos/droid/opengl/gles/CEGLImpl.h"
#include "elastos/droid/opengl/gles/CEGLConfigImpl.h"
#include "elastos/droid/opengl/gles/CEGL10Helper.h"

#include <elastos/utility/logging/Logger.h>

#include <opengl/EGL/egl_display.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <gui/GLConsumer.h>

#include <skia/core/SkBitmap.h>
#include <skia/core/SkPixelRef.h>
#include <ui/ANativeObjectBase.h>
#include <elastos/core/AutoLock.h>

using Elastosx::Microedition::Khronos::Egl::EIID_IEGL;
using Elastosx::Microedition::Khronos::Egl::IEGL10;
using Elastosx::Microedition::Khronos::Egl::EIID_IEGL10;
using Elastosx::Microedition::Khronos::Egl::IEGL11;
using Elastosx::Microedition::Khronos::Egl::EIID_IEGL11;
using Elastosx::Microedition::Khronos::Egl::IEGL10Helper;
using Elastosx::Microedition::Khronos::Egl::EIID_IEGL10Helper;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

static const String TAG("CEGLImpl");

static const Int32 gNull_attrib_base[] = {EGL_NONE};

static inline EGLDisplay getDisplay(Elastosx::Microedition::Khronos::Egl::IEGLDisplay* o) {
    if (!o) return EGL_NO_DISPLAY;
    return (EGLDisplay)(((CEGLDisplayImpl*)o)->GetEGLDisplay());
}
static inline EGLSurface getSurface(Elastosx::Microedition::Khronos::Egl::IEGLSurface* o) {
    if (!o) return EGL_NO_SURFACE;
    return (EGLSurface)(((CEGLSurfaceImpl*)o)->GetEGLSurface());
}
static inline EGLContext getContext(Elastosx::Microedition::Khronos::Egl::IEGLContext* o) {
    if (!o) return EGL_NO_CONTEXT;
    return (EGLContext)(((CEGLContextImpl*)o)->GetEGLContext());
}
static inline EGLConfig getConfig(Elastosx::Microedition::Khronos::Egl::IEGLConfig* o) {
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

static android::PixelFormat convertPixelFormat(SkColorType format)
{
    switch (format) {
    case kN32_SkColorType:   return android::PIXEL_FORMAT_RGBA_8888;
    case kARGB_4444_SkColorType:   return android::PIXEL_FORMAT_RGBA_4444;
    case kRGB_565_SkColorType:     return android::PIXEL_FORMAT_RGB_565;
    default:                            return android::PIXEL_FORMAT_NONE;
    }
}
static inline Boolean EglBoolToJBool(EGLBoolean eglBool) {
    return eglBool == EGL_TRUE ? TRUE : FALSE;
}

CAR_INTERFACE_IMPL_2(CEGLImpl, Object, IEGL10, IEGL)

CAR_OBJECT_IMPL(CEGLImpl)

ECode CEGLImpl::constructor()
{
    CEGLContextImpl::NewByFriend(-1, (CEGLContextImpl**)&mContext);
    CEGLDisplayImpl::NewByFriend(-1, (CEGLDisplayImpl**)&mDisplay);
    CEGLSurfaceImpl::NewByFriend(-1, (CEGLSurfaceImpl**)&mSurface);
    return NOERROR;
}

ECode CEGLImpl::EglChooseConfig(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [in] */ ArrayOf<XIEGLConfig*>* configs,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (display == NULL
        || !validAttribList(attrib_list)
        || (configs != NULL && configs->GetLength() < config_size)
        || (num_config != NULL && num_config->GetLength() < 1)) {
        Logger::E(TAG, "EglChooseConfig invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLBoolean success = EGL_FALSE;

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

    if (success && configs != NULL) {
        for (Int32 i = 0 ; i < num ; i++) {
            AutoPtr<XIEGLConfig> obj;
            CEGLConfigImpl::New(reinterpret_cast<Int64>(nativeConfigs[i]), (XIEGLConfig**)&obj);
            configs->Set(i, obj);
        }
    }
    *result = EglBoolToJBool(success);
    return NOERROR;
}

ECode CEGLImpl::EglCopyBuffers(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [in] */ IInterface* native_pixmap,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || surface == NULL || native_pixmap == NULL) {
        *result = FALSE;
        Logger::E(TAG, "EglCopyBuffers invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // TODO: Implement this
    *result = FALSE;
    return NOERROR;
}

ECode CEGLImpl::EglCreateContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ XIEGLContext* share_context,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLContext** context)
{
    VALIDATE_NOT_NULL(context)

    if (display == NULL || config == NULL || share_context == NULL
        || !validAttribList(attrib_list)) {
        *context = NULL;
        Logger::E(TAG, "EglCreateContext invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLConfig  cnf = getConfig(config);
    EGLContext shr = getContext(share_context);
    Int32* base = beginNativeAttribList(attrib_list);
    EGLContext ctx = eglCreateContext(dpy, cnf, shr, base);
    Int64 eglContextId = reinterpret_cast<Int64>(ctx);
    if (eglContextId == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        return helper->GetNoContext(context);
    }

    return CEGLContextImpl::New(eglContextId, context);
}

ECode CEGLImpl::EglCreatePbufferSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    if (display == NULL || config == NULL
        || !validAttribList(attrib_list)) {
        *surface = NULL;
        Logger::E(TAG, "EglCreatePbufferSurface invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLConfig  cnf = getConfig(config);
    Int32* base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreatePbufferSurface(dpy, cnf, base);
    Int64 eglSurfaceId = reinterpret_cast<Int64>(sur);
    if (eglSurfaceId == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        return helper->GetNoSurface(surface);
    }
    return CEGLSurfaceImpl::New(eglSurfaceId, surface);
}

ECode CEGLImpl::EglCreatePixmapSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_pixmap,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    AutoPtr<CEGLSurfaceImpl> out_sur;
    CEGLSurfaceImpl::NewByFriend((CEGLSurfaceImpl**)&out_sur);
    if (display == NULL || config == NULL || native_pixmap == NULL
        || !validAttribList(attrib_list)) {
        *surface = NULL;
        Logger::E(TAG, "EglCreatePixmapSurface invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLConfig  cnf = getConfig(config);
    Int32* base = 0;

    SkBitmap const * nativeBitmap = NULL;
    if (IBitmap::Probe(native_pixmap) != NULL) {
        AutoPtr<IBitmap> bt = IBitmap::Probe(native_pixmap);
        Handle64 nbt;
        bt->GetNativeBitmap(&nbt);
        nativeBitmap = (SkBitmap const *)nbt;
    }
    SkPixelRef* ref = nativeBitmap ? nativeBitmap->pixelRef() : 0;
    if (ref == NULL) {
        Logger::E("CEGLImpl", "Bitmap has no PixelRef");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SkSafeRef(ref);
    ref->lockPixels();

    egl_native_pixmap_t pixmap;
    pixmap.version = sizeof(pixmap);
    pixmap.width  = nativeBitmap->width();
    pixmap.height = nativeBitmap->height();
    pixmap.stride = nativeBitmap->rowBytes() / nativeBitmap->bytesPerPixel();
    pixmap.format = convertPixelFormat(nativeBitmap->colorType());
    pixmap.data   = (uint8_t*)ref->pixels();

    base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreatePixmapSurface(dpy, cnf, &pixmap, base);

    if (sur != EGL_NO_SURFACE) {
        out_sur->mEGLSurface = reinterpret_cast<Int64>(sur);
        out_sur->mNativePixelRef = reinterpret_cast<Int64>(ref);
    } else {
        ref->unlockPixels();
        SkSafeUnref(ref);
    }

    if (out_sur->mEGLSurface == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        return helper->GetNoSurface(surface);
    }
    *surface = out_sur;
    REFCOUNT_ADD(*surface)
    return NOERROR;
}

ECode CEGLImpl::EglCreateWindowSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_window,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

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

    Int64 eglSurfaceId;
    if (sur != NULL) {
        FAIL_RETURN(_eglCreateWindowSurface(display, config, sur, attrib_list, &eglSurfaceId));
    } else if (ISurfaceTexture::Probe(native_window) != NULL) {
        FAIL_RETURN(_eglCreateWindowSurfaceTexture(display, config,
                native_window, attrib_list, &eglSurfaceId));
    } else {
        Logger::E("CEGLImpl", "eglCreateWindowSurface() can only be called with an instance of "
            "Surface, SurfaceView, SurfaceHolder or SurfaceTexture at the moment.");
    }

    if (eglSurfaceId == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        return helper->GetNoSurface(surface);
    }
    return CEGLSurfaceImpl::New(eglSurfaceId, surface);
}

ECode CEGLImpl::EglDestroyContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || context == NULL) {
        *result = FALSE;
        Logger::E(TAG, "EglDestroyContext invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext ctx = getContext(context);
    *result = EglBoolToJBool(eglDestroyContext(dpy, ctx));
    return NOERROR;
}

ECode CEGLImpl::EglDestroySurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || surface == NULL) {
        *result = FALSE;
        Logger::E(TAG, "EglDestroySurface invalid argument.");
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
    *result = EglBoolToJBool(eglDestroySurface(dpy, sur));
    return NOERROR;
}

ECode CEGLImpl::EglGetConfigAttrib(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || config == NULL
        || (value == NULL || value->GetLength() < 1)) {
        *result = FALSE;
        Logger::E(TAG, "EglGetConfigAttrib invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext cnf = getConfig(config);
    EGLBoolean success = EGL_FALSE;
    Int32 localValue;
    success = eglGetConfigAttrib(dpy, cnf, attribute, &localValue);
    if (success) {
       (*value)[0] = localValue;
    }
    *result = EglBoolToJBool(success);
    return NOERROR;
}

ECode CEGLImpl::EglGetConfigs(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<XIEGLConfig*>* configs,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || (configs != NULL && configs->GetLength() < config_size)
        || (num_config != NULL && num_config->GetLength() < 1)) {
        *result = FALSE;
        Logger::E(TAG, "EglGetConfigs invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLBoolean success = EGL_FALSE;
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
            CEGLConfigImpl::New(reinterpret_cast<Int64>(nativeConfigs[i]), (XIEGLConfig**)&cfg);
            configs->Set(i, cfg);
        }
    }
    *result = EglBoolToJBool(success);
    return NOERROR;
}

ECode CEGLImpl::EglGetCurrentContext(
    /* [out] */ XIEGLContext** context)
{
    VALIDATE_NOT_NULL(context)

    Int64 value = reinterpret_cast<Int64>(eglGetCurrentContext());
    if (value == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        return helper->GetNoContext(context);
    }
    if (mContext->mEGLContext != value) {
        mContext = NULL;
        CEGLContextImpl::NewByFriend(value, (CEGLContextImpl**)&mContext);
    }
    *context = (XIEGLContext*)mContext.Get();
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CEGLImpl::EglGetCurrentDisplay(
    /* [out] */ XIEGLDisplay** display)
{
    VALIDATE_NOT_NULL(display)

    Int64 value = reinterpret_cast<Int64>(eglGetCurrentDisplay());
    if (value == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        return helper->GetNoDisplay(display);
    }
    if (mDisplay->mEGLDisplay != value){
        mDisplay = NULL;
        CEGLDisplayImpl::NewByFriend(value, (CEGLDisplayImpl**)&mDisplay);
    }
    *display = (XIEGLDisplay*)mDisplay.Get();
    REFCOUNT_ADD(*display);
    return NOERROR;
}

ECode CEGLImpl::EglGetCurrentSurface(
    /* [in] */ Int32 readdraw,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
    AutoPtr<XIEGLSurface> noSurface;
    helper->GetNoSurface((XIEGLSurface**)&noSurface);

    if ((readdraw != EGL_READ) && (readdraw != EGL_DRAW)) {
        *surface = noSurface;
        REFCOUNT_ADD(*surface)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 value = reinterpret_cast<Int64>(eglGetCurrentSurface(readdraw));
    if (value == 0) {
        *surface = noSurface;
        REFCOUNT_ADD(*surface)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mSurface->mEGLSurface != value){
        mSurface = NULL;
        CEGLSurfaceImpl::NewByFriend(value, (CEGLSurfaceImpl**)&mSurface);
    }
    *surface = (XIEGLSurface*)mSurface.Get();
    REFCOUNT_ADD(*surface);
    return NOERROR;
}

ECode CEGLImpl::EglGetDisplay(
    /* [in] */ IInterface* native_display,
    /* [out] */ XIEGLDisplay** display)
{
    VALIDATE_NOT_NULL(display)

    Int64 value = reinterpret_cast<Int64>(eglGetDisplay(EGL_DEFAULT_DISPLAY));
    if (value == 0) {
        AutoPtr<IEGL10Helper> helper;
        CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
        return helper->GetNoDisplay(display);
    }
    if (mDisplay->mEGLDisplay != value) {
        mDisplay = NULL;
        CEGLDisplayImpl::NewByFriend(value, (CEGLDisplayImpl**)&mDisplay);
    }
    *display = (XIEGLDisplay*)mDisplay.Get();
    REFCOUNT_ADD(*display);
    return NOERROR;
}

ECode CEGLImpl::EglGetError(
    /* [out] */ Int32* eRst)
{
    VALIDATE_NOT_NULL(eRst)

    EGLint error = eglGetError();
    *eRst = error;
    return NOERROR;
}

ECode CEGLImpl::EglInitialize(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<Int32>* major_minor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || (major_minor != NULL &&
            major_minor->GetLength() < 2)) {
        *result = FALSE;
        Logger::E(TAG, "EglInitialize invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    EGLDisplay dpy = getDisplay(display);
    EGLBoolean success = eglInitialize(dpy, NULL, NULL);
    if (success && major_minor) {
        Int32 len = major_minor->GetLength();
        if (len) {
            // we're exposing only EGL 1.0
            Int32* base = major_minor->GetPayload();
            if (len >= 1) base[0] = 1;
            if (len >= 2) base[1] = 0;
        }
    }
    *result = EglBoolToJBool(success);
    return NOERROR;
}

ECode CEGLImpl::EglMakeCurrent(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* draw,
    /* [in] */ XIEGLSurface* read,
    /* [in] */ XIEGLContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || draw == NULL || read == NULL || context == NULL) {
        *result = FALSE;
        Logger::E(TAG, "EglMakeCurrent invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLSurface sdr = getSurface(draw);
    EGLSurface srd = getSurface(read);
    EGLContext ctx = getContext(context);
    *result = EglBoolToJBool(eglMakeCurrent(dpy, sdr, srd, ctx));
    return NOERROR;
}

ECode CEGLImpl::EglQueryContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLContext* context,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || context == NULL || value == NULL
        || value->GetLength() < 1) {
        *result = FALSE;
        Logger::E(TAG, "EglQueryContext invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext ctx = getContext(context);
    EGLBoolean success = EGL_FALSE;
    Int32 len = value->GetLength();
    if (len) {
        Int32* base = value->GetPayload();
        success = eglQueryContext(dpy, ctx, attribute, base);
    }
    *result = EglBoolToJBool(success);
    return NOERROR;
}

ECode CEGLImpl::EglQueryString(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    if (display == NULL) {
        *str = String(NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    const char* chars = eglQueryString(dpy, name);
    *str = String(chars);
    return NOERROR;
}

ECode CEGLImpl::EglQuerySurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || surface == NULL || value == NULL
        || value->GetLength() < 1) {
        *result = FALSE;
        Logger::E(TAG, "EglQuerySurface invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext sur = getSurface(surface);

    EGLBoolean success = EGL_FALSE;
    Int32 len = value->GetLength();
    if (len) {
        Int32* base = value->GetPayload();
        success = eglQuerySurface(dpy, sur, attribute, base);
    }
    *result = EglBoolToJBool(success);
    return NOERROR;
}

/** @hide **/
ECode CEGLImpl::EglReleaseThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = EglBoolToJBool(eglReleaseThread());
    return NOERROR;
}

ECode CEGLImpl::EglSwapBuffers(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL || surface == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLSurface sur = getSurface(surface);
    *result = EglBoolToJBool(eglSwapBuffers(dpy, sur));
    return NOERROR;
}

ECode CEGLImpl::EglTerminate(
    /* [in] */ XIEGLDisplay* display,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (display == NULL) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    *result = EglBoolToJBool(eglTerminate(dpy));
    return NOERROR;
}

ECode CEGLImpl::EglWaitGL(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = EglBoolToJBool(eglWaitGL());
    return NOERROR;
}

ECode CEGLImpl::EglWaitNative(
    /* [in] */ Int32 engine,
    /* [in] */ IInterface* bindTarget,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = EglBoolToJBool(eglWaitNative(engine));
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
    ISurface* surface = ISurface::Probe(surfaceObj);
    if (surface == NULL) {
        Logger::E("CEGLImpl", "CEGLImpl::SurfaceGetNativeWindow");
        return NULL;
    }

    Int64 nativeSurf;
    surface->GetNativeSurface(&nativeSurf);
    android::sp<android::Surface> sur = reinterpret_cast<android::Surface*>(nativeSurf);
    return sur;
}

ECode CEGLImpl::_eglCreateWindowSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_window,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ Int64* rst)
{
    VALIDATE_NOT_NULL(rst)

    if (display == NULL || config == NULL
        || !validAttribList(attrib_list)) {
        *rst = 0;
        Logger::E(TAG, "_eglCreateWindowSurface invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext cnf = getConfig(config);
    android::sp<ANativeWindow> window;
    if (native_window == NULL) {
        Logger::E("CEGLImpl", "_eglCreateWindowSurface native_window == NULL");
not_valid_surface:
        Logger::E("CEGLImpl", "Make sure the SurfaceView or associated SurfaceHolder has a valid Surface");
        *rst = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    window = SurfaceGetNativeWindow(native_window);
    if (window == NULL) {
        Logger::E("CEGLImpl", "_eglCreateWindowSurface window == NULL");
        goto not_valid_surface;
    }

    Int32* base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreateWindowSurface(dpy, cnf, window.get(), base);
    *rst = reinterpret_cast<Int64>(sur);
    return NOERROR;
}


ECode CEGLImpl::_eglCreateWindowSurfaceTexture(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_window,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ Int64* rst)
{
    VALIDATE_NOT_NULL(rst)

    if (display == NULL || config == NULL
        || !validAttribList(attrib_list)) {
        *rst = 0;
        Logger::E(TAG, "_eglCreateWindowSurfaceTexture invalid argument.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLDisplay dpy = getDisplay(display);
    EGLContext cnf = getConfig(config);
    android::sp<ANativeWindow> window;
    if (native_window == 0) {
not_valid_surface:
        Logger::E("CEGLImpl", "Make sure the SurfaceTexture is valid");
        *rst = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISurfaceTexture> nw = ISurfaceTexture::Probe(native_window);
    CSurfaceTexture* stImpl = (CSurfaceTexture*)nw.Get();
    android::sp<android::IGraphicBufferProducer> producer((android::IGraphicBufferProducer*)stImpl->mProducer);

    window = new android::Surface(producer, true);
    if (window == NULL)
        goto not_valid_surface;

    Int32* base = beginNativeAttribList(attrib_list);
    EGLSurface sur = eglCreateWindowSurface(dpy, cnf, window.get(), base);
    *rst = reinterpret_cast<Int64>(sur);
    return NOERROR;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos