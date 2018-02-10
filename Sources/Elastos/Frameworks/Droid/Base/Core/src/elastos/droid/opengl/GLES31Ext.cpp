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

#include "elastos/droid/opengl/GLES31Ext.h"

#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>

#include <stdint.h>
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
        SLOGGERE("GLES31Ext", "GetDirectBufferPointer: Must use a native order direct Buffer")
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
        SLOGGERD("CGLES31", _exceptionMessage)
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

CAR_INTERFACE_IMPL(GLES31Ext, Singleton, IGLES31Ext)

ECode GLES31Ext::GlBlendBarrierKHR()
{
    glBlendBarrierKHR();
    return NOERROR;
}

// C function void glDebugMessageControlKHR ( GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled )

ECode GLES31Ext::GlDebugMessageControlKHR(
    /* [in] */ Int32 source,
    /* [in] */ Int32 type,
    /* [in] */ Int32 severity,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* ids_ref,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean enabled)
{
    GET_REF_GL_DATA_ENTER(ids, GLuint)
    glDebugMessageControlKHR(
        (GLenum)source,
        (GLenum)type,
        (GLenum)severity,
        (GLsizei)count,
        (GLuint *)ids,
        (GLboolean)enabled
    );
    GL_EXIT()
}

// C function void glDebugMessageControlKHR ( GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled )

ECode GLES31Ext::GlDebugMessageControlKHR(
    /* [in] */ Int32 source,
    /* [in] */ Int32 type,
    /* [in] */ Int32 severity,
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* ids_buf,
    /* [in] */ Boolean enabled)
{
    GET_BUFFER_GL_DATA(ids, GLuint)
    glDebugMessageControlKHR(
        (GLenum)source,
        (GLenum)type,
        (GLenum)severity,
        (GLsizei)count,
        (GLuint *)ids,
        (GLboolean)enabled
    );
    return NOERROR;
}

// C function void glDebugMessageInsertKHR ( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf )

ECode GLES31Ext::GlDebugMessageInsertKHR(
    /* [in] */ Int32 source,
    /* [in] */ Int32 type,
    /* [in] */ Int32 id,
    /* [in] */ Int32 severity,
    /* [in] */ const String& buf)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    const char* _nativebuf = 0;
    Int32 _length = 0;

    if (!buf) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "buf == null";
        goto exit;
    }
    _nativebuf = buf.string();
    _length = buf.GetLength();

    glDebugMessageInsertKHR(
        (GLenum)source,
        (GLenum)type,
        (GLuint)id,
        (GLenum)severity,
        (GLsizei)_length,
        (GLchar *)_nativebuf
    );

exit:
    if (_exception) {
        SLOGGERE("GLES31Ext", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glDebugMessageCallbackKHR ( GLDEBUGPROCKHR callback, const void *userParam )

ECode GLES31Ext::GlDebugMessageCallbackKHR(
    /* [in] */ IDebugProcKHR* cb)
{
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

ECode GLES31Ext::GlGetDebugMessageLogKHR(
    /* [in] */ Int32 count,
    /* [in] */ Int32 bufSize,
    /* [in] */ ArrayOf<Int32>* sources,
    /* [in] */ Int32 sourcesOffset,
    /* [in] */ ArrayOf<Int32>* types,
    /* [in] */ Int32 typesOffset,
    /* [in] */ ArrayOf<Int32>* ids,
    /* [in] */ Int32 idsOffset,
    /* [in] */ ArrayOf<Int32>* severities,
    /* [in] */ Int32 severitiesOffset,
    /* [in] */ ArrayOf<Int32>* lengths,
    /* [in] */ Int32 lengthsOffset,
    /* [in] */ ArrayOf<Byte>* messageLog,
    /* [in] */ Int32 messageLogOffset,
    /* [out] */ Int32* result)
{
    *result = 0;
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

ECode GLES31Ext::GlGetDebugMessageLogKHR(
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* sources,
    /* [in] */ IInt32Buffer* types,
    /* [in] */ IInt32Buffer* ids,
    /* [in] */ IInt32Buffer* severities,
    /* [in] */ IInt32Buffer* lengths,
    /* [in] */ IByteBuffer* messageLog,
    /* [out] */ Int32* result)
{
    *result = 0;
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

ECode GLES31Ext::GlGetDebugMessageLogKHR(
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* sources,
    /* [in] */ Int32 sourcesOffset,
    /* [in] */ ArrayOf<Int32>* types,
    /* [in] */ Int32 typesOffset,
    /* [in] */ ArrayOf<Int32>* ids,
    /* [in] */ Int32 idsOffset,
    /* [in] */ ArrayOf<Int32>* severities,
    /* [in] */ Int32 severitiesOffset,
    /* [out, callee] */ ArrayOf<String>** result)
{
    *result = NULL;
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

ECode GLES31Ext::GlGetDebugMessageLogKHR(
    /* [in] */ Int32 count,
    /* [in] */ IInt32Buffer* sources,
    /* [in] */ IInt32Buffer* types,
    /* [in] */ IInt32Buffer* ids,
    /* [in] */ IInt32Buffer* severities,
    /* [out, callee] */ ArrayOf<String>** result)
{
    *result = NULL;
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function void glPushDebugGroupKHR ( GLenum source, GLuint id, GLsizei length, const GLchar *message )

ECode GLES31Ext::GlPushDebugGroupKHR(
    /* [in] */ Int32 source,
    /* [in] */ Int32 id,
    /* [in] */ Int32 length,
    /* [in] */ const String& message)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    const char* _nativemessage = 0;

    if (message == NULL) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "message == null";
        goto exit;
    }
    _nativemessage = message.string();

    glPushDebugGroupKHR(
        (GLenum)source,
        (GLuint)id,
        (GLsizei)length,
        (GLchar *)_nativemessage
    );

exit:
    if (_exception) {
        SLOGGERE("GLES31Ext", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glPopDebugGroupKHR ( void )

ECode GLES31Ext::GlPopDebugGroupKHR()
{
    glPopDebugGroupKHR();
    return NOERROR;
}

// C function void glObjectLabelKHR ( GLenum identifier, GLuint name, GLsizei length, const GLchar *label )

ECode GLES31Ext::GlObjectLabelKHR(
    /* [in] */ Int32 identifier,
    /* [in] */ Int32 name,
    /* [in] */ Int32 length,
    /* [in] */ const String& label)
{
    Int32 _exception = 0;
    ECode _exceptionType = NOERROR;
    const char * _exceptionMessage = NULL;
    const char* _nativelabel = 0;

    if (label == NULL) {
        _exception = 1;
        _exceptionType = E_ILLEGAL_ARGUMENT_EXCEPTION;
        _exceptionMessage = "label == null";
        goto exit;
    }
    _nativelabel = label.string();

    glObjectLabelKHR(
        (GLenum)identifier,
        (GLuint)name,
        (GLsizei)length,
        (GLchar *)_nativelabel
    );

exit:
    if (_exception) {
        SLOGGERE("GLES31Ext", _exceptionMessage)
    }
    return _exceptionType;
}

// C function void glGetObjectLabelKHR ( GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label )

ECode GLES31Ext::GlGetObjectLabelKHR(
    /* [in] */ Int32 identifier,
    /* [in] */ Int32 name,
    /* [out] */ String* result)
{
    *result = NULL;
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function void glObjectPtrLabelKHR ( const void *ptr, GLsizei length, const GLchar *label )

ECode GLES31Ext::GlObjectPtrLabelKHR(
    /* [in] */ Int64 ptr,
    /* [in] */ const String& label)
{
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function void glGetObjectPtrLabelKHR ( const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label )

ECode GLES31Ext::GlGetObjectPtrLabelKHR(
    /* [in] */ Int64 ptr,
    /* [out] */ String* label)
{
    *label = NULL;
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function void glGetPointervKHR ( GLenum pname, void **params )

ECode GLES31Ext::GlGetDebugMessageCallbackKHR(
    /* [out] */ IDebugProcKHR** khr)
{
    *khr = NULL;
    SLOGGERE("GLES31Ext", "not yet implemented")
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// C function void glMinSampleShadingOES ( GLfloat value )

ECode GLES31Ext::GlMinSampleShadingOES(
    /* [in] */ Float value)
{
    glMinSampleShadingOES(
        (GLfloat)value
    );
    return NOERROR;
}

// C function void glTexStorage3DMultisampleOES ( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )

ECode GLES31Ext::GlTexStorage3DMultisampleOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 samples,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Boolean fixedsamplelocations)
{
    glTexStorage3DMultisampleOES(
        (GLenum)target,
        (GLsizei)samples,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLsizei)depth,
        (GLboolean)fixedsamplelocations
    );
    return NOERROR;
}

// C function void glCopyImageSubDataEXT ( GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth )

ECode GLES31Ext::GlCopyImageSubDataEXT(
    /* [in] */ Int32 srcName,
    /* [in] */ Int32 srcTarget,
    /* [in] */ Int32 srcLevel,
    /* [in] */ Int32 srcX,
    /* [in] */ Int32 srcY,
    /* [in] */ Int32 srcZ,
    /* [in] */ Int32 dstName,
    /* [in] */ Int32 dstTarget,
    /* [in] */ Int32 dstLevel,
    /* [in] */ Int32 dstX,
    /* [in] */ Int32 dstY,
    /* [in] */ Int32 dstZ,
    /* [in] */ Int32 srcWidth,
    /* [in] */ Int32 srcHeight,
    /* [in] */ Int32 srcDepth)
{
    glCopyImageSubDataEXT(
        (GLuint)srcName,
        (GLenum)srcTarget,
        (GLint)srcLevel,
        (GLint)srcX,
        (GLint)srcY,
        (GLint)srcZ,
        (GLuint)dstName,
        (GLenum)dstTarget,
        (GLint)dstLevel,
        (GLint)dstX,
        (GLint)dstY,
        (GLint)dstZ,
        (GLsizei)srcWidth,
        (GLsizei)srcHeight,
        (GLsizei)srcDepth
    );
    return NOERROR;
}

// C function void glEnableiEXT ( GLenum target, GLuint index )

ECode GLES31Ext::GlEnableiEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index)
{
    glEnableiEXT(
        (GLenum)target,
        (GLuint)index
    );
    return NOERROR;
}

// C function void glDisableiEXT ( GLenum target, GLuint index )

ECode GLES31Ext::GlDisableiEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index)
{
    glDisableiEXT(
        (GLenum)target,
        (GLuint)index
    );
    return NOERROR;
}

// C function void glBlendEquationiEXT ( GLuint buf, GLenum mode )

ECode GLES31Ext::GlBlendEquationiEXT(
    /* [in] */ Int32 buf,
    /* [in] */ Int32 mode)
{
    glBlendEquationiEXT(
        (GLuint)buf,
        (GLenum)mode
    );
    return NOERROR;
}

// C function void glBlendEquationSeparateiEXT ( GLuint buf, GLenum modeRGB, GLenum modeAlpha )

ECode GLES31Ext::GlBlendEquationSeparateiEXT(
    /* [in] */ Int32 buf,
    /* [in] */ Int32 modeRGB,
    /* [in] */ Int32 modeAlpha)
{
    glBlendEquationSeparateiEXT(
        (GLuint)buf,
        (GLenum)modeRGB,
        (GLenum)modeAlpha
    );
    return NOERROR;
}

// C function void glBlendFunciEXT ( GLuint buf, GLenum src, GLenum dst )

ECode GLES31Ext::GlBlendFunciEXT(
    /* [in] */ Int32 buf,
    /* [in] */ Int32 src,
    /* [in] */ Int32 dst)
{
    glBlendFunciEXT(
        (GLuint)buf,
        (GLenum)src,
        (GLenum)dst
    );
    return NOERROR;
}

// C function void glBlendFuncSeparateiEXT ( GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )

ECode GLES31Ext::GlBlendFuncSeparateiEXT(
    /* [in] */ Int32 buf,
    /* [in] */ Int32 srcRGB,
    /* [in] */ Int32 dstRGB,
    /* [in] */ Int32 srcAlpha,
    /* [in] */ Int32 dstAlpha)
{
    glBlendFuncSeparateiEXT(
        (GLuint)buf,
        (GLenum)srcRGB,
        (GLenum)dstRGB,
        (GLenum)srcAlpha,
        (GLenum)dstAlpha
    );
    return NOERROR;
}

// C function void glColorMaskiEXT ( GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a )

ECode GLES31Ext::GlColorMaskiEXT(
    /* [in] */ Int32 index,
    /* [in] */ Boolean r,
    /* [in] */ Boolean g,
    /* [in] */ Boolean b,
    /* [in] */ Boolean a)
{
    glColorMaskiEXT(
        (GLuint)index,
        (GLboolean)r,
        (GLboolean)g,
        (GLboolean)b,
        (GLboolean)a
    );
    return NOERROR;
}

// C function GLboolean glIsEnablediEXT ( GLenum target, GLuint index )

ECode GLES31Ext::GlIsEnablediEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    GLboolean _returnValue;
    _returnValue = glIsEnablediEXT(
        (GLenum)target,
        (GLuint)index
    );
    *result = (Boolean)_returnValue;
    return NOERROR;
}

// C function void glFramebufferTextureEXT ( GLenum target, GLenum attachment, GLuint texture, GLint level )

ECode GLES31Ext::GlFramebufferTextureEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level)
{
    glFramebufferTextureEXT(
        (GLenum)target,
        (GLenum)attachment,
        (GLuint)texture,
        (GLint)level
    );
    return NOERROR;
}

// C function void glPrimitiveBoundingBoxEXT ( GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW )

ECode GLES31Ext::GlPrimitiveBoundingBoxEXT(
    /* [in] */ Float minX,
    /* [in] */ Float minY,
    /* [in] */ Float minZ,
    /* [in] */ Float minW,
    /* [in] */ Float maxX,
    /* [in] */ Float maxY,
    /* [in] */ Float maxZ,
    /* [in] */ Float maxW)
{
    glPrimitiveBoundingBoxEXT(
        (GLfloat)minX,
        (GLfloat)minY,
        (GLfloat)minZ,
        (GLfloat)minW,
        (GLfloat)maxX,
        (GLfloat)maxY,
        (GLfloat)maxZ,
        (GLfloat)maxW
        );
    return NOERROR;
}

// C function void glPatchParameteriEXT ( GLenum pname, GLint value )

ECode GLES31Ext::GlPatchParameteriEXT(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 value)
{
    glPatchParameteriEXT(
        (GLenum)pname,
        (GLint)value
    );
    return NOERROR;
}

// C function void glTexParameterIivEXT ( GLenum target, GLenum pname, const GLint *params )

ECode GLES31Ext::GlTexParameterIivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLint)
    glTexParameterIivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glTexParameterIivEXT ( GLenum target, GLenum pname, const GLint *params )

ECode GLES31Ext::GlTexParameterIivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glTexParameterIivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glTexParameterIuivEXT ( GLenum target, GLenum pname, const GLuint *params )

ECode GLES31Ext::GlTexParameterIuivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLuint)
    glTexParameterIuivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLuint *)params
    );
    GL_EXIT()
}

// C function void glTexParameterIuivEXT ( GLenum target, GLenum pname, const GLuint *params )

ECode GLES31Ext::GlTexParameterIuivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLuint)
    glTexParameterIuivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLuint *)params
    );
    return NOERROR;
}

// C function void glGetTexParameterIivEXT ( GLenum target, GLenum pname, GLint *params )

ECode GLES31Ext::GlGetTexParameterIivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLint)
    glGetTexParameterIivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetTexParameterIivEXT ( GLenum target, GLenum pname, GLint *params )

ECode GLES31Ext::GlGetTexParameterIivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetTexParameterIivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetTexParameterIuivEXT ( GLenum target, GLenum pname, GLuint *params )

ECode GLES31Ext::GlGetTexParameterIuivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLuint)
    glGetTexParameterIuivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLuint *)params
    );
    GL_EXIT()
}

// C function void glGetTexParameterIuivEXT ( GLenum target, GLenum pname, GLuint *params )

ECode GLES31Ext::GlGetTexParameterIuivEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLuint)
    glGetTexParameterIuivEXT(
        (GLenum)target,
        (GLenum)pname,
        (GLuint *)params
    );
    return NOERROR;
}

// C function void glSamplerParameterIivEXT ( GLuint sampler, GLenum pname, const GLint *param )

ECode GLES31Ext::GlSamplerParameterIivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* param_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(param, GLint)
    glSamplerParameterIivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)param
    );
    GL_EXIT()
}

// C function void glSamplerParameterIivEXT ( GLuint sampler, GLenum pname, const GLint *param )

ECode GLES31Ext::GlSamplerParameterIivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* param_buf)
{
    GET_BUFFER_GL_DATA(param, GLint)
    glSamplerParameterIivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)param
    );
    return NOERROR;
}

// C function void glSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, const GLuint *param )

ECode GLES31Ext::GlSamplerParameterIuivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* param_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(param, GLuint)
    glSamplerParameterIuivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLuint *)param
    );
    GL_EXIT()
}

// C function void glSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, const GLuint *param )

ECode GLES31Ext::GlSamplerParameterIuivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* param_buf)
{
    GET_BUFFER_GL_DATA(param, GLuint)
    glSamplerParameterIuivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLuint *)param
    );
    return NOERROR;
}

// C function void glGetSamplerParameterIivEXT ( GLuint sampler, GLenum pname, GLint *params )

ECode GLES31Ext::GlGetSamplerParameterIivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLint)
    glGetSamplerParameterIivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)params
    );
    GL_EXIT()
}

// C function void glGetSamplerParameterIivEXT ( GLuint sampler, GLenum pname, GLint *params )

ECode GLES31Ext::GlGetSamplerParameterIivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLint)
    glGetSamplerParameterIivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

// C function void glGetSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, GLuint *params )

ECode GLES31Ext::GlGetSamplerParameterIuivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GET_REF_GL_DATA_ENTER(params, GLuint)
    glGetSamplerParameterIuivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLuint *)params
    );
    GL_EXIT()
}

// C function void glGetSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, GLuint *params )

ECode GLES31Ext::GlGetSamplerParameterIuivEXT(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    GET_BUFFER_GL_DATA(params, GLuint)
    glGetSamplerParameterIuivEXT(
        (GLuint)sampler,
        (GLenum)pname,
        (GLuint *)params
    );
    return NOERROR;
}

// C function void glTexBufferEXT ( GLenum target, GLenum internalformat, GLuint buffer )

ECode GLES31Ext::GlTexBufferEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 buffer)
{
    glTexBufferEXT(
        (GLenum)target,
        (GLenum)internalformat,
        (GLuint)buffer
    );
    return NOERROR;
}

// C function void glTexBufferRangeEXT ( GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size )

ECode GLES31Ext::GlTexBufferRangeEXT(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size)
{
    glTexBufferRangeEXT(
        (GLenum)target,
        (GLenum)internalformat,
        (GLuint)buffer,
        (GLintptr)offset,
        (GLsizeiptr)size
    );
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
