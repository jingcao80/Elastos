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

#ifndef __ELASTOS_DROID_OPENGL_GLES20_H__
#define __ELASTOS_DROID_OPENGL_GLES20_H__

#include "Elastos.Droid.Opengl.h"
#include "Elastos.CoreLibrary.IO.h"

#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastosx::Microedition::Khronos::Egl::IEGL10;

typedef Elastosx::Microedition::Khronos::Egl::IEGLConfig XIEGLConfig;
typedef Elastosx::Microedition::Khronos::Egl::IEGLContext XIEGLContext;
typedef Elastosx::Microedition::Khronos::Egl::IEGLDisplay XIEGLDisplay;
typedef Elastosx::Microedition::Khronos::Egl::IEGLSurface XIEGLSurface;

using Elastos::IO::IBuffer;
using Elastos::IO::IFloatBuffer;
using Elastos::IO::IInt32Buffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLES20
    : public Singleton
    , public IGLES20
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GlActiveTexture(
        /* [in] */ Int32 texture);

    // C function void glAttachShader ( GLuint program, GLuint shader )

    CARAPI GlAttachShader(
        /* [in] */ Int32 program,
        /* [in] */ Int32 shader);

    // C function void glBindAttribLocation ( GLuint program, GLuint index, const char *name )

    CARAPI GlBindAttribLocation(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ const String& name);

    // C function void glBindBuffer ( GLenum target, GLuint buffer )

    CARAPI GlBindBuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 buffer);

    // C function void glBindFramebuffer ( GLenum target, GLuint framebuffer )

    CARAPI GlBindFramebuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 framebuffer);

    // C function void glBindRenderbuffer ( GLenum target, GLuint renderbuffer )

    CARAPI GlBindRenderbuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 renderbuffer);

    // C function void glBindTexture ( GLenum target, GLuint texture )

    CARAPI GlBindTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 texture);

    // C function void glBlendColor ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )

    CARAPI GlBlendColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    // C function void glBlendEquation ( GLenum mode )

    CARAPI GlBlendEquation(
        /* [in] */ Int32 mode);

    // C function void glBlendEquationSeparate ( GLenum modeRGB, GLenum modeAlpha )

    CARAPI GlBlendEquationSeparate(
        /* [in] */ Int32 modeRGB,
        /* [in] */ Int32 modeAlpha);

    // C function void glBlendFunc ( GLenum sfactor, GLenum dfactor )

    CARAPI GlBlendFunc(
        /* [in] */ Int32 sfactor,
        /* [in] */ Int32 dfactor);

    // C function void glBlendFuncSeparate ( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )

    CARAPI GlBlendFuncSeparate(
        /* [in] */ Int32 srcRGB,
        /* [in] */ Int32 dstRGB,
        /* [in] */ Int32 srcAlpha,
        /* [in] */ Int32 dstAlpha);

    // C function void glBufferData ( GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage )

    CARAPI GlBufferData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data,
        /* [in] */ Int32 usage);

    // C function void glBufferSubData ( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data )

    CARAPI GlBufferSubData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data);

    // C function GLenum glCheckFramebufferStatus ( GLenum target )

    CARAPI GlCheckFramebufferStatus(
        /* [in] */ Int32 target,
        /* [out] */ Int32* rst);

    // C function void glClear ( GLbitfield mask )

    CARAPI GlClear(
        /* [in] */ Int32 mask);

    // C function void glClearColor ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )

    CARAPI GlClearColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    // C function void glClearDepthf ( GLclampf depth )

    CARAPI GlClearDepthf(
        /* [in] */ Float depth);

    // C function void glClearStencil ( GLint s )

    CARAPI GlClearStencil(
        /* [in] */ Int32 s);

    // C function void glColorMask ( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )

    CARAPI GlColorMask(
        /* [in] */ Boolean red,
        /* [in] */ Boolean green,
        /* [in] */ Boolean blue,
        /* [in] */ Boolean alpha);

    // C function void glCompileShader ( GLuint shader )

    CARAPI GlCompileShader(
        /* [in] */ Int32 shader);

    // C function void glCompressedTexImage2D ( GLenum target, GLint level, GLenum internalformat,
    // GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data )

    CARAPI GlCompressedTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ IBuffer* data);

    // C function void glCompressedTexSubImage2D ( GLenum target, GLint level, GLint xoffset, GLint yoffset,
    // GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data )

    CARAPI GlCompressedTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 imageSize,
        /* [in] */ IBuffer* data);

    // C function void glCopyTexImage2D ( GLenum target, GLint level, GLenum internalformat, GLint x,
    // GLint y, GLsizei width, GLsizei height, GLint border )

    CARAPI GlCopyTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border);

    // C function void glCopyTexSubImage2D ( GLenum target, GLint level, GLint xoffset, GLint yoffset,
    // GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI GlCopyTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // C function GLuint glCreateProgram ( void )

    CARAPI GlCreateProgram(
        /* [out] */ Int32* rst);

    // C function GLuint glCreateShader ( GLenum type )

    CARAPI GlCreateShader(
        /* [in] */ Int32 type,
        /* [out] */ Int32* rst);

    // C function void glCullFace ( GLenum mode )

    CARAPI GlCullFace(
        /* [in] */ Int32 mode);

    // C function void glDeleteBuffers ( GLsizei n, const GLuint *buffers )

    CARAPI GlDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    // C function void glDeleteBuffers ( GLsizei n, const GLuint *buffers )

    CARAPI GlDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers);

    // C function void glDeleteFramebuffers ( GLsizei n, const GLuint *framebuffers )

    CARAPI GlDeleteFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    // C function void glDeleteFramebuffers ( GLsizei n, const GLuint *framebuffers )

    CARAPI GlDeleteFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    // C function void glDeleteProgram ( GLuint program )

    CARAPI GlDeleteProgram(
        /* [in] */ Int32 program);

    // C function void glDeleteRenderbuffers ( GLsizei n, const GLuint *renderbuffers )

    CARAPI GlDeleteRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    // C function void glDeleteRenderbuffers ( GLsizei n, const GLuint *renderbuffers )

    CARAPI GlDeleteRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

    // C function void glDeleteShader ( GLuint shader )

    CARAPI GlDeleteShader(
        /* [in] */ Int32 shader);

    // C function void glDeleteTextures ( GLsizei n, const GLuint *textures )

    CARAPI GlDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    // C function void glDeleteTextures ( GLsizei n, const GLuint *textures )

    CARAPI GlDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    // C function void glDepthFunc ( GLenum func )

    CARAPI GlDepthFunc(
        /* [in] */ Int32 func);

    // C function void glDepthMask ( GLboolean flag )

    CARAPI GlDepthMask(
        /* [in] */ Boolean flag);

    // C function void glDepthRangef ( GLclampf zNear, GLclampf zFar )

    CARAPI GlDepthRangef(
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    // C function void glDetachShader ( GLuint program, GLuint shader )

    CARAPI GlDetachShader(
        /* [in] */ Int32 program,
        /* [in] */ Int32 shader);

    // C function void glDisable ( GLenum cap )

    CARAPI GlDisable(
        /* [in] */ Int32 cap);

    // C function void glDisableVertexAttribArray ( GLuint index )

    CARAPI GlDisableVertexAttribArray(
        /* [in] */ Int32 index);

    // C function void glDrawArrays ( GLenum mode, GLint first, GLsizei count )

    CARAPI GlDrawArrays(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 first,
        /* [in] */ Int32 count);

    // C function void glDrawElements ( GLenum mode, GLsizei count, GLenum type, GLint offset )

    CARAPI GlDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset);

    // C function void glDrawElements ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )

    CARAPI GlDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* indices);

    // C function void glEnable ( GLenum cap )

    CARAPI GlEnable(
        /* [in] */ Int32 cap);

    // C function void glEnableVertexAttribArray ( GLuint index )

    CARAPI GlEnableVertexAttribArray(
        /* [in] */ Int32 index);

    // C function void glFinish ( void )

    CARAPI GlFinish();

    // C function void glFlush ( void )

    CARAPI GlFlush();

    // C function void glFramebufferRenderbuffer ( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )

    CARAPI GlFramebufferRenderbuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 renderbuffertarget,
        /* [in] */ Int32 renderbuffer);

    // C function void glFramebufferTexture2D ( GLenum target, GLenum attachment, GLenum textarget,
    // GLuint texture, GLint level )

    CARAPI GlFramebufferTexture2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 textarget,
        /* [in] */ Int32 texture,
        /* [in] */ Int32 level);

    // C function void glFrontFace ( GLenum mode )

    CARAPI GlFrontFace(
        /* [in] */ Int32 mode);

    // C function void glGenBuffers ( GLsizei n, GLuint *buffers )

    CARAPI GlGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    // C function void glGenBuffers ( GLsizei n, GLuint *buffers )

    CARAPI GlGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers);

    // C function void glGenerateMipmap ( GLenum target )

    CARAPI GlGenerateMipmap(
        /* [in] */ Int32 target);

    // C function void glGenFramebuffers ( GLsizei n, GLuint *framebuffers )

    CARAPI GlGenFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    // C function void glGenFramebuffers ( GLsizei n, GLuint *framebuffers )

    CARAPI GlGenFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    // C function void glGenRenderbuffers ( GLsizei n, GLuint *renderbuffers )

    CARAPI GlGenRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    // C function void glGenRenderbuffers ( GLsizei n, GLuint *renderbuffers )

    CARAPI GlGenRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

    // C function void glGenTextures ( GLsizei n, GLuint *textures )

    CARAPI GlGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    // C function void glGenTextures ( GLsizei n, GLuint *textures )

    CARAPI GlGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    // C function void glGetActiveAttrib ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveAttrib(
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
        /* [in] */ Int32 nameOffset);

    // C function void glGetActiveAttrib ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveAttrib(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ Int32 bufsize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [in] */ Byte name);

    // C function void glGetActiveAttrib ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveAttrib(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* size,
        /* [in] */ Int32 sizeOffset,
        /* [in] */ ArrayOf<Int32>* type,
        /* [in] */ Int32 typeOffset,
        /* [out] */ String* str);

    // C function void glGetActiveAttrib ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveAttrib(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [out] */ String* rst);
    // C function void glGetActiveUniform ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveUniform(
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
        /* [in] */ Int32 nameOffset);

    // C function void glGetActiveUniform ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveUniform(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ Int32 bufsize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [in] */ Byte name);
    // C function void glGetActiveUniform ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveUniform(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* size,
        /* [in] */ Int32 sizeOffset,
        /* [in] */ ArrayOf<Int32>* type,
        /* [in] */ Int32 typeOffset,
        /* [out] */ String* rst);

    // C function void glGetActiveUniform ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI GlGetActiveUniform(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [out] */ String* str);
    // C function void glGetAttachedShaders ( GLuint program, GLsizei maxcount, GLsizei *count, GLuint *shaders )

    CARAPI GlGetAttachedShaders(
        /* [in] */ Int32 program,
        /* [in] */ Int32 maxcount,
        /* [in] */ ArrayOf<Int32>* count,
        /* [in] */ Int32 countOffset,
        /* [in] */ ArrayOf<Int32>* shaders,
        /* [in] */ Int32 shadersOffset);

    // C function void glGetAttachedShaders ( GLuint program, GLsizei maxcount, GLsizei *count, GLuint *shaders )

    CARAPI GlGetAttachedShaders(
        /* [in] */ Int32 program,
        /* [in] */ Int32 maxcount,
        /* [in] */ IInt32Buffer* count,
        /* [in] */ IInt32Buffer* shaders);

    // C function int glGetAttribLocation ( GLuint program, const char *name )

    CARAPI GlGetAttribLocation(
        /* [in] */ Int32 program,
        /* [in] */ const String& name,
        /* [out] */ Int32* rst);

    // C function void glGetBooleanv ( GLenum pname, GLboolean *params )

    CARAPI GlGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Boolean>* params,
        /* [in] */ Int32 offset);

    // C function void glGetBooleanv ( GLenum pname, GLboolean *params )

    CARAPI GlGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetBufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetBufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function GLenum glGetError ( void )

    CARAPI GlGetError(
        /* [out] */ Int32* rst);

    // C function void glGetFloatv ( GLenum pname, GLfloat *params )

    CARAPI GlGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetFloatv ( GLenum pname, GLfloat *params )

    CARAPI GlGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetFramebufferAttachmentParameteriv ( GLenum target, GLenum attachment,
    // GLenum pname, GLint *params )

    CARAPI GlGetFramebufferAttachmentParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetFramebufferAttachmentParameteriv ( GLenum target, GLenum attachment, GLenum pname,
    // GLint *params )

    CARAPI GlGetFramebufferAttachmentParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetIntegerv ( GLenum pname, GLint *params )

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetIntegerv ( GLenum pname, GLint *params )

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetProgramiv ( GLuint program, GLenum pname, GLint *params )

    CARAPI GlGetProgramiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetProgramiv ( GLuint program, GLenum pname, GLint *params )

    CARAPI GlGetProgramiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetProgramInfoLog(
        /* [in] */ Int32 program,
        /* [out] */ String* str);
    // C function void glGetRenderbufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetRenderbufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetRenderbufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetRenderbufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetShaderiv ( GLuint shader, GLenum pname, GLint *params )

    CARAPI GlGetShaderiv(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetShaderiv ( GLuint shader, GLenum pname, GLint *params )

    CARAPI GlGetShaderiv(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetShaderInfoLog( GLuint shader, GLsizei maxLength, GLsizei * length,
    //     CARAPI Glchar * infoLog);

    CARAPI GlGetShaderInfoLog(
        /* [in] */ Int32 shader,
        /* [out] */ String* str);

    // C function void glGetShaderPrecisionFormat ( GLenum shadertype, GLenum precisiontype,
    // GLint *range, GLint *precision )

    CARAPI GlGetShaderPrecisionFormat(
        /* [in] */ Int32 shadertype,
        /* [in] */ Int32 precisiontype,
        /* [in] */ ArrayOf<Int32>* range,
        /* [in] */ Int32 rangeOffset,
        /* [in] */ ArrayOf<Int32>* precision,
        /* [in] */ Int32 precisionOffset);

    // C function void glGetShaderPrecisionFormat ( GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision )

    CARAPI GlGetShaderPrecisionFormat(
        /* [in] */ Int32 shadertype,
        /* [in] */ Int32 precisiontype,
        /* [in] */ IInt32Buffer* range,
        /* [in] */ IInt32Buffer* precision);

    // C function void glGetShaderSource ( GLuint shader, GLsizei bufsize, GLsizei *length, char *source )

    CARAPI GlGetShaderSource(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 bufsize,
        /* [in] */ ArrayOf<Int32>* length,
        /* [in] */ Int32 lengthOffset,
        /* [in] */ ArrayOf<Byte>* source,
        /* [in] */ Int32 sourceOffset);

    // C function void glGetShaderSource ( GLuint shader, GLsizei bufsize, GLsizei *length, char *source )

    CARAPI GlGetShaderSource(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 bufsize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ Byte source);

    // C function void glGetShaderSource ( GLuint shader, GLsizei bufsize, GLsizei *length, char *source )

    CARAPI GlGetShaderSource(
        /* [in] */ Int32 shader,
        /* [out] */ String* rst);


    // C function const GLubyte * glGetString ( GLenum name )

    CARAPI GlGetString(
        /* [in] */ Int32 name,
        /* [out] */ String* str);
    // C function void glGetTexParameterfv ( GLenum target, GLenum pname, GLfloat *params )

    CARAPI GlGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetTexParameterfv ( GLenum target, GLenum pname, GLfloat *params )

    CARAPI GlGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetTexParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetTexParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetUniformfv ( GLuint program, GLint location, GLfloat *params )

    CARAPI GlGetUniformfv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetUniformfv ( GLuint program, GLint location, GLfloat *params )

    CARAPI GlGetUniformfv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetUniformiv ( GLuint program, GLint location, GLint *params )

    CARAPI GlGetUniformiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetUniformiv ( GLuint program, GLint location, GLint *params )

    CARAPI GlGetUniformiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ IInt32Buffer* params);

    // C function int glGetUniformLocation ( GLuint program, const char *name )

    CARAPI GlGetUniformLocation(
        /* [in] */ Int32 program,
        /* [in] */ const String& name,
        /* [out] */ Int32* rst);

    // C function void glGetVertexAttribfv ( GLuint index, GLenum pname, GLfloat *params )

    CARAPI GlGetVertexAttribfv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetVertexAttribfv ( GLuint index, GLenum pname, GLfloat *params )

    CARAPI GlGetVertexAttribfv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetVertexAttribiv ( GLuint index, GLenum pname, GLint *params )

    CARAPI GlGetVertexAttribiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetVertexAttribiv ( GLuint index, GLenum pname, GLint *params )

    CARAPI GlGetVertexAttribiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glHint ( GLenum target, GLenum mode )

    CARAPI GlHint(
        /* [in] */ Int32 target,
        /* [in] */ Int32 mode);

    // C function GLboolean glIsBuffer ( GLuint buffer )

    CARAPI GlIsBuffer(
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsEnabled ( GLenum cap )

    CARAPI GlIsEnabled(
        /* [in] */ Int32 cap,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsFramebuffer ( GLuint framebuffer )

    CARAPI GlIsFramebuffer(
        /* [in] */ Int32 framebuffer,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsProgram ( GLuint program )

    CARAPI GlIsProgram(
        /* [in] */ Int32 program,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsRenderbuffer ( GLuint renderbuffer )

    CARAPI GlIsRenderbuffer(
        /* [in] */ Int32 renderbuffer,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsShader ( GLuint shader )

    CARAPI GlIsShader(
        /* [in] */ Int32 shader,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsTexture ( GLuint texture )

    CARAPI GlIsTexture(
        /* [in] */ Int32 texture,
        /* [out] */ Boolean* rst);

    // C function void glLineWidth ( GLfloat width )

    CARAPI GlLineWidth(
        /* [in] */ Float width);

    // C function void glLinkProgram ( GLuint program )

    CARAPI GlLinkProgram(
        /* [in] */ Int32 program);

    // C function void glPixelStorei ( GLenum pname, GLint param )

    CARAPI GlPixelStorei(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    // C function void glPolygonOffset ( GLfloat factor, GLfloat units )

    CARAPI GlPolygonOffset(
        /* [in] */ Float factor,
        /* [in] */ Float units);

    // C function void glReadPixels ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
    // GLenum type, GLvoid *pixels )

    CARAPI GlReadPixels(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    // C function void glReleaseShaderCompiler ( void )

    CARAPI GlReleaseShaderCompiler();

    // C function void glRenderbufferStorage ( GLenum target, GLenum internalformat, GLsizei width, GLsizei height )

    CARAPI GlRenderbufferStorage(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // C function void glSampleCoverage ( GLclampf value, GLboolean invert )

    CARAPI GlSampleCoverage(
        /* [in] */ Float value,
        /* [in] */ Boolean invert);

    // C function void glScissor ( GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI GlScissor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // C function void glShaderBinary ( GLsizei n, const GLuint *shaders, GLenum binaryformat,
    // const GLvoid *binary, GLsizei length )

    CARAPI GlShaderBinary(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* shaders,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 binaryformat,
        /* [in] */ IBuffer* binary,
        /* [in] */ Int32 length);

    // C function void glShaderBinary ( GLsizei n, const GLuint *shaders, GLenum binaryformat,
    // const GLvoid *binary, GLsizei length )

    CARAPI GlShaderBinary(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* shaders,
        /* [in] */ Int32 binaryformat,
        /* [in] */ IBuffer* binary,
        /* [in] */ Int32 length);

    // C function void glShaderSource ( GLuint shader, GLsizei count, const GLchar ** string, const GLint* length )

    CARAPI GlShaderSource(
        /* [in] */ Int32 shader,
        /* [in] */ const String& string);
    // C function void glStencilFunc ( GLenum func, GLint ref, GLuint mask )

    CARAPI GlStencilFunc(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    // C function void glStencilFuncSeparate ( GLenum face, GLenum func, GLint ref, GLuint mask )

    CARAPI GlStencilFuncSeparate(
        /* [in] */ Int32 face,
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    // C function void glStencilMask ( GLuint mask )

    CARAPI GlStencilMask(
        /* [in] */ Int32 mask);

    // C function void glStencilMaskSeparate ( GLenum face, GLuint mask )

    CARAPI GlStencilMaskSeparate(
        /* [in] */ Int32 face,
        /* [in] */ Int32 mask);

    // C function void glStencilOp ( GLenum fail, GLenum zfail, GLenum zpass )

    CARAPI GlStencilOp(
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    // C function void glStencilOpSeparate ( GLenum face, GLenum fail, GLenum zfail, GLenum zpass )

    CARAPI GlStencilOpSeparate(
        /* [in] */ Int32 face,
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    // C function void glTexImage2D ( GLenum target, GLint level, GLint internalformat, GLsizei width,
    // GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels )

    CARAPI GlTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    // C function void glTexParameterf ( GLenum target, GLenum pname, GLfloat param )

    CARAPI GlTexParameterf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    // C function void glTexParameterfv ( GLenum target, GLenum pname, const GLfloat *params )

    CARAPI GlTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glTexParameterfv ( GLenum target, GLenum pname, const GLfloat *params )

    CARAPI GlTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glTexParameteri ( GLenum target, GLenum pname, GLint param )

    CARAPI GlTexParameteri(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    // C function void glTexParameteriv ( GLenum target, GLenum pname, const GLint *params )

    CARAPI GlTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glTexParameteriv ( GLenum target, GLenum pname, const GLint *params )

    CARAPI GlTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glTexSubImage2D ( GLenum target, GLint level, GLint xoffset,
    // GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )

    CARAPI GlTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    // C function void glUniform1f ( GLint location, GLfloat x )

    CARAPI GlUniform1f(
        /* [in] */ Int32 location,
        /* [in] */ Float x);

    // C function void glUniform1fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform1fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform1fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform1fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform1i ( GLint location, GLint x )

    CARAPI GlUniform1i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x);

    // C function void glUniform1iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform1iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform1iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform1iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniform2f ( GLint location, GLfloat x, GLfloat y )

    CARAPI GlUniform2f(
        /* [in] */ Int32 location,
        /* [in] */ Float x,
        /* [in] */ Float y);

    // C function void glUniform2fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform2fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform2i ( GLint location, GLint x, GLint y )

    CARAPI GlUniform2i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // C function void glUniform2iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform2iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform2iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform2iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniform3f ( GLint location, GLfloat x, GLfloat y, GLfloat z )

    CARAPI GlUniform3f(
        /* [in] */ Int32 location,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    // C function void glUniform3fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform3fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform3i ( GLint location, GLint x, GLint y, GLint z )

    CARAPI GlUniform3i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    // C function void glUniform3iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform3iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform3iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform3iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniform4f ( GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w )

    CARAPI GlUniform4f(
        /* [in] */ Int32 location,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z,
        /* [in] */ Float w);

    // C function void glUniform4fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform4fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI GlUniform4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform4i ( GLint location, GLint x, GLint y, GLint z, GLint w )

    CARAPI GlUniform4i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w);

    // C function void glUniform4iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform4iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform4iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI GlUniform4iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniformMatrix2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glUniformMatrix2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glUniformMatrix3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glUniformMatrix3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glUniformMatrix4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glUniformMatrix4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlUniformMatrix4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glUseProgram ( GLuint program )

    CARAPI GlUseProgram(
        /* [in] */ Int32 program);

    // C function void glValidateProgram ( GLuint program )

    CARAPI GlValidateProgram(
        /* [in] */ Int32 program);

    // C function void glVertexAttrib1f ( GLuint indx, GLfloat x )

    CARAPI GlVertexAttrib1f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x);

    // C function void glVertexAttrib1fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib1fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib1fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib1fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttrib2f ( GLuint indx, GLfloat x, GLfloat y )

    CARAPI GlVertexAttrib2f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x,
        /* [in] */ Float y);

    // C function void glVertexAttrib2fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib2fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib2fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib2fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttrib3f ( GLuint indx, GLfloat x, GLfloat y, GLfloat z )

    CARAPI GlVertexAttrib3f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    // C function void glVertexAttrib3fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib3fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib3fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib3fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttrib4f ( GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w )

    CARAPI GlVertexAttrib4f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z,
        /* [in] */ Float w);

    // C function void glVertexAttrib4fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib4fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib4fv ( GLuint indx, const GLfloat *values )

    CARAPI GlVertexAttrib4fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttribPointer ( GLuint indx, GLint size, GLenum type, GLboolean normalized,
    // GLsizei stride, GLint offset )

    CARAPI GlVertexAttribPointer(
        /* [in] */ Int32 indx,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Boolean normalized,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    // C function void glVertexAttribPointer ( GLuint indx, GLint size, GLenum type,
    // GLboolean normalized, GLsizei stride, const GLvoid *ptr )

    CARAPI GlVertexAttribPointer(
        /* [in] */ Int32 indx,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Boolean normalized,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* ptr);

    // C function void glViewport ( GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI GlViewport(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    CARAPI GlVertexAttribPointerBounds(
        /* [in] */ Int32 indx,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Boolean normalized,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* ptr,
        /* [in] */ Int32 remaining);
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __CGLUTILS_H__
