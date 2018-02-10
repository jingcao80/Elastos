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

#include "Elastos.Droid.View.h"
#include "elastos/droid/opengl/GLES30.h"

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <utils/misc.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

using Elastos::IO::CNIOAccess;
using Elastos::IO::INIOAccess;
using Elastos::IO::CDirectByteBufferHelper;
using Elastos::IO::IDirectByteBufferHelper;
using Elastos::IO::IDirectByteBuffer;

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

static ECode GetPointer(
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
        *rst = (HANDLE)(pointer);
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

static ECode GetDirectBufferPointer(
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
        effectiveDirectAddress += (position << elementSizeShift);
    } else {
        SLOGGERD("GLES30", "GetDirectBufferPointer: Must use a native order direct Buffer")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = effectiveDirectAddress;
    return NOERROR;
}

// --------------------------------------------------------------------------

/*
 * returns the number of values glGet returns for a given pname.
 *
 * The code below is written such that pnames requiring only one values
 * are the default (and are not explicitely tested for). This makes the
 * checking code much shorter/readable/efficient.
 *
 * This means that unknown pnames (e.g.: extensions) will default to 1. If
 * that unknown pname needs more than 1 value, then the validation check
 * is incomplete and the app may crash if it passed the wrong number params.
 */
static int getNeededCount(GLint pname) {
    int needed = 1;
#ifdef GL_ES_VERSION_2_0
    // GLES 2.x pnames
    switch (pname) {
        case GL_ALIASED_LINE_WIDTH_RANGE:
        case GL_ALIASED_POINT_SIZE_RANGE:
            needed = 2;
            break;

        case GL_BLEND_COLOR:
        case GL_COLOR_CLEAR_VALUE:
        case GL_COLOR_WRITEMASK:
        case GL_SCISSOR_BOX:
        case GL_VIEWPORT:
            needed = 4;
            break;

        case GL_COMPRESSED_TEXTURE_FORMATS:
            glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &needed);
            break;

        case GL_SHADER_BINARY_FORMATS:
            glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &needed);
            break;
    }
#endif

#ifdef GL_VERSION_ES_CM_1_1
    // GLES 1.x pnames
    switch (pname) {
        case GL_ALIASED_LINE_WIDTH_RANGE:
        case GL_ALIASED_POINT_SIZE_RANGE:
        case GL_DEPTH_RANGE:
        case GL_SMOOTH_LINE_WIDTH_RANGE:
        case GL_SMOOTH_POINT_SIZE_RANGE:
            needed = 2;
            break;

        case GL_CURRENT_NORMAL:
        case GL_POINT_DISTANCE_ATTENUATION:
            needed = 3;
            break;

        case GL_COLOR_CLEAR_VALUE:
        case GL_COLOR_WRITEMASK:
        case GL_CURRENT_COLOR:
        case GL_CURRENT_TEXTURE_COORDS:
        case GL_FOG_COLOR:
        case GL_LIGHT_MODEL_AMBIENT:
        case GL_SCISSOR_BOX:
        case GL_VIEWPORT:
            needed = 4;
            break;

        case GL_MODELVIEW_MATRIX:
        case GL_PROJECTION_MATRIX:
        case GL_TEXTURE_MATRIX:
            needed = 16;
            break;

        case GL_COMPRESSED_TEXTURE_FORMATS:
            glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &needed);
            break;
    }
#endif
    return needed;
}

template <typename ATYPE, typename CTYPE, void GET(GLenum, CTYPE*)>
static ECode
get
  (Int32 pname, ArrayOf<ATYPE>* params_ref, Int32 offset) {
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage;
    CTYPE *params_base = (CTYPE *) 0;
    Int32 _remaining;
    CTYPE *params = (CTYPE *) 0;
    Int32 _needed = 0;

    if (!params_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "params == null";
        goto exit;
    }
    if (offset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "offset < 0";
        goto exit;
    }
    _remaining = params_ref->GetLength() - offset;
    _needed = getNeededCount(pname);
    // if we didn't find this pname, we just assume the user passed
    // an array of the right size -- this might happen with extensions
    // or if we forget an enum here.
    if (_remaining < _needed) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "length - offset < needed";
        goto exit;
    }
    params_base = (CTYPE *)(params_ref->GetPayload());
    params = params_base + offset;

    GET(
        (GLenum)pname,
        (CTYPE *)params
    );

exit:
    if (_exception) {
        SLOGGERD("CGLES30", _exceptionMessage)
    }
    return _exceptionType;
}


template <typename CTYPE, void GET(GLenum, CTYPE*)>
static ECode
getarray
  (Int32 pname, IBuffer* params_buf) {
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage;
    HANDLE _array = (HANDLE) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    HANDLE data;
    CTYPE *params = (CTYPE *) 0;
    Int32 _needed = 0;

    if (params_buf)
    {
        FAIL_RETURN(GetPointer(params_buf, &_array, &_remaining, &_bufferOffset, &data))
        params = (CTYPE*)data;
    }
    _remaining /= sizeof(CTYPE);    // convert from bytes to item count
    _needed = getNeededCount(pname);
    // if we didn't find this pname, we just assume the user passed
    // an array of the right size -- this might happen with extensions
    // or if we forget an enum here.
    if (_needed>0 && _remaining < _needed) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "remaining() < needed";
        goto exit;
    }

    if (params_buf && params == NULL) {
        char * _pBase = reinterpret_cast<char *>(_array);
        params = (CTYPE *) (_pBase + _bufferOffset);
    }

    GET(
        (GLenum)pname,
        (CTYPE *)params
    );

exit:
    if (_exception) {
        SLOGGERD("CGLES30", _exceptionMessage)
    }
    return _exceptionType;
}

#define GET_BUFFER_GL_DATA(_Name, GLTYPE)                                                                                 \
    HANDLE _array, _dataHandle;                                                                                                      \
    Int32 _bufferOffset = (Int32) 0;                                                                                                      \
    Int32 _remaining;                                                                                                                           \
    GLTYPE *_Name = (GLTYPE *) 0;                                                                                               \
    FAIL_RETURN(GetPointer(IBuffer::Probe(_Name##_buf), &_array, &_remaining, &_bufferOffset, &_dataHandle))      \
    _Name = (GLTYPE *)_dataHandle;                                                                                            \
    if (_Name##_buf && _Name == NULL) {                                                                                        \
        char * _dataBase = reinterpret_cast<char *>(_array);                                                                \
        _Name = (GLTYPE *) (_dataBase + _bufferOffset);                                                              \
    }


#define GET_REF_GL_DATA_BEGIN(_Name, GLTYPE)                                                             \
    Int32 _exception = 0;                                                             \
    ECode _exceptionType = NOERROR;                                                             \
    const char * _exceptionMessage = NULL;                                                             \
    GLTYPE *_Name##_base = (GLTYPE *) 0;                                                             \
    GLTYPE *_Name = (GLTYPE *) 0;                                                             \
    if (!_Name##_ref) {                                                             \
        _exception = 1;                                                             \
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;                                                             \
        _exceptionMessage = "data == null";                                                             \
        goto exit;                                                             \
    }                                                             \
    if (offset < 0) {                                                             \
        _exception = 1;                                                             \
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;                                                             \
        _exceptionMessage = "offset < 0";                                                             \
        goto exit;                                                             \
    }                                                             \
    _Name##_base = (GLTYPE *) _Name##_ref->GetPayload();                                                             \
    _Name = _Name##_base + offset;

#define GL_EXIT()                                                             \
exit:                                                             \
    if (_exception) {                                                             \
        SLOGGERD("CGLES10", _exceptionMessage)                                                             \
        return _exceptionType;                                                             \
    }                                                             \
    return _exceptionType;

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(GLES30, GLES20, IGLES30)

ECode GLES30::GlReadBuffer(
    /* [in] */ Int32 mode)
{
    glReadBuffer(
        (GLenum)mode
    );
    return NOERROR;
}

// C function void glDrawRangeElements ( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices )

ECode GLES30::GlDrawRangeElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* indices_buf)
{
    GET_BUFFER_GL_DATA(indices, GLvoid)
    glDrawRangeElements(
        (GLenum)mode,
        (GLuint)start,
        (GLuint)end,
        (GLsizei)count,
        (GLenum)type,
        (GLvoid *)indices
    );
    return NOERROR;
}

// C function void glDrawRangeElements ( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizei offset )

ECode GLES30::GlDrawRangeElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset)
{
    glDrawRangeElements(
        (GLenum)mode,
        (GLuint)start,
        (GLuint)end,
        (GLsizei)count,
        (GLenum)type,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

// C function void glTexImage3D ( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels )

ECode GLES30::GlTexImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 border,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels_buf)
{
    GET_BUFFER_GL_DATA(pixels, GLvoid)
    glTexImage3D(
        (GLenum)target,
        (GLint)level,
        (GLint)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLint)border,
        (GLenum)format,
        (GLenum)type,
        (GLvoid *)pixels
    );

    return NOERROR;
}

// C function void glTexImage3D ( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLsizei offset )

ECode GLES30::GlTexImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 border,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset)
{
    glTexImage3D(
        (GLenum)target,
        (GLint)level,
        (GLint)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLint)border,
        (GLenum)format,
        (GLenum)type,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

// C function void glTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels )

ECode GLES30::GlTexSubImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 zoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels_buf)
{
    GET_BUFFER_GL_DATA(pixels, GLvoid)
    glTexSubImage3D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLint)zoffset,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLenum)format,
        (GLenum)type,
        (GLvoid *)pixels
    );

    return NOERROR;
}

// C function void glTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei offset )

ECode GLES30::GlTexSubImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 zoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset)
{
    glTexSubImage3D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLint)zoffset,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLenum)format,
        (GLenum)type,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

// C function void glCopyTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height )

ECode GLES30::GlCopyTexSubImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 zoffset,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glCopyTexSubImage3D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLint)zoffset,
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

// C function void glCompressedTexImage3D ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data )

ECode GLES30::GlCompressedTexImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 border,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data_buf)
{
    GET_BUFFER_GL_DATA(data, GLvoid)
    glCompressedTexImage3D(
        (GLenum)target,
        (GLint)level,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLint)border,
        (GLsizei)imageSize,
        (GLvoid *)data
    );
    return NOERROR;
}

// C function void glCompressedTexImage3D ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLsizei offset )

ECode GLES30::GlCompressedTexImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 border,
    /* [in] */ Int32 imageSize,
    /* [in] */ Int32 offset)
{
    glCompressedTexImage3D(
        (GLenum)target,
        (GLint)level,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLint)border,
        (GLsizei)imageSize,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

// C function void glCompressedTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data )

ECode GLES30::GlCompressedTexSubImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 zoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 format,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data_buf)
{
    GET_BUFFER_GL_DATA(data, GLvoid)
    glCompressedTexSubImage3D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLint)zoffset,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLenum)format,
        (GLsizei)imageSize,
        (GLvoid *)data
    );
    return NOERROR;
}

// C function void glCompressedTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLsizei offset )

ECode GLES30::GlCompressedTexSubImage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 zoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int32 format,
    /* [in] */ Int32 imageSize,
    /* [in] */ Int32 offset)
{
    glCompressedTexSubImage3D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLint)zoffset,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLenum)format,
        (GLsizei)imageSize,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

// C function void glGenQueries ( GLsizei n, GLuint *ids )

ECode GLES30::GlGenQueries(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* ids_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(ids, GLuint)
    glGenQueries(
        (GLsizei)n,
        (GLuint *)ids
    );
    GL_EXIT()
}

// C function void glGenQueries ( GLsizei n, GLuint *ids )

ECode GLES30::GlGenQueries(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* ids_buf)
{
    GET_BUFFER_GL_DATA(ids, GLuint)
    glGenQueries(
        (GLsizei)n,
        (GLuint *)ids
    );
    return NOERROR;
}

// C function void glDeleteQueries ( GLsizei n, const GLuint *ids )

ECode GLES30::GlDeleteQueries(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* ids_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(ids, GLuint)
    glDeleteQueries(
        (GLsizei)n,
        (GLuint *)ids
    );
    GL_EXIT()
}

// C function void glDeleteQueries ( GLsizei n, const GLuint *ids )

ECode GLES30::GlDeleteQueries(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* ids_buf)
{
    GET_BUFFER_GL_DATA(ids, GLuint)
    glDeleteQueries(
        (GLsizei)n,
        (GLuint *)ids
    );
    return NOERROR;
}

// C function GLboolean glIsQuery ( GLuint id
ECode GLES30::GlIsQuery(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsQuery(
        (GLuint)id
    );
    *result = _returnValue;
    return NOERROR;
}

// C function void glBeginQuery ( GLenum target, GLuint id )

ECode GLES30::GlBeginQuery(
    /* [in] */ Int32 target,
    /* [in] */ Int32 id)
{
    glBeginQuery(
        (GLenum)target,
        (GLuint)id
    );
    return NOERROR;
}

// C function void glEndQuery ( GLenum target )

ECode GLES30::GlEndQuery(
    /* [in] */ Int32 target)
{
    glEndQuery(
        (GLenum)target
    );
    return NOERROR;
}

// C function void glGetQueryiv ( GLenum target, GLenum pname, GLint *params )

ECode GLES30::GlGetQueryiv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLint)
    glGetQueryiv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetQueryiv ( GLenum target, GLenum pname, GLint *params )

ECode GLES30::GlGetQueryiv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetQueryiv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetQueryObjectuiv ( GLuint id, GLenum pname, GLuint *params )

ECode GLES30::GlGetQueryObjectuiv(
    /* [in] */ Int32 id,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLuint)
    glGetQueryObjectuiv(
        (GLuint)id,
        (GLenum)pname,
        (GLuint *)params
    );
    GL_EXIT()
}

// C function void glGetQueryObjectuiv ( GLuint id, GLenum pname, GLuint *params )

ECode GLES30::GlGetQueryObjectuiv(
    /* [in] */ Int32 id,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLuint)
    glGetQueryObjectuiv(
        (GLuint)id,
        (GLenum)pname,
        (GLuint *)params
    );
    return NOERROR;
}

// C function GLboolean glUnmapBuffer ( GLenum target )

ECode GLES30::GlUnmapBuffer(
    /* [in] */ Int32 target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glUnmapBuffer(
        (GLenum)target
    );
    *result = _returnValue;
    return NOERROR;
}

static ECode NewDirectByteBuffer(
    /* [in] */ Int64 address,
    /* [in] */ Int64 capacity,
    /* [out] */ IBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    if (capacity < 0) {
      SLOGGERD("NewDirectByteBuffer", "negative buffer capacity: %lld", capacity);
      *buffer = NULL;
      return NOERROR;
    }
    if (address == 0 && capacity != 0) {
      SLOGGERD("NewDirectByteBuffer", "non-zero capacity for nullptr pointer: %lld", capacity);
      *buffer = NULL;
      return NOERROR;
    }

    // At the moment, the capacity of DirectByteBuffer is limited to a signed int.
    if (capacity > Elastos::Core::Math::INT32_MAX_VALUE) {
      SLOGGERD("NewDirectByteBuffer", "buffer capacity greater than maximum jint: %lld", capacity);
      *buffer = NULL;
      return NOERROR;
    }
    AutoPtr<IDirectByteBufferHelper> helper;
    CDirectByteBufferHelper::AcquireSingleton((IDirectByteBufferHelper**)&helper);
    AutoPtr<IDirectByteBuffer> directBuffer;
    FAIL_RETURN(helper->GetNewInstance(address, capacity, (IDirectByteBuffer**)&directBuffer))
    *buffer = IBuffer::Probe(directBuffer);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}
// C function void glGetBufferPointerv ( GLenum target, GLenum pname, GLvoid** params )

ECode GLES30::GlGetBufferPointerv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [out] */ IBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    GLint64 _mapLength;
    GLvoid* _p;
    glGetBufferParameteri64v((GLenum)target, GL_BUFFER_MAP_LENGTH, &_mapLength);
    glGetBufferPointerv((GLenum)target, (GLenum)pname, &_p);
    return NewDirectByteBuffer(reinterpret_cast<Int64>(_p), _mapLength, buffer);
}

// C function void glDrawBuffers ( GLsizei n, const GLenum *bufs )

ECode GLES30::GlDrawBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* bufs_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(bufs, GLenum)
    glDrawBuffers(
        (GLsizei)n,
        (GLenum *)bufs
    );
    GL_EXIT()
}

// C function void glDrawBuffers ( GLsizei n, const GLenum *bufs )

ECode GLES30::GlDrawBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* bufs_buf)
{
    GET_BUFFER_GL_DATA(bufs, GLenum)
    glDrawBuffers(
        (GLsizei)n,
        (GLenum *)bufs
    );
    return NOERROR;
}

// C function void glUniformMatrix2x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix2x3fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLfloat)
    glUniformMatrix2x3fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glUniformMatrix2x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix2x3fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glUniformMatrix2x3fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glUniformMatrix3x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix3x2fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLfloat)
    glUniformMatrix3x2fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glUniformMatrix3x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix3x2fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glUniformMatrix3x2fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glUniformMatrix2x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix2x4fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLfloat)
    glUniformMatrix2x4fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glUniformMatrix2x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix2x4fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glUniformMatrix2x4fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glUniformMatrix4x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix4x2fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLfloat)
    glUniformMatrix4x2fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glUniformMatrix4x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix4x2fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glUniformMatrix4x2fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glUniformMatrix3x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix3x4fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLfloat)
    glUniformMatrix3x4fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glUniformMatrix3x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix3x4fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glUniformMatrix3x4fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glUniformMatrix4x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix4x3fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLfloat)
    glUniformMatrix4x3fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glUniformMatrix4x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES30::GlUniformMatrix4x3fv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glUniformMatrix4x3fv(
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glBlitFramebuffer ( GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter )

ECode GLES30::GlBlitFramebuffer(
    /* [in] */ Int32 srcX0,
    /* [in] */ Int32 srcY0,
    /* [in] */ Int32 srcX1,
    /* [in] */ Int32 srcY1,
    /* [in] */ Int32 dstX0,
    /* [in] */ Int32 dstY0,
    /* [in] */ Int32 dstX1,
    /* [in] */ Int32 dstY1,
    /* [in] */ Int32 mask,
    /* [in] */ Int32 filter)
{
    glBlitFramebuffer(
        (GLint)srcX0,
        (GLint)srcY0,
        (GLint)srcX1,
        (GLint)srcY1,
        (GLint)dstX0,
        (GLint)dstY0,
        (GLint)dstX1,
        (GLint)dstY1,
        (GLbitfield)mask,
        (GLenum)filter
    );
    return NOERROR;
}

// C function void glRenderbufferStorageMultisample ( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )

ECode GLES30::GlRenderbufferStorageMultisample(
    /* [in] */ Int32 target,
    /* [in] */ Int32 samples,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glRenderbufferStorageMultisample(
        (GLenum)target,
        (GLsizei)samples,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

// C function void glFramebufferTextureLayer ( GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer )

ECode GLES30::GlFramebufferTextureLayer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level,
    /* [in] */ Int32 layer)
{
    glFramebufferTextureLayer(
        (GLenum)target,
        (GLenum)attachment,
        (GLuint)texture,
        (GLint)level,
        (GLint)layer
    );
    return NOERROR;
}

// C function GLvoid * glMapBufferRange ( GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access )

ECode GLES30::GlMapBufferRange(
    /* [in] */ Int32 target,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 access,
    /* [out] */ IBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    GLvoid* _p = glMapBufferRange((GLenum)target,
            (GLintptr)offset, (GLsizeiptr)length, (GLbitfield)access);
    *buffer = NULL;
    if (_p) {
        return NewDirectByteBuffer(reinterpret_cast<Int64>(_p), (Int64)length, buffer);
    }
    return NOERROR;
}

// C function void glFlushMappedBufferRange ( GLenum target, GLintptr offset, GLsizeiptr length )

ECode GLES30::GlFlushMappedBufferRange(
    /* [in] */ Int32 target,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    glFlushMappedBufferRange(
        (GLenum)target,
        (GLintptr)offset,
        (GLsizeiptr)length
    );
    return NOERROR;
}

// C function void glBindVertexArray ( GLuint array )

ECode GLES30::GlBindVertexArray(
    /* [in] */ Int32 array)
{
    glBindVertexArray(
        (GLuint)array
    );
    return NOERROR;
}

// C function void glDeleteVertexArrays ( GLsizei n, const GLuint *arrays )

ECode GLES30::GlDeleteVertexArrays(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* arrays_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(arrays, GLuint)
    glDeleteVertexArrays(
        (GLsizei)n,
        (GLuint *)arrays
    );
    GL_EXIT()
}

// C function void glDeleteVertexArrays ( GLsizei n, const GLuint *arrays )

ECode GLES30::GlDeleteVertexArrays(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* arrays_buf)
{
    GET_BUFFER_GL_DATA(arrays, GLuint)
    glDeleteVertexArrays(
        (GLsizei)n,
        (GLuint *)arrays
    );
    return NOERROR;
}

// C function void glGenVertexArrays ( GLsizei n, GLuint *arrays )

ECode GLES30::GlGenVertexArrays(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* arrays_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(arrays, GLuint)
    glGenVertexArrays(
        (GLsizei)n,
        (GLuint *)arrays
    );
    GL_EXIT()
}

// C function void glGenVertexArrays ( GLsizei n, GLuint *arrays )

ECode GLES30::GlGenVertexArrays(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* arrays_buf)
{
    GET_BUFFER_GL_DATA(arrays, GLuint)
    glGenVertexArrays(
        (GLsizei)n,
        (GLuint *)arrays
    );
    return NOERROR;
}

// C function GLboolean glIsVertexArray ( GLuint array )

ECode GLES30::GlIsVertexArray(
    /* [in] */ Int32 array,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue = glIsVertexArray(
        (GLuint)array
    );
    *result = _returnValue;
    return NOERROR;
}

// C function void glGetIntegeri_v ( GLenum target, GLuint index, GLint *data )

ECode GLES30::GlGetIntegeri_v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Int32>* data_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(data, GLint)
    glGetIntegeri_v(
        (GLenum)target,
        (GLuint)index,
        (GLint *)data
    );
    GL_EXIT()
}

// C function void glGetIntegeri_v ( GLenum target, GLuint index, GLint *data )

ECode GLES30::GlGetIntegeri_v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ IInt32Buffer* data_buf)
{
    GET_BUFFER_GL_DATA(data, GLint)
    glGetIntegeri_v(
        (GLenum)target,
        (GLuint)index,
        (GLint *)data
    );
    return NOERROR;
}

// C function void glBeginTransformFeedback ( GLenum primitiveMode )

ECode GLES30::GlBeginTransformFeedback(
    /* [in] */ Int32 primitiveMode)
{
    glBeginTransformFeedback(
        (GLenum)primitiveMode
    );
    return NOERROR;
}

// C function void glEndTransformFeedback ( void )

ECode GLES30::GlEndTransformFeedback()
{
    glEndTransformFeedback();
    return NOERROR;
}

// C function void glBindBufferRange ( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )

ECode GLES30::GlBindBufferRange(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size)
{
    glBindBufferRange(
        (GLenum)target,
        (GLuint)index,
        (GLuint)buffer,
        (GLintptr)offset,
        (GLsizeiptr)size
    );
    return NOERROR;
}

// C function void glBindBufferBase ( GLenum target, GLuint index, GLuint buffer )

ECode GLES30::GlBindBufferBase(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ Int32 buffer)
{
    glBindBufferBase(
        (GLenum)target,
        (GLuint)index,
        (GLuint)buffer
    );
    return NOERROR;
}

// C function void glTransformFeedbackVaryings ( GLuint program, GLsizei count, const GLchar *varyings, GLenum bufferMode )

ECode GLES30::GlTransformFeedbackVaryings(
    /* [in] */ Int32 program,
    /* [in] */ ArrayOf<String>* varyings_ref,
    /* [in] */ Int32 bufferMode)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char* _exceptionMessage = NULL;
    Int32 _count = 0, _i;
    const char** _varyings = NULL;

    if (!varyings_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "varyings == null";
        goto exit;
    }

    _count = varyings_ref->GetLength();
    _varyings = (const char**)calloc(_count, sizeof(const char*));
    for (_i = 0; _i < _count; _i++) {

        String _varying = (*varyings_ref)[_i];
        if (!_varying) {
            _exception = 1;
            _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
            _exceptionMessage = "null varyings element";
            goto exit;
        }
        _varyings[_i] = _varying.string();
    }

    glTransformFeedbackVaryings(program, _count, _varyings, bufferMode);

exit:
    free(_varyings);
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
        return _exceptionType;
    }
    return _exceptionType;
}

// C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

ECode GLES30::GlGetTransformFeedbackVarying(
    /* [in] */ Int32 program,
    /* [in] */ Int32 index,
    /* [in] */ Int32 bufsize,
    /* [in] */ ArrayOf<Int32>* length_ref,
    /* [in] */ Int32 lengthOffset,
    /* [in] */ ArrayOf<Int32>* size_ref,
    /* [in] */ Int32 sizeOffset,
    /* [in] */ ArrayOf<Int32>* type_ref,
    /* [in] */ Int32 typeOffset,
    /* [in] */ ArrayOf<Byte>* name_ref,
    /* [in] */ Int32 nameOffset)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage;
    GLsizei *length_base = (GLsizei *) 0;
    // Int32 _lengthRemaining;
    GLsizei *length = (GLsizei *) 0;
    GLint *size_base = (GLint *) 0;
    // Int32 _sizeRemaining;
    GLint *size = (GLint *) 0;
    GLenum *type_base = (GLenum *) 0;
    // Int32 _typeRemaining;
    GLenum *type = (GLenum *) 0;
    char *name_base = (char *) 0;
    // Int32 _nameRemaining;
    char *name = (char *) 0;

    if (!length_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "length == null";
        goto exit;
    }
    if (lengthOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "lengthOffset < 0";
        goto exit;
    }
    // _lengthRemaining = length_ref->GetLength() - lengthOffset;
    length_base = (GLsizei *)length_ref->GetPayload();
    length = length_base + lengthOffset;

    if (!size_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "size == null";
        goto exit;
    }
    if (sizeOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "sizeOffset < 0";
        goto exit;
    }
    // _sizeRemaining = size_ref->GetLength() - sizeOffset;
    size_base = (GLint *)size_ref->GetPayload();
    size = size_base + sizeOffset;

    if (!type_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "type == null";
        goto exit;
    }
    if (typeOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "typeOffset < 0";
        goto exit;
    }
    // _typeRemaining = type_ref->GetLength() - typeOffset;
    type_base = (GLenum *)type_ref->GetPayload();
    type = type_base + typeOffset;

    if (!name_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "name == null";
        goto exit;
    }
    if (nameOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "nameOffset < 0";
        goto exit;
    }
    // _nameRemaining = name_ref->GetLength() - nameOffset;
    name_base = (char *)name_ref->GetPayload();
    name = name_base + nameOffset;

    glGetTransformFeedbackVarying(
        (GLuint)program,
        (GLuint)index,
        (GLsizei)bufsize,
        (GLsizei *)length,
        (GLint *)size,
        (GLenum *)type,
        (char *)name
    );

exit:
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
        return _exceptionType;
    }

    return _exceptionType;
}

// C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

ECode GLES30::GlGetTransformFeedbackVarying(
    /* [in] */ Int32 program,
    /* [in] */ Int32 index,
    /* [in] */ Int32 bufsize,
    /* [in] */ IInt32Buffer* length_buf,
    /* [in] */ IInt32Buffer* size_buf,
    /* [in] */ IInt32Buffer* type_buf,
    /* [in] */ Byte name)
{
    HANDLE _lengthArray = 0;
    HANDLE _lengthTmp;
    Int32 _lengthBufferOffset = 0;
    HANDLE _sizeArray = 0;
    HANDLE _sizeTmp;
    Int32 _sizeBufferOffset = 0;
    HANDLE _typeArray = 0;
    HANDLE _typeTmp;
    Int32 _typeBufferOffset = 0;
    Int32 _lengthRemaining;
    GLsizei *length = (GLsizei *) 0;
    Int32 _sizeRemaining;
    GLint *size = (GLint *) 0;
    Int32 _typeRemaining;
    GLenum *type = (GLenum *) 0;

    FAIL_RETURN(GetPointer(IBuffer::Probe(length_buf), &_lengthArray, &_lengthRemaining, &_lengthBufferOffset, &_lengthTmp))
    length = (GLsizei *)_lengthTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(size_buf), &_sizeArray, &_sizeRemaining, &_sizeBufferOffset, &_sizeTmp))
    size = (GLint *)_sizeTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(type_buf), &_typeArray, &_typeRemaining, &_typeBufferOffset, &_typeTmp))
    type = (GLenum *)_typeTmp;

    if (length == NULL) {
        char * _lengthBase = reinterpret_cast<char *>(_lengthArray);
        length = (GLsizei *) (_lengthBase + _lengthBufferOffset);
    }
    if (size == NULL) {
        char * _sizeBase = reinterpret_cast<char *>(_sizeArray);
        size = (GLint *) (_sizeBase + _sizeBufferOffset);
    }
    if (type == NULL) {
        char * _typeBase = reinterpret_cast<char *>(_typeArray);
        type = (GLenum *) (_typeBase + _typeBufferOffset);
    }
    glGetTransformFeedbackVarying(
        (GLuint)program,
        (GLuint)index,
        (GLsizei)bufsize,
        (GLsizei *)length,
        (GLint *)size,
        (GLenum *)type,
        reinterpret_cast<char *>(name)
    );

    return NOERROR;
}

// C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

ECode GLES30::GlGetTransformFeedbackVarying(
    /* [in] */ Int32 program,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Int32>* size_ref,
    /* [in] */ Int32 sizeOffset,
    /* [in] */ ArrayOf<Int32>* type_ref,
    /* [in] */ Int32 typeOffset,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage;
    GLint *size_base = (GLint *) 0;
    GLint *size = (GLint *) 0;
    GLenum *type_base = (GLenum *) 0;
    GLenum *type = (GLenum *) 0;

    GLint len = 0;
    glGetProgramiv((GLuint)program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &len);
    if (!len) {
        *result = String("");
    }
    char* buf = (char*) malloc(len);

    if (buf == NULL) {
        SLOGGERD("GLES30", "out of memory")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!size_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "size == null";
        goto exit;
    }
    if (sizeOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "sizeOffset < 0";
        goto exit;
    }
    size_base = (GLint *)size_ref->GetPayload();
    size = size_base + sizeOffset;

    if (!type_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "type == null";
        goto exit;
    }
    if (typeOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "typeOffset < 0";
        goto exit;
    }
    type_base = (GLenum *)type_ref->GetPayload();
    type = type_base + typeOffset;

    glGetTransformFeedbackVarying(
        (GLuint)program,
        (GLuint)index,
        (GLsizei)len,
        NULL,
        (GLint *)size,
        (GLenum *)type,
        (char *)buf
    );
exit:
    if (_exception != 1) {
        *result = String(buf);
    }
    if (buf) {
        free(buf);
    }
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
        return _exceptionType;
    }
    if (result == 0) {
        *result = String("");
    }

    return _exceptionType;
}

// C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

ECode GLES30::GlGetTransformFeedbackVarying(
    /* [in] */ Int32 program,
    /* [in] */ Int32 index,
    /* [in] */ IInt32Buffer* size_buf,
    /* [in] */ IInt32Buffer* type_buf,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    HANDLE _sizeArray = 0;
    HANDLE _sizeTmp = 0;
    Int32 _sizeBufferOffset = 0;
    HANDLE _typeArray = 0;
    HANDLE _typeTmp = 0;
    Int32 _typeBufferOffset = 0;
    Int32 _sizeRemaining;
    GLint *size = (GLint *) 0;
    Int32 _typeRemaining;
    GLenum *type = (GLenum *) 0;

    GLint len = 0;
    glGetProgramiv((GLuint)program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &len);
    if (!len) {
        *result = String("");
        return NOERROR;
    }
    char* buf = (char*) malloc(len);

    if (buf == NULL) {
        SLOGGERD("GLES30", "out of memory")
        *result = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(GetPointer(IBuffer::Probe(size_buf), &_sizeArray, &_sizeRemaining, &_sizeBufferOffset, &_sizeTmp))
    size = (GLint *)_sizeTmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(type_buf), &_typeArray, &_typeRemaining, &_typeBufferOffset, &_typeTmp))
    type = (GLenum *)_typeTmp;
    if (size == NULL) {
        char * _sizeBase = reinterpret_cast<char *>(_sizeArray);
        size = (GLint *) (_sizeBase + _sizeBufferOffset);
    }
    if (type == NULL) {
        char * _typeBase = reinterpret_cast<char *>(_typeArray);
        type = (GLenum *) (_typeBase + _typeBufferOffset);
    }
    glGetTransformFeedbackVarying(
        (GLuint)program,
        (GLuint)index,
        (GLsizei)len,
        NULL,
        (GLint *)size,
        (GLenum *)type,
        (char *)buf
    );

    *result = String(buf);
    if (buf) {
        free(buf);
    }
    return NOERROR;
}

// C function void glVertexAttribIPointer ( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )

ECode GLES30::GlVertexAttribIPointer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf)
{
    Int32 remaining;
    pointer_buf->GetRemaining(&remaining);
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        HANDLE tmp;
        FAIL_RETURN(GetDirectBufferPointer(pointer_buf, &tmp))
        pointer = (GLvoid *) tmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glVertexAttribIPointerBounds(
        (GLuint)index,
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

// C function void glVertexAttribIPointer ( GLuint index, GLint size, GLenum type, GLsizei stride, GLsizei offset )

ECode GLES30::GlVertexAttribIPointer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    glVertexAttribIPointer(
        (GLuint)index,
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

// C function void glGetVertexAttribIiv ( GLuint index, GLenum pname, GLint *params )

ECode GLES30::GlGetVertexAttribIiv(
    /* [in] */ Int32 index,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLint)
    glGetVertexAttribIiv(
        (GLuint)index,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetVertexAttribIiv ( GLuint index, GLenum pname, GLint *params )

ECode GLES30::GlGetVertexAttribIiv(
    /* [in] */ Int32 index,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLuint)
    glGetVertexAttribIiv(
        (GLuint)index,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetVertexAttribIuiv ( GLuint index, GLenum pname, GLuint *params )

ECode GLES30::GlGetVertexAttribIuiv(
    /* [in] */ Int32 index,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLuint)
    glGetVertexAttribIuiv(
        (GLuint)index,
        (GLenum)pname,
        (GLuint *)params
    );
    GL_EXIT()
}

// C function void glGetVertexAttribIuiv ( GLuint index, GLenum pname, GLuint *params )

ECode GLES30::GlGetVertexAttribIuiv(
    /* [in] */ Int32 index,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLuint)
    glGetVertexAttribIuiv(
        (GLuint)index,
        (GLenum)pname,
        (GLuint *)params
    );
    return NOERROR;
}

// C function void glVertexAttribI4i ( GLuint index, GLint x, GLint y, GLint z, GLint w )

ECode GLES30::GlVertexAttribI4i(
    /* [in] */ Int32 index,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 w)
{
    glVertexAttribI4i(
        (GLuint)index,
        (GLint)x,
        (GLint)y,
        (GLint)z,
        (GLint)w
    );
    return NOERROR;
}

// C function void glVertexAttribI4ui ( GLuint index, GLuint x, GLuint y, GLuint z, GLuint w )

ECode GLES30::GlVertexAttribI4ui(
    /* [in] */ Int32 index,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 w)
{
    glVertexAttribI4ui(
        (GLuint)index,
        (GLuint)x,
        (GLuint)y,
        (GLuint)z,
        (GLuint)w
    );
    return NOERROR;
}

// C function void glVertexAttribI4iv ( GLuint index, const GLint *v )

ECode GLES30::GlVertexAttribI4iv(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Int32>* v_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(v, GLint)
    glVertexAttribI4iv(
        (GLuint)index,
        (GLint *)v
    );
    GL_EXIT()
}

// C function void glVertexAttribI4iv ( GLuint index, const GLint *v )

ECode GLES30::GlVertexAttribI4iv(
    /* [in] */ Int32 index,
    /* [in] */ IInt32Buffer* v_buf)
{
    GET_BUFFER_GL_DATA(v, GLint)
    glVertexAttribI4iv(
        (GLuint)index,
        (GLint *)v
    );
    return NOERROR;
}

// C function void glVertexAttribI4uiv ( GLuint index, const GLuint *v )

ECode GLES30::GlVertexAttribI4uiv(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Int32>* v_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(v, GLuint)
    glVertexAttribI4uiv(
        (GLuint)index,
        (GLuint *)v
    );
    GL_EXIT()
}

// C function void glVertexAttribI4uiv ( GLuint index, const GLuint *v )

ECode GLES30::GlVertexAttribI4uiv(
    /* [in] */ Int32 index,
    /* [in] */ IInt32Buffer* v_buf)
{
    GET_BUFFER_GL_DATA(v, GLuint)
    glVertexAttribI4uiv(
        (GLuint)index,
        (GLuint *)v
    );
    return NOERROR;
}

// C function void glGetUniformuiv ( GLuint program, GLint location, GLuint *params )

ECode GLES30::GlGetUniformuiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLuint)
    glGetUniformuiv(
        (GLuint)program,
        (GLint)location,
        (GLuint *)params
    );
    GL_EXIT()
}

// C function void glGetUniformuiv ( GLuint program, GLint location, GLuint *params )

ECode GLES30::GlGetUniformuiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLuint)
    glGetUniformuiv(
        (GLuint)program,
        (GLint)location,
        (GLuint *)params
    );
    return NOERROR;
}

// C function GLint glGetFragDataLocation ( GLuint program, const GLchar *name )

ECode GLES30::GlGetFragDataLocation(
    /* [in] */ Int32 program,
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    GLint _returnValue = 0;
    const char* _nativename = 0;

    if (!name) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "name == null";
        goto exit;
    }
    _nativename = name.string();

    _returnValue = glGetFragDataLocation(
        (GLuint)program,
        (GLchar *)_nativename
    );

exit:
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
        return _exceptionType;
    }
    *result = _returnValue;
    return _exceptionType;
}

// C function void glUniform1ui ( GLint location, GLuint v0 )

ECode GLES30::GlUniform1ui(
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0)
{
    glUniform1ui(
        (GLint)location,
        (GLuint)v0
    );
    return NOERROR;
}

// C function void glUniform2ui ( GLint location, GLuint v0, GLuint v1 )

ECode GLES30::GlUniform2ui(
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1)
{
    glUniform2ui(
        (GLint)location,
        (GLuint)v0,
        (GLuint)v1
    );
    return NOERROR;
}

// C function void glUniform3ui ( GLint location, GLuint v0, GLuint v1, GLuint v2 )

ECode GLES30::GlUniform3ui(
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1,
    /* [in] */ Int32 v2)
{
    glUniform3ui(
        (GLint)location,
        (GLuint)v0,
        (GLuint)v1,
        (GLuint)v2
    );
    return NOERROR;
}

// C function void glUniform4ui ( GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 )

ECode GLES30::GlUniform4ui(
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1,
    /* [in] */ Int32 v2,
    /* [in] */ Int32 v3)
{
    glUniform4ui(
        (GLint)location,
        (GLuint)v0,
        (GLuint)v1,
        (GLuint)v2,
        (GLuint)v3
    );
    return NOERROR;
}

// C function void glUniform1uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform1uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLuint)
    glUniform1uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glUniform1uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform1uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLuint)
    glUniform1uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glUniform2uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform2uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLuint)
    glUniform2uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glUniform2uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform2uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLuint)
    glUniform2uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glUniform3uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform3uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLuint)
    glUniform3uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glUniform3uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform3uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLuint)
    glUniform3uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glUniform4uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform4uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLuint)
    glUniform4uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glUniform4uiv ( GLint location, GLsizei count, const GLuint *value )

ECode GLES30::GlUniform4uiv(
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLuint)
    glUniform4uiv(
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glClearBufferiv ( GLenum buffer, GLint drawbuffer, const GLint *value )

ECode GLES30::GlClearBufferiv(
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 drawbuffer,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLint)
    glClearBufferiv(
        (GLenum)buffer,
        (GLint)drawbuffer,
        (GLint *)value
    );
    GL_EXIT()
    return NOERROR;
}

// C function void glClearBufferiv ( GLenum buffer, GLint drawbuffer, const GLint *value )

ECode GLES30::GlClearBufferiv(
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 drawbuffer,
    /* [in] */ IInt32Buffer* value)
{
    return NOERROR;
}

// C function void glClearBufferuiv ( GLenum buffer, GLint drawbuffer, const GLuint *value )

ECode GLES30::GlClearBufferuiv(
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 drawbuffer,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLint)
    glClearBufferiv(
        (GLenum)buffer,
        (GLint)drawbuffer,
        (GLint *)value
    );
    GL_EXIT()
    return NOERROR;
}

// C function void glClearBufferuiv ( GLenum buffer, GLint drawbuffer, const GLuint *value )

ECode GLES30::GlClearBufferuiv(
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 drawbuffer,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLuint)
    glClearBufferuiv(
        (GLenum)buffer,
        (GLint)drawbuffer,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glClearBufferfv ( GLenum buffer, GLint drawbuffer, const GLfloat *value )

ECode GLES30::GlClearBufferfv(
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 drawbuffer,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(value, GLfloat)
    glClearBufferfv(
        (GLenum)buffer,
        (GLint)drawbuffer,
        (GLfloat *)value
    );
    GL_EXIT()
    return NOERROR;
}

// C function void glClearBufferfv ( GLenum buffer, GLint drawbuffer, const GLfloat *value )

ECode GLES30::GlClearBufferfv(
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 drawbuffer,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glClearBufferfv(
        (GLenum)buffer,
        (GLint)drawbuffer,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glClearBufferfi ( GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil )

ECode GLES30::GlClearBufferfi(
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 drawbuffer,
    /* [in] */ Float depth,
    /* [in] */ Int32 stencil)
{
    glClearBufferfi(
        (GLenum)buffer,
        (GLint)drawbuffer,
        (GLfloat)depth,
        (GLint)stencil
    );
    return NOERROR;
}

// C function const GLubyte * glGetStringi ( GLenum name, GLuint index )

ECode GLES30::GlGetStringi(
    /* [in] */ Int32 name,
    /* [in] */ Int32 index,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    const GLubyte* _chars = glGetStringi((GLenum)name, (GLuint)index);
    *result = String((const char*)_chars);
    return NOERROR;
}

// C function void glCopyBufferSubData ( GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size )

ECode GLES30::GlCopyBufferSubData(
    /* [in] */ Int32 readTarget,
    /* [in] */ Int32 writeTarget,
    /* [in] */ Int32 readOffset,
    /* [in] */ Int32 writeOffset,
    /* [in] */ Int32 size)
{
    glCopyBufferSubData(
        (GLenum)readTarget,
        (GLenum)writeTarget,
        (GLintptr)readOffset,
        (GLintptr)writeOffset,
        (GLsizeiptr)size
    );
    return NOERROR;
}

// C function void glGetUniformIndices ( GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices )

ECode GLES30::GlGetUniformIndices(
    /* [in] */ Int32 program,
    /* [in] */ ArrayOf<String>* uniformNames_ref,
    /* [in] */ ArrayOf<Int32>* uniformIndices_ref,
    /* [in] */ Int32 uniformIndicesOffset)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char* _exceptionMessage = NULL;
    Int32 _count = 0;
    Int32 _i;
    const char** _names = NULL;
    GLuint* _indices_base = NULL;
    GLuint* _indices = NULL;

    if (!uniformNames_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformNames == null";
        goto exit;
    }
    _count = uniformNames_ref->GetLength();
    _names = (const char**)calloc(_count, sizeof(const char*));
    for (_i = 0; _i < _count; _i++) {
        String _name = (*uniformNames_ref)[_i];
        if (!_name) {
            _exception = 1;
            _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
            _exceptionMessage = "null uniformNames element";
            goto exit;
        }
        _names[_i] = _name.string();
    }

    if (!uniformIndices_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformIndices == null";
        goto exit;
    }
    if (uniformIndicesOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformIndicesOffset < 0";
        goto exit;
    }
    if (uniformIndices_ref->GetLength() - uniformIndicesOffset < _count) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "not enough space in uniformIndices";
        goto exit;
    }
    _indices_base = (GLuint*)uniformIndices_ref->GetPayload();
    _indices = _indices_base + uniformIndicesOffset;

    glGetUniformIndices(program, _count, _names, _indices);

exit:
    free(_names);
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glGetUniformIndices ( GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices )

ECode GLES30::GlGetUniformIndices(
    /* [in] */ Int32 program,
    /* [in] */ ArrayOf<String>* uniformNames_ref,
    /* [in] */ IInt32Buffer* uniformIndices_buf)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char* _exceptionMessage = NULL;
    Int32 _count = 0;
    Int32 _i;
    const char** _names = NULL;
    HANDLE _uniformIndicesArray = 0;
    Int32 _uniformIndicesRemaining;
    Int32 _uniformIndicesOffset = 0;
    GLuint* _indices = NULL;
    char* _indicesBase = NULL;

    if (!uniformNames_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformNames == null";
        goto exit;
    }
    if (!uniformIndices_buf) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformIndices == null";
        goto exit;
    }

    _count = uniformNames_ref->GetLength();
    _names = (const char**)calloc(_count, sizeof(const char*));
    for (_i = 0; _i < _count; _i++) {
        String _name = (*uniformNames_ref)[_i];
        if (!_name) {
            _exception = 1;
            _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
            _exceptionMessage = "null uniformNames element";
            goto exit;
        }
        _names[_i] = _name.string();
    }

    HANDLE tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(uniformIndices_buf), &_uniformIndicesArray, &_uniformIndicesRemaining, &_uniformIndicesOffset, &tmp))
    _indices = (GLuint*)tmp;
    if (!_indices) {
        _indicesBase = reinterpret_cast<char*>(_uniformIndicesArray);
        _indices = (GLuint*)(_indicesBase + _uniformIndicesOffset);
    }
    if (_uniformIndicesRemaining < _count) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "not enough space in uniformIndices";
        goto exit;
    }

    glGetUniformIndices(program, _count, _names, _indices);

exit:
    free(_names);
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glGetActiveUniformsiv ( GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params )

ECode GLES30::GlGetActiveUniformsiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformCount,
    /* [in] */ ArrayOf<Int32>* uniformIndices_ref,
    /* [in] */ Int32 uniformIndicesOffset,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 paramsOffset)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    GLuint *uniformIndices_base = (GLuint *) 0;
    Int32 UNUSED(_uniformIndicesRemaining);
    GLuint *uniformIndices = (GLuint *) 0;
    GLint *params_base = (GLint *) 0;
    Int32 UNUSED(_paramsRemaining);
    GLint *params = (GLint *) 0;

    if (!uniformIndices_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformIndices == null";
        goto exit;
    }
    if (uniformIndicesOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformIndicesOffset < 0";
        goto exit;
    }
    _uniformIndicesRemaining = uniformIndices_ref->GetLength() - uniformIndicesOffset;
    uniformIndices_base = (GLuint *)uniformIndices_ref->GetPayload();
    uniformIndices = uniformIndices_base + uniformIndicesOffset;

    if (!params_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "params == null";
        goto exit;
    }
    if (paramsOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "paramsOffset < 0";
        goto exit;
    }
    _paramsRemaining = params_ref->GetLength() - paramsOffset;
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + paramsOffset;

    glGetActiveUniformsiv(
        (GLuint)program,
        (GLsizei)uniformCount,
        (GLuint *)uniformIndices,
        (GLenum)pname,
        (GLint *)params
    );

exit:
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glGetActiveUniformsiv ( GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params )

ECode GLES30::GlGetActiveUniformsiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformCount,
    /* [in] */ IInt32Buffer* uniformIndices_buf,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    HANDLE _uniformIndicesArray =  0;
    Int32 _uniformIndicesBufferOffset =  0;
    HANDLE _paramsArray =  0;
    Int32 _paramsBufferOffset =  0;
    Int32 _uniformIndicesRemaining;
    GLuint *uniformIndices = (GLuint *) 0;
    Int32 _paramsRemaining;
    GLint *params = (GLint *) 0;

    HANDLE tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(uniformIndices_buf), &_uniformIndicesArray, &_uniformIndicesRemaining, &_uniformIndicesBufferOffset, &tmp))
    uniformIndices = (GLuint *)tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(params_buf), &_paramsArray, &_paramsRemaining, &_paramsBufferOffset, &tmp))
    params = (GLint *)tmp;
    if (uniformIndices == NULL) {
        char * _uniformIndicesBase = reinterpret_cast<char *>(_uniformIndicesArray);
        uniformIndices = (GLuint *) (_uniformIndicesBase + _uniformIndicesBufferOffset);
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_paramsArray);
        params = (GLint *) (_paramsBase + _paramsBufferOffset);
    }
    glGetActiveUniformsiv(
        (GLuint)program,
        (GLsizei)uniformCount,
        (GLuint *)uniformIndices,
        (GLenum)pname,
        (GLint *)params
    );

    return NOERROR;
}

// C function GLuint glGetUniformBlockIndex ( GLuint program, const GLchar *uniformBlockName )

ECode GLES30::GlGetUniformBlockIndex(
    /* [in] */ Int32 program,
    /* [in] */ const String& uniformBlockName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    GLuint _returnValue = 0;
    const char* _nativeuniformBlockName = 0;

    if (uniformBlockName == NULL) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformBlockName == null";
        goto exit;
    }
    _nativeuniformBlockName = uniformBlockName.string();

    _returnValue = glGetUniformBlockIndex(
        (GLuint)program,
        (GLchar *)_nativeuniformBlockName
    );

exit:
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
        return _exceptionType;
    }
    *result = (Int32)_returnValue;
    return NOERROR;
}

// C function void glGetActiveUniformBlockiv ( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params )

ECode GLES30::GlGetActiveUniformBlockiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformBlockIndex,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLint)
    glGetActiveUniformBlockiv(
        (GLuint)program,
        (GLuint)uniformBlockIndex,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetActiveUniformBlockiv ( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params )

ECode GLES30::GlGetActiveUniformBlockiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformBlockIndex,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetActiveUniformBlockiv(
        (GLuint)program,
        (GLuint)uniformBlockIndex,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetActiveUniformBlockName ( GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName )

ECode GLES30::GlGetActiveUniformBlockName(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformBlockIndex,
    /* [in] */ Int32 bufSize,
    /* [in] */ ArrayOf<Int32>* length_ref,
    /* [in] */ Int32 lengthOffset,
    /* [in] */ ArrayOf<Byte>* name_ref,
    /* [in] */ Int32 nameOffset)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char* _exceptionMessage;
    GLsizei* _length_base = (GLsizei*)0;
    Int32 UNUSED(_lengthRemaining);
    GLsizei* _length = (GLsizei*)0;
    GLchar* _name_base = (GLchar*)0;
    Int32 UNUSED(_nameRemaining);
    GLchar* _name = (GLchar*)0;

    if (!length_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "length == null";
        goto exit;
    }
    if (lengthOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "lengthOffset < 0";
        goto exit;
    }
    _lengthRemaining = length_ref->GetLength() - lengthOffset;
    _length_base = (GLsizei*)length_ref->GetPayload();
    _length = _length_base + lengthOffset;

    if (!name_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformBlockName == null";
        goto exit;
    }
    if (nameOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "uniformBlockNameOffset < 0";
        goto exit;
    }
    _nameRemaining = name_ref->GetLength() - nameOffset;
    _name_base = (GLchar*)name_ref->GetPayload();
    _name = _name_base + nameOffset;

    glGetActiveUniformBlockName(
        (GLuint)program,
        (GLuint)uniformBlockIndex,
        (GLsizei)bufSize,
        (GLsizei*)_length,
        (GLchar*)_name
    );

exit:
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glGetActiveUniformBlockName ( GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName )

ECode GLES30::GlGetActiveUniformBlockName(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformBlockIndex,
    /* [in] */ IBuffer* length_buf,
    /* [in] */ IBuffer* uniformBlockName_buf)
{
    HANDLE _lengthArray = 0;
    Int32 _lengthBufferOffset = 0;
    GLsizei* _length = (GLsizei*)0;
    Int32 _lengthRemaining;
    HANDLE _nameArray = 0;
    Int32 _nameBufferOffset = 0;
    GLchar* _name = (GLchar*)0;
    Int32 _nameRemaining;

    HANDLE tmp;
    FAIL_RETURN(GetPointer(length_buf, &_lengthArray, &_lengthRemaining, &_lengthBufferOffset, &tmp))
    _length = (GLsizei*)tmp;
    if (_length == NULL) {
        GLsizei* _lengthBase = (GLsizei*)_lengthArray;
        _length = (GLsizei*)(_lengthBase + _lengthBufferOffset);
    }

    FAIL_RETURN(GetPointer(uniformBlockName_buf, &_nameArray, &_nameRemaining, &_nameBufferOffset, &tmp))
    _name = (GLchar*)tmp;
    if (_name == NULL) {
        GLchar* _nameBase = (GLchar*)_nameArray;
        _name = (GLchar*)(_nameBase + _nameBufferOffset);
    }

    glGetActiveUniformBlockName(
        (GLuint)program,
        (GLuint)uniformBlockIndex,
        (GLsizei)_nameRemaining,
        _length, _name
    );
    return NOERROR;
}

// C function void glGetActiveUniformBlockName ( GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName )

ECode GLES30::GlGetActiveUniformBlockName(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformBlockIndex,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    GLint len = 0;
    glGetActiveUniformBlockiv((GLuint)program, (GLuint)uniformBlockIndex,
            GL_UNIFORM_BLOCK_NAME_LENGTH, &len);
    GLchar* name = (GLchar*)malloc(len);
    glGetActiveUniformBlockName((GLuint)program, (GLuint)uniformBlockIndex,
        len, NULL, name);
    *result = String((const char*)name);
    free(name);
    return NOERROR;
}

// C function void glUniformBlockBinding ( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding )

ECode GLES30::GlUniformBlockBinding(
    /* [in] */ Int32 program,
    /* [in] */ Int32 uniformBlockIndex,
    /* [in] */ Int32 uniformBlockBinding)
{
    glUniformBlockBinding(
        (GLuint)program,
        (GLuint)uniformBlockIndex,
        (GLuint)uniformBlockBinding
    );
    return NOERROR;
}

// C function void glDrawArraysInstanced ( GLenum mode, GLint first, GLsizei count, GLsizei instanceCount )

ECode GLES30::GlDrawArraysInstanced(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 first,
    /* [in] */ Int32 count,
    /* [in] */ Int32 instanceCount)
{
    glDrawArraysInstanced(
        (GLenum)mode,
        (GLint)first,
        (GLsizei)count,
        (GLsizei)instanceCount
    );
    return NOERROR;
}

// C function void glDrawElementsInstanced ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instanceCount )

ECode GLES30::GlDrawElementsInstanced(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* indices_buf,
    /* [in] */ Int32 instanceCount)
{
    GET_BUFFER_GL_DATA(indices, GLvoid)
    glDrawElementsInstanced(
        (GLenum)mode,
        (GLsizei)count,
        (GLenum)type,
        (GLvoid *)indices,
        (GLsizei)instanceCount
    );
    return NOERROR;
}

// C function void glDrawElementsInstanced ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instanceCount )

ECode GLES30::GlDrawElementsInstanced(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ Int32 indicesOffset,
    /* [in] */ Int32 instanceCount)
{
    glDrawElementsInstanced(
        (GLenum)mode,
        (GLsizei)count,
        (GLenum)type,
        (GLvoid *)indicesOffset,
        (GLsizei)instanceCount
    );
    return NOERROR;
}

// C function GLsync glFenceSync ( GLenum condition, GLbitfield flags )

ECode GLES30::GlFenceSync(
    /* [in] */ Int32 condition,
    /* [in] */ Int32 flags,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    GLsync _returnValue;
    _returnValue = glFenceSync(
        (GLenum)condition,
        (GLbitfield)flags
    );
    *result = (Int64)_returnValue;
    return NOERROR;
}

// C function GLboolean glIsSync ( GLsync sync )

ECode GLES30::GlIsSync(
    /* [in] */ Int64 sync,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    glDeleteSync(
        (GLsync)sync
    );
    return NOERROR;
}

// C function void glDeleteSync ( GLsync sync )

ECode GLES30::GlDeleteSync(
    /* [in] */ Int64 sync)
{
    glDeleteSync(
        (GLsync)sync
    );
    return NOERROR;
}

// C function GLenum glClientWaitSync ( GLsync sync, GLbitfield flags, GLuint64 timeout )

ECode GLES30::GlClientWaitSync(
    /* [in] */ Int64 sync,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 timeout,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    GLenum _returnValue;
    _returnValue = glClientWaitSync(
        (GLsync)sync,
        (GLbitfield)flags,
        (GLuint64)timeout
    );
    *result = (Int32)_returnValue;
    return NOERROR;
}

// C function void glWaitSync ( GLsync sync, GLbitfield flags, GLuint64 timeout )

ECode GLES30::GlWaitSync(
    /* [in] */ Int64 sync,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 timeout)
{
    glWaitSync(
        (GLsync)sync,
        (GLbitfield)flags,
        (GLuint64)timeout
    );
    return NOERROR;
}

// C function void glGetInteger64v ( GLenum pname, GLint64 *params )

ECode GLES30::GlGetInteger64v(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int64>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLint64)
    glGetInteger64v(
        (GLenum)pname,
        (GLint64 *)params
    );
    GL_EXIT()
}

// C function void glGetInteger64v ( GLenum pname, GLint64 *params )

ECode GLES30::GlGetInteger64v(
    /* [in] */ Int32 pname,
    /* [in] */ IInt64Buffer*  params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint64)
    glGetInteger64v(
        (GLenum)pname,
        (GLint64 *)params
    );
    return NOERROR;
}

// C function void glGetSynciv ( GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values )

ECode GLES30::GlGetSynciv(
    /* [in] */ Int64 sync,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 bufSize,
    /* [in] */ ArrayOf<Int32>* length_ref,
    /* [in] */ Int32 lengthOffset,
    /* [in] */ ArrayOf<Int32>* values_ref,
    /* [in] */ Int32 valuesOffset)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    GLsizei *length_base = (GLsizei *) 0;
    Int32 UNUSED(_lengthRemaining);
    GLsizei *length = (GLsizei *) 0;
    GLint *values_base = (GLint *) 0;
    Int32 UNUSED(_valuesRemaining);
    GLint *values = (GLint *) 0;

    if (!length_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "length == null";
        goto exit;
    }
    if (lengthOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "lengthOffset < 0";
        goto exit;
    }
    _lengthRemaining = length_ref->GetLength() - lengthOffset;
    length_base = (GLsizei *)length_ref->GetPayload();
    length = length_base + lengthOffset;

    if (!values_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "values == null";
        goto exit;
    }
    if (valuesOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "valuesOffset < 0";
        goto exit;
    }
    _valuesRemaining = values_ref->GetLength() - valuesOffset;
    values_base = (GLint *)values_ref->GetPayload();
    values = values_base + valuesOffset;

    glGetSynciv(
        (GLsync)sync,
        (GLenum)pname,
        (GLsizei)bufSize,
        (GLsizei *)length,
        (GLint *)values
    );

exit:
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glGetSynciv ( GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values )

ECode GLES30::GlGetSynciv(
    /* [in] */ Int64 sync,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 bufSize,
    /* [in] */ IInt32Buffer* length_buf,
    /* [in] */ IInt32Buffer* values_buf)
{
    HANDLE _lengthArray = 0;
    Int32 _lengthBufferOffset = (Int32) 0;
    HANDLE _valuesArray = 0;
    Int32 _valuesBufferOffset = (Int32) 0;
    Int32 _lengthRemaining;
    GLsizei *length = (GLsizei *) 0;
    Int32 _valuesRemaining;
    GLint *values = (GLint *) 0;

    HANDLE tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(length_buf), &_lengthArray, &_lengthRemaining, &_lengthBufferOffset, &tmp))
    length = (GLsizei *)tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(values_buf), &_valuesArray, &_valuesRemaining, &_valuesBufferOffset, &tmp))
    values = (GLint *)tmp;
    if (length == NULL) {
        char * _lengthBase = reinterpret_cast<char *>(_lengthArray);
        length = (GLsizei *) (_lengthBase + _lengthBufferOffset);
    }
    if (values == NULL) {
        char * _valuesBase = reinterpret_cast<char *>(_valuesArray);
        values = (GLint *) (_valuesBase + _valuesBufferOffset);
    }
    glGetSynciv(
        (GLsync)sync,
        (GLenum)pname,
        (GLsizei)bufSize,
        (GLsizei *)length,
        (GLint *)values
    );
    return NOERROR;
}

// C function void glGetInteger64i_v ( GLenum target, GLuint index, GLint64 *data )

ECode GLES30::GlGetInteger64i_v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Int64>* data_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(data, GLint64)
    glGetInteger64i_v(
        (GLenum)target,
        (GLuint)index,
        (GLint64 *)data
    );
    GL_EXIT()
}

// C function void glGetInteger64i_v ( GLenum target, GLuint index, GLint64 *data )

ECode GLES30::GlGetInteger64i_v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ IInt64Buffer*  data_buf)
{
    GET_BUFFER_GL_DATA(data, GLint64)
    glGetInteger64i_v(
        (GLenum)target,
        (GLuint)index,
        (GLint64 *)data
    );
    return NOERROR;
}

// C function void glGetBufferParameteri64v ( GLenum target, GLenum pname, GLint64 *params )

ECode GLES30::GlGetBufferParameteri64v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int64>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLint64)
    glGetBufferParameteri64v(
        (GLenum)target,
        (GLenum)pname,
        (GLint64 *)params
    );
    GL_EXIT()
}

// C function void glGetBufferParameteri64v ( GLenum target, GLenum pname, GLint64 *params )

ECode GLES30::GlGetBufferParameteri64v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt64Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint64)
    glGetBufferParameteri64v(
        (GLenum)target,
        (GLenum)pname,
        (GLint64 *)params
    );
    return NOERROR;
}

// C function void glGenSamplers ( GLsizei count, GLuint *samplers )

ECode GLES30::GlGenSamplers(
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* samplers_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(samplers, GLuint)
    glGenSamplers(
        (GLsizei)count,
        (GLuint *)samplers
    );
    GL_EXIT()
}

// C function void glGenSamplers ( GLsizei count, GLuint *samplers )

ECode GLES30::GlGenSamplers(
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* samplers_buf)
{
    GET_BUFFER_GL_DATA(samplers, GLuint)
    glGenSamplers(
        (GLsizei)count,
        (GLuint *)samplers
    );
    return NOERROR;
}

// C function void glDeleteSamplers ( GLsizei count, const GLuint *samplers )

ECode GLES30::GlDeleteSamplers(
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* samplers_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(samplers, GLuint)
    glDeleteSamplers(
        (GLsizei)count,
        (GLuint *)samplers
    );
    GL_EXIT()
}

// C function void glDeleteSamplers ( GLsizei count, const GLuint *samplers )

ECode GLES30::GlDeleteSamplers(
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* samplers_buf)
{
    GET_BUFFER_GL_DATA(samplers, GLuint)
    glDeleteSamplers(
        (GLsizei)count,
        (GLuint *)samplers
    );
    return NOERROR;
}

// C function GLboolean glIsSampler ( GLuint sampler )

ECode GLES30::GlIsSampler(
    /* [in] */ Int32 sampler,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsSampler(
        (GLuint)sampler
    );
    *result = (Boolean)_returnValue;
    return NOERROR;
}

// C function void glBindSampler ( GLuint unit, GLuint sampler )

ECode GLES30::GlBindSampler(
    /* [in] */ Int32 unit,
    /* [in] */ Int32 sampler)
{
    glBindSampler(
        (GLuint)unit,
        (GLuint)sampler
    );
    return NOERROR;
}

// C function void glSamplerParameteri ( GLuint sampler, GLenum pname, GLint param )

ECode GLES30::GlSamplerParameteri(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glSamplerParameteri(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint)param
    );
    return NOERROR;
}

// C function void glSamplerParameteriv ( GLuint sampler, GLenum pname, const GLint *param )

ECode GLES30::GlSamplerParameteriv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* param_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(param, GLint)
    glSamplerParameteriv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)param
    );
    GL_EXIT()
}

// C function void glSamplerParameteriv ( GLuint sampler, GLenum pname, const GLint *param )

ECode GLES30::GlSamplerParameteriv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* param_buf)
{
    GET_BUFFER_GL_DATA(param, GLint)
    glSamplerParameteriv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)param
    );
    return NOERROR;
}

// C function void glSamplerParameterf ( GLuint sampler, GLenum pname, GLfloat param )

ECode GLES30::GlSamplerParameterf(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glSamplerParameterf(
        (GLuint)sampler,
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

// C function void glSamplerParameterfv ( GLuint sampler, GLenum pname, const GLfloat *param )

ECode GLES30::GlSamplerParameterfv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* param_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(param, GLfloat)
    glSamplerParameterfv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLfloat *)param
    );
    GL_EXIT()
}

// C function void glSamplerParameterfv ( GLuint sampler, GLenum pname, const GLfloat *param )

ECode GLES30::GlSamplerParameterfv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* param_buf)
{
    GET_BUFFER_GL_DATA(param, GLfloat)
    glSamplerParameterfv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLfloat *)param
    );
    return NOERROR;
}

// C function void glGetSamplerParameteriv ( GLuint sampler, GLenum pname, GLint *params )

ECode GLES30::GlGetSamplerParameteriv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLint)
    glGetSamplerParameteriv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetSamplerParameteriv ( GLuint sampler, GLenum pname, GLint *params )

ECode GLES30::GlGetSamplerParameteriv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetSamplerParameteriv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetSamplerParameterfv ( GLuint sampler, GLenum pname, GLfloat *params )

ECode GLES30::GlGetSamplerParameterfv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLfloat)
    glGetSamplerParameterfv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLfloat *)params
    );
    GL_EXIT()
}

// C function void glGetSamplerParameterfv ( GLuint sampler, GLenum pname, GLfloat *params )

ECode GLES30::GlGetSamplerParameterfv(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLfloat)
    glGetSamplerParameterfv(
        (GLuint)sampler,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

// C function void glVertexAttribDivisor ( GLuint index, GLuint divisor )

ECode GLES30::GlVertexAttribDivisor(
    /* [in] */ Int32 index,
    /* [in] */ Int32 divisor)
{
    glVertexAttribDivisor(
        (GLuint)index,
        (GLuint)divisor
    );
    return NOERROR;
}

// C function void glBindTransformFeedback ( GLenum target, GLuint id )

ECode GLES30::GlBindTransformFeedback(
    /* [in] */ Int32 target,
    /* [in] */ Int32 id)
{
    glBindTransformFeedback(
        (GLenum)target,
        (GLuint)id
    );
    return NOERROR;
}

// C function void glDeleteTransformFeedbacks ( GLsizei n, const GLuint *ids )

ECode GLES30::GlDeleteTransformFeedbacks(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* ids_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(ids, GLuint)
    glDeleteTransformFeedbacks(
        (GLsizei)n,
        (GLuint *)ids
    );
    GL_EXIT()
}

// C function void glDeleteTransformFeedbacks ( GLsizei n, const GLuint *ids )

ECode GLES30::GlDeleteTransformFeedbacks(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* ids_buf)
{
    GET_BUFFER_GL_DATA(ids, GLuint)
    glDeleteTransformFeedbacks(
        (GLsizei)n,
        (GLuint *)ids
    );
    return NOERROR;
}

// C function void glGenTransformFeedbacks ( GLsizei n, GLuint *ids )

ECode GLES30::GlGenTransformFeedbacks(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* ids_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(ids, GLuint)
    glGenTransformFeedbacks(
        (GLsizei)n,
        (GLuint *)ids
    );
    GL_EXIT()
}

// C function void glGenTransformFeedbacks ( GLsizei n, GLuint *ids )

ECode GLES30::GlGenTransformFeedbacks(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* ids_buf)
{
    GET_BUFFER_GL_DATA(ids, GLuint)
    glGenTransformFeedbacks(
        (GLsizei)n,
        (GLuint *)ids
    );
    return NOERROR;
}

// C function GLboolean glIsTransformFeedback ( GLuint id )

ECode GLES30::GlIsTransformFeedback(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsTransformFeedback(
        (GLuint)id
    );
    *result = (Boolean)_returnValue;
    return NOERROR;
}

// C function void glPauseTransformFeedback ( void )

ECode GLES30::GlPauseTransformFeedback()
{
    glPauseTransformFeedback();
    return NOERROR;
}

// C function void glResumeTransformFeedback ( void )

ECode GLES30::GlResumeTransformFeedback()
{
    glResumeTransformFeedback();
    return NOERROR;
}

// C function void glGetProgramBinary ( GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, GLvoid *binary )

ECode GLES30::GlGetProgramBinary(
    /* [in] */ Int32 program,
    /* [in] */ Int32 bufSize,
    /* [in] */ ArrayOf<Int32>* length_ref,
    /* [in] */ Int32 lengthOffset,
    /* [in] */ ArrayOf<Int32>* binaryFormat_ref,
    /* [in] */ Int32 binaryFormatOffset,
    /* [in] */ IBuffer* binary_buf)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    HANDLE _array = 0;
    Int32 _bufferOffset = (Int32) 0;
    GLsizei *length_base = (GLsizei *) 0;
    Int32 UNUSED(_lengthRemaining);
    GLsizei *length = (GLsizei *) 0;
    GLenum *binaryFormat_base = (GLenum *) 0;
    Int32 UNUSED(_binaryFormatRemaining);
    GLenum *binaryFormat = (GLenum *) 0;
    Int32 _binaryRemaining;
    GLvoid *binary = (GLvoid *) 0;

    if (!length_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "length == null";
        goto exit;
    }
    if (lengthOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "lengthOffset < 0";
        goto exit;
    }
    _lengthRemaining = length_ref->GetLength() - lengthOffset;
    length_base = (GLsizei *)length_ref->GetPayload();
    length = length_base + lengthOffset;

    if (!binaryFormat_ref) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "binaryFormat == null";
        goto exit;
    }
    if (binaryFormatOffset < 0) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "binaryFormatOffset < 0";
        goto exit;
    }
    _binaryFormatRemaining = binaryFormat_ref->GetLength() - binaryFormatOffset;
    binaryFormat_base = (GLenum *)binaryFormat_ref->GetPayload();
    binaryFormat = binaryFormat_base + binaryFormatOffset;

    HANDLE tmp;
    FAIL_RETURN(GetPointer(binary_buf, &_array, &_binaryRemaining, &_bufferOffset, &tmp))
    binary = (GLvoid *)tmp;
    if (binary == NULL) {
        char * _binaryBase = reinterpret_cast<char *>(_array);
        binary = (GLvoid *) (_binaryBase + _bufferOffset);
    }
    glGetProgramBinary(
        (GLuint)program,
        (GLsizei)bufSize,
        (GLsizei *)length,
        (GLenum *)binaryFormat,
        (GLvoid *)binary
    );

exit:
    if (_exception) {
        SLOGGERD("GLES30", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glGetProgramBinary ( GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, GLvoid *binary )

ECode GLES30::GlGetProgramBinary(
    /* [in] */ Int32 program,
    /* [in] */ Int32 bufSize,
    /* [in] */ IInt32Buffer* length_buf,
    /* [in] */ IInt32Buffer* binaryFormat_buf,
    /* [in] */ IBuffer* binary_buf)
{
    HANDLE _lengthArray = 0;
    Int32 _lengthBufferOffset = 0;
    HANDLE _binaryFormatArray = 0;
    Int32 _binaryFormatBufferOffset = 0;
    HANDLE _binaryArray = 0;
    Int32 _binaryBufferOffset = 0;
    Int32 _lengthRemaining;
    GLsizei *length = (GLsizei *) 0;
    Int32 _binaryFormatRemaining;
    GLenum *binaryFormat = (GLenum *) 0;
    Int32 _binaryRemaining;
    GLvoid *binary = (GLvoid *) 0;

    HANDLE tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(length_buf), &_lengthArray, &_lengthRemaining, &_lengthBufferOffset, &tmp))
    length = (GLsizei *)tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(binaryFormat_buf), &_binaryFormatArray, &_binaryFormatRemaining, &_binaryFormatBufferOffset, &tmp))
    binaryFormat = (GLenum *)tmp;
    FAIL_RETURN(GetPointer(IBuffer::Probe(binary_buf), &_binaryArray, &_binaryRemaining, &_binaryBufferOffset, &tmp))
    binary = (GLvoid *)tmp;
    if (length == NULL) {
        char * _lengthBase = reinterpret_cast<char *>(_lengthArray);
        length = (GLsizei *) (_lengthBase + _lengthBufferOffset);
    }
    if (binaryFormat == NULL) {
        char * _binaryFormatBase = reinterpret_cast<char *>(_binaryFormatArray);
        binaryFormat = (GLenum *) (_binaryFormatBase + _binaryFormatBufferOffset);
    }
    if (binary == NULL) {
        char * _binaryBase = reinterpret_cast<char *>(_binaryArray);
        binary = (GLvoid *) (_binaryBase + _binaryBufferOffset);
    }
    glGetProgramBinary(
        (GLuint)program,
        (GLsizei)bufSize,
        (GLsizei *)length,
        (GLenum *)binaryFormat,
        (GLvoid *)binary
    );
    return NOERROR;
}

// C function void glProgramBinary ( GLuint program, GLenum binaryFormat, const GLvoid *binary, GLsizei length )

ECode GLES30::GlProgramBinary(
    /* [in] */ Int32 program,
    /* [in] */ Int32 binaryFormat,
    /* [in] */ IBuffer* binary_buf,
    /* [in] */ Int32 length)
{
    GET_BUFFER_GL_DATA(binary, GLvoid)
    glProgramBinary(
        (GLuint)program,
        (GLenum)binaryFormat,
        (GLvoid *)binary,
        (GLsizei)length
    );
    return NOERROR;
}

// C function void glProgramParameteri ( GLuint program, GLenum pname, GLint value )

ECode GLES30::GlProgramParameteri(
    /* [in] */ Int32 program,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 value)
{
    glProgramParameteri(
        (GLuint)program,
        (GLenum)pname,
        (GLint)value
    );
    return NOERROR;
}

// C function void glInvalidateFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments )

ECode GLES30::GlInvalidateFramebuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 numAttachments,
    /* [in] */ ArrayOf<Int32>* attachments_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(attachments, GLenum)
    glInvalidateFramebuffer(
        (GLenum)target,
        (GLsizei)numAttachments,
        (GLenum *)attachments
    );
    GL_EXIT()
}

// C function void glInvalidateFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments )

ECode GLES30::GlInvalidateFramebuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 numAttachments,
    /* [in] */ IInt32Buffer* attachments_buf)
{
    GET_BUFFER_GL_DATA(attachments, GLenum)
    glInvalidateFramebuffer(
        (GLenum)target,
        (GLsizei)numAttachments,
        (GLenum *)attachments
    );
    return NOERROR;
}

// C function void glInvalidateSubFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height )

ECode GLES30::GlInvalidateSubFramebuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 numAttachments,
    /* [in] */ ArrayOf<Int32>* attachments_ref,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GET_REF_GL_DATA_BEGIN(attachments, GLenum)
    glInvalidateSubFramebuffer(
        (GLenum)target,
        (GLsizei)numAttachments,
        (GLenum *)attachments,
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height
    );
    GL_EXIT()
}

// C function void glInvalidateSubFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height )

ECode GLES30::GlInvalidateSubFramebuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 numAttachments,
    /* [in] */ IInt32Buffer* attachments_buf,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GET_BUFFER_GL_DATA(attachments, GLenum)
    glInvalidateSubFramebuffer(
        (GLenum)target,
        (GLsizei)numAttachments,
        (GLenum *)attachments,
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

// C function void glTexStorage2D ( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height )

ECode GLES30::GlTexStorage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 levels,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glTexStorage2D(
        (GLenum)target,
        (GLsizei)levels,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

// C function void glTexStorage3D ( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth )

ECode GLES30::GlTexStorage3D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 levels,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth)
{
    glTexStorage3D(
        (GLenum)target,
        (GLsizei)levels,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth
    );
    return NOERROR;
}

// C function void glGetInternalformativ ( GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params )

ECode GLES30::GlGetInternalformativ(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 bufSize,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_BEGIN(params, GLint)
    glGetInternalformativ(
        (GLenum)target,
        (GLenum)internalformat,
        (GLenum)pname,
        (GLsizei)bufSize,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetInternalformativ ( GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params )

ECode GLES30::GlGetInternalformativ(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 bufSize,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetInternalformativ(
        (GLenum)target,
        (GLenum)internalformat,
        (GLenum)pname,
        (GLsizei)bufSize,
        (GLint *)params
    );
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos