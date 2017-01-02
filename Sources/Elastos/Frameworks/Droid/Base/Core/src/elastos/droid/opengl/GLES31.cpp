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
#include "elastos/droid/opengl/GLES31.h"

#include <stdint.h>
#include <GLES3/gl31.h>
#include <utils/misc.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

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

static ECode GetPointer(
    /* [in] */ IBuffer* buffer,
    /* [in, out] */ Handle64* array,
    /* [in, out] */ Int32* remaining,
    /* [in, out] */ Int32* offset,
    /* [out] */ Handle64* rst)
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
        *rst = (Handle64)(pointer);
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
    /* [out] */ Handle64* result)
{
    VALIDATE_NOT_NULL(result)

    Handle64 effectiveDirectAddress;
    buffer->GetEffectiveDirectAddress(&effectiveDirectAddress);
    if (effectiveDirectAddress != 0) {
        Int32 position, elementSizeShift;
        buffer->GetPosition(&position);
        buffer->GetElementSizeShift(&elementSizeShift);
        effectiveDirectAddress += (position << elementSizeShift);
    } else {
        SLOGGERE("GLES31", "GetDirectBufferPointer: Must use a native order direct Buffer")
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
        SLOGGERD("CGLES31", _exceptionMessage)
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
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    Handle64 data;
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
        SLOGGERD("CGLES31", _exceptionMessage)
    }
    return _exceptionType;
}

#define GET_BUFFER_GL_DATA(_Name, GLTYPE)                                                                                 \
    Handle64 _array, _dataHandle;                                                                                                      \
    Int32 _bufferOffset = (Int32) 0;                                                                                                      \
    Int32 _remaining;                                                                                                                           \
    GLTYPE *_Name = (GLTYPE *) 0;                                                                                               \
    FAIL_RETURN(GetPointer(IBuffer::Probe(_Name##_buf), &_array, &_remaining, &_bufferOffset, &_dataHandle))      \
    _Name = (GLTYPE *)_dataHandle;                                                                                            \
    if (_Name##_buf && _Name == NULL) {                                                                                        \
        char * _dataBase = reinterpret_cast<char *>(_array);                                                                \
        _Name = (GLTYPE *) (_dataBase + _bufferOffset);                                                              \
    }

#define GET_BUFFER_GL_DATA_CONTINUE(_Name, GLTYPE)                                    \
    GLTYPE *_Name = (GLTYPE *) 0;                                                                                               \
    FAIL_RETURN(GetPointer(IBuffer::Probe(_Name##_buf), &_array, &_remaining, &_bufferOffset, &_dataHandle))      \
    _Name = (GLTYPE *)_dataHandle;                                                                                            \
    if (_Name##_buf && _Name == NULL) {                                                                                        \
        char * _dataBase = reinterpret_cast<char *>(_array);                                                                \
        _Name = (GLTYPE *) (_dataBase + _bufferOffset);                                                              \
    }

#define GET_REF_GL_DATA_ENTER(_Name, GLTYPE)                                                             \
    Int32 _exception = 0;                                                             \
    ECode _exceptionType = NOERROR;                                                             \
    const char * _exceptionMessage = NULL;                                                             \
    GLTYPE *_Name##_base = (GLTYPE *) 0;                                                             \
    GLTYPE *_Name = (GLTYPE *) 0;                                                             \
    if (!_Name##_ref) {                                                             \
        _exception = 1;                                                             \
        String _Name##Str(#_Name);                            \
        _exceptionMessage = (_Name##Str + " == null").string();                                                             \
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

#define GET_REF_GL_DATA_BEGIN(_Name, GLTYPE)                                                             \
    GLTYPE *_Name##_base = (GLTYPE *) 0;                                                             \
    GLTYPE *_Name = (GLTYPE *) 0;                                                             \
    if (!_Name##_ref) {                                                             \
        String _Name##Str(#_Name);                                    \
        SLOGGERD("CGLES31", _Name##Str += " == null")                                                             \
        return E_ILLEGAL_ARGUMENT_EXCEPTION;                                   \
    }                                                             \
    if (_Name##Offset < 0) {                                                             \
        String _Name##Str(#_Name);                                    \
        SLOGGERD("CGLES31", _Name##Str += "Offset == null")                                                             \
        return E_ILLEGAL_ARGUMENT_EXCEPTION;                                   \
    }                                                             \
    _Name##_base = (GLTYPE *) _Name##_ref->GetPayload();                                                             \
    _Name = _Name##_base + _Name##Offset;

#define GET_REF_GL_DATA_CONTINUE(_Name, GLTYPE)                                      \
    GLTYPE *_Name##_base = (GLTYPE *) 0;                                                             \
    GLTYPE *_Name = (GLTYPE *) 0;                                                             \
    if (!_Name##_ref) {                                                             \
        String _Name##Str(#_Name);                                    \
        SLOGGERD("CGLES31", _Name##Str += " == null")                                                             \
        return E_ILLEGAL_ARGUMENT_EXCEPTION;                                    \
    }                                                             \
    if (_Name##Offset < 0) {                                                             \
        String _Name##Str(#_Name);                                    \
        SLOGGERD("CGLES31", _Name##Str += "Offset == null")                                                             \
        return E_ILLEGAL_ARGUMENT_EXCEPTION;                                   \
    }                                                             \
    _Name##_base = (GLTYPE *) _Name##_ref->GetPayload();                                                             \
    _Name = _Name##_base + _Name##Offset;


#define GL_EXIT()                                                             \
exit:                                                             \
    if (_exception) {                                                             \
        SLOGGERD("CGLES30", _exceptionMessage)                                                             \
        return _exceptionType;                                                             \
    }                                                             \
    return _exceptionType;

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(GLES31, GLES30, IGLES31)
// C function void glDispatchCompute ( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z )

ECode GLES31::GlDispatchCompute(
    /* [in] */ Int32 num_groups_x,
    /* [in] */ Int32 num_groups_y,
    /* [in] */ Int32 num_groups_z)
{
    glDispatchCompute(
        (GLuint)num_groups_x,
        (GLuint)num_groups_y,
        (GLuint)num_groups_z
    );
    return NOERROR;
}

// C function void glDispatchComputeIndirect ( GLintptr indirect );

ECode GLES31::GlDispatchComputeIndirect(
    /* [in] */ Int64 indirect)
{
    // 'indirect' is a byte offset, not a pointer. GL checks for negative and too-large values.
    // Here we only need to check for successful 64-bit to 32-bit conversion.
    // - jlong is a int64_t (jni.h)
    // - GLintptr is a long (khrplatform.h)
    if (sizeof(GLintptr) != sizeof(Int64) && (indirect < LONG_MIN || indirect > LONG_MAX)) {
        // jniThrowException(_env, "java/lang/IllegalArgumentException", "indirect offset too large");
        SLOGGERE("GLES31", "indirect offset too large")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    glDispatchComputeIndirect((GLintptr)indirect);
    return NOERROR;
}

// C function void glDrawArraysIndirect ( GLenum mode, const void *indirect );

ECode GLES31::GlDrawArraysIndirect(
    /* [in] */ Int32 mode,
    /* [in] */ Int64 indirect)
{
    // In OpenGL ES, 'indirect' is a byte offset into a buffer, not a raw pointer.
    // GL checks for too-large values. Here we only need to check for successful signed 64-bit
    // to unsigned 32-bit conversion.
    if (sizeof(void*) != sizeof(Int64) && indirect > UINTPTR_MAX) {
        // jniThrowException(_env, "java/lang/IllegalArgumentException", "indirect offset too large");
        SLOGGERE("GLES31", "indirect offset too large")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    glDrawArraysIndirect(mode, (const void*)indirect);
    return NOERROR;
}

// C function glDrawElementsIndirect ( GLenum mode, GLenum type, const void *indirect );

ECode GLES31::GlDrawElementsIndirect(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 type,
    /* [in] */ Int64 indirect)
{
    // In OpenGL ES, 'indirect' is a byte offset into a buffer, not a raw pointer.
    // GL checks for too-large values. Here we only need to check for successful signed 64-bit
    // to unsigned 32-bit conversion.
    if (sizeof(void*) != sizeof(Int64) && indirect > UINTPTR_MAX) {
        SLOGGERE("GLES31", "indirect offset too large")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    glDrawElementsIndirect(mode, type, (const void*)indirect);
    return NOERROR;
}

// C function void glFramebufferParameteri ( GLenum target, GLenum pname, GLint param )

ECode GLES31::GlFramebufferParameteri(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glFramebufferParameteri(
        (GLenum)target,
        (GLenum)pname,
        (GLint)param
    );
    return NOERROR;
}

// C function void glGetFramebufferParameteriv ( GLenum target, GLenum pname, GLint *params )

ECode GLES31::GlGetFramebufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLint)
    glGetFramebufferParameteriv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetFramebufferParameteriv ( GLenum target, GLenum pname, GLint *params )

ECode GLES31::GlGetFramebufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetFramebufferParameteriv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetProgramInterfaceiv ( GLuint program, GLenum programInterface, GLenum pname, GLint *params )

ECode GLES31::GlGetProgramInterfaceiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 programInterface,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLint)
    glGetProgramInterfaceiv(
        (GLuint)program,
        (GLenum)programInterface,
        (GLenum)pname,
        (GLint *)params
    );
   GL_EXIT()
}

// C function void glGetProgramInterfaceiv ( GLuint program, GLenum programInterface, GLenum pname, GLint *params )

ECode GLES31::GlGetProgramInterfaceiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 programInterface,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetProgramInterfaceiv(
        (GLuint)program,
        (GLenum)programInterface,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function GLuint glGetProgramResourceIndex ( GLuint program, GLenum programInterface, const GLchar *name )

ECode GLES31::GlGetProgramResourceIndex(
    /* [in] */ Int32 program,
    /* [in] */ Int32 programInterface,
    /* [in] */ const String& name,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    GLuint _returnValue = 0;
    const char* _nativename = 0;

    if (name == NULL) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "name == null";
        goto exit;
    }
    _nativename = name.string();

    _returnValue = glGetProgramResourceIndex(
        (GLuint)program,
        (GLenum)programInterface,
        (GLchar *)_nativename
    );

exit:
    if (_exception) {
        SLOGGERE("GLES31", _exceptionMessage)
    }
    *index = _returnValue;
    return _exceptionType;
}

// C function void glGetProgramResourceName ( GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name )

ECode GLES31::GlGetProgramResourceName(
    /* [in] */ Int32 program,
    /* [in] */ Int32 programInterface,
    /* [in] */ Int32 index,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    SLOGGERE("GLES31", "not yet implemented");
    *result = NULL;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function void glGetProgramResourceiv ( GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params )

ECode GLES31::GlGetProgramResourceiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 programInterface,
    /* [in] */ Int32 index,
    /* [in] */ Int32 propCount,
    /* [in] */ ArrayOf<Int32>* props_ref,
    /* [in] */ Int32 propsOffset,
    /* [in] */ Int32 bufSize,
    /* [in] */ ArrayOf<Int32>* length_ref,
    /* [in] */ Int32 lengthOffset,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 paramsOffset)
{
    GET_REF_GL_DATA_BEGIN(props, GLenum)
    GET_REF_GL_DATA_CONTINUE(length, GLsizei)
    GET_REF_GL_DATA_CONTINUE(params, GLint)
    glGetProgramResourceiv(
        (GLuint)program,
        (GLenum)programInterface,
        (GLuint)index,
        (GLsizei)propCount,
        (GLenum *)props,
        (GLsizei)bufSize,
        (GLsizei *)length,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetProgramResourceiv ( GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params )

ECode GLES31::GlGetProgramResourceiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 programInterface,
    /* [in] */ Int32 index,
    /* [in] */ Int32 propCount,
    /* [in] */ IInt32Buffer* props_buf,
    /* [in] */ Int32 bufSize,
    /* [in] */ IInt32Buffer* length_buf,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(props, GLenum)
    GET_BUFFER_GL_DATA_CONTINUE(length, GLsizei)
    GET_BUFFER_GL_DATA_CONTINUE(params, GLint)
    glGetProgramResourceiv(
        (GLuint)program,
        (GLenum)programInterface,
        (GLuint)index,
        (GLsizei)propCount,
        (GLenum *)props,
        (GLsizei)bufSize,
        (GLsizei *)length,
        (GLint *)params
    );
    return NOERROR;
}

// C function GLint glGetProgramResourceLocation ( GLuint program, GLenum programInterface, const GLchar *name )

ECode GLES31::GlGetProgramResourceLocation(
    /* [in] */ Int32 program,
    /* [in] */ Int32 programInterface,
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    GLuint _returnValue = 0;
    const char* _nativename = 0;

    if (name == NULL) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "name == null";
        goto exit;
    }
    _nativename = name.string();

    _returnValue = glGetProgramResourceLocation(
        (GLuint)program,
        (GLenum)programInterface,
        (GLchar *)_nativename
    );

exit:
    if (_exception) {
        SLOGGERE("GLES31", _exceptionMessage)
    }
    *result = _returnValue;
    return _exceptionType;
}

// C function void glUseProgramStages ( GLuint pipeline, GLbitfield stages, GLuint program )

ECode GLES31::GlUseProgramStages(
    /* [in] */ Int32 pipeline,
    /* [in] */ Int32 stages,
    /* [in] */ Int32 program)
{
    glUseProgramStages(
        (GLuint)pipeline,
        (GLbitfield)stages,
        (GLuint)program
    );
    return NOERROR;
}

// C function void glActiveShaderProgram ( GLuint pipeline, GLuint program )

ECode GLES31::GlActiveShaderProgram(
    /* [in] */ Int32 pipeline,
    /* [in] */ Int32 program)
{
    glActiveShaderProgram(
        (GLuint)pipeline,
        (GLuint)program
    );
    return NOERROR;
}

// C function GLuint glCreateShaderProgramv ( GLenum type, GLsizei count, const GLchar *const *strings )

ECode GLES31::GlCreateShaderProgramv(
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<String>* strings,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    SLOGGERE("GLES31", "not yet implemented");
    *result = 0;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function void glBindProgramPipeline ( GLuint pipeline )

ECode GLES31::GlBindProgramPipeline(
    /* [in] */ Int32 pipeline)
{
    glBindProgramPipeline(
        (GLuint)pipeline
    );
    return NOERROR;
}

// C function void glDeleteProgramPipelines ( GLsizei n, const GLuint *pipelines )

ECode GLES31::GlDeleteProgramPipelines(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* pipelines_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(pipelines, GLuint)
    glDeleteProgramPipelines(
        (GLsizei)n,
        (GLuint *)pipelines
    );
    GL_EXIT()
}

// C function void glDeleteProgramPipelines ( GLsizei n, const GLuint *pipelines )

ECode GLES31::GlDeleteProgramPipelines(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* pipelines_buf)
{
    GET_BUFFER_GL_DATA(pipelines, GLuint)
    glDeleteProgramPipelines(
        (GLsizei)n,
        (GLuint *)pipelines
    );
    return NOERROR;
}

// C function void glGenProgramPipelines ( GLsizei n, GLuint *pipelines )

ECode GLES31::GlGenProgramPipelines(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* pipelines_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(pipelines, GLuint)
    glGenProgramPipelines(
        (GLsizei)n,
        (GLuint *)pipelines
    );
    GL_EXIT()
}

// C function void glGenProgramPipelines ( GLsizei n, GLuint *pipelines )

ECode GLES31::GlGenProgramPipelines(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* pipelines_buf)
{
    GET_BUFFER_GL_DATA(pipelines, GLuint)
    glGenProgramPipelines(
        (GLsizei)n,
        (GLuint *)pipelines
    );
    return NOERROR;
}

// C function GLboolean glIsProgramPipeline ( GLuint pipeline )

ECode GLES31::GlIsProgramPipeline(
    /* [in] */ Int32 pipeline,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue = glIsProgramPipeline(
        (GLuint)pipeline
    );
    *result = _returnValue;
    return NOERROR;
}

// C function void glGetProgramPipelineiv ( GLuint pipeline, GLenum pname, GLint *params )

ECode GLES31::GlGetProgramPipelineiv(
    /* [in] */ Int32 pipeline,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLint)
    glGetProgramPipelineiv(
        (GLuint)pipeline,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetProgramPipelineiv ( GLuint pipeline, GLenum pname, GLint *params )

ECode GLES31::GlGetProgramPipelineiv(
    /* [in] */ Int32 pipeline,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetProgramPipelineiv(
        (GLuint)pipeline,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glProgramUniform1i ( GLuint program, GLint location, GLint v0 )

ECode GLES31::GlProgramUniform1i(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0)
{
    glProgramUniform1i(
        (GLuint)program,
        (GLint)location,
        (GLint)v0
    );
    return NOERROR;
}

// C function void glProgramUniform2i ( GLuint program, GLint location, GLint v0, GLint v1 )

ECode GLES31::GlProgramUniform2i(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1)
{
    glProgramUniform2i(
        (GLuint)program,
        (GLint)location,
        (GLint)v0,
        (GLint)v1
    );
    return NOERROR;
}

// C function void glProgramUniform3i ( GLuint program, GLint location, GLint v0, GLint v1, GLint v2 )

ECode GLES31::GlProgramUniform3i(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1,
    /* [in] */ Int32 v2)
{
    glProgramUniform3i(
        (GLuint)program,
        (GLint)location,
        (GLint)v0,
        (GLint)v1,
        (GLint)v2
    );
    return NOERROR;
}

// C function void glProgramUniform4i ( GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )

ECode GLES31::GlProgramUniform4i(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1,
    /* [in] */ Int32 v2,
    /* [in] */ Int32 v3)
{
    glProgramUniform4i(
        (GLuint)program,
        (GLint)location,
        (GLint)v0,
        (GLint)v1,
        (GLint)v2,
        (GLint)v3
    );
    return NOERROR;
}

// C function void glProgramUniform1ui ( GLuint program, GLint location, GLuint v0 )

ECode GLES31::GlProgramUniform1ui(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0)
{
    glProgramUniform1ui(
        (GLuint)program,
        (GLint)location,
        (GLuint)v0
    );
    return NOERROR;
}

// C function void glProgramUniform2ui ( GLuint program, GLint location, GLuint v0, GLuint v1 )

ECode GLES31::GlProgramUniform2ui(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1)
{
    glProgramUniform2ui(
        (GLuint)program,
        (GLint)location,
        (GLuint)v0,
        (GLuint)v1
    );
    return NOERROR;
}

// C function void glProgramUniform3ui ( GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2 )

ECode GLES31::GlProgramUniform3ui(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1,
    /* [in] */ Int32 v2)
{
    glProgramUniform3ui(
        (GLuint)program,
        (GLint)location,
        (GLuint)v0,
        (GLuint)v1,
        (GLuint)v2
    );
    return NOERROR;
}

// C function void glProgramUniform4ui ( GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 )

ECode GLES31::GlProgramUniform4ui(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 v0,
    /* [in] */ Int32 v1,
    /* [in] */ Int32 v2,
    /* [in] */ Int32 v3)
{
    glProgramUniform4ui(
        (GLuint)program,
        (GLint)location,
        (GLuint)v0,
        (GLuint)v1,
        (GLuint)v2,
        (GLuint)v3
    );
    return NOERROR;
}

// C function void glProgramUniform1f ( GLuint program, GLint location, GLfloat v0 )

ECode GLES31::GlProgramUniform1f(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Float v0)
{
    glProgramUniform1f(
        (GLuint)program,
        (GLint)location,
        (GLfloat)v0
    );
    return NOERROR;
}

// C function void glProgramUniform2f ( GLuint program, GLint location, GLfloat v0, GLfloat v1 )

ECode GLES31::GlProgramUniform2f(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Float v0,
    /* [in] */ Float v1)
{
    glProgramUniform2f(
        (GLuint)program,
        (GLint)location,
        (GLfloat)v0,
        (GLfloat)v1
    );
    return NOERROR;
}

// C function void glProgramUniform3f ( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )

ECode GLES31::GlProgramUniform3f(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Float v0,
    /* [in] */ Float v1,
    /* [in] */ Float v2)
{
    glProgramUniform3f(
        (GLuint)program,
        (GLint)location,
        (GLfloat)v0,
        (GLfloat)v1,
        (GLfloat)v2
    );
    return NOERROR;
}

// C function void glProgramUniform4f ( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )

ECode GLES31::GlProgramUniform4f(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Float v0,
    /* [in] */ Float v1,
    /* [in] */ Float v2,
    /* [in] */ Float v3)
{
    glProgramUniform4f(
        (GLuint)program,
        (GLint)location,
        (GLfloat)v0,
        (GLfloat)v1,
        (GLfloat)v2,
        (GLfloat)v3
    );
    return NOERROR;
}

// C function void glProgramUniform1iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform1iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLint)
    glProgramUniform1iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform1iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform1iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLint)
    glProgramUniform1iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform2iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform2iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLint)
    glProgramUniform2iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform2iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform2iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLint)
    glProgramUniform2iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform3iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform3iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLint)
    glProgramUniform3iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform3iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform3iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLint)
    glProgramUniform3iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform4iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform4iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLint)
    glProgramUniform4iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform4iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

ECode GLES31::GlProgramUniform4iv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLint)
    glProgramUniform4iv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform1uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform1uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLuint)
    glProgramUniform1uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform1uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform1uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLuint)
    glProgramUniform1uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform2uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform2uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLint)
    glProgramUniform2uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform2uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform2uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLint)
    glProgramUniform2uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform3uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform3uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLint)
    glProgramUniform3uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform3uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform3uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLint)
    glProgramUniform3uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform4uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform4uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLuint)
    glProgramUniform4uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform4uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

ECode GLES31::GlProgramUniform4uiv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLuint)
    glProgramUniform4uiv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLuint *)value
    );
    return NOERROR;
}

// C function void glProgramUniform1fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform1fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniform1fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    GL_EXIT()
}
// C function void glProgramUniform1fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform1fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniform1fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniform2fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniform1fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform2fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniform1fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniform3fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniform3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform3fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniform3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniform4fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniform4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniform4fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

ECode GLES31::GlProgramUniform4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniform4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix2fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix2fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix2x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix2x3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix2x3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix2x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix2x3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix2x3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix3x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix3x2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix3x2fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix3x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix3x2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix3x2fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix2x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix2x4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix2x4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix2x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix2x4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix2x4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix4x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix4x2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix4x2fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix4x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix4x2fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix4x2fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix3x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix3x4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix3x4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix3x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix3x4fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix3x4fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glProgramUniformMatrix4x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix4x3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ ArrayOf<Float>* value_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(value, GLfloat)
    glProgramUniformMatrix4x3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    GL_EXIT()
}

// C function void glProgramUniformMatrix4x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

ECode GLES31::GlProgramUniformMatrix4x3fv(
    /* [in] */ Int32 program,
    /* [in] */ Int32 location,
    /* [in] */ Int32 count,
    /* [in] */ Boolean transpose,
    /* [in] */ IFloatBuffer* value_buf)
{
    GET_BUFFER_GL_DATA(value, GLfloat)
    glProgramUniformMatrix4x3fv(
        (GLuint)program,
        (GLint)location,
        (GLsizei)count,
        (GLboolean)transpose,
        (GLfloat *)value
    );
    return NOERROR;
}

// C function void glValidateProgramPipeline ( GLuint pipeline )

ECode GLES31::GlValidateProgramPipeline(
    /* [in] */ Int32 pipeline)
{
    return NOERROR;
}

// C function void glGetProgramPipelineInfoLog( GLuint program, GLsizei maxLength, GLsizei * length, GLchar * infoLog);

ECode GLES31::GlGetProgramPipelineInfoLog(
    /* [in] */ Int32 shader,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    GLint infoLen = 0;
    glGetProgramPipelineiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (!infoLen) {
        *result = String("");
        return NOERROR;
    }
    char* buf = (char*) malloc(infoLen);
    if (buf == NULL) {
        SLOGGERE("GLES31","out of memory" )
        *result = NULL;
        return E_OUT_OF_MEMORY_ERROR;
    }
    glGetProgramPipelineInfoLog(shader, infoLen, NULL, buf);
    *result = String(buf);
    free(buf);
    return NOERROR;
}

// C function void glBindImageTexture ( GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format )

ECode GLES31::GlBindImageTexture(
    /* [in] */ Int32 unit,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level,
    /* [in] */ Boolean layered,
    /* [in] */ Int32 layer,
    /* [in] */ Int32 access,
    /* [in] */ Int32 format)
{
    glBindImageTexture(
        (GLuint)unit,
        (GLuint)texture,
        (GLint)level,
        (GLboolean)layered,
        (GLint)layer,
        (GLenum)access,
        (GLenum)format
    );
    return NOERROR;
}

// C function void glGetBooleani_v ( GLenum target, GLuint index, GLboolean *data )

ECode GLES31::GlGetBooleani_v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Boolean>* data_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(data, GLboolean)
    glGetBooleani_v(
        (GLenum)target,
        (GLuint)index,
        (GLboolean *)data
    );
    GL_EXIT()
}

// C function void glGetBooleani_v ( GLenum target, GLuint index, GLboolean *data )

ECode GLES31::GlGetBooleani_v(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [in] */ IInt32Buffer* data_buf)
{
    GET_BUFFER_GL_DATA(data, GLboolean)
    glGetBooleani_v(
        (GLenum)target,
        (GLuint)index,
        (GLboolean *)data
    );
    return NOERROR;
}

// C function void glMemoryBarrier ( GLbitfield barriers )

ECode GLES31::GlMemoryBarrier(
    /* [in] */ Int32 barriers)
{
    glMemoryBarrier(
        (GLbitfield)barriers
    );
    return NOERROR;
}

// C function void glMemoryBarrierByRegion ( GLbitfield barriers )

ECode GLES31::GlMemoryBarrierByRegion(
    /* [in] */ Int32 barriers)
{
    glMemoryBarrierByRegion(
        (GLbitfield)barriers
    );
    return NOERROR;
}

// C function void glTexStorage2DMultisample ( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )

ECode GLES31::GlTexStorage2DMultisample(
    /* [in] */ Int32 target,
    /* [in] */ Int32 samples,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean fixedsamplelocations)
{
    glTexStorage2DMultisample(
        (GLenum)target,
        (GLsizei)samples,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLboolean)fixedsamplelocations
    );
    return NOERROR;
}

// C function void glGetMultisamplefv ( GLenum pname, GLuint index, GLfloat *val )

ECode GLES31::GlGetMultisamplefv(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Float>* val_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(val, GLfloat)
    glGetMultisamplefv(
        (GLenum)pname,
        (GLuint)index,
        (GLfloat *)val
    );
    GL_EXIT()
}

// C function void glGetMultisamplefv ( GLenum pname, GLuint index, GLfloat *val )

ECode GLES31::GlGetMultisamplefv(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 index,
    /* [in] */ IFloatBuffer* val_buf)
{
    GET_BUFFER_GL_DATA(val, GLfloat)
    glGetMultisamplefv(
        (GLenum)pname,
        (GLuint)index,
        (GLfloat *)val
    );
    return NOERROR;
}

// C function void glSampleMaski ( GLuint maskNumber, GLbitfield mask )

ECode GLES31::GlSampleMaski(
    /* [in] */ Int32 maskNumber,
    /* [in] */ Int32 mask)
{
    glSampleMaski(
        (GLuint)maskNumber,
        (GLbitfield)mask
    );
    return NOERROR;
}

// C function void glGetTexLevelParameteriv ( GLenum target, GLint level, GLenum pname, GLint *params )

ECode GLES31::GlGetTexLevelParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLint)
    glGetTexLevelParameteriv(
        (GLenum)target,
        (GLint)level,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetTexLevelParameteriv ( GLenum target, GLint level, GLenum pname, GLint *params )

ECode GLES31::GlGetTexLevelParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetTexLevelParameteriv(
        (GLenum)target,
        (GLint)level,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetTexLevelParameterfv ( GLenum target, GLint level, GLenum pname, GLfloat *params )

ECode GLES31::GlGetTexLevelParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLfloat)
    glGetTexLevelParameterfv(
        (GLenum)target,
        (GLint)level,
        (GLenum)pname,
        (GLfloat *)params
    );
    GL_EXIT()
}

// C function void glGetTexLevelParameterfv ( GLenum target, GLint level, GLenum pname, GLfloat *params )

ECode GLES31::GlGetTexLevelParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLfloat)
    glGetTexLevelParameterfv(
        (GLenum)target,
        (GLint)level,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

// C function void glBindVertexBuffer ( GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride )

ECode GLES31::GlBindVertexBuffer(
    /* [in] */ Int32 bindingindex,
    /* [in] */ Int32 buffer,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 stride)
{
    if (sizeof(GLintptr) != sizeof(Int64) && (offset < LONG_MIN || offset > LONG_MAX)) {
        SLOGGERD("GLES31", "offset too large")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    glBindVertexBuffer(
        (GLuint)bindingindex,
        (GLuint)buffer,
        (GLintptr)offset,
        (GLsizei)stride
    );
    return NOERROR;
}

// C function void glVertexAttribFormat ( GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset )

ECode GLES31::GlVertexAttribFormat(
    /* [in] */ Int32 attribindex,
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Boolean normalized,
    /* [in] */ Int32 relativeoffset)
{
    glVertexAttribFormat(
        (GLuint)attribindex,
        (GLint)size,
        (GLenum)type,
        (GLboolean)normalized,
        (GLuint)relativeoffset
    );
    return NOERROR;
}

// C function void glVertexAttribIFormat ( GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset )

ECode GLES31::GlVertexAttribIFormat(
    /* [in] */ Int32 attribindex,
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 relativeoffset)
{
    glVertexAttribIFormat(
        (GLuint)attribindex,
        (GLint)size,
        (GLenum)type,
        (GLuint)relativeoffset
    );
    return NOERROR;
}

// C function void glVertexAttribBinding ( GLuint attribindex, GLuint bindingindex )

ECode GLES31::GlVertexAttribBinding(
    /* [in] */ Int32 attribindex,
    /* [in] */ Int32 bindingindex)
{
    glVertexAttribBinding(
        (GLuint)attribindex,
        (GLuint)bindingindex
    );
    return NOERROR;
}

// C function void glVertexBindingDivisor ( GLuint bindingindex, GLuint divisor )

ECode GLES31::GlVertexBindingDivisor(
    /* [in] */ Int32 bindingindex,
    /* [in] */ Int32 divisor)
{
    glVertexBindingDivisor(
        (GLuint)bindingindex,
        (GLuint)divisor
    );
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
