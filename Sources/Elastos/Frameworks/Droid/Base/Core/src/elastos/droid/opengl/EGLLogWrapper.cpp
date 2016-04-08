#include "elastos/droid/opengl/EGLLogWrapper.h"
#include "elastos/droid/opengl/gles/CEGL10Helper.h"
#include "Elastos.CoreLibrary.IO.h"

#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastosx::Microedition::Khronos::Egl::IEGL10Helper;
using Elastos::Droid::Opengl::Gles::CEGL10Helper;
using Elastosx::Microedition::Khronos::Egl::EIID_IEGL;
using Elastosx::Microedition::Khronos::Egl::EIID_IEGL10;
using Elastosx::Microedition::Khronos::Egl::EIID_IEGL11;

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL_3(EGLLogWrapper, Object, IEGL, IEGL10, IEGL11)

EGLLogWrapper::EGLLogWrapper(
    /* [in] */ IEGL* egl,
    /* [in] */ Int32 configFlags,
    /* [in] */ IWriter* log)
{
    mEgl10 = (IEGL10*)egl;
    mLog = log;
    mLogArgumentNames = (IGLDebugHelper::_CONFIG_LOG_ARGUMENT_NAMES & configFlags) != 0;
    mCheckError = (IGLDebugHelper::_CONFIG_CHECK_GL_ERROR & configFlags) != 0;
}

ECode EGLLogWrapper::EglChooseConfig(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [in] */ ArrayOf<XIEGLConfig*>* configs,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglChooseConfig"));
    Arg(String("display"), display);
    Arg(String("attrib_list"), attrib_list);
    Arg(String("config_size"), config_size);
    End();

    mEgl10->EglChooseConfig(display, attrib_list, configs, config_size, num_config, result);
    Int32 len = configs->GetLength();
    AutoPtr< ArrayOf<IInterface*> > temp = ArrayOf<IInterface*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        temp->Set(i, (IInterface*)((*configs)[i])->Probe(EIID_IInterface));
    }
    Arg(String("configs"), temp);
    Arg(String("num_config"), num_config);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglCopyBuffers(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [in] */ IInterface* native_pixmap,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglCopyBuffers"));
    Arg(String("display"), display);
    Arg(String("surface"), surface);
    Arg(String("native_pixmap"), native_pixmap);
    End();

    mEgl10->EglCopyBuffers(display, surface, native_pixmap, result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglCreateContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ XIEGLContext* share_context,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLContext** context)
{
    VALIDATE_NOT_NULL(context)

    Begin(String("eglCreateContext"));
    Arg(String("display"), display);
    Arg(String("config"), config);
    Arg(String("share_context"), share_context);
    Arg(String("attrib_list"), attrib_list);
    End();

    mEgl10->EglCreateContext(display, config, share_context, attrib_list, context);
    Returns(*context);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglCreatePbufferSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    Begin(String("eglCreatePbufferSurface"));
    Arg(String("display"), display);
    Arg(String("config"), config);
    Arg(String("attrib_list"), attrib_list);
    End();

    mEgl10->EglCreatePbufferSurface(display, config, attrib_list, surface);
    Returns(*surface);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglCreatePixmapSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_pixmap,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    Begin(String("eglCreatePixmapSurface"));
    Arg(String("display"), display);
    Arg(String("config"), config);
    Arg(String("native_pixmap"), native_pixmap);
    Arg(String("attrib_list"), attrib_list);
    End();

    mEgl10->EglCreatePixmapSurface(display, config, native_pixmap, attrib_list, surface);
    Returns(*surface);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglCreateWindowSurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* native_window,
    /* [in] */ ArrayOf<Int32>* attrib_list,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    Begin(String("eglCreateWindowSurface"));
    Arg(String("display"), display);
    Arg(String("config"), config);
    Arg(String("native_window"), native_window);
    Arg(String("attrib_list"), attrib_list);
    End();

    mEgl10->EglCreateWindowSurface(display, config, native_window, attrib_list, surface);
    Returns(*surface);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglDestroyContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglDestroyContext"));
    Arg(String("display"), display);
    Arg(String("context"), context);
    End();

    mEgl10->EglDestroyContext(display, context, result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglDestroySurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglDestroySurface"));
    Arg(String("display"), display);
    Arg(String("surface"), surface);
    End();

    mEgl10->EglDestroySurface(display, surface, result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglGetConfigAttrib(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglGetConfigAttrib"));
    Arg(String("display"), display);
    Arg(String("config"), config);
    Arg(String("attribute"), attribute);
    End();
    mEgl10->EglGetConfigAttrib(display, config, attribute, value, result);
    Arg(String("value"), value);
    Returns(*result);
    CheckError();
    *result = FALSE;
    return NOERROR;
}

ECode EGLLogWrapper::EglGetConfigs(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<XIEGLConfig*>* configs,
    /* [in] */ Int32 config_size,
    /* [in] */ ArrayOf<Int32>* num_config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglGetConfigs"));
    Arg(String("display"), display);
    Arg(String("config_size"), config_size);
    End();

    mEgl10->EglGetConfigs(display, configs, config_size, num_config, result);
    Int32 len = configs->GetLength();
    AutoPtr< ArrayOf<IInterface*> > temp = ArrayOf<IInterface*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        temp->Set(i, (IInterface*)((*configs)[i])->Probe(EIID_IInterface));
    }
    Arg(String("configs"), temp);
    Arg(String("num_config"), num_config);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglGetCurrentContext(
    /* [out] */ XIEGLContext** context)
{
    VALIDATE_NOT_NULL(context)

    Begin(String("eglGetCurrentContext"));
    End();

    mEgl10->EglGetCurrentContext(context);
    Returns(*context);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglGetCurrentDisplay(
    /* [out] */ XIEGLDisplay** display)
{
    VALIDATE_NOT_NULL(display)

    Begin(String("eglGetCurrentDisplay"));
    End();

    mEgl10->EglGetCurrentDisplay(display);
    Returns(*display);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglGetCurrentSurface(
    /* [in] */ Int32 readdraw,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    Begin(String("eglGetCurrentSurface"));
    Arg(String("readdraw"), readdraw);
    End();

    mEgl10->EglGetCurrentSurface(readdraw, surface);
    Returns(*surface);

    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglGetDisplay(
    /* [in] */ IInterface* native_display,
    /* [out] */ XIEGLDisplay** display)
{
    VALIDATE_NOT_NULL(display)

    Begin(String("eglGetDisplay"));
    Arg(String("native_display"), native_display);
    End();

    mEgl10->EglGetDisplay(native_display, display);
    Returns(*display);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglGetError(
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error)

    Begin(String("eglGetError"));
    End();

    mEgl10->EglGetError(error);
    Returns(GetErrorString(*error));
    return NOERROR;
}

ECode EGLLogWrapper::EglInitialize(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<Int32>* major_minor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglInitialize"));
    Arg(String("display"), display);
    End();
    mEgl10->EglInitialize(display, major_minor, result);
    Returns(*result);
    Arg(String("major_minor"), major_minor);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglMakeCurrent(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* draw,
    /* [in] */ XIEGLSurface* read,
    /* [in] */ XIEGLContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglMakeCurrent"));
    Arg(String("display"), display);
    Arg(String("draw"), draw);
    Arg(String("read"), read);
    Arg(String("context"), context);
    End();
    mEgl10->EglMakeCurrent(display, draw, read, context, result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglQueryContext(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLContext* context,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglQueryContext"));
    Arg(String("display"), display);
    Arg(String("context"), context);
    Arg(String("attribute"), attribute);
    End();
    mEgl10->EglQueryContext(display, context, attribute, value, result);
    Returns((*value)[0]);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglQueryString(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Begin(String("eglQueryString"));
    Arg(String("display"), display);
    Arg(String("name"), name);
    End();
    mEgl10->EglQueryString(display, name, str);
    Returns(*str);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglQuerySurface(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [in] */ Int32 attribute,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglQuerySurface"));
    Arg(String("display"), display);
    Arg(String("surface"), surface);
    Arg(String("attribute"), attribute);
    End();
    mEgl10->EglQuerySurface(display, surface, attribute, value, result);
    Returns((*value)[0]);
    Returns(*result);
    CheckError();
    return NOERROR;
}

/** @hide **/
ECode EGLLogWrapper::EglReleaseThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglReleaseThread"));
    End();
    mEgl10->EglReleaseThread(result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglSwapBuffers(
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglInitialize"));
    Arg(String("display"), display);
    Arg(String("surface"), surface);
    End();
    mEgl10->EglSwapBuffers(display, surface, result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglTerminate(
    /* [in] */ XIEGLDisplay* display,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglTerminate"));
    Arg(String("display"), display);
    End();
    mEgl10->EglTerminate(display, result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglWaitGL(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglWaitGL"));
    End();
    mEgl10->EglWaitGL(result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

ECode EGLLogWrapper::EglWaitNative(
    /* [in] */ Int32 engine,
    /* [in] */ IInterface* bindTarget,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Begin(String("eglWaitNative"));
    Arg(String("engine"), engine);
    Arg(String("bindTarget"), bindTarget);
    End();
    mEgl10->EglWaitNative(engine, bindTarget, result);
    Returns(*result);
    CheckError();
    return NOERROR;
}

String EGLLogWrapper::GetErrorString(
    /* [in] */ Int32 error)
{
    switch (error) {
    case IEGL10::_EGL_SUCCESS:
        return String("EGL_SUCCESS");

    case IEGL10::_EGL_NOT_INITIALIZED:
        return String("EGL_NOT_INITIALIZED");

    case IEGL10::_EGL_BAD_ACCESS:
        return String("EGL_BAD_ACCESS");

    case IEGL10::_EGL_BAD_ALLOC:
        return String("EGL_BAD_ALLOC");

    case IEGL10::_EGL_BAD_ATTRIBUTE:
        return String("EGL_BAD_ATTRIBUTE");

    case IEGL10::_EGL_BAD_CONFIG:
        return String("EGL_BAD_CONFIG");

    case IEGL10::_EGL_BAD_CONTEXT:
        return String("EGL_BAD_CONTEXT");

    case IEGL10::_EGL_BAD_CURRENT_SURFACE:
        return String("EGL_BAD_CURRENT_SURFACE");

    case IEGL10::_EGL_BAD_DISPLAY:
        return String("EGL_BAD_DISPLAY");

    case IEGL10::_EGL_BAD_MATCH:
        return String("EGL_BAD_MATCH");

    case IEGL10::_EGL_BAD_NATIVE_PIXMAP:
        return String("EGL_BAD_NATIVE_PIXMAP");

    case IEGL10::_EGL_BAD_NATIVE_WINDOW:
        return String("EGL_BAD_NATIVE_WINDOW");

    case IEGL10::_EGL_BAD_PARAMETER:
        return String("EGL_BAD_PARAMETER");

    case IEGL10::_EGL_BAD_SURFACE:
        return String("EGL_BAD_SURFACE");

    case IEGL11::_EGL_CONTEXT_LOST:
        return String("EGL_CONTEXT_LOST");

    default:
        return GetHex(error);

    }
}

ECode EGLLogWrapper::CheckError()
{
    Int32 eglError;
    mEgl10->EglGetError(&eglError);
    if (eglError != IEGL10::_EGL_SUCCESS) {
        String errorMessage = String("eglError: ") + GetErrorString(eglError);
        LogLine(errorMessage);
        if (mCheckError) {
            //throw new GLException(eglError, errorMessage);
        }
    }
    return NOERROR;
}

ECode EGLLogWrapper::LogLine(
    /* [in] */ const String& message)
{
    Char32 c = '\n';
    Log(message + c);
    return NOERROR;
}

ECode EGLLogWrapper::Log(
    /* [in] */ const String& message)
{
    ECode ec = mLog->Write(message);
    if (FAILED(ec)) {
        // Ignore exception, keep on trying
    }
    return NOERROR;
}

ECode EGLLogWrapper::Begin(
    /* [in] */ const String& message)
{
    Char32 c = '(';
    Log(message + c);
    mArgCount = 0;
    return NOERROR;
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (mArgCount++ > 0) {
        Log(String(", "));
    }
    if (mLogArgumentNames) {
        Log(name + String("="));
    }
    Log(value);
    return NOERROR;
}

ECode EGLLogWrapper::End()
{
    Log(String(");\n"));
    Flush();
    return NOERROR;
}

ECode EGLLogWrapper::Flush()
{
    IFlushable* flush = (IFlushable*)mLog.Get();
    ECode ec = flush->Flush();
    if (FAILED(ec)) {
        mLog = NULL;
    }
    return NOERROR;
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    Arg(name, StringUtils::ToString(value));
    return NOERROR;
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ IInterface* obj)
{
    IObject* temp = IObject::Probe(obj);
    assert(temp);
    String str;
    temp->ToString(&str);
    Arg(name, str);
    return NOERROR;
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ XIEGLDisplay* obj)
{
    IInterface* src = (IInterface*)obj->Probe(EIID_IInterface);
    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
    AutoPtr<IInterface> tempDe;
    helper->GetDefaultDisplay((IInterface**)&tempDe);
    AutoPtr<XIEGLDisplay> tempNo;
    helper->GetNoDisplay((XIEGLDisplay**)&tempNo);

    if (src == tempDe) {
        Arg(name, String("EGL10.EGL_DEFAULT_DISPLAY"));
    } else if (obj == tempNo) {
        Arg(name, String("EGL10.EGL_NO_DISPLAY"));
    } else {
        IObject* temp = IObject::Probe(obj);
        assert(temp);
        String str;
        temp->ToString(&str);
        Arg(name, str);
    }
    return NOERROR;
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ XIEGLContext* obj)
{
    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);

    AutoPtr<XIEGLContext> temp;
    helper->GetNoContext((XIEGLContext**)&temp);

    if (obj == temp) {
        Arg(name, String("EGL10.EGL_NO_CONTEXT"));
    } else {
        IObject* temp = IObject::Probe(obj);
        assert(temp);
        String str;
        temp->ToString(&str);
        Arg(name, str);
    }
    return NOERROR;
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ XIEGLSurface* obj)
{
    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);

    AutoPtr<XIEGLSurface> temp;
    helper->GetNoSurface((XIEGLSurface**)&temp);

    if(obj == temp) {
        Arg(name, String("EGL10.EGL_NO_SURFACE"));
    } else {
        IObject* temp = IObject::Probe(obj);
        assert(temp);
        String str;
        temp->ToString(&str);
        Arg(name, str);
    }
    return NOERROR;
}

ECode EGLLogWrapper::Returns(
    /* [in] */ const String& result)
{
    Log(String(" returns ") + result + String(";\n"));
    Flush();
    return NOERROR;
}

ECode EGLLogWrapper::Returns(
    /* [in] */ Int32 result)
{
    Returns(StringUtils::ToString(result));
    return NOERROR;
}

ECode EGLLogWrapper::Returns(
    /* [in] */ Boolean result)
{
    Returns(StringUtils::BooleanToString(result));
    return NOERROR;
}

ECode EGLLogWrapper::Returns(
    /* [in] */ IInterface* result)
{
    IObject* temp = IObject::Probe(result);
    assert(temp);
    String str;
    temp->ToString(&str);
    Returns(str);
    return NOERROR;
}

String EGLLogWrapper::ToString(
    /* [in] */ IInterface* obj)
{
    if (!obj) {
        return String("null");
    } else {
        IObject* temp = IObject::Probe(obj);
        assert(temp);
        String str;
        temp->ToString(&str);
        return str;
    }
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Int32>* obj)
{
    if (!obj) {
        Arg(name, String("null"));
    } else {
        Int32 len = obj->GetLength();
        Arg(name, ToString(len, obj, 0));
    }
    return NOERROR;
}

ECode EGLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<IInterface*>* obj)
{
    if (!obj) {
        Arg(name, String("null"));
    } else {
        Int32 len = obj->GetLength();
        Arg(name, ToString(len, obj, 0));
    }
    return NOERROR;
}

String EGLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* obj,
    /* [in] */ Int32 offset)
{
    StringBuilder buf;
    buf.Append(String("{\n"));
    Int32 len = obj->GetLength();
    for (Int32 i = 0; i < n; i++) {
        Int32 index = offset + i;
        buf.Append(String(" ["));
        buf.Append(index);
        buf.Append(String("] = "));
        if (index < 0 || index >= len) {
            buf.Append(String("out of bounds"));
        } else {
            buf.Append((*obj)[index]);
        }
        buf.Append(String("\n"));
    }
    buf.Append(String("}"));
    String result;
    buf.ToString(&result);
    return result;
}

String EGLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<IInterface*>* obj,
    /* [in] */ Int32 offset)
{
    StringBuilder buf;
    buf.Append(String("{\n"));
    Int32 len = obj->GetLength();
    for (Int32 i = 0; i < n; i++) {
        Int32 index = offset + i;
        buf.Append(String(" ["));
        buf.Append(index);
        buf.Append(String("] = "));
        if (index < 0 || index >= len) {
            buf.Append(String("out of bounds"));
        } else {
            buf.Append((*obj)[index]);
        }
        buf.Append(String("\n"));
    }
    buf.Append(String("}"));
    String result;
    buf.ToString(&result);
    return result;
}

String EGLLogWrapper::GetHex(
    /* [in] */ Int32 value)
{
    return String("0x") + StringUtils::ToHexString(value);
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
