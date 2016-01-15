
#ifndef __ELASTOS_DROID_OPENGL_CGLES20_H__
#define __ELASTOS_DROID_OPENGL_CGLES20_H__

#include "_Elastos_Droid_Opengl_CGLES20.h"

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

CarClass(CGLES20)
{
public:
    CARAPI glActiveTexture(
        /* [in] */ Int32 texture);

    // C function void glAttachShader ( GLuint program, GLuint shader )

    CARAPI glAttachShader(
        /* [in] */ Int32 program,
        /* [in] */ Int32 shader);

    // C function void glBindAttribLocation ( GLuint program, GLuint index, const char *name )

    CARAPI glBindAttribLocation(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ const String& name);

    // C function void glBindBuffer ( GLenum target, GLuint buffer )

    CARAPI glBindBuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 buffer);

    // C function void glBindFramebuffer ( GLenum target, GLuint framebuffer )

    CARAPI glBindFramebuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 framebuffer);

    // C function void glBindRenderbuffer ( GLenum target, GLuint renderbuffer )

    CARAPI glBindRenderbuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 renderbuffer);

    // C function void glBindTexture ( GLenum target, GLuint texture )

    CARAPI glBindTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 texture);

    // C function void glBlendColor ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )

    CARAPI glBlendColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    // C function void glBlendEquation ( GLenum mode )

    CARAPI glBlendEquation(
        /* [in] */ Int32 mode);

    // C function void glBlendEquationSeparate ( GLenum modeRGB, GLenum modeAlpha )

    CARAPI glBlendEquationSeparate(
        /* [in] */ Int32 modeRGB,
        /* [in] */ Int32 modeAlpha);

    // C function void glBlendFunc ( GLenum sfactor, GLenum dfactor )

    CARAPI glBlendFunc(
        /* [in] */ Int32 sfactor,
        /* [in] */ Int32 dfactor);

    // C function void glBlendFuncSeparate ( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )

    CARAPI glBlendFuncSeparate(
        /* [in] */ Int32 srcRGB,
        /* [in] */ Int32 dstRGB,
        /* [in] */ Int32 srcAlpha,
        /* [in] */ Int32 dstAlpha);

    // C function void glBufferData ( GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage )

    CARAPI glBufferData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data,
        /* [in] */ Int32 usage);

    // C function void glBufferSubData ( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data )

    CARAPI glBufferSubData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data);

    // C function GLenum glCheckFramebufferStatus ( GLenum target )

    CARAPI glCheckFramebufferStatus(
        /* [in] */ Int32 target,
        /* [out] */ Int32* rst);

    // C function void glClear ( GLbitfield mask )

    CARAPI glClear(
        /* [in] */ Int32 mask);

    // C function void glClearColor ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )

    CARAPI glClearColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    // C function void glClearDepthf ( GLclampf depth )

    CARAPI glClearDepthf(
        /* [in] */ Float depth);

    // C function void glClearStencil ( GLint s )

    CARAPI glClearStencil(
        /* [in] */ Int32 s);

    // C function void glColorMask ( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )

    CARAPI glColorMask(
        /* [in] */ Boolean red,
        /* [in] */ Boolean green,
        /* [in] */ Boolean blue,
        /* [in] */ Boolean alpha);

    // C function void glCompileShader ( GLuint shader )

    CARAPI glCompileShader(
        /* [in] */ Int32 shader);

    // C function void glCompressedTexImage2D ( GLenum target, GLint level, GLenum internalformat,
    // GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data )

    CARAPI glCompressedTexImage2D(
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

    CARAPI glCompressedTexSubImage2D(
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

    CARAPI glCopyTexImage2D(
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

    CARAPI glCopyTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // C function GLuint glCreateProgram ( void )

    CARAPI glCreateProgram(
        /* [out] */ Int32* rst);

    // C function GLuint glCreateShader ( GLenum type )

    CARAPI glCreateShader(
        /* [in] */ Int32 type,
        /* [out] */ Int32* rst);

    // C function void glCullFace ( GLenum mode )

    CARAPI glCullFace(
        /* [in] */ Int32 mode);

    // C function void glDeleteBuffers ( GLsizei n, const GLuint *buffers )

    CARAPI glDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    // C function void glDeleteBuffers ( GLsizei n, const GLuint *buffers )

    CARAPI glDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers);

    // C function void glDeleteFramebuffers ( GLsizei n, const GLuint *framebuffers )

    CARAPI glDeleteFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    // C function void glDeleteFramebuffers ( GLsizei n, const GLuint *framebuffers )

    CARAPI glDeleteFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    // C function void glDeleteProgram ( GLuint program )

    CARAPI glDeleteProgram(
        /* [in] */ Int32 program);

    // C function void glDeleteRenderbuffers ( GLsizei n, const GLuint *renderbuffers )

    CARAPI glDeleteRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    // C function void glDeleteRenderbuffers ( GLsizei n, const GLuint *renderbuffers )

    CARAPI glDeleteRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

    // C function void glDeleteShader ( GLuint shader )

    CARAPI glDeleteShader(
        /* [in] */ Int32 shader);

    // C function void glDeleteTextures ( GLsizei n, const GLuint *textures )

    CARAPI glDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    // C function void glDeleteTextures ( GLsizei n, const GLuint *textures )

    CARAPI glDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    // C function void glDepthFunc ( GLenum func )

    CARAPI glDepthFunc(
        /* [in] */ Int32 func);

    // C function void glDepthMask ( GLboolean flag )

    CARAPI glDepthMask(
        /* [in] */ Boolean flag);

    // C function void glDepthRangef ( GLclampf zNear, GLclampf zFar )

    CARAPI glDepthRangef(
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    // C function void glDetachShader ( GLuint program, GLuint shader )

    CARAPI glDetachShader(
        /* [in] */ Int32 program,
        /* [in] */ Int32 shader);

    // C function void glDisable ( GLenum cap )

    CARAPI glDisable(
        /* [in] */ Int32 cap);

    // C function void glDisableVertexAttribArray ( GLuint index )

    CARAPI glDisableVertexAttribArray(
        /* [in] */ Int32 index);

    // C function void glDrawArrays ( GLenum mode, GLint first, GLsizei count )

    CARAPI glDrawArrays(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 first,
        /* [in] */ Int32 count);

    // C function void glDrawElements ( GLenum mode, GLsizei count, GLenum type, GLint offset )

    CARAPI glDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset);

    // C function void glDrawElements ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )

    CARAPI glDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* indices);

    // C function void glEnable ( GLenum cap )

    CARAPI glEnable(
        /* [in] */ Int32 cap);

    // C function void glEnableVertexAttribArray ( GLuint index )

    CARAPI glEnableVertexAttribArray(
        /* [in] */ Int32 index);

    // C function void glFinish ( void )

    CARAPI glFinish();

    // C function void glFlush ( void )

    CARAPI glFlush();

    // C function void glFramebufferRenderbuffer ( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )

    CARAPI glFramebufferRenderbuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 renderbuffertarget,
        /* [in] */ Int32 renderbuffer);

    // C function void glFramebufferTexture2D ( GLenum target, GLenum attachment, GLenum textarget,
    // GLuint texture, GLint level )

    CARAPI glFramebufferTexture2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 textarget,
        /* [in] */ Int32 texture,
        /* [in] */ Int32 level);

    // C function void glFrontFace ( GLenum mode )

    CARAPI glFrontFace(
        /* [in] */ Int32 mode);

    // C function void glGenBuffers ( GLsizei n, GLuint *buffers )

    CARAPI glGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    // C function void glGenBuffers ( GLsizei n, GLuint *buffers )

    CARAPI glGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers);

    // C function void glGenerateMipmap ( GLenum target )

    CARAPI glGenerateMipmap(
        /* [in] */ Int32 target);

    // C function void glGenFramebuffers ( GLsizei n, GLuint *framebuffers )

    CARAPI glGenFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    // C function void glGenFramebuffers ( GLsizei n, GLuint *framebuffers )

    CARAPI glGenFramebuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    // C function void glGenRenderbuffers ( GLsizei n, GLuint *renderbuffers )

    CARAPI glGenRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    // C function void glGenRenderbuffers ( GLsizei n, GLuint *renderbuffers )

    CARAPI glGenRenderbuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

    // C function void glGenTextures ( GLsizei n, GLuint *textures )

    CARAPI glGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    // C function void glGenTextures ( GLsizei n, GLuint *textures )

    CARAPI glGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    // C function void glGetActiveAttrib ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI glGetActiveAttrib(
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

    CARAPI glGetActiveAttrib(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ Int32 bufsize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [in] */ Byte name);

    // C function void glGetActiveAttrib ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name )

    CARAPI glGetActiveAttrib(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* size,
        /* [in] */ Int32 sizeOffset,
        /* [in] */ ArrayOf<Int32>* type,
        /* [in] */ Int32 typeOffset,
        /* [out] */ String* str);

    // C function void glGetActiveAttrib ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name )

    CARAPI glGetActiveAttrib(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [out] */ String* rst);
    // C function void glGetActiveUniform ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name )

    CARAPI glGetActiveUniform(
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

    CARAPI glGetActiveUniform(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ Int32 bufsize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [in] */ Byte name);
    // C function void glGetActiveUniform ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI glGetActiveUniform(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* size,
        /* [in] */ Int32 sizeOffset,
        /* [in] */ ArrayOf<Int32>* type,
        /* [in] */ Int32 typeOffset,
        /* [out] */ String* rst);

    // C function void glGetActiveUniform ( GLuint program, GLuint index, GLsizei bufsize, GLsizei *length,
    // GLint *size, GLenum *type, char *name )

    CARAPI glGetActiveUniform(
        /* [in] */ Int32 program,
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* size,
        /* [in] */ IInt32Buffer* type,
        /* [out] */ String* str);
    // C function void glGetAttachedShaders ( GLuint program, GLsizei maxcount, GLsizei *count, GLuint *shaders )

    CARAPI glGetAttachedShaders(
        /* [in] */ Int32 program,
        /* [in] */ Int32 maxcount,
        /* [in] */ ArrayOf<Int32>* count,
        /* [in] */ Int32 countOffset,
        /* [in] */ ArrayOf<Int32>* shaders,
        /* [in] */ Int32 shadersOffset);

    // C function void glGetAttachedShaders ( GLuint program, GLsizei maxcount, GLsizei *count, GLuint *shaders )

    CARAPI glGetAttachedShaders(
        /* [in] */ Int32 program,
        /* [in] */ Int32 maxcount,
        /* [in] */ IInt32Buffer* count,
        /* [in] */ IInt32Buffer* shaders);

    // C function int glGetAttribLocation ( GLuint program, const char *name )

    CARAPI glGetAttribLocation(
        /* [in] */ Int32 program,
        /* [in] */ const String& name,
        /* [out] */ Int32* rst);

    // C function void glGetBooleanv ( GLenum pname, GLboolean *params )

    CARAPI glGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Boolean>* params,
        /* [in] */ Int32 offset);

    // C function void glGetBooleanv ( GLenum pname, GLboolean *params )

    CARAPI glGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetBufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI glGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetBufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI glGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function GLenum glGetError ( void )

    CARAPI glGetError(
        /* [out] */ Int32* rst);

    // C function void glGetFloatv ( GLenum pname, GLfloat *params )

    CARAPI glGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetFloatv ( GLenum pname, GLfloat *params )

    CARAPI glGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetFramebufferAttachmentParameteriv ( GLenum target, GLenum attachment,
    // GLenum pname, GLint *params )

    CARAPI glGetFramebufferAttachmentParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetFramebufferAttachmentParameteriv ( GLenum target, GLenum attachment, GLenum pname,
    // GLint *params )

    CARAPI glGetFramebufferAttachmentParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetIntegerv ( GLenum pname, GLint *params )

    CARAPI glGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetIntegerv ( GLenum pname, GLint *params )

    CARAPI glGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetProgramiv ( GLuint program, GLenum pname, GLint *params )

    CARAPI glGetProgramiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetProgramiv ( GLuint program, GLenum pname, GLint *params )

    CARAPI glGetProgramiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetProgramInfoLog(
        /* [in] */ Int32 program,
        /* [out] */ String* str);
    // C function void glGetRenderbufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI glGetRenderbufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetRenderbufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI glGetRenderbufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetShaderiv ( GLuint shader, GLenum pname, GLint *params )

    CARAPI glGetShaderiv(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetShaderiv ( GLuint shader, GLenum pname, GLint *params )

    CARAPI glGetShaderiv(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetShaderInfoLog( GLuint shader, GLsizei maxLength, GLsizei * length,
    //     CARAPI glchar * infoLog);

    CARAPI glGetShaderInfoLog(
        /* [in] */ Int32 shader,
        /* [out] */ String* str);

    // C function void glGetShaderPrecisionFormat ( GLenum shadertype, GLenum precisiontype,
    // GLint *range, GLint *precision )

    CARAPI glGetShaderPrecisionFormat(
        /* [in] */ Int32 shadertype,
        /* [in] */ Int32 precisiontype,
        /* [in] */ ArrayOf<Int32>* range,
        /* [in] */ Int32 rangeOffset,
        /* [in] */ ArrayOf<Int32>* precision,
        /* [in] */ Int32 precisionOffset);

    // C function void glGetShaderPrecisionFormat ( GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision )

    CARAPI glGetShaderPrecisionFormat(
        /* [in] */ Int32 shadertype,
        /* [in] */ Int32 precisiontype,
        /* [in] */ IInt32Buffer* range,
        /* [in] */ IInt32Buffer* precision);

    // C function void glGetShaderSource ( GLuint shader, GLsizei bufsize, GLsizei *length, char *source )

    CARAPI glGetShaderSource(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 bufsize,
        /* [in] */ ArrayOf<Int32>* length,
        /* [in] */ Int32 lengthOffset,
        /* [in] */ ArrayOf<Byte>* source,
        /* [in] */ Int32 sourceOffset);

    // C function void glGetShaderSource ( GLuint shader, GLsizei bufsize, GLsizei *length, char *source )

    CARAPI glGetShaderSource(
        /* [in] */ Int32 shader,
        /* [in] */ Int32 bufsize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ Byte source);

    // C function void glGetShaderSource ( GLuint shader, GLsizei bufsize, GLsizei *length, char *source )

    CARAPI glGetShaderSource(
        /* [in] */ Int32 shader,
        /* [out] */ String* rst);


    // C function const GLubyte * glGetString ( GLenum name )

    CARAPI glGetString(
        /* [in] */ Int32 name,
        /* [out] */ String* str);
    // C function void glGetTexParameterfv ( GLenum target, GLenum pname, GLfloat *params )

    CARAPI glGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetTexParameterfv ( GLenum target, GLenum pname, GLfloat *params )

    CARAPI glGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetTexParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI glGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetTexParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI glGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetUniformfv ( GLuint program, GLint location, GLfloat *params )

    CARAPI glGetUniformfv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetUniformfv ( GLuint program, GLint location, GLfloat *params )

    CARAPI glGetUniformfv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetUniformiv ( GLuint program, GLint location, GLint *params )

    CARAPI glGetUniformiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetUniformiv ( GLuint program, GLint location, GLint *params )

    CARAPI glGetUniformiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ IInt32Buffer* params);

    // C function int glGetUniformLocation ( GLuint program, const char *name )

    CARAPI glGetUniformLocation(
        /* [in] */ Int32 program,
        /* [in] */ const String& name,
        /* [out] */ Int32* rst);

    // C function void glGetVertexAttribfv ( GLuint index, GLenum pname, GLfloat *params )

    CARAPI glGetVertexAttribfv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetVertexAttribfv ( GLuint index, GLenum pname, GLfloat *params )

    CARAPI glGetVertexAttribfv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glGetVertexAttribiv ( GLuint index, GLenum pname, GLint *params )

    CARAPI glGetVertexAttribiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetVertexAttribiv ( GLuint index, GLenum pname, GLint *params )

    CARAPI glGetVertexAttribiv(
        /* [in] */ Int32 index,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glHint ( GLenum target, GLenum mode )

    CARAPI glHint(
        /* [in] */ Int32 target,
        /* [in] */ Int32 mode);

    // C function GLboolean glIsBuffer ( GLuint buffer )

    CARAPI glIsBuffer(
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsEnabled ( GLenum cap )

    CARAPI glIsEnabled(
        /* [in] */ Int32 cap,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsFramebuffer ( GLuint framebuffer )

    CARAPI glIsFramebuffer(
        /* [in] */ Int32 framebuffer,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsProgram ( GLuint program )

    CARAPI glIsProgram(
        /* [in] */ Int32 program,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsRenderbuffer ( GLuint renderbuffer )

    CARAPI glIsRenderbuffer(
        /* [in] */ Int32 renderbuffer,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsShader ( GLuint shader )

    CARAPI glIsShader(
        /* [in] */ Int32 shader,
        /* [out] */ Boolean* rst);

    // C function GLboolean glIsTexture ( GLuint texture )

    CARAPI glIsTexture(
        /* [in] */ Int32 texture,
        /* [out] */ Boolean* rst);

    // C function void glLineWidth ( GLfloat width )

    CARAPI glLineWidth(
        /* [in] */ Float width);

    // C function void glLinkProgram ( GLuint program )

    CARAPI glLinkProgram(
        /* [in] */ Int32 program);

    // C function void glPixelStorei ( GLenum pname, GLint param )

    CARAPI glPixelStorei(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    // C function void glPolygonOffset ( GLfloat factor, GLfloat units )

    CARAPI glPolygonOffset(
        /* [in] */ Float factor,
        /* [in] */ Float units);

    // C function void glReadPixels ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
    // GLenum type, GLvoid *pixels )

    CARAPI glReadPixels(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    // C function void glReleaseShaderCompiler ( void )

    CARAPI glReleaseShaderCompiler();

    // C function void glRenderbufferStorage ( GLenum target, GLenum internalformat, GLsizei width, GLsizei height )

    CARAPI glRenderbufferStorage(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // C function void glSampleCoverage ( GLclampf value, GLboolean invert )

    CARAPI glSampleCoverage(
        /* [in] */ Float value,
        /* [in] */ Boolean invert);

    // C function void glScissor ( GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI glScissor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // C function void glShaderBinary ( GLsizei n, const GLuint *shaders, GLenum binaryformat,
    // const GLvoid *binary, GLsizei length )

    CARAPI glShaderBinary(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* shaders,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 binaryformat,
        /* [in] */ IBuffer* binary,
        /* [in] */ Int32 length);

    // C function void glShaderBinary ( GLsizei n, const GLuint *shaders, GLenum binaryformat,
    // const GLvoid *binary, GLsizei length )

    CARAPI glShaderBinary(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* shaders,
        /* [in] */ Int32 binaryformat,
        /* [in] */ IBuffer* binary,
        /* [in] */ Int32 length);

    // C function void glShaderSource ( GLuint shader, GLsizei count, const GLchar ** string, const GLint* length )

    CARAPI glShaderSource(
        /* [in] */ Int32 shader,
        /* [in] */ const String& string);
    // C function void glStencilFunc ( GLenum func, GLint ref, GLuint mask )

    CARAPI glStencilFunc(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    // C function void glStencilFuncSeparate ( GLenum face, GLenum func, GLint ref, GLuint mask )

    CARAPI glStencilFuncSeparate(
        /* [in] */ Int32 face,
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    // C function void glStencilMask ( GLuint mask )

    CARAPI glStencilMask(
        /* [in] */ Int32 mask);

    // C function void glStencilMaskSeparate ( GLenum face, GLuint mask )

    CARAPI glStencilMaskSeparate(
        /* [in] */ Int32 face,
        /* [in] */ Int32 mask);

    // C function void glStencilOp ( GLenum fail, GLenum zfail, GLenum zpass )

    CARAPI glStencilOp(
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    // C function void glStencilOpSeparate ( GLenum face, GLenum fail, GLenum zfail, GLenum zpass )

    CARAPI glStencilOpSeparate(
        /* [in] */ Int32 face,
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    // C function void glTexImage2D ( GLenum target, GLint level, GLint internalformat, GLsizei width,
    // GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels )

    CARAPI glTexImage2D(
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

    CARAPI glTexParameterf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    // C function void glTexParameterfv ( GLenum target, GLenum pname, const GLfloat *params )

    CARAPI glTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glTexParameterfv ( GLenum target, GLenum pname, const GLfloat *params )

    CARAPI glTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glTexParameteri ( GLenum target, GLenum pname, GLint param )

    CARAPI glTexParameteri(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    // C function void glTexParameteriv ( GLenum target, GLenum pname, const GLint *params )

    CARAPI glTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glTexParameteriv ( GLenum target, GLenum pname, const GLint *params )

    CARAPI glTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glTexSubImage2D ( GLenum target, GLint level, GLint xoffset,
    // GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )

    CARAPI glTexSubImage2D(
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

    CARAPI glUniform1f(
        /* [in] */ Int32 location,
        /* [in] */ Float x);

    // C function void glUniform1fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform1fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform1fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform1fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform1i ( GLint location, GLint x )

    CARAPI glUniform1i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x);

    // C function void glUniform1iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform1iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform1iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform1iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniform2f ( GLint location, GLfloat x, GLfloat y )

    CARAPI glUniform2f(
        /* [in] */ Int32 location,
        /* [in] */ Float x,
        /* [in] */ Float y);

    // C function void glUniform2fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform2fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform2i ( GLint location, GLint x, GLint y )

    CARAPI glUniform2i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // C function void glUniform2iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform2iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform2iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform2iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniform3f ( GLint location, GLfloat x, GLfloat y, GLfloat z )

    CARAPI glUniform3f(
        /* [in] */ Int32 location,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    // C function void glUniform3fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform3fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform3i ( GLint location, GLint x, GLint y, GLint z )

    CARAPI glUniform3i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    // C function void glUniform3iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform3iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform3iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform3iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniform4f ( GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w )

    CARAPI glUniform4f(
        /* [in] */ Int32 location,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z,
        /* [in] */ Float w);

    // C function void glUniform4fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform4fv ( GLint location, GLsizei count, const GLfloat *v )

    CARAPI glUniform4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* v);

    // C function void glUniform4i ( GLint location, GLint x, GLint y, GLint z, GLint w )

    CARAPI glUniform4i(
        /* [in] */ Int32 location,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w);

    // C function void glUniform4iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform4iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* v,
        /* [in] */ Int32 offset);

    // C function void glUniform4iv ( GLint location, GLsizei count, const GLint *v )

    CARAPI glUniform4iv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* v);

    // C function void glUniformMatrix2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI glUniformMatrix2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glUniformMatrix2fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI glUniformMatrix2fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glUniformMatrix3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI glUniformMatrix3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glUniformMatrix3fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI glUniformMatrix3fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glUniformMatrix4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI glUniformMatrix4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glUniformMatrix4fv ( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI glUniformMatrix4fv(
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glUseProgram ( GLuint program )

    CARAPI glUseProgram(
        /* [in] */ Int32 program);

    // C function void glValidateProgram ( GLuint program )

    CARAPI glValidateProgram(
        /* [in] */ Int32 program);

    // C function void glVertexAttrib1f ( GLuint indx, GLfloat x )

    CARAPI glVertexAttrib1f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x);

    // C function void glVertexAttrib1fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib1fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib1fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib1fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttrib2f ( GLuint indx, GLfloat x, GLfloat y )

    CARAPI glVertexAttrib2f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x,
        /* [in] */ Float y);

    // C function void glVertexAttrib2fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib2fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib2fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib2fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttrib3f ( GLuint indx, GLfloat x, GLfloat y, GLfloat z )

    CARAPI glVertexAttrib3f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    // C function void glVertexAttrib3fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib3fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib3fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib3fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttrib4f ( GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w )

    CARAPI glVertexAttrib4f(
        /* [in] */ Int32 indx,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z,
        /* [in] */ Float w);

    // C function void glVertexAttrib4fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib4fv(
        /* [in] */ Int32 indx,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 offset);

    // C function void glVertexAttrib4fv ( GLuint indx, const GLfloat *values )

    CARAPI glVertexAttrib4fv(
        /* [in] */ Int32 indx,
        /* [in] */ IFloatBuffer* values);

    // C function void glVertexAttribPointer ( GLuint indx, GLint size, GLenum type, GLboolean normalized,
    // GLsizei stride, GLint offset )

    CARAPI glVertexAttribPointer(
        /* [in] */ Int32 indx,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Boolean normalized,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    // C function void glVertexAttribPointer ( GLuint indx, GLint size, GLenum type,
    // GLboolean normalized, GLsizei stride, const GLvoid *ptr )

    CARAPI glVertexAttribPointer(
        /* [in] */ Int32 indx,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Boolean normalized,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* ptr);

    // C function void glViewport ( GLint x, GLint y, GLsizei width, GLsizei height )

    CARAPI glViewport(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //a temporal workaroud for egl initialize
    static ECode InitGL(IInterface* surface);
private:
    static CARAPI_(Int32) GetNumCompressedTextureFormats();

    static CARAPI GetDirectBufferPointer(
    /* [in] */ IBuffer* buffer,
    /* [out] */ Handle32* result);

    CARAPI glVertexAttribPointerBounds(
        /* [in] */ Int32 indx,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Boolean normalized,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* ptr,
        /* [in] */ Int32 remaining);

    CARAPI GetPointer(
        /* [in] */ IBuffer* buffer,
        /* [in, out] */ Handle32* array,
        /* [in, out] */ Int32* remaining,
        /* [in, out] */ Int32* offset,
        /* [out] */ Handle32* rst);

    //begin a temporal workaroud for egl initialize
    static AutoPtr<IEGL10> mEgl;
    static AutoPtr<XIEGLDisplay> mEglDisplay;
    static AutoPtr<XIEGLSurface> mEglSurface;
    static AutoPtr<XIEGLConfig> mEglConfig;
    static AutoPtr<XIEGLContext> mEglContext;
    //end a temporal workaroud for egl initialize

};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __CGLUTILS_H__
