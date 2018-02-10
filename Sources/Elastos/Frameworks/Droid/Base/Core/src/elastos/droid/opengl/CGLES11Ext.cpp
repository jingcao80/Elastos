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

#include "elastos/droid/opengl/CGLES11Ext.h"
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <utils/misc.h>
#include <assert.h>
#include <elastos/utility/logging/Slogger.h>

#define LOGD(msg) SLOGGERD("CGLES11Ext", msg)

using Elastos::IO::CNIOAccess;
using Elastos::IO::INIOAccess;


/* special calls implemented in Android's GLES wrapper used to more
 * efficiently bound-check passed arrays */
extern "C" {
#ifdef GL_VERSION_ES_CM_1_1
GL_API void GL_APIENTRY glColorPointerBounds(GLint size, GLenum type, GLsizei stride,
        const GLvoid *ptr, GLsizei count);
GL_API void GL_APIENTRY glNormalPointerBounds(GLenum type, GLsizei stride,
        const GLvoid *pointer, GLsizei count);
GL_API void GL_APIENTRY glTexCoordPointerBounds(GLint size, GLenum type,
        GLsizei stride, const GLvoid *pointer, GLsizei count);
GL_API void GL_APIENTRY glVertexPointerBounds(GLint size, GLenum type,
        GLsizei stride, const GLvoid *pointer, GLsizei count);
GL_API void GL_APIENTRY glPointSizePointerOESBounds(GLenum type,
        GLsizei stride, const GLvoid *pointer, GLsizei count);
GL_API void GL_APIENTRY glMatrixIndexPointerOESBounds(GLint size, GLenum type,
        GLsizei stride, const GLvoid *pointer, GLsizei count);
GL_API void GL_APIENTRY glWeightPointerOESBounds(GLint size, GLenum type,
        GLsizei stride, const GLvoid *pointer, GLsizei count);
#endif
#ifdef GL_ES_VERSION_2_0
static void glVertexAttribPointerBounds(GLuint indx, GLint size, GLenum type,
        GLboolean normalized, GLsizei stride, const GLvoid *pointer, GLsizei count) {
    glVertexAttribPointer(indx, size, type, normalized, stride, pointer);
}
#endif
#ifdef GL_ES_VERSION_3_0
static void glVertexAttribIPointerBounds(GLuint indx, GLint size, GLenum type,
        GLsizei stride, const GLvoid *pointer, GLsizei count) {
    glVertexAttribIPointer(indx, size, type, stride, pointer);
}
#endif
}

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(CGLES11Ext, Singleton, IGLES11Ext)

CAR_SINGLETON_IMPL(CGLES11Ext)

AutoPtr<IBuffer> CGLES11Ext::sMatrixIndexPointerOES = NULL;

ECode CGLES11Ext::GlBlendEquationSeparateOES(
    /* [in] */ Int32 modeRGB,
    /* [in] */ Int32 modeAlpha)
{
    glBlendEquationSeparateOES(
        (GLenum)modeRGB,
        (GLenum)modeAlpha
    );
    return NOERROR;
}

ECode CGLES11Ext::GlBlendFuncSeparateOES(
    /* [in] */ Int32 srcRGB,
    /* [in] */ Int32 dstRGB,
    /* [in] */ Int32 srcAlpha,
    /* [in] */ Int32 dstAlpha)
{
    glBlendFuncSeparateOES(
        (GLenum)srcRGB,
        (GLenum)dstRGB,
        (GLenum)srcAlpha,
        (GLenum)dstAlpha
    );
    return NOERROR;
}

ECode CGLES11Ext::GlBlendEquationOES(
    /* [in] */ Int32 mode)
{
    glBlendEquationOES(
        (GLenum)mode
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexsOES(
    /* [in] */ Int16 x,
    /* [in] */ Int16 y,
    /* [in] */ Int16 z,
    /* [in] */ Int16 width,
    /* [in] */ Int16 height)
{
    glDrawTexsOES(
        (GLshort)x,
        (GLshort)y,
        (GLshort)z,
        (GLshort)width,
        (GLshort)height
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexiOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glDrawTexiOES(
        (GLint)x,
        (GLint)y,
        (GLint)z,
        (GLint)width,
        (GLint)height
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexxOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glDrawTexxOES(
        (GLfixed)x,
        (GLfixed)y,
        (GLfixed)z,
        (GLfixed)width,
        (GLfixed)height
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexsvOES(
    /* [in] */ ArrayOf<Int16>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLshort *coords_base = (GLshort *) 0;
    Int32 _remaining;
    GLshort *coords = (GLshort *) 0;

    if (!coords_ref) {
        LOGD("GlDrawTexsvOES: coords == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlDrawTexsvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        LOGD("GlDrawTexsvOES: length - offset < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLshort *) coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexsvOES(
        (GLshort *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexsvOES(
    /* [in] */ Elastos::IO::IInt16Buffer* coords_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLshort *coords = (GLshort *) 0;
    HANDLE coordsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &coordsTmp));
    coords = (GLshort *) coordsTmp;
    if (_remaining < 5) {
        LOGD("GlDrawTexsvOESEx: remaining() < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (coords == NULL) {
        char * _coordsBase = reinterpret_cast<char *>(_array);
        coords = (GLshort *) (_coordsBase + _bufferOffset);
    }
    glDrawTexsvOES(
        (GLshort *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexivOES(
    /* [in] */ ArrayOf<Int32>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLint *coords_base = (GLint *) 0;
    Int32 _remaining;
    GLint *coords = (GLint *) 0;

    if (!coords_ref) {
        LOGD("GlDrawTexivOES: coords == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlDrawTexivOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        LOGD("GlDrawTexivOES: length - offset < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLint *) coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexivOES(
        (GLint *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexivOES(
    /* [in] */ Elastos::IO::IInt32Buffer* coords_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *coords = (GLint *) 0;
    HANDLE coordsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &coordsTmp));
    coords = (GLint *) coordsTmp;
    if (_remaining < 5) {
        LOGD("GlDrawTexivOESEx: remaining() < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (coords == NULL) {
        char * _coordsBase = reinterpret_cast<char *>(_array);
        coords = (GLint *) (_coordsBase + _bufferOffset);
    }
    glDrawTexivOES(
        (GLint *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexxvOES(
    /* [in] */ ArrayOf<Int32>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *coords_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *coords = (GLfixed *) 0;

    if (!coords_ref) {
        LOGD("GlDrawTexxvOES: coords == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlDrawTexxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        LOGD("GlDrawTexxvOES: length - offset < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLfixed *) coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexxvOES(
        (GLfixed *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexxvOES(
    /* [in] */ Elastos::IO::IInt32Buffer* coords_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *coords = (GLfixed *) 0;
    HANDLE coordsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &coordsTmp));
    coords = (GLfixed *) coordsTmp;
    if (_remaining < 5) {
        LOGD("GlDrawTexxvOESEx: remaining() < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (coords == NULL) {
        char * _coordsBase = reinterpret_cast<char *>(_array);
        coords = (GLfixed *) (_coordsBase + _bufferOffset);
    }
    glDrawTexxvOES(
        (GLfixed *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexfOES(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    glDrawTexfOES(
        (GLfloat)x,
        (GLfloat)y,
        (GLfloat)z,
        (GLfloat)width,
        (GLfloat)height
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexfvOES(
    /* [in] */ ArrayOf<Float>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *coords_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *coords = (GLfloat *) 0;

    if (!coords_ref) {
        LOGD("GlDrawTexfvOES: coords == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlDrawTexfvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        LOGD("GlDrawTexfvOES: length - offset < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLfloat *) coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexfvOES(
        (GLfloat *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDrawTexfvOES(
    /* [in] */ Elastos::IO::IFloatBuffer* coords_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *coords = (GLfloat *) 0;
    HANDLE coordsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &coordsTmp));
    coords = (GLfloat *) coordsTmp;
    if (_remaining < 5) {
        LOGD("GlDrawTexfvOESEx: remaining() < 5 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (coords == NULL) {
        char * _coordsBase = reinterpret_cast<char *>(_array);
        coords = (GLfloat *) (_coordsBase + _bufferOffset);
    }
    glDrawTexfvOES(
        (GLfloat *)coords
    );
    return NOERROR;
}

ECode CGLES11Ext::GlEGLImageTargetTexture2DOES(
    /* [in] */ Int32 target,
    /* [in] */ Elastos::IO::IBuffer* image_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLeglImageOES image = (GLeglImageOES) 0;
    HANDLE imageTmp;

    FAIL_RETURN(GetPointer(image_buf, &_array, &_remaining, &_bufferOffset, &imageTmp));
    image = (GLeglImageOES) imageTmp;
    if (image == NULL) {
        char * _imageBase = reinterpret_cast<char *>(_array);
        image = (GLeglImageOES) (_imageBase + _bufferOffset);
    }
    glEGLImageTargetTexture2DOES(
        (GLenum)target,
        (GLeglImageOES)image
    );
    return NOERROR;
}

ECode CGLES11Ext::GlEGLImageTargetRenderbufferStorageOES(
    /* [in] */ Int32 target,
    /* [in] */ Elastos::IO::IBuffer* image_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLeglImageOES image = (GLeglImageOES) 0;
    HANDLE imageTmp;

    FAIL_RETURN(GetPointer(image_buf, &_array, &_remaining, &_bufferOffset, &imageTmp));
    image = (GLeglImageOES) imageTmp;
    if (image == NULL) {
        char * _imageBase = reinterpret_cast<char *>(_array);
        image = (GLeglImageOES) (_imageBase + _bufferOffset);
    }
    glEGLImageTargetRenderbufferStorageOES(
        (GLenum)target,
        (GLeglImageOES)image
    );
    return NOERROR;
}

ECode CGLES11Ext::GlAlphaFuncxOES(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref)
{
    glAlphaFuncxOES(
        (GLenum)func,
        (GLclampx)ref
    );
    return NOERROR;
}

ECode CGLES11Ext::GlClearColorxOES(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    glClearColorxOES(
        (GLclampx)red,
        (GLclampx)green,
        (GLclampx)blue,
        (GLclampx)alpha
    );
    return NOERROR;
}

ECode CGLES11Ext::GlClearDepthxOES(
    /* [in] */ Int32 depth)
{
    glClearDepthxOES(
        (GLclampx)depth
    );
    return NOERROR;
}

ECode CGLES11Ext::GlClipPlanexOES(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Int32>* equation_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *equation_base = (GLfixed *) 0;
    GLfixed *equation = (GLfixed *) 0;

    if (!equation_ref) {
        LOGD("GlClipPlanexOES: equation == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlClipPlanexOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    equation_base = (GLfixed *) equation_ref->GetPayload();
    equation = equation_base + offset;

    glClipPlanexOES(
        (GLenum)plane,
        (GLfixed *)equation
    );
    return NOERROR;
}

ECode CGLES11Ext::GlClipPlanexOES(
    /* [in] */ Int32 plane,
    /* [in] */ Elastos::IO::IInt32Buffer* equation_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *equation = (GLfixed *) 0;
    HANDLE equationTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(equation_buf), &_array, &_remaining, &_bufferOffset, &equationTmp));
    equation = (GLfixed *) equationTmp;
    if (equation == NULL) {
        char * _equationBase = reinterpret_cast<char *>(_array);
        equation = (GLfixed *) (_equationBase + _bufferOffset);
    }
    glClipPlanexOES(
        (GLenum)plane,
        (GLfixed *)equation
    );
    return NOERROR;
}

ECode CGLES11Ext::GlColor4xOES(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    glColor4xOES(
        (GLfixed)red,
        (GLfixed)green,
        (GLfixed)blue,
        (GLfixed)alpha
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDepthRangexOES(
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    glDepthRangexOES(
        (GLclampx)zNear,
        (GLclampx)zFar
    );
    return NOERROR;
}

ECode CGLES11Ext::GlFogxOES(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glFogxOES(
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlFogxvOES(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlFogxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlFogxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glFogxvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlFogxvOES(
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glFogxvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlFrustumxOES(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    glFrustumxOES(
        (GLfixed)left,
        (GLfixed)right,
        (GLfixed)bottom,
        (GLfixed)top,
        (GLfixed)zNear,
        (GLfixed)zFar
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetClipPlanexOES(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* eqn_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *eqn_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *eqn = (GLfixed *) 0;

    if (!eqn_ref) {
        LOGD("GlGetClipPlanexOES: eqn == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetClipPlanexOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = eqn_ref->GetLength() - offset;
    if (_remaining < 4) {
        LOGD("GlGetClipPlanexOES: length - offset < 4 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    eqn_base = (GLfixed *) eqn_ref->GetPayload();
    eqn = eqn_base + offset;

    glGetClipPlanexOES(
        (GLenum)pname,
        (GLfixed *)eqn
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetClipPlanexOES(
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* eqn_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *eqn = (GLfixed *) 0;
    HANDLE eqnTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(eqn_buf), &_array, &_remaining, &_bufferOffset, &eqnTmp));
    eqn = (GLfixed *) eqnTmp;
    if (_remaining < 4) {
        LOGD("GlGetClipPlanexOESEx: remaining() < 4 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (eqn == NULL) {
        char * _eqnBase = reinterpret_cast<char *>(_array);
        eqn = (GLfixed *) (_eqnBase + _bufferOffset);
    }
    glGetClipPlanexOES(
        (GLenum)pname,
        (GLfixed *)eqn
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetFixedvOES(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlGetFixedvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetFixedvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glGetFixedvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetFixedvOES(
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetFixedvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetLightxvOES(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlGetLightxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetLightxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glGetLightxvOES(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetLightxvOES(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetLightxvOES(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetMaterialxvOES(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlGetMaterialxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetMaterialxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glGetMaterialxvOES(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );

    return NOERROR;
}

ECode CGLES11Ext::GlGetMaterialxvOES(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetMaterialxvOES(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexEnvxvOES(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlGetTexEnvxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetTexEnvxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glGetTexEnvxvOES(
        (GLenum)env,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexEnvxvOES(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetTexEnvxvOES(
        (GLenum)env,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexParameterxvOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlGetTexParameterxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetTexParameterxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glGetTexParameterxvOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexParameterxvOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetTexParameterxvOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLightModelxOES(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glLightModelxOES(
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLightModelxvOES(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlLightModelxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlLightModelxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glLightModelxvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLightModelxvOES(
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glLightModelxvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLightxOES(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glLightxOES(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLightxvOES(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlLightxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlLightxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glLightxvOES(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLightxvOES(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;

    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glLightxvOES(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLineWidthxOES(
    /* [in] */ Int32 width)
{
    glLineWidthxOES(
        (GLfixed)width
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLoadMatrixxOES(
    /* [in] */ ArrayOf<Int32>* m_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *m_base = (GLfixed *) 0;
    GLfixed *m = (GLfixed *) 0;

    if (!m_ref) {
        LOGD("GlLoadMatrixxOES: m == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlLoadMatrixxOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    m_base = (GLfixed *) m_ref->GetPayload();
    m = m_base + offset;

    glLoadMatrixxOES(
        (GLfixed *)m
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLoadMatrixxOES(
    /* [in] */ Elastos::IO::IInt32Buffer* m_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *m = (GLfixed *) 0;
    HANDLE mTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(m_buf), &_array, &_remaining, &_bufferOffset, &mTmp));
    m = (GLfixed *) mTmp;
    if (m == NULL) {
        char * _mBase = reinterpret_cast<char *>(_array);
        m = (GLfixed *) (_mBase + _bufferOffset);
    }
    glLoadMatrixxOES(
        (GLfixed *)m
    );
    return NOERROR;
}

ECode CGLES11Ext::GlMaterialxOES(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glMaterialxOES(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlMaterialxvOES(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlMaterialxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlMaterialxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glMaterialxvOES(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlMaterialxvOES(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glMaterialxvOES(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlMultMatrixxOES(
    /* [in] */ ArrayOf<Int32>* m_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *m_base = (GLfixed *) 0;
    GLfixed *m = (GLfixed *) 0;

    if (!m_ref) {
        LOGD("GlMultMatrixxOES: m == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlMultMatrixxOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    m_base = (GLfixed *) m_ref->GetPayload();
    m = m_base + offset;

    glMultMatrixxOES(
        (GLfixed *)m
    );
    return NOERROR;
}

ECode CGLES11Ext::GlMultMatrixxOES(
    /* [in] */ Elastos::IO::IInt32Buffer* m_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *m = (GLfixed *) 0;
    HANDLE mTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(m_buf), &_array, &_remaining, &_bufferOffset, &mTmp));
    m = (GLfixed *) mTmp;
    if (m == NULL) {
        char * _mBase = reinterpret_cast<char *>(_array);
        m = (GLfixed *) (_mBase + _bufferOffset);
    }
    glMultMatrixxOES(
        (GLfixed *)m
    );
    return NOERROR;
}

ECode CGLES11Ext::GlMultiTexCoord4xOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 s,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 q)
{
    glMultiTexCoord4xOES(
        (GLenum)target,
        (GLfixed)s,
        (GLfixed)t,
        (GLfixed)r,
        (GLfixed)q
    );
    return NOERROR;
}

ECode CGLES11Ext::GlNormal3xOES(
    /* [in] */ Int32 nx,
    /* [in] */ Int32 ny,
    /* [in] */ Int32 nz)
{
    glNormal3xOES(
        (GLfixed)nx,
        (GLfixed)ny,
        (GLfixed)nz
    );
    return NOERROR;
}

ECode CGLES11Ext::GlOrthoxOES(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    glOrthoxOES(
        (GLfixed)left,
        (GLfixed)right,
        (GLfixed)bottom,
        (GLfixed)top,
        (GLfixed)zNear,
        (GLfixed)zFar
    );
    return NOERROR;
}

ECode CGLES11Ext::GlPointParameterxOES(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glPointParameterxOES(
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlPointParameterxvOES(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlPointParameterxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlPointParameterxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glPointParameterxvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlPointParameterxvOES(
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glPointParameterxvOES(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlPointSizexOES(
    /* [in] */ Int32 size)
{
    glPointSizexOES(
        (GLfixed)size
    );
    return NOERROR;
}

ECode CGLES11Ext::GlPolygonOffsetxOES(
    /* [in] */ Int32 factor,
    /* [in] */ Int32 units)
{
    glPolygonOffsetxOES(
        (GLfixed)factor,
        (GLfixed)units
    );
    return NOERROR;
}

ECode CGLES11Ext::GlRotatexOES(
    /* [in] */ Int32 angle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    glRotatexOES(
        (GLfixed)angle,
        (GLfixed)x,
        (GLfixed)y,
        (GLfixed)z
    );
    return NOERROR;
}

ECode CGLES11Ext::GlSampleCoveragexOES(
    /* [in] */ Int32 value,
    /* [in] */ Boolean invert)
{
    glSampleCoveragexOES(
        (GLclampx)value,
        (GLboolean)invert
    );
    return NOERROR;
}

ECode CGLES11Ext::GlScalexOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    glScalexOES(
        (GLfixed)x,
        (GLfixed)y,
        (GLfixed)z
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexEnvxOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexEnvxOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexEnvxvOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlTexEnvxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlTexEnvxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glTexEnvxvOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexEnvxvOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glTexEnvxvOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexParameterxOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexParameterxOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexParameterxvOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlTexParameterxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlTexParameterxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glTexParameterxvOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexParameterxvOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glTexParameterxvOES(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTranslatexOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    glTranslatexOES(
        (GLfixed)x,
        (GLfixed)y,
        (GLfixed)z
    );
    return NOERROR;
}

ECode CGLES11Ext::GlIsRenderbufferOES(
    /* [in] */ Int32 renderbuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsRenderbufferOES(
        (GLuint)renderbuffer
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CGLES11Ext::GlBindRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 renderbuffer)
{
    glBindRenderbufferOES(
        (GLenum)target,
        (GLuint)renderbuffer
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers_ref,
    /* [in] */ Int32 offset)
{
    GLuint *renderbuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;

    if (!renderbuffers_ref) {
        LOGD("GlDeleteRenderbuffersOES: renderbuffers == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlDeleteRenderbuffersOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = renderbuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        LOGD("GlDeleteRenderbuffersOES: length - offset < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    renderbuffers_base = (GLuint *) renderbuffers_ref->GetPayload();
    renderbuffers = renderbuffers_base + offset;

    glDeleteRenderbuffersOES(
        (GLsizei)n,
        (GLuint *)renderbuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ Elastos::IO::IInt32Buffer* renderbuffers_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;
    HANDLE renderbuffersTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(renderbuffers_buf), &_array, &_remaining, &_bufferOffset, &renderbuffersTmp));
    renderbuffers = (GLuint *) renderbuffersTmp;
    if (_remaining < n) {
        LOGD("GlDeleteRenderbuffersOESEx: remaining() < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (renderbuffers == NULL) {
        char * _renderbuffersBase = reinterpret_cast<char *>(_array);
        renderbuffers = (GLuint *) (_renderbuffersBase + _bufferOffset);
    }
    glDeleteRenderbuffersOES(
        (GLsizei)n,
        (GLuint *)renderbuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers_ref,
    /* [in] */ Int32 offset)
{
    GLuint *renderbuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;

    if (!renderbuffers_ref) {
        LOGD("GlGenRenderbuffersOES: renderbuffers == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGenRenderbuffersOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = renderbuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        LOGD("GlGenRenderbuffersOES: length - offset < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    renderbuffers_base = (GLuint *) renderbuffers_ref->GetPayload();
    renderbuffers = renderbuffers_base + offset;

    glGenRenderbuffersOES(
        (GLsizei)n,
        (GLuint *)renderbuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ Elastos::IO::IInt32Buffer* renderbuffers_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;
    HANDLE renderbuffersTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(renderbuffers_buf), &_array, &_remaining, &_bufferOffset, &renderbuffersTmp));
    renderbuffers = (GLuint *) renderbuffersTmp;
    if (_remaining < n) {
        LOGD("GlGenRenderbuffersOESEx: remaining() < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (renderbuffers == NULL) {
        char * _renderbuffersBase = reinterpret_cast<char *>(_array);
        renderbuffers = (GLuint *) (_renderbuffersBase + _bufferOffset);
    }
    glGenRenderbuffersOES(
        (GLsizei)n,
        (GLuint *)renderbuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlRenderbufferStorageOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glRenderbufferStorageOES(
        (GLenum)target,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        LOGD("GlGetRenderbufferParameterivOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetRenderbufferParameterivOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        LOGD("GlGetRenderbufferParameterivOES: length - offset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *) params_ref->GetPayload();
    params = params_base + offset;

    glGetRenderbufferParameterivOES(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLint *) paramsTmp;
    if (_remaining < 1) {
        LOGD("GlGetRenderbufferParameterivOESEx: remaining() < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetRenderbufferParameterivOES(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlIsFramebufferOES(
    /* [in] */ Int32 framebuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsFramebufferOES(
        (GLuint)framebuffer
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CGLES11Ext::GlBindFramebufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 framebuffer)
{
    glBindFramebufferOES(
        (GLenum)target,
        (GLuint)framebuffer
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers_ref,
    /* [in] */ Int32 offset)
{
    GLuint *framebuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;

    if (!framebuffers_ref) {
        LOGD("GlDeleteFramebuffersOES: framebuffers == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlDeleteFramebuffersOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = framebuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        LOGD("GlDeleteFramebuffersOES: length - offset < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    framebuffers_base = (GLuint *) framebuffers_ref->GetPayload();
    framebuffers = framebuffers_base + offset;

    glDeleteFramebuffersOES(
        (GLsizei)n,
        (GLuint *)framebuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ Elastos::IO::IInt32Buffer* framebuffers_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;
    HANDLE framebuffersTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(framebuffers_buf), &_array, &_remaining, &_bufferOffset, &framebuffersTmp));
    framebuffers = (GLuint *) framebuffersTmp;

    if (_remaining < n) {
        LOGD("GlDeleteFramebuffersOESEx: remaining() < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (framebuffers == NULL) {
        char * _framebuffersBase = reinterpret_cast<char *>(_array);
        framebuffers = (GLuint *) (_framebuffersBase + _bufferOffset);
    }
    glDeleteFramebuffersOES(
        (GLsizei)n,
        (GLuint *)framebuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers_ref,
    /* [in] */ Int32 offset)
{
    GLuint *framebuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;

    if (!framebuffers_ref) {
        LOGD("GlGenFramebuffersOES: framebuffers == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGenFramebuffersOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = framebuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        LOGD("GlGenFramebuffersOES: length - offset < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    framebuffers_base = (GLuint *) framebuffers_ref->GetPayload();
    framebuffers = framebuffers_base + offset;

    glGenFramebuffersOES(
        (GLsizei)n,
        (GLuint *)framebuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ Elastos::IO::IInt32Buffer* framebuffers_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;
    HANDLE framebuffersTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(framebuffers_buf), &_array, &_remaining, &_bufferOffset, &framebuffersTmp));
    framebuffers = (GLuint *) framebuffersTmp;
    if (_remaining < n) {
        LOGD("GlGenFramebuffersOESEx: remaining() < n < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (framebuffers == NULL) {
        char * _framebuffersBase = reinterpret_cast<char *>(_array);
        framebuffers = (GLuint *) (_framebuffersBase + _bufferOffset);
    }
    glGenFramebuffersOES(
        (GLsizei)n,
        (GLuint *)framebuffers
    );
    return NOERROR;
}

ECode CGLES11Ext::GlCheckFramebufferStatusOES(
    /* [in] */ Int32 target,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)

    GLenum _returnValue;
    _returnValue = glCheckFramebufferStatusOES(
        (GLenum)target
    );
    *status = _returnValue;
    return NOERROR;
}

ECode CGLES11Ext::GlFramebufferRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 renderbuffertarget,
    /* [in] */ Int32 renderbuffer)
{
    glFramebufferRenderbufferOES(
        (GLenum)target,
        (GLenum)attachment,
        (GLenum)renderbuffertarget,
        (GLuint)renderbuffer
    );
    return NOERROR;
}

ECode CGLES11Ext::GlFramebufferTexture2DOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 textarget,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level)
{
    glFramebufferTexture2DOES(
        (GLenum)target,
        (GLenum)attachment,
        (GLenum)textarget,
        (GLuint)texture,
        (GLint)level
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        LOGD("GlGetFramebufferAttachmentParameterivOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetFramebufferAttachmentParameterivOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        LOGD("GlGetFramebufferAttachmentParameterivOES: length - offset < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *) params_ref->GetPayload();
    params = params_base + offset;

    glGetFramebufferAttachmentParameterivOES(
        (GLenum)target,
        (GLenum)attachment,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLint *) paramsTmp;
    if (_remaining < 1) {
        LOGD("GlGetFramebufferAttachmentParameterivOESEx: remaining() < 1 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetFramebufferAttachmentParameterivOES(
        (GLenum)target,
        (GLenum)attachment,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGenerateMipmapOES(
    /* [in] */ Int32 target)
{
    glGenerateMipmapOES(
        (GLenum)target
    );
    return NOERROR;
}

ECode CGLES11Ext::GlCurrentPaletteMatrixOES(
    /* [in] */ Int32 matrixpaletteindex)
{
    glCurrentPaletteMatrixOES(
        (GLuint)matrixpaletteindex
    );
    return NOERROR;
}

ECode CGLES11Ext::GlLoadPaletteFromModelViewMatrixOES()
{
    glLoadPaletteFromModelViewMatrixOES();
    return NOERROR;
}

ECode CGLES11Ext::GlMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Elastos::IO::IBuffer* pointer)
{
    Int32 remaining;
    pointer->GetRemaining(&remaining);
    ECode ec = GlMatrixIndexPointerOESBounds(
        size,
        type,
        stride,
        pointer,
        remaining
    );
    if (((size == 2) ||
         (size == 3) ||
         (size == 4)) &&
        ((type == GL_FLOAT) ||
         (type == GL_BYTE) ||
         (type == GL_SHORT) ||
         (type == GL_FIXED)) &&
        (stride >= 0)) {
        sMatrixIndexPointerOES = pointer;
    }
    return ec;
}

ECode CGLES11Ext::GlWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Elastos::IO::IBuffer* pointer)
{
    Int32 remaining;
    pointer->GetRemaining(&remaining);
    return GlWeightPointerOESBounds(
        size,
        type,
        stride,
        pointer,
        remaining
    );
}

ECode CGLES11Ext::GlDepthRangefOES(
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    glDepthRangefOES(
        (GLclampf)zNear,
        (GLclampf)zFar
    );
    return NOERROR;
}

ECode CGLES11Ext::GlFrustumfOES(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    glFrustumfOES(
        (GLfloat)left,
        (GLfloat)right,
        (GLfloat)bottom,
        (GLfloat)top,
        (GLfloat)zNear,
        (GLfloat)zFar
    );
    return NOERROR;
}

ECode CGLES11Ext::GlOrthofOES(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    glOrthofOES(
        (GLfloat)left,
        (GLfloat)right,
        (GLfloat)bottom,
        (GLfloat)top,
        (GLfloat)zNear,
        (GLfloat)zFar
    );
    return NOERROR;
}

ECode CGLES11Ext::GlClipPlanefOES(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Float>* equation_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *equation_base = (GLfloat *) 0;
    GLfloat *equation = (GLfloat *) 0;

    if (!equation_ref) {
        LOGD("GlClipPlanefOES: equation == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlClipPlanefOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    equation_base = (GLfloat *) equation_ref->GetPayload();
    equation = equation_base + offset;

    glClipPlanefOES(
        (GLenum)plane,
        (GLfloat *)equation
    );
    return NOERROR;
}

ECode CGLES11Ext::GlClipPlanefOES(
    /* [in] */ Int32 plane,
    /* [in] */ Elastos::IO::IFloatBuffer* equation_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *equation = (GLfloat *) 0;
    HANDLE equationTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(equation_buf), &_array, &_remaining, &_bufferOffset, &equationTmp));
    equation = (GLfloat *) equationTmp;
    if (equation == NULL) {
        char * _equationBase = reinterpret_cast<char *>(_array);
        equation = (GLfloat *) (_equationBase + _bufferOffset);
    }
    glClipPlanefOES(
        (GLenum)plane,
        (GLfloat *)equation
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetClipPlanefOES(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* eqn_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *eqn_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *eqn = (GLfloat *) 0;

    if (!eqn_ref) {
        LOGD("GlGetClipPlanefOES: eqn == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetClipPlanefOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = eqn_ref->GetLength() - offset;
    if (_remaining < 4) {
        LOGD("GlGetClipPlanefOES: length - offset < 4 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    eqn_base = (GLfloat *) eqn_ref->GetPayload();
    eqn = eqn_base + offset;

    glGetClipPlanefOES(
        (GLenum)pname,
        (GLfloat *)eqn
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetClipPlanefOES(
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IFloatBuffer* eqn_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *eqn = (GLfloat *) 0;
    HANDLE eqnTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(eqn_buf), &_array, &_remaining, &_bufferOffset, &eqnTmp));
    eqn = (GLfloat *) eqnTmp;
    if (_remaining < 4) {
        LOGD("GlGetClipPlanefOESEx: remaining() < 4 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (eqn == NULL) {
        char * _eqnBase = reinterpret_cast<char *>(_array);
        eqn = (GLfloat *) (_eqnBase + _bufferOffset);
    }
    glGetClipPlanefOES(
        (GLenum)pname,
        (GLfloat *)eqn
    );
    return NOERROR;
}

ECode CGLES11Ext::GlClearDepthfOES(
    /* [in] */ Float depth)
{
    glClearDepthfOES(
        (GLclampf)depth
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenfOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glTexGenfOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenfvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        LOGD("GlTexGenfvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlTexGenfvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *) params_ref->GetPayload();
    params = params_base + offset;

    glTexGenfvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenfvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IFloatBuffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfloat *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glTexGenfvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGeniOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexGeniOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLint)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenivOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        LOGD("GlTexGenivOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlTexGenivOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *) params_ref->GetPayload();
    params = params_base + offset;

    glTexGenivOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenivOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLint *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glTexGenivOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenxOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexGenxOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenxvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlTexGenxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlTexGenxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glTexGenxvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlTexGenxvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;

    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glTexGenxvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexGenfvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        LOGD("GlGetTexGenfvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetTexGenfvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *) params_ref->GetPayload();
    params = params_base + offset;

    glGetTexGenfvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexGenfvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IFloatBuffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfloat *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glGetTexGenfvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexGenivOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        LOGD("GlGetTexGenivOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetTexGenivOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *) params_ref->GetPayload();
    params = params_base + offset;

    glGetTexGenivOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexGenivOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLint *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetTexGenivOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexGenxvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        LOGD("GlGetTexGenxvOES: params == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        LOGD("GlGetTexGenxvOES: offset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glGetTexGenxvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlGetTexGenxvOES(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Elastos::IO::IInt32Buffer* params_buf)
{
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;
    HANDLE paramsTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &paramsTmp));
    params = (GLfixed *) paramsTmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetTexGenxvOES(
        (GLenum)coord,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLES11Ext::GlMatrixIndexPointerOESBounds(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Elastos::IO::IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;
    HANDLE pointerTmp;

    if (pointer_buf) {
        FAIL_RETURN(GetDirectBufferPointer(pointer_buf, &pointerTmp));
        pointer = (GLvoid *) pointerTmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glMatrixIndexPointerOESBounds(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

ECode CGLES11Ext::GlWeightPointerOESBounds(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Elastos::IO::IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;
    HANDLE pointerTmp;

    if (pointer_buf) {
        FAIL_RETURN(GetDirectBufferPointer(pointer_buf, &pointerTmp));
        pointer = (GLvoid *) pointerTmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glWeightPointerOESBounds(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

ECode CGLES11Ext::GetPointer(
    /* [in] */ IBuffer* buffer,
    /* [in, out] */ HANDLE* array,
    /* [in, out] */ Int32* remaining,
    /* [in, out] */ Int32* offset,
    /* [out] */ HANDLE* rst)
{
    VALIDATE_NOT_NULL(rst)

    Int32 position;
    Int32 limit;
    Int32 elementSizeShift;
    Int64 pointer;
    buffer->GetPosition(&position);
    buffer->GetLimit(&limit);
    buffer->GetElementSizeShift(&elementSizeShift);
    *remaining = (limit - position) << elementSizeShift;

    AutoPtr<INIOAccess> helper;
    CNIOAccess::AcquireSingleton((INIOAccess**)&helper);

    helper->GetBasePointer(buffer, &pointer);
    if (pointer != 0L) {
        *array = 0;
        *rst = (HANDLE)pointer;
        return NOERROR;
    }

    Boolean hasArray;
    if (buffer->HasArray(&hasArray), hasArray) {
        buffer->GetPrimitiveArray(array);
    } else {
        *array = 0;
    }
    helper->GetBaseArrayOffset(buffer, offset);

    *rst = 0;
    return NOERROR;
}

ECode CGLES11Ext::GetDirectBufferPointer(
    /* [in] */ IBuffer* buffer,
    /* [out] */ HANDLE* result)
{
    VALIDATE_NOT_NULL(result)

    HANDLE effectiveDirectAddress;
    buffer->GetEffectiveDirectAddress(&effectiveDirectAddress);
    if (effectiveDirectAddress != 0) {
        Int32 position, elementSizeShift;
        buffer->GetPosition(&position);
        buffer->GetElementSizeShift(&elementSizeShift);
        effectiveDirectAddress += position << elementSizeShift;
    } else {
        LOGD("GetDirectBufferPointer: Must use a native order direct Buffer")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = effectiveDirectAddress;
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

