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

#ifndef __ELASTOS_DROID_OPENGL_CGLES30_H__
#define __ELASTOS_DROID_OPENGL_CGLES30_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/opengl/GLES20.h"

using Elastos::IO::IBuffer;
using Elastos::IO::IInt32Buffer;
using Elastos::IO::IInt64Buffer;
using Elastos::IO::IFloatBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLES30
    : public GLES20
    , public IGLES30
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GlReadBuffer(
        /* [in] */ Int32 mode
    );

    // C function void glDrawRangeElements ( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices )

    CARAPI GlDrawRangeElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* indices
    );

    // C function void glDrawRangeElements ( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizei offset )

    CARAPI GlDrawRangeElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset
    );

    // C function void glTexImage3D ( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels )

    CARAPI GlTexImage3D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 depth,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels
    );

    // C function void glTexImage3D ( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLsizei offset )

    CARAPI GlTexImage3D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 depth,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset
    );

    // C function void glTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels )

    CARAPI GlTexSubImage3D(
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
        /* [in] */ IBuffer* pixels
    );

    // C function void glTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei offset )

    CARAPI GlTexSubImage3D(
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
        /* [in] */ Int32 offset
    );

    // C function void glCopyTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI GlCopyTexSubImage3D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 zoffset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height
    );

    // C function void glCompressedTexImage3D ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data )

    CARAPI GlCompressedTexImage3D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 depth,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ IBuffer* data
    );

    // C function void glCompressedTexImage3D ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLsizei offset )

    CARAPI GlCompressedTexImage3D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 depth,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ Int32 offset
    );

    // C function void glCompressedTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data )

    CARAPI GlCompressedTexSubImage3D(
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
        /* [in] */ IBuffer* data
    );

    // C function void glCompressedTexSubImage3D ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLsizei offset )

    CARAPI GlCompressedTexSubImage3D(
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
        /* [in] */ Int32 offset
    );

    // C function void glGenQueries ( GLsizei n, GLuint *ids )

    CARAPI GlGenQueries(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* ids,
        /* [in] */ Int32 offset
    );

    // C function void glGenQueries ( GLsizei n, GLuint *ids )

    CARAPI GlGenQueries(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* ids
    );

    // C function void glDeleteQueries ( GLsizei n, const GLuint *ids )

    CARAPI GlDeleteQueries(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* ids,
        /* [in] */ Int32 offset
    );

    // C function void glDeleteQueries ( GLsizei n, const GLuint *ids )

    CARAPI GlDeleteQueries(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* ids
    );

    // C function GLboolean glIsQuery ( GLuint id
    CARAPI GlIsQuery(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* result
    );

    // C function void glBeginQuery ( GLenum target, GLuint id )

    CARAPI GlBeginQuery(
        /* [in] */ Int32 target,
        /* [in] */ Int32 id
    );

    // C function void glEndQuery ( GLenum target )

    CARAPI GlEndQuery(
        /* [in] */ Int32 target
    );

    // C function void glGetQueryiv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetQueryiv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetQueryiv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetQueryiv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glGetQueryObjectuiv ( GLuint id, GLenum pname, GLuint *params )

    CARAPI GlGetQueryObjectuiv(
        /* [in] */ Int32 id,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetQueryObjectuiv ( GLuint id, GLenum pname, GLuint *params )

    CARAPI GlGetQueryObjectuiv(
        /* [in] */ Int32 id,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function GLboolean glUnmapBuffer ( GLenum target )

    CARAPI GlUnmapBuffer(
        /* [in] */ Int32 target,
        /* [out] */ Boolean* result
    );

    // C function void glGetBufferPointerv ( GLenum target, GLenum pname, GLvoid** params )

    CARAPI GlGetBufferPointerv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [out] */ IBuffer** buffer
    );

    // C function void glDrawBuffers ( GLsizei n, const GLenum *bufs )

    CARAPI GlDrawBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* bufs,
        /* [in] */ Int32 offset
    );

    // C function void glDrawBuffers ( GLsizei n, const GLenum *bufs )

    CARAPI GlDrawBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* bufs
    );

    // C function void glUniformMatrix2x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix2x3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniformMatrix2x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix2x3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value
    );

    // C function void glUniformMatrix3x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix3x2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniformMatrix3x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix3x2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value
    );

    // C function void glUniformMatrix2x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix2x4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniformMatrix2x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix2x4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value
    );

    // C function void glUniformMatrix4x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix4x2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniformMatrix4x2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix4x2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value
    );

    // C function void glUniformMatrix3x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix3x4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniformMatrix3x4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix3x4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value
    );

    // C function void glUniformMatrix4x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix4x3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniformMatrix4x3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix4x3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value
    );

    // C function void glBlitFramebuffer ( GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter )

    CARAPI GlBlitFramebuffer(
        /* [in] */ Int32 srcX0,
        /* [in] */ Int32 srcY0,
        /* [in] */ Int32 srcX1,
        /* [in] */ Int32 srcY1,
        /* [in] */ Int32 dstX0,
        /* [in] */ Int32 dstY0,
        /* [in] */ Int32 dstX1,
        /* [in] */ Int32 dstY1,
        /* [in] */ Int32 mask,
        /* [in] */ Int32 filter
    );

    // C function void glRenderbufferStorageMultisample ( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )

    CARAPI GlRenderbufferStorageMultisample(
        /* [in] */ Int32 target,
        /* [in] */ Int32 samples,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height
    );

    // C function void glFramebufferTextureLayer ( GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer )

    CARAPI GlFramebufferTextureLayer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 texture,
        /* [in] */ Int32 level,
        /* [in] */ Int32 layer
    );

    // C function GLvoid * glMapBufferRange ( GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access )

    CARAPI GlMapBufferRange(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Int32 access,
        /* [out] */ IBuffer** buffer
    );

    // C function void glFlushMappedBufferRange ( GLenum target, GLintptr offset, GLsizeiptr length )

    CARAPI GlFlushMappedBufferRange(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length
    );

    // C function void glBindVertexArray ( GLuint array )

    CARAPI GlBindVertexArray(
        /* [in] */ Int32 array
    );

    // C function void glDeleteVertexArrays ( GLsizei n, const GLuint *arrays )

    CARAPI GlDeleteVertexArrays(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* arrays,
        /* [in] */ Int32 offset
    );

    // C function void glDeleteVertexArrays ( GLsizei n, const GLuint *arrays )

    CARAPI GlDeleteVertexArrays(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* arrays
    );

    // C function void glGenVertexArrays ( GLsizei n, GLuint *arrays )

    CARAPI GlGenVertexArrays(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* arrays,
        /* [in] */ Int32 offset
    );

    // C function void glGenVertexArrays ( GLsizei n, GLuint *arrays )

    CARAPI GlGenVertexArrays(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* arrays
    );

    // C function GLboolean glIsVertexArray ( GLuint array )

    CARAPI GlIsVertexArray(
        /* [in] */ Int32 array,
        /* [out] */ Boolean* result
    );

    // C function void glGetIntegeri_v ( GLenum target, GLuint index, GLint *data )

    CARAPI GlGetIntegeri_v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* data,
        /* [in] */ Int32 offset
    );

    // C function void glGetIntegeri_v ( GLenum target, GLuint index, GLint *data )

    CARAPI GlGetIntegeri_v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* data
    );

    // C function void glBeginTransformFeedback ( GLenum primitiveMode )

    CARAPI GlBeginTransformFeedback(
        /* [in] */ Int32 primitiveMode
    );

    // C function void glEndTransformFeedback ( void )

    CARAPI GlEndTransformFeedback(
    );

    // C function void glBindBufferRange ( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )

    CARAPI GlBindBufferRange(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size
    );

    // C function void glBindBufferBase ( GLenum target, GLuint index, GLuint buffer )

    CARAPI GlBindBufferBase(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ Int32 buffer
    );

    // C function void glTransformFeedbackVaryings ( GLuint program, GLsizei count, const GLchar *varyings, GLenum bufferMode )

    CARAPI GlTransformFeedbackVaryings(
        /* [in] */ Int32 program,
        /* [in] */ ArrayOf<String>* varyings,
        /* [in] */ Int32 bufferMode
    );

    // C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

    CARAPI GlGetTransformFeedbackVarying(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ Int32 bufsize,
        /* [in] */ ArrayOf<Int32>* length,
        /* [in] */ Int32 lengthOffset,
        /* [in] */ ArrayOf<Int32>* size,
        /* [in] */ Int32 sizeOffset,
        /* [in] */ ArrayOf<Int32>* type,
        /* [in] */ Int32 typeOffset,
        /* [in] */ ArrayOf<Byte>* name,
        /* [in] */ Int32 nameOffset
    );

    // C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

    CARAPI GlGetTransformFeedbackVarying(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ Int32 bufsize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [in] */ Byte name
    );

    // C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

    CARAPI GlGetTransformFeedbackVarying(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* size,
        /* [in] */ Int32 sizeOffset,
        /* [in] */ ArrayOf<Int32>* type,
        /* [in] */ Int32 typeOffset,
        /* [out] */ String* result
    );

    // C function void glGetTransformFeedbackVarying ( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name )

    CARAPI GlGetTransformFeedbackVarying(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [out] */ String* result
    );

    // C function void glVertexAttribIPointer ( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )

    CARAPI GlVertexAttribIPointer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer
    );

    // C function void glVertexAttribIPointer ( GLuint index, GLint size, GLenum type, GLsizei stride, GLsizei offset )

    CARAPI GlVertexAttribIPointer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset
    );

    // C function void glGetVertexAttribIiv ( GLuint index, GLenum pname, GLint *params )

    CARAPI GlGetVertexAttribIiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetVertexAttribIiv ( GLuint index, GLenum pname, GLint *params )

    CARAPI GlGetVertexAttribIiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glGetVertexAttribIuiv ( GLuint index, GLenum pname, GLuint *params )

    CARAPI GlGetVertexAttribIuiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetVertexAttribIuiv ( GLuint index, GLenum pname, GLuint *params )

    CARAPI GlGetVertexAttribIuiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glVertexAttribI4i ( GLuint index, GLint x, GLint y, GLint z, GLint w )

    CARAPI GlVertexAttribI4i(
        /* [in] */ Int32 index,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w
    );

    // C function void glVertexAttribI4ui ( GLuint index, GLuint x, GLuint y, GLuint z, GLuint w )

    CARAPI GlVertexAttribI4ui(
        /* [in] */ Int32 index,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w
    );

    // C function void glVertexAttribI4iv ( GLuint index, const GLint *v )

    CARAPI GlVertexAttribI4iv(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset
    );

    // C function void glVertexAttribI4iv ( GLuint index, const GLint *v )

    CARAPI GlVertexAttribI4iv(
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* v
    );

    // C function void glVertexAttribI4uiv ( GLuint index, const GLuint *v )

    CARAPI GlVertexAttribI4uiv(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset
    );

    // C function void glVertexAttribI4uiv ( GLuint index, const GLuint *v )

    CARAPI GlVertexAttribI4uiv(
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* v
    );

    // C function void glGetUniformuiv ( GLuint program, GLint location, GLuint *params )

    CARAPI GlGetUniformuiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetUniformuiv ( GLuint program, GLint location, GLuint *params )

    CARAPI GlGetUniformuiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ IInt32Buffer* params
    );

    // C function GLint glGetFragDataLocation ( GLuint program, const GLchar *name )

    CARAPI GlGetFragDataLocation(
        /* [in] */ Int32 program,
        /* [in] */ const String& name,
        /* [out] */ Int32* result
    );

    // C function void glUniform1ui ( GLint location, GLuint v0 )

    CARAPI GlUniform1ui(
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0
    );

    // C function void glUniform2ui ( GLint location, GLuint v0, GLuint v1 )

    CARAPI GlUniform2ui(
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1
    );

    // C function void glUniform3ui ( GLint location, GLuint v0, GLuint v1, GLuint v2 )

    CARAPI GlUniform3ui(
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1,
        /* [in] */ Int32 v2
    );

    // C function void glUniform4ui ( GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 )

    CARAPI GlUniform4ui(
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1,
        /* [in] */ Int32 v2,
        /* [in] */ Int32 v3
    );

    // C function void glUniform1uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform1uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniform1uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform1uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value
    );

    // C function void glUniform2uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform2uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniform2uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform2uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value
    );

    // C function void glUniform3uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform3uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniform3uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform3uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value
    );

    // C function void glUniform4uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform4uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset
    );

    // C function void glUniform4uiv ( GLint location, GLsizei count, const GLuint *value )

    CARAPI GlUniform4uiv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value
    );

    // C function void glClearBufferiv ( GLenum buffer, GLint drawbuffer, const GLint *value )

    CARAPI GlClearBufferiv(
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 drawbuffer,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset
    );

    // C function void glClearBufferiv ( GLenum buffer, GLint drawbuffer, const GLint *value )

    CARAPI GlClearBufferiv(
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 drawbuffer,
        /* [in] */ IInt32Buffer* value
    );

    // C function void glClearBufferuiv ( GLenum buffer, GLint drawbuffer, const GLuint *value )

    CARAPI GlClearBufferuiv(
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 drawbuffer,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset
    );

    // C function void glClearBufferuiv ( GLenum buffer, GLint drawbuffer, const GLuint *value )

    CARAPI GlClearBufferuiv(
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 drawbuffer,
        /* [in] */ IInt32Buffer* value
    );

    // C function void glClearBufferfv ( GLenum buffer, GLint drawbuffer, const GLfloat *value )

    CARAPI GlClearBufferfv(
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 drawbuffer,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset
    );

    // C function void glClearBufferfv ( GLenum buffer, GLint drawbuffer, const GLfloat *value )

    CARAPI GlClearBufferfv(
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 drawbuffer,
        /* [in] */ IFloatBuffer* value
    );

    // C function void glClearBufferfi ( GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil )

    CARAPI GlClearBufferfi(
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 drawbuffer,
        /* [in] */ Float depth,
        /* [in] */ Int32 stencil
    );

    // C function const GLubyte * glGetStringi ( GLenum name, GLuint index )

    CARAPI GlGetStringi(
        /* [in] */ Int32 name,
        /* [in] */ Int32 index,
        /* [out] */ String* result
    );

    // C function void glCopyBufferSubData ( GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size )

    CARAPI GlCopyBufferSubData(
        /* [in] */ Int32 readTarget,
        /* [in] */ Int32 writeTarget,
        /* [in] */ Int32 readOffset,
        /* [in] */ Int32 writeOffset,
        /* [in] */ Int32 size
    );

    // C function void glGetUniformIndices ( GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices )

    CARAPI GlGetUniformIndices(
        /* [in] */ Int32 program,
        /* [in] */ ArrayOf<String>* uniformNames,
        /* [in] */ ArrayOf<Int32>* uniformIndices,
        /* [in] */ Int32 uniformIndicesOffset
    );

    // C function void glGetUniformIndices ( GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices )

    CARAPI GlGetUniformIndices(
        /* [in] */ Int32 program,
        /* [in] */ ArrayOf<String>* uniformNames,
        /* [in] */ IInt32Buffer* uniformIndices
    );

    // C function void glGetActiveUniformsiv ( GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params )

    CARAPI GlGetActiveUniformsiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformCount,
        /* [in] */ ArrayOf<Int32>* uniformIndices,
        /* [in] */ Int32 uniformIndicesOffset,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 paramsOffset
    );

    // C function void glGetActiveUniformsiv ( GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params )

    CARAPI GlGetActiveUniformsiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformCount,
        /* [in] */ IInt32Buffer* uniformIndices,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function GLuint glGetUniformBlockIndex ( GLuint program, const GLchar *uniformBlockName )

    CARAPI GlGetUniformBlockIndex(
        /* [in] */ Int32 program,
        /* [in] */ const String& uniformBlockName,
        /* [out] */ Int32* result
    );

    // C function void glGetActiveUniformBlockiv ( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params )

    CARAPI GlGetActiveUniformBlockiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformBlockIndex,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetActiveUniformBlockiv ( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params )

    CARAPI GlGetActiveUniformBlockiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformBlockIndex,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glGetActiveUniformBlockName ( GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName )

    CARAPI GlGetActiveUniformBlockName(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformBlockIndex,
        /* [in] */ Int32 bufSize,
        /* [in] */ ArrayOf<Int32>* length,
        /* [in] */ Int32 lengthOffset,
        /* [in] */ ArrayOf<Byte>* uniformBlockName,
        /* [in] */ Int32 uniformBlockNameOffset
    );

    // C function void glGetActiveUniformBlockName ( GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName )

    CARAPI GlGetActiveUniformBlockName(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformBlockIndex,
        /* [in] */ IBuffer* length,
        /* [in] */ IBuffer* uniformBlockName
    );

    // C function void glGetActiveUniformBlockName ( GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName )

    CARAPI GlGetActiveUniformBlockName(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformBlockIndex,
        /* [out] */ String* result
    );

    // C function void glUniformBlockBinding ( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding )

    CARAPI GlUniformBlockBinding(
        /* [in] */ Int32 program,
        /* [in] */ Int32 uniformBlockIndex,
        /* [in] */ Int32 uniformBlockBinding
    );

    // C function void glDrawArraysInstanced ( GLenum mode, GLint first, GLsizei count, GLsizei instanceCount )

    CARAPI GlDrawArraysInstanced(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 first,
        /* [in] */ Int32 count,
        /* [in] */ Int32 instanceCount
    );

    // C function void glDrawElementsInstanced ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instanceCount )

    CARAPI GlDrawElementsInstanced(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* indices,
        /* [in] */ Int32 instanceCount
    );

    // C function void glDrawElementsInstanced ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instanceCount )

    CARAPI GlDrawElementsInstanced(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 indicesOffset,
        /* [in] */ Int32 instanceCount
    );

    // C function GLsync glFenceSync ( GLenum condition, GLbitfield flags )

    CARAPI GlFenceSync(
        /* [in] */ Int32 condition,
        /* [in] */ Int32 flags,
        /* [out] */ Int64* result
    );

    // C function GLboolean glIsSync ( GLsync sync )

    CARAPI GlIsSync(
        /* [in] */ Int64 sync,
        /* [out] */ Boolean* result
    );

    // C function void glDeleteSync ( GLsync sync )

    CARAPI GlDeleteSync(
        /* [in] */ Int64 sync
    );

    // C function GLenum glClientWaitSync ( GLsync sync, GLbitfield flags, GLuint64 timeout )

    CARAPI GlClientWaitSync(
        /* [in] */ Int64 sync,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 timeout,
        /* [out] */ Int32* result
    );

    // C function void glWaitSync ( GLsync sync, GLbitfield flags, GLuint64 timeout )

    CARAPI GlWaitSync(
        /* [in] */ Int64 sync,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 timeout
    );

    // C function void glGetInteger64v ( GLenum pname, GLint64 *params )

    CARAPI GlGetInteger64v(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int64>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetInteger64v ( GLenum pname, GLint64 *params )

    CARAPI GlGetInteger64v(
        /* [in] */ Int32 pname,
        /* [in] */ IInt64Buffer*  params
    );

    // C function void glGetSynciv ( GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values )

    CARAPI GlGetSynciv(
        /* [in] */ Int64 sync,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 bufSize,
        /* [in] */ ArrayOf<Int32>* length,
        /* [in] */ Int32 lengthOffset,
        /* [in] */ ArrayOf<Int32>* values,
        /* [in] */ Int32 valuesOffset
    );

    // C function void glGetSynciv ( GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values )

    CARAPI GlGetSynciv(
        /* [in] */ Int64 sync,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 bufSize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* values
    );

    // C function void glGetInteger64i_v ( GLenum target, GLuint index, GLint64 *data )

    CARAPI GlGetInteger64i_v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int64>* data,
        /* [in] */ Int32 offset
    );

    // C function void glGetInteger64i_v ( GLenum target, GLuint index, GLint64 *data )

    CARAPI GlGetInteger64i_v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ IInt64Buffer*  data
    );

    // C function void glGetBufferParameteri64v ( GLenum target, GLenum pname, GLint64 *params )

    CARAPI GlGetBufferParameteri64v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int64>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetBufferParameteri64v ( GLenum target, GLenum pname, GLint64 *params )

    CARAPI GlGetBufferParameteri64v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt64Buffer*  params
    );

    // C function void glGenSamplers ( GLsizei count, GLuint *samplers )

    CARAPI GlGenSamplers(
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* samplers,
        /* [in] */ Int32 offset
    );

    // C function void glGenSamplers ( GLsizei count, GLuint *samplers )

    CARAPI GlGenSamplers(
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* samplers
    );

    // C function void glDeleteSamplers ( GLsizei count, const GLuint *samplers )

    CARAPI GlDeleteSamplers(
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* samplers,
        /* [in] */ Int32 offset
    );

    // C function void glDeleteSamplers ( GLsizei count, const GLuint *samplers )

    CARAPI GlDeleteSamplers(
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* samplers
    );

    // C function GLboolean glIsSampler ( GLuint sampler )

    CARAPI GlIsSampler(
        /* [in] */ Int32 sampler,
        /* [out] */ Boolean* result
    );

    // C function void glBindSampler ( GLuint unit, GLuint sampler )

    CARAPI GlBindSampler(
        /* [in] */ Int32 unit,
        /* [in] */ Int32 sampler
    );

    // C function void glSamplerParameteri ( GLuint sampler, GLenum pname, GLint param )

    CARAPI GlSamplerParameteri(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param
    );

    // C function void glSamplerParameteriv ( GLuint sampler, GLenum pname, const GLint *param )

    CARAPI GlSamplerParameteriv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ Int32 offset
    );

    // C function void glSamplerParameteriv ( GLuint sampler, GLenum pname, const GLint *param )

    CARAPI GlSamplerParameteriv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* param
    );

    // C function void glSamplerParameterf ( GLuint sampler, GLenum pname, GLfloat param )

    CARAPI GlSamplerParameterf(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ Float param
    );

    // C function void glSamplerParameterfv ( GLuint sampler, GLenum pname, const GLfloat *param )

    CARAPI GlSamplerParameterfv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* param,
        /* [in] */ Int32 offset
    );

    // C function void glSamplerParameterfv ( GLuint sampler, GLenum pname, const GLfloat *param )

    CARAPI GlSamplerParameterfv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* param
    );

    // C function void glGetSamplerParameteriv ( GLuint sampler, GLenum pname, GLint *params )

    CARAPI GlGetSamplerParameteriv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetSamplerParameteriv ( GLuint sampler, GLenum pname, GLint *params )

    CARAPI GlGetSamplerParameteriv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glGetSamplerParameterfv ( GLuint sampler, GLenum pname, GLfloat *params )

    CARAPI GlGetSamplerParameterfv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetSamplerParameterfv ( GLuint sampler, GLenum pname, GLfloat *params )

    CARAPI GlGetSamplerParameterfv(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params
    );

    // C function void glVertexAttribDivisor ( GLuint index, GLuint divisor )

    CARAPI GlVertexAttribDivisor(
        /* [in] */ Int32 index,
        /* [in] */ Int32 divisor
    );

    // C function void glBindTransformFeedback ( GLenum target, GLuint id )

    CARAPI GlBindTransformFeedback(
        /* [in] */ Int32 target,
        /* [in] */ Int32 id
    );

    // C function void glDeleteTransformFeedbacks ( GLsizei n, const GLuint *ids )

    CARAPI GlDeleteTransformFeedbacks(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* ids,
        /* [in] */ Int32 offset
    );

    // C function void glDeleteTransformFeedbacks ( GLsizei n, const GLuint *ids )

    CARAPI GlDeleteTransformFeedbacks(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* ids
    );

    // C function void glGenTransformFeedbacks ( GLsizei n, GLuint *ids )

    CARAPI GlGenTransformFeedbacks(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* ids,
        /* [in] */ Int32 offset
    );

    // C function void glGenTransformFeedbacks ( GLsizei n, GLuint *ids )

    CARAPI GlGenTransformFeedbacks(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* ids
    );

    // C function GLboolean glIsTransformFeedback ( GLuint id )

    CARAPI GlIsTransformFeedback(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* result
    );

    // C function void glPauseTransformFeedback ( void )

    CARAPI GlPauseTransformFeedback(
    );

    // C function void glResumeTransformFeedback ( void )

    CARAPI GlResumeTransformFeedback(
    );

    // C function void glGetProgramBinary ( GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, GLvoid *binary )

    CARAPI GlGetProgramBinary(
        /* [in] */ Int32 program,
        /* [in] */ Int32 bufSize,
        /* [in] */ ArrayOf<Int32>* length,
        /* [in] */ Int32 lengthOffset,
        /* [in] */ ArrayOf<Int32>* binaryFormat,
        /* [in] */ Int32 binaryFormatOffset,
        /* [in] */ IBuffer* binary
    );

    // C function void glGetProgramBinary ( GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, GLvoid *binary )

    CARAPI GlGetProgramBinary(
        /* [in] */ Int32 program,
        /* [in] */ Int32 bufSize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* binaryFormat,
        /* [in] */ IBuffer* binary
    );

    // C function void glProgramBinary ( GLuint program, GLenum binaryFormat, const GLvoid *binary, GLsizei length )

    CARAPI GlProgramBinary(
        /* [in] */ Int32 program,
        /* [in] */ Int32 binaryFormat,
        /* [in] */ IBuffer* binary,
        /* [in] */ Int32 length
    );

    // C function void glProgramParameteri ( GLuint program, GLenum pname, GLint value )

    CARAPI GlProgramParameteri(
        /* [in] */ Int32 program,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 value
    );

    // C function void glInvalidateFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments )

    CARAPI GlInvalidateFramebuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 numAttachments,
        /* [in] */ ArrayOf<Int32>* attachments,
        /* [in] */ Int32 offset
    );

    // C function void glInvalidateFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments )

    CARAPI GlInvalidateFramebuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 numAttachments,
        /* [in] */ IInt32Buffer* attachments
    );

    // C function void glInvalidateSubFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI GlInvalidateSubFramebuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 numAttachments,
        /* [in] */ ArrayOf<Int32>* attachments,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height
    );

    // C function void glInvalidateSubFramebuffer ( GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI GlInvalidateSubFramebuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 numAttachments,
        /* [in] */ IInt32Buffer* attachments,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height
    );

    // C function void glTexStorage2D ( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height )

    CARAPI GlTexStorage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 levels,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height
    );

    // C function void glTexStorage3D ( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth )

    CARAPI GlTexStorage3D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 levels,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 depth
    );

    // C function void glGetInternalformativ ( GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params )

    CARAPI GlGetInternalformativ(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 bufSize,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetInternalformativ ( GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params )

    CARAPI GlGetInternalformativ(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 bufSize,
        /* [in] */ IInt32Buffer* params
    );

};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES30_H__