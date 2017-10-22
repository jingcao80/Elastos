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

#include "elastos/droid/opengl/CEGL14.h"
#include "elastos/droid/opengl/EGLContextWrapper.h"
#include "elastos/droid/opengl/EGLDisplayWrapper.h"
#include "elastos/droid/opengl/EGLSurfaceWrapper.h"
#include "elastos/droid/opengl/EGLConfigWrapper.h"
#include "elastos/droid/graphics/CSurfaceTexture.h"
#include "elastos/droid/view/Surface.h"
#include "Elastos.Droid.View.h"

#include <EGL/egl.h>
#include <gui/GLConsumer.h>
#include <gui/Surface.h>
#include <elastos/utility/logging/Slogger.h>

#include <ui/ANativeObjectBase.h>

#define LOGD(msg) SLOGGERD("CEGL14", msg)

using Elastos::Droid::View::Surface;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Graphics::CSurfaceTexture;


namespace Elastos {
namespace Droid {
namespace Opengl {

AutoPtr<IEGLContext> CEGL14::InitStaticContext()
{
    AutoPtr<IEGLContext> tmp = EGLContextWrapper::CreateInstance(reinterpret_cast<Int64>(EGL_NO_CONTEXT));
    return tmp;
}

AutoPtr<IEGLDisplay> CEGL14::InitStaticDisplay()
{
    AutoPtr<IEGLDisplay> tmp = EGLDisplayWrapper::CreateInstance(reinterpret_cast<Int64>(EGL_NO_DISPLAY));
    return tmp;
}

AutoPtr<IEGLSurface> CEGL14::InitStaticSurface()
{
    AutoPtr<IEGLSurface> tmp = EGLSurfaceWrapper::CreateInstance(reinterpret_cast<Int64>(EGL_NO_SURFACE));
    return tmp;
}

AutoPtr<IEGLContext> CEGL14::eglNoContextObject = InitStaticContext();
AutoPtr<IEGLDisplay> CEGL14::eglNoDisplayObject = InitStaticDisplay();
AutoPtr<IEGLSurface> CEGL14::eglNoSurfaceObject = InitStaticSurface();

CAR_INTERFACE_IMPL(CEGL14, Singleton, IEGL14)

CAR_SINGLETON_IMPL(CEGL14)

ECode CEGL14::EglGetError(
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error)

    EGLint _returnValue = (EGLint) 0;
    _returnValue = eglGetError();
    *error = _returnValue;
    return NOERROR;
}

ECode CEGL14::EglGetDisplay(
    /* [in] */ Int32 display_id,
    /* [out] */ IEGLDisplay** display)
{
    VALIDATE_NOT_NULL(display)
    *display = NULL;

    if ((EGLNativeDisplayType)display_id != (EGLNativeDisplayType)IEGL14::_EGL_DEFAULT_DISPLAY) {
        // jniThrowException(_env, "java/lang/UnsupportedOperationException", "eglGetDisplay");
        SLOGGERD("CEGL14", "eglGetDisplay E_UNSUPPORTED_OPERATION_EXCEPTION")
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return EglGetDisplayInner(display_id, display);
}

ECode CEGL14::EglGetDisplayInner(
    /* [in] */ Int64 display_id,
    /* [out] */ IEGLDisplay** display)
{
    VALIDATE_NOT_NULL(display)

    EGLDisplay _returnValue = (EGLDisplay) 0;
    _returnValue = eglGetDisplay(
        reinterpret_cast<EGLNativeDisplayType>(display_id)
    );

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLDisplay, _returnValue);
    *display = IEGLDisplay::Probe(obj);
    REFCOUNT_ADD(*display)
    return NOERROR;
}

ECode CEGL14::EglGetDisplay(
    /* [in] */ Int64 display_id,
    /* [out] */ IEGLDisplay** display)
{
    return EglGetDisplayInner(display_id, display);
}

ECode CEGL14::EglInitialize(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ ArrayOf<Int32>* major_ref,
    /* [in] */ Int32 majorOffset,
    /* [in] */ ArrayOf<Int32>* minor_ref,
    /* [in] */ Int32 minorOffset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ECode returnEcode = NOERROR;
    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglInitialize: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    EGLint *major_base = (EGLint *) 0;
    Int32 _majorRemaining;
    EGLint *major = (EGLint *) 0;
    EGLint *minor_base = (EGLint *) 0;
    Int32 _minorRemaining;
    EGLint *minor = (EGLint *) 0;

    if (!major_ref) {
        LOGD("EglInitialize: major == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (majorOffset < 0) {
        LOGD("EglInitialize: majorOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _majorRemaining = major_ref->GetLength() - majorOffset;
    if (_majorRemaining < 1) {
        LOGD("EglInitialize: length - majorOffset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    major_base = (EGLint *) major_ref->GetPayload();
    major = major_base + majorOffset;

    if (!minor_ref) {
        LOGD("EglInitialize: minor == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (minorOffset < 0) {
        LOGD("EglInitialize: minorOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _minorRemaining = minor_ref->GetLength() - minorOffset;
    if (_minorRemaining < 1) {
        LOGD("EglInitialize: length - minorOffset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    minor_base = (EGLint *) minor_ref->GetPayload();
    minor = minor_base + minorOffset;

    _returnValue = eglInitialize(
        (EGLDisplay)dpy_native,
        (EGLint *)major,
        (EGLint *)minor
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglTerminate(
    /* [in] */ IEGLDisplay* dpy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglTerminate: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglTerminate(
        (EGLDisplay)dpy_native
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglQueryString(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = NULL;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglQueryString: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    const char* chars = (const char*) eglQueryString( dpy_native, (EGLint)name);
    *str = String(chars);
    return returnEcode;
}

ECode CEGL14::EglGetConfigs(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ ArrayOf<IEGLConfig *>* configs_ref,
    /* [in] */ Int32 configsOffset,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config_ref,
    /* [in] */ Int32 num_configOffset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglGetConfigs: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 _configsRemaining;
    EGLConfig *configs = (EGLConfig *) 0;
    EGLint *num_config_base = (EGLint *) 0;
    EGLint *num_config = (EGLint *) 0;

    if (!configs_ref) {
        LOGD("EglGetConfigs: configs == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (configsOffset < 0) {
        LOGD("EglGetConfigs: configsOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _configsRemaining = configs_ref->GetLength() - configsOffset;
    if (_configsRemaining < config_size) {
        LOGD("EglGetConfigs: length - configsOffset < config_size < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    configs = new EGLConfig[_configsRemaining];

    if (!num_config_ref) {
        LOGD("EglGetConfigs: num_config == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (num_configOffset < 0) {
        LOGD("EglGetConfigs: num_configOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    num_config_base = (EGLint *) num_config_ref->GetPayload();
    num_config = num_config_base + num_configOffset;

    _returnValue = eglGetConfigs(
        (EGLDisplay)dpy_native,
        (EGLConfig *)configs,
        (EGLint)config_size,
        (EGLint *)num_config
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglChooseConfig(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 attrib_listOffset,
    /* [in] */ ArrayOf<IEGLConfig *>* configs_ref,
    /* [in] */ Int32 configsOffset,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config_ref,
    /* [in] */ Int32 num_configOffset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglChooseConfig: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean attrib_list_sentinel = FALSE;
    EGLint *attrib_list_base = (EGLint *) 0;
    Int32 _attrib_listRemaining;
    EGLint *attrib_list = (EGLint *) 0;
    Int32 _configsRemaining;
    EGLConfig *configs = (EGLConfig *) 0;
    EGLint *num_config_base = (EGLint *) 0;
    Int32 _num_configRemaining;
    EGLint *num_config = (EGLint *) 0;

    if (!attrib_list_ref) {
        LOGD("EglChooseConfig: attrib_list == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (attrib_listOffset < 0) {
        LOGD("EglChooseConfig: attrib_listOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _attrib_listRemaining = attrib_list_ref->GetLength() - attrib_listOffset;
    attrib_list_base = (EGLint *) attrib_list_ref->GetPayload();
    attrib_list = attrib_list_base + attrib_listOffset;
    attrib_list_sentinel = FALSE;
    for (int i = _attrib_listRemaining - 1; i >= 0; i--)  {
        if (attrib_list[i] == EGL_NONE){
            attrib_list_sentinel = TRUE;
            break;
        }
    }
    if (attrib_list_sentinel == FALSE) {
        LOGD("EglChooseConfig: attrib_list must contain EGL_NONE!")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!configs_ref) {
        LOGD("EglChooseConfig: configs == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (configsOffset < 0) {
        LOGD("EglChooseConfig: configsOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _configsRemaining = configs_ref->GetLength() - configsOffset;
    if (_configsRemaining < config_size) {
        LOGD("EglChooseConfig: length - configsOffset < config_size < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    configs = new EGLConfig[_configsRemaining];

    if (!num_config_ref) {
        LOGD("EglChooseConfig: num_config == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (num_configOffset < 0) {
        LOGD("EglChooseConfig: num_configOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _num_configRemaining = num_config_ref->GetLength() - num_configOffset;
    if (_num_configRemaining < 1) {
        LOGD("EglChooseConfig: length - num_configOffset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    num_config_base = (EGLint *) num_config_ref->GetPayload();
    num_config = num_config_base + num_configOffset;

    _returnValue = eglChooseConfig(
        (EGLDisplay)dpy_native,
        (EGLint *)attrib_list,
        (EGLConfig *)configs,
        (EGLint)config_size,
        (EGLint *)num_config
    );

    if (configs) {
        for (Int32 i = 0; i < _configsRemaining; ++i) {
            AutoPtr<IInterface> configs_new = ToEGLHandle(EIID_IEGLConfig, configs[i]);
            configs_ref->Set(i + configsOffset, IEGLConfig::Probe(configs_new));
        }
        delete[] configs;
    }
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglGetConfigAttrib(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLConfig* config,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglGetConfigAttrib: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLConfig config_native = (EGLConfig) FromEGLHandle(IEGLObjectHandle::Probe(config));
    if (!config_native) {
        LOGD("EglGetConfigAttrib: config_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    EGLint *value_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *value = (EGLint *) 0;

    if (!value_ref) {
        LOGD("EglGetConfigAttrib: value == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("EglGetConfigAttrib: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = value_ref->GetLength() - offset;
    if (_remaining < 1) {
        LOGD("EglGetConfigAttrib: length - offset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    value_base = (EGLint *) value_ref->GetPayload();
    value = value_base + offset;

    _returnValue = eglGetConfigAttrib(
        (EGLDisplay)dpy_native,
        (EGLConfig)config_native,
        (EGLint)attribute,
        (EGLint *)value
    );

    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglCreateWindowSurface(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLConfig* config,
    /* [in] */ IInterface* win,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

        AutoPtr<ISurface> sur;
        if (ISurfaceView::Probe(win) != NULL) {
            AutoPtr<ISurfaceView> surfaceView = ISurfaceView::Probe(win);
            AutoPtr<ISurfaceHolder> holder;
            surfaceView->GetHolder((ISurfaceHolder**)&holder);
            holder->GetSurface((ISurface**)&sur);
        } else if (ISurfaceHolder::Probe(win) != NULL) {
            AutoPtr<ISurfaceHolder> holder = ISurfaceHolder::Probe(win);
            holder->GetSurface((ISurface**)&sur);
        } else if (ISurface::Probe(win) != NULL) {
            sur = ISurface::Probe(win);
        }

        if (sur != NULL) {
             _EglCreateWindowSurface(dpy, config, sur, attrib_list_ref, offset, surface);
        } else if (ISurfaceTexture::Probe(win)) {
            _EglCreateWindowSurfaceTexture(dpy, config,
                    win, attrib_list_ref, offset, surface);
        } else {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

    return NOERROR;
}

ECode CEGL14::_EglCreateWindowSurface(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLConfig* config,
    /* [in] */ IInterface* win,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    *surface = NULL;
    ECode returnEcode = NOERROR;

    EGLSurface _returnValue = (EGLSurface) 0;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("_EglCreateWindowSurface: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLConfig config_native = (EGLConfig) FromEGLHandle(IEGLObjectHandle::Probe(config));
    if (!config_native) {
        LOGD("_EglCreateWindowSurface: config_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 attrib_list_sentinel = 0;
    EGLint *attrib_list_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *attrib_list = (EGLint *) 0;
    android::sp<ANativeWindow> window;

    if (!attrib_list_ref) {
        LOGD("_EglCreateWindowSurface: attrib_list == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("_EglCreateWindowSurface: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (win == NULL) {
not_valid_surface:
        LOGD("_EglCreateWindowSurface: Make sure the SurfaceView or associated SurfaceHolder has a valid Surface")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    window = SurfaceGetNativeWindow(win);

    if (window == NULL)
        goto not_valid_surface;

    _remaining = attrib_list_ref->GetLength() - offset;
    attrib_list_base = (EGLint *) attrib_list_ref->GetPayload();
    attrib_list = attrib_list_base + offset;
    attrib_list_sentinel = 0;
    for (Int32 i = _remaining - 1; i >= 0; i--)  {
        if (*((EGLint*)(attrib_list + i)) == EGL_NONE){
            attrib_list_sentinel = 1;
            break;
        }
    }
    if (attrib_list_sentinel == 0) {
        LOGD("_EglCreateWindowSurface: attrib_list must contain EGL_NONE!")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglCreateWindowSurface(
        (EGLDisplay)dpy_native,
        (EGLConfig)config_native,
        (EGLNativeWindowType)window.get(),
        (EGLint *)attrib_list
    );

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLSurface, _returnValue);
    *surface = IEGLSurface::Probe(obj);
    REFCOUNT_ADD(*surface)
    return returnEcode;
}

ECode CEGL14::EglCreatePbufferSurface(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLConfig* config,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    *surface = NULL;

    EGLSurface _returnValue = (EGLSurface) 0;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglCreatePbufferSurface: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLConfig config_native = (EGLConfig) FromEGLHandle(IEGLObjectHandle::Probe(config));
    if (!config_native) {
        LOGD("EglCreatePbufferSurface: config_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean attrib_list_sentinel = FALSE;
    EGLint *attrib_list_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *attrib_list = (EGLint *) 0;

    if (!attrib_list_ref) {
        LOGD("EglCreatePbufferSurface: attrib_list == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("EglCreatePbufferSurface: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = attrib_list_ref->GetLength() - offset;
    attrib_list_base = (EGLint *) attrib_list_ref->GetPayload();
    attrib_list = attrib_list_base + offset;
    attrib_list_sentinel = FALSE;
    for (Int32 i = _remaining - 1; i >= 0; i--)  {
        if (attrib_list[i] == EGL_NONE){
            attrib_list_sentinel = TRUE;
            break;
        }
    }
    if (attrib_list_sentinel == FALSE) {
        LOGD("EglCreatePbufferSurface: attrib_list must contain EGL_NONE!")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglCreatePbufferSurface(
        (EGLDisplay)dpy_native,
        (EGLConfig)config_native,
        (EGLint *)attrib_list
    );

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLSurface, _returnValue);
    *surface = IEGLSurface::Probe(obj);
    REFCOUNT_ADD(*surface)
    return returnEcode;
}

ECode CEGL14::EglCreatePixmapSurface(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLConfig* config,
    /* [in] */ Int32 pixmap,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    *surface = NULL;

    LOGD("EglCreatePixmapSurface: Unsupported Operation")
    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLSurface, (EGLSurface) 0);
    *surface = IEGLSurface::Probe(obj);
    REFCOUNT_ADD(*surface)
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CEGL14::EglDestroySurface(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglDestroySurface: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface surface_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(surface));
    if (!surface_native) {
        LOGD("EglDestroySurface: surface_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglDestroySurface(
        (EGLDisplay)dpy_native,
        (EGLSurface)surface_native
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglQuerySurface(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* surface,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglQuerySurface: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface surface_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(surface));
    if (!surface_native) {
        LOGD("EglQuerySurface: surface_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLint *value_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *value = (EGLint *) 0;

    if (!value_ref) {
        SLOGGERD("CEGL14", "EglQuerySurface: value == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        SLOGGERD("CEGL14", "EglQuerySurface: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = value_ref->GetLength() - offset;
    if (_remaining < 1) {
        SLOGGERD("CEGL14", "EglQuerySurface: length - offset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    value_base = (EGLint *) value_ref->GetPayload();
    value = value_base + offset;

    _returnValue = eglQuerySurface(
        (EGLDisplay)dpy_native,
        (EGLSurface)surface_native,
        (EGLint)attribute,
        (EGLint *)value
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglBindAPI(
    /* [in] */ Int32 api,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    _returnValue = eglBindAPI(
        (EGLenum)api
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CEGL14::EglQueryAPI(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    EGLenum _returnValue = (EGLenum) 0;
    _returnValue = eglQueryAPI();
    *value = _returnValue;
    return NOERROR;
}

ECode CEGL14::EglWaitClient(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    _returnValue = eglWaitClient();
    *result = _returnValue;
    return NOERROR;
}

ECode CEGL14::EglReleaseThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    _returnValue = eglReleaseThread();
    *result = _returnValue;
    return NOERROR;
}

ECode CEGL14::EglCreatePbufferFromClientBuffer(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ Int32 buftype,
    /* [in] */ Int32 buffer,
    /* [in] */ IEGLConfig* config,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    if(sizeof(void*) != sizeof(uint32_t)) {
        // jniThrowException(_env, "java/lang/UnsupportedOperationException", "eglCreatePbufferFromClientBuffer");
        SLOGGERD("CEGL14", "eglCreatePbufferFromClientBuffer E_UNSUPPORTED_OPERATION_EXCEPTION")
        *surface = NULL;
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return EglCreatePbufferFromClientBufferInner(dpy, buftype, buffer, config, attrib_list_ref, offset, surface);
}

ECode CEGL14::EglCreatePbufferFromClientBuffer(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ Int32 buftype,
    /* [in] */ Int64 buffer,
    /* [in] */ IEGLConfig* config,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    return EglCreatePbufferFromClientBufferInner(dpy, buftype, buffer, config, attrib_list_ref, offset, surface);
}

ECode CEGL14::EglCreatePbufferFromClientBufferInner(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ Int32 buftype,
    /* [in] */ Int64 buffer,
    /* [in] */ IEGLConfig* config,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    *surface = NULL;
    ECode returnEcode = NOERROR;

    EGLSurface _returnValue = (EGLSurface) 0;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglCreatePbufferFromClientBufferInner: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLConfig config_native = (EGLConfig) FromEGLHandle(IEGLObjectHandle::Probe(config));
    if (!config_native) {
        LOGD("EglCreatePbufferFromClientBufferInner: config_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean attrib_list_sentinel = FALSE;
    EGLint *attrib_list_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *attrib_list = (EGLint *) 0;

    if (!attrib_list_ref) {
        LOGD("EglCreatePbufferFromClientBuffer: attrib_list == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("EglCreatePbufferFromClientBuffer: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = attrib_list_ref->GetLength() - offset;
    attrib_list_base = (EGLint *) attrib_list_ref->GetPayload();
    attrib_list = attrib_list_base + offset;
    attrib_list_sentinel = FALSE;
    for (Int32 i = _remaining - 1; i >= 0; i--)  {
        if (attrib_list[i] == EGL_NONE){
            attrib_list_sentinel = TRUE;
            break;
        }
    }
    if (attrib_list_sentinel == FALSE) {
        LOGD("EglCreatePbufferFromClientBuffer: attrib_list must contain EGL_NONE!")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglCreatePbufferFromClientBuffer(
        (EGLDisplay)dpy_native,
        (EGLenum)buftype,
        reinterpret_cast<EGLClientBuffer>(buffer),
        (EGLConfig)config_native,
        (EGLint *)attrib_list
    );

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLSurface, _returnValue);
    *surface = IEGLSurface::Probe(obj);
    REFCOUNT_ADD(*surface)
    return returnEcode;
}

ECode CEGL14::EglSurfaceAttrib(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* surface,
    /* [in] */ Int32 attribute,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglSurfaceAttrib: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface surface_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(surface));
    if (!surface_native) {
        LOGD("EglSurfaceAttrib: surface_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglSurfaceAttrib(
        (EGLDisplay)dpy_native,
        (EGLSurface)surface_native,
        (EGLint)attribute,
        (EGLint)value
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglBindTexImage(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* surface,
    /* [in] */ Int32 buffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglBindTexImage: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface surface_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(surface));
    if (!surface_native) {
        LOGD("EglBindTexImage: surface_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglBindTexImage(
        (EGLDisplay)dpy_native,
        (EGLSurface)surface_native,
        (EGLint)buffer
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglReleaseTexImage(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* surface,
    /* [in] */ Int32 buffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglReleaseTexImage: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface surface_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(surface));
    if (!surface_native) {
        LOGD("EglReleaseTexImage: surface_native== null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglReleaseTexImage(
        (EGLDisplay)dpy_native,
        (EGLSurface)surface_native,
        (EGLint)buffer
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglSwapInterval(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ Int32 interval,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    ECode returnEcode = NOERROR;

    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglSwapInterval: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglSwapInterval(
        (EGLDisplay)dpy_native,
        (EGLint)interval
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglCreateContext(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLConfig* config,
    /* [in] */ IEGLContext* share_context,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = NULL;
    ECode returnEcode = NOERROR;

    EGLContext _returnValue = (EGLContext) 0;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglCreateContext: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLConfig config_native = (EGLConfig) FromEGLHandle(IEGLObjectHandle::Probe(config));
    if (!config_native) {
        LOGD("EglCreateContext: config_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLContext share_context_native = (EGLContext) FromEGLHandle(IEGLObjectHandle::Probe(share_context));
    if (!share_context_native) {
        LOGD("EglCreateContext: share_context_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    bool attrib_list_sentinel = FALSE;
    EGLint *attrib_list_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *attrib_list = (EGLint *) 0;

    if (!attrib_list_ref) {
        LOGD("EglCreateContext: attrib_list == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("EglCreateContext: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = attrib_list_ref->GetLength() - offset;
    attrib_list_base = (EGLint *) attrib_list_ref->GetPayload();
    attrib_list = attrib_list_base + offset;
    attrib_list_sentinel = FALSE;
    for (Int32 i = _remaining - 1; i >= 0; i--)  {
        if (attrib_list[i] == EGL_NONE){
            attrib_list_sentinel = TRUE;
            break;
        }
    }
    if (attrib_list_sentinel == FALSE) {
        LOGD("EglCreateContext: attrib_list must contain EGL_NONE!")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglCreateContext(
        (EGLDisplay)dpy_native,
        (EGLConfig)config_native,
        (EGLContext)share_context_native,
        (EGLint *)attrib_list
    );

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLContext, _returnValue);
    *context = IEGLContext::Probe(obj);
    REFCOUNT_ADD(*context)
    return returnEcode;
}

ECode CEGL14::EglDestroyContext(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLContext* ctx,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode returnEcode = NOERROR;

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglDestroyContext: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLContext ctx_native = (EGLContext) FromEGLHandle(IEGLObjectHandle::Probe(ctx));
    if (!ctx_native) {
        // LOGD("EglDestroyContext: ctx_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglDestroyContext(
        (EGLDisplay)dpy_native,
        (EGLContext)ctx_native
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglMakeCurrent(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* draw,
    /* [in] */ IEGLSurface* read,
    /* [in] */ IEGLContext* ctx,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode returnEcode = NOERROR;

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglMakeCurrent: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface draw_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(draw));
    if (!draw_native) {
        // LOGD("EglMakeCurrent: draw_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface read_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(read));
    if (!read_native) {
        // LOGD("EglMakeCurrent: read_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLContext ctx_native = (EGLContext) FromEGLHandle(IEGLObjectHandle::Probe(ctx));
    if (!ctx_native) {
        // LOGD("EglMakeCurrent: ctx_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglMakeCurrent(
        (EGLDisplay)dpy_native,
        (EGLSurface)draw_native,
        (EGLSurface)read_native,
        (EGLContext)ctx_native
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglGetCurrentContext(
    /* [out] */ IEGLContext** context)
{
    VALIDATE_NOT_NULL(context)

    EGLContext _returnValue = (EGLContext) 0;
    _returnValue = eglGetCurrentContext();

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLContext, _returnValue);
    *context = IEGLContext::Probe(obj);
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode CEGL14::EglGetCurrentSurface(
    /* [in] */ Int32 readdraw,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    EGLSurface _returnValue = (EGLSurface) 0;
    _returnValue = eglGetCurrentSurface(
        (EGLint)readdraw
    );

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLSurface, _returnValue);
    *surface = IEGLSurface::Probe(obj);
    REFCOUNT_ADD(*surface)
    return NOERROR;
}

ECode CEGL14::EglGetCurrentDisplay(
    /* [out] */ IEGLDisplay** display)
{
    VALIDATE_NOT_NULL(display)

    EGLDisplay _returnValue = (EGLDisplay) 0;
    _returnValue = eglGetCurrentDisplay();

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLDisplay, _returnValue);
    *display = IEGLDisplay::Probe(obj);
    REFCOUNT_ADD(*display)
    return NOERROR;
}

ECode CEGL14::EglQueryContext(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLContext* ctx,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode returnEcode = NOERROR;

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglQueryContext: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLContext ctx_native = (EGLContext) FromEGLHandle(IEGLObjectHandle::Probe(ctx));
    if (!ctx_native) {
        LOGD("EglQueryContext: ctx_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLint *value_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *value = (EGLint *) 0;

    if (!value_ref) {
        LOGD("EglQueryContext: value == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("EglQueryContext: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = value_ref->GetLength() - offset;
    if (_remaining < 1) {
        LOGD("EglQueryContext: length - offset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    value_base = (EGLint *) value_ref->GetPayload();
    value = value_base + offset;

    _returnValue = eglQueryContext(
        (EGLDisplay)dpy_native,
        (EGLContext)ctx_native,
        (EGLint)attribute,
        (EGLint *)value
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglWaitGL(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    _returnValue = eglWaitGL();
    *result = _returnValue;
    return NOERROR;
}

ECode CEGL14::EglWaitNative(
    /* [in] */ Int32 engine,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EGLBoolean _returnValue = (EGLBoolean) 0;
    _returnValue = eglWaitNative(
        (EGLint)engine
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CEGL14::EglSwapBuffers(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode returnEcode = NOERROR;

    EGLBoolean _returnValue = (EGLBoolean) 0;
    *result = _returnValue;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("EglSwapBuffers: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLSurface surface_native = (EGLSurface) FromEGLHandle(IEGLObjectHandle::Probe(surface));
    if (!surface_native) {
        LOGD("EglSwapBuffers: surface_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglSwapBuffers(
        (EGLDisplay)dpy_native,
        (EGLSurface)surface_native
    );
    *result = _returnValue;
    return returnEcode;
}

ECode CEGL14::EglCopyBuffers(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLSurface* surface,
    /* [in] */ Int32 target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (EGLBoolean)0;
    LOGD("EglCopyBuffers: Unsupported Operation")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

AutoPtr<IInterface> CEGL14::ToEGLHandle(
    /* [in] */ REIID riid,
    /* [in] */ void* handle)
{
    if (riid == EIID_IEGLContext) {
        if ((EGLContext)handle == EGL_NO_CONTEXT) {
            return eglNoContextObject;
        }

        return EGLContextWrapper::CreateInstance(reinterpret_cast<Int64>(handle));
    }

    if (riid == EIID_IEGLDisplay) {
        if ((EGLDisplay)handle == EGL_NO_DISPLAY) {
            return eglNoDisplayObject;
        }

        return EGLDisplayWrapper::CreateInstance(reinterpret_cast<Int64>(handle));
    }

    if (riid == EIID_IEGLSurface) {
        if ((EGLSurface)handle == EGL_NO_SURFACE) {
            return eglNoContextObject;
        }

        return EGLSurfaceWrapper::CreateInstance(reinterpret_cast<Int64>(handle));
    }

    if (riid == EIID_IEGLConfig) {
        return EGLConfigWrapper::CreateInstance(reinterpret_cast<Int64>(handle));
    }

    return NULL;
}

Int64 CEGL14::FromEGLHandle(
    /* [in] */ IEGLObjectHandle* obj)
{
    if (obj == NULL) {
        // LOGD("FromEGLHandle: Object is set to null.")
        //return E_ILLEGAL_ARGUMENT_EXCEPTION;
        return 0;//NULL;
    }
    Int64 handle;
    obj->GetNativeHandle(&handle);
    return handle;
}

android::sp<ANativeWindow> CEGL14::SurfaceGetNativeWindow(
    /* [in] */ IInterface* surfaceObj)
{
    AutoPtr<ISurface> surface = ISurface::Probe(surfaceObj);
    if (surface == NULL)
        return NULL;
    Surface* surfImpl = (Surface*)surface.Get();
    android::sp<android::Surface> result =reinterpret_cast<android::Surface*>(surfImpl->mNativeObject);

    return result;
}

ECode CEGL14::_EglCreateWindowSurfaceTexture(
    /* [in] */ IEGLDisplay* dpy,
    /* [in] */ IEGLConfig* config,
    /* [in] */ IInterface* win,
    /* [in] */ ArrayOf<Int32>* attrib_list_ref,
    /* [in] */ Int32 offset,
    /* [out] */ IEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    *surface = NULL;
    ECode returnEcode = NOERROR;

    EGLSurface _returnValue = (EGLSurface) 0;
    EGLDisplay dpy_native = (EGLDisplay) FromEGLHandle(IEGLObjectHandle::Probe(dpy));
    if (!dpy_native) {
        LOGD("_EglCreateWindowSurfaceTexture: dpy_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    EGLConfig config_native = (EGLConfig) FromEGLHandle(IEGLObjectHandle::Probe(config));
    if (!config_native) {
        LOGD("_EglCreateWindowSurfaceTexture: config_native == null")
        returnEcode = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 attrib_list_sentinel = 0;
    EGLint *attrib_list_base = (EGLint *) 0;
    Int32 _remaining;
    EGLint *attrib_list = (EGLint *) 0;
    android::sp<ANativeWindow> window;
    android::sp<android::IGraphicBufferProducer> producer;

    if (!attrib_list_ref) {
        LOGD("_EglCreateWindowSurfaceTexture: attrib_list == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("_EglCreateWindowSurfaceTexture: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (win == NULL) {
not_valid_surface:
        LOGD("_EglCreateWindowSurfaceTexture: Make sure the SurfaceView or associated SurfaceHolder has a valid Surface")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ISurfaceTexture> st = ISurfaceTexture::Probe(win);
    CSurfaceTexture* stImpl = (CSurfaceTexture*)(st.Get());
    producer = (android::IGraphicBufferProducer*)(stImpl->mProducer);

    if (producer == NULL)
        goto not_valid_surface;

    window = new android::Surface(producer, true);

    if (window == NULL)
        goto not_valid_surface;

    _remaining = attrib_list_ref->GetLength() - offset;
    attrib_list_base = (EGLint *) attrib_list_ref->GetPayload();
    attrib_list = attrib_list_base + offset;
    attrib_list_sentinel = 0;
    for (Int32 i = _remaining - 1; i >= 0; i--)  {
        if (*((EGLint*)(attrib_list + i)) == EGL_NONE){
            attrib_list_sentinel = 1;
            break;
        }
    }
    if (attrib_list_sentinel == 0) {
        LOGD("_EglCreateWindowSurfaceTexture: attrib_list must contain EGL_NONE!")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _returnValue = eglCreateWindowSurface(
        (EGLDisplay)dpy_native,
        (EGLConfig)config_native,
        (EGLNativeWindowType)window.get(),
        (EGLint *)attrib_list
    );

    AutoPtr<IInterface> obj = ToEGLHandle(EIID_IEGLSurface, _returnValue);
    *surface = IEGLSurface::Probe(obj);
    REFCOUNT_ADD(*surface)
    return returnEcode;
}


} // namespace Opengl
} // namespace Droid
} // namespace Elastos

