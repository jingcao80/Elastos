#include "elastos/droid/opengl/gles/CGLImpl.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Build.h"
#include "Elastos.CoreLibrary.IO.h"

#include <elastos/utility/logging/Slogger.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

// Work around differences between the generated name and the actual name.

#define glBlendEquation glBlendEquationOES
#define glBlendEquationSeparate glBlendEquationSeparateOES
#define glBlendFuncSeparate glBlendFuncSeparateOES
#define glGetTexGenfv glGetTexGenfvOES
#define glGetTexGeniv glGetTexGenivOES
#define glGetTexGenxv glGetTexGenxvOES
#define glTexGenf glTexGenfOES
#define glTexGenfv glTexGenfvOES
#define glTexGeni glTexGeniOES
#define glTexGeniv glTexGenivOES
#define glTexGenx glTexGenxOES
#define glTexGenxv glTexGenxvOES

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::App::AppGlobals;
using Elastos::IO::INIOAccess;
using Elastos::IO::CNIOAccess;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11ExtensionPack;

extern "C" {
extern char*  __progname;
}

/* special calls implemented in Android's GLES wrapper used to more
 * efficiently bound-check passed arrays */
extern "C" {
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
}

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

static Int32
getNumCompressedTextureFormats() {
    Int32 numCompressedTextureFormats = 0;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numCompressedTextureFormats);
    return numCompressedTextureFormats;
}

// Check if the extension at the head of pExtensions is pExtension. Note that pExtensions is
// terminated by either 0 or space, while pExtension is terminated by 0.

static bool
extensionEqual(const GLubyte* pExtensions, const GLubyte* pExtension) {
    while (true) {
        char a = *pExtensions++;
        char b = *pExtension++;
        bool aEnd = a == '\0' || a == ' ';
        bool bEnd = b == '\0';
        if ( aEnd || bEnd) {
            return aEnd == bEnd;
        }
        if ( a != b ) {
            return false;
        }
    }
}

static const GLubyte*
nextExtension(const GLubyte* pExtensions) {
    while (TRUE) {
        char a = *pExtensions++;
        if ( a == '\0') {
            return pExtensions-1;
        } else if ( a == ' ') {
            return pExtensions;
        }
    }
}

static bool
checkForExtension(const GLubyte* pExtensions, const GLubyte* pExtension) {
    for (;*pExtensions != '\0'; pExtensions = nextExtension(pExtensions)) {
        if (extensionEqual(pExtensions, pExtension)) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CGLImpl::SupportsExtension(
    /* [in] */ CGLImpl* impl,
    /* [in] */ Int32 id)
{
    if (!impl->mHaveCheckedExtensions) {
        impl->mHaveCheckedExtensions = TRUE;
        const GLubyte* sExtensions = glGetString(GL_EXTENSIONS);
        impl->mHave_OES_blend_equation_separate =
            checkForExtension(sExtensions, (const GLubyte*) "GL_OES_blend_equation_separate");
        impl->mHave_OES_blend_subtract =
            checkForExtension(sExtensions, (const GLubyte*) "GL_OES_blend_subtract");
        impl->mHave_OES_framebuffer_object =
            checkForExtension(sExtensions, (const GLubyte*) "GL_OES_framebuffer_object");
        impl->mHave_OES_texture_cube_map =
            checkForExtension(sExtensions, (const GLubyte*) "GL_OES_texture_cube_map");
    }
    Boolean rst = FALSE;
    switch(id) {
    case HAVE_OES_FRAMEBUFFER_OBJECTID:
        rst = impl->mHave_OES_framebuffer_object;
        break;
    case HAVE_OES_BLEND_SUBTRACTID:
        rst = impl->mHave_OES_framebuffer_object;
        break;
    case HAVE_OES_BLEND_EQUATION_SEPARATEID:
        rst = impl->mHave_OES_blend_equation_separate;
        break;
    case HAVE_OES_TEXTURE_CUBE_MAPID:
        rst = impl->mHave_OES_texture_cube_map;
        break;
    }
    return rst;
}

CAR_INTERFACE_IMPL_5(CGLImpl, Object, IGL10, IGL10Ext, IGL11, IGL11Ext, IGL11ExtensionPack)
CAR_OBJECT_IMPL(CGLImpl)

ECode CGLImpl::constructor()
{
    return NOERROR;
}

ECode CGLImpl::GlActiveTexture(
    /* [in] */ Int32 texture)
{
    glActiveTexture(
        (GLenum)texture
    );
    return NOERROR;
}

ECode CGLImpl::GlAlphaFunc(
    /* [in] */ Int32 func,
    /* [in] */ Float ref)
{
    glAlphaFunc(
        (GLenum)func,
        (GLclampf)ref
    );
    return NOERROR;
}

ECode CGLImpl::GlAlphaFuncx(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref)
{
    glAlphaFuncx(
        (GLenum)func,
        (GLclampx)ref
    );
    return NOERROR;
}

ECode CGLImpl::GlBindTexture(
    /* [in] */ Int32 target,
    /* [in] */ Int32 texture)
{
    glBindTexture(
        (GLenum)target,
        (GLuint)texture
    );
    return NOERROR;
}

ECode CGLImpl::GlBlendFunc(
    /* [in] */ Int32 sfactor,
    /* [in] */ Int32 dfactor)
{
    glBlendFunc(
        (GLenum)sfactor,
        (GLenum)dfactor
    );
    return NOERROR;
}

ECode CGLImpl::GlClear(
    /* [in] */ Int32 mask)
{
    glClear(
        (GLbitfield)mask
    );
    return NOERROR;
}

ECode CGLImpl::GlClearColor(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    glClearColor(
        (GLclampf)red,
        (GLclampf)green,
        (GLclampf)blue,
        (GLclampf)alpha
    );
    return NOERROR;
}

ECode CGLImpl::GlClearColorx(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    glClearColorx(
        (GLclampx)red,
        (GLclampx)green,
        (GLclampx)blue,
        (GLclampx)alpha
    );
    return NOERROR;
}

ECode CGLImpl::GlClearDepthf(
    /* [in] */ Float depth)
{
    glClearDepthf(
        (GLclampf)depth
    );
    return NOERROR;
}

ECode CGLImpl::GlClearDepthx(
    /* [in] */ Int32 depth)
{
    glClearDepthx(
        (GLclampx)depth
    );
    return NOERROR;
}

ECode CGLImpl::GlClearStencil(
    /* [in] */ Int32 s)
{
    glClearStencil(
        (GLint)s
    );
    return NOERROR;
}

ECode CGLImpl::GlClientActiveTexture(
    /* [in] */ Int32 texture)
{
    glClientActiveTexture(
        (GLenum)texture
    );
    return NOERROR;
}

ECode CGLImpl::GlColor4f(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    glColor4f(
        (GLfloat)red,
        (GLfloat)green,
        (GLfloat)blue,
        (GLfloat)alpha
    );
    return NOERROR;
}

ECode CGLImpl::GlColor4x(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    glColor4x(
        (GLfixed)red,
        (GLfixed)green,
        (GLfixed)blue,
        (GLfixed)alpha
    );
    return NOERROR;
}

ECode CGLImpl::GlColorMask(
    /* [in] */ Boolean red,
    /* [in] */ Boolean green,
    /* [in] */ Boolean blue,
    /* [in] */ Boolean alpha)
{
    glColorMask(
        (GLboolean)red,
        (GLboolean)green,
        (GLboolean)blue,
        (GLboolean)alpha
    );
    return NOERROR;
}

ECode CGLImpl::GlColorPointerBounds(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        Handle64 tmp;
        GetDirectBufferPointer(pointer_buf, &tmp);
        pointer = (GLvoid *) tmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glColorPointerBounds(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

ECode CGLImpl::GlColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Int32 remaining;
    pointer->GetRemaining(&remaining);
    GlColorPointerBounds(
        size,
        type,
        stride,
        pointer,
        remaining
    );
    if ((size == 4) &&
        ((type == GL_FLOAT) ||
         (type == GL_UNSIGNED_BYTE) ||
         (type == GL_FIXED)) &&
        (stride >= 0)) {
        m_ColorPointer = pointer;
    }
    return NOERROR;
}

ECode CGLImpl::GlCompressedTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *data = (GLvoid *) 0;
    Handle64 tmp;
    GetPointer(data_buf, &_array, &_remaining, &_bufferOffset, &tmp);
    data = (GLvoid *) tmp;
    if (data == NULL) {
        char * _dataBase = reinterpret_cast<char *>(_array);
        data = (GLvoid *) (_dataBase + _bufferOffset);
    }
    glCompressedTexSubImage2D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLsizei)width,
        (GLsizei)height,
        (GLenum)format,
        (GLsizei)imageSize,
        (GLvoid *)data
    );
    return NOERROR;
}

ECode CGLImpl::GlCopyTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glCopyTexSubImage2D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

ECode CGLImpl::GlCullFace(
    /* [in] */ Int32 mode)
{
    glCullFace(
        (GLenum)mode
    );
    return NOERROR;
}

ECode CGLImpl::GlDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures_ref,
    /* [in] */ Int32 offset)
{
    GLuint *textures_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *textures = (GLuint *) 0;

    if (!textures_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = textures_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    textures_base = (GLuint *) textures_ref->GetPayload();
    textures = textures_base + offset;

    glDeleteTextures(
        (GLsizei)n,
        (GLuint *)textures
    );
    return NOERROR;
}

ECode CGLImpl::GlDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *textures = (GLuint *) 0;
    Handle64 tmp;
    GetPointer(IBuffer::Probe(textures_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    textures = (GLuint *)tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (textures == NULL) {
        char * _texturesBase = reinterpret_cast<char *>(_array);
        textures = (GLuint *) (_texturesBase + _bufferOffset);
    }
    glDeleteTextures(
        (GLsizei)n,
        (GLuint *)textures
    );
    return NOERROR;
}

ECode CGLImpl::GlDepthFunc(
    /* [in] */ Int32 func)
{
    glDepthFunc(
        (GLenum)func
    );
    return NOERROR;
}

ECode CGLImpl::GlDepthMask(
    /* [in] */ Boolean flag)
{
    glDepthMask(
        (GLboolean)flag
    );
    return NOERROR;
}

ECode CGLImpl::GlDepthRangef(
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    glDepthRangef(
        (GLclampf)zNear,
        (GLclampf)zFar
    );
    return NOERROR;
}

ECode CGLImpl::GlDepthRangex(
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    glDepthRangex(
        (GLclampx)zNear,
        (GLclampx)zFar
    );
    return NOERROR;
}

ECode CGLImpl::GlDisable(
    /* [in] */ Int32 cap)
{
    glDisable(
        (GLenum)cap
    );
    return NOERROR;
}

ECode CGLImpl::GlDisableClientState(
    /* [in] */ Int32 array)
{
    glDisableClientState(
        (GLenum)array
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawArrays(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 first,
    /* [in] */ Int32 count)
{
    glDrawArrays(
        (GLenum)mode,
        (GLint)first,
        (GLsizei)count
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* indices_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *indices = (GLvoid *) 0;

    Handle64 tmp;
    GetPointer(indices_buf, &_array, &_remaining, &_bufferOffset, &tmp);
    indices = (GLvoid *) tmp;
    if (_remaining < count) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (indices == NULL) {
        char * _indicesBase = reinterpret_cast<char *>(_array);
        indices = (GLvoid *) (_indicesBase + _bufferOffset);
    }
    glDrawElements(
        (GLenum)mode,
        (GLsizei)count,
        (GLenum)type,
        (GLvoid *)indices
    );
    return NOERROR;
}

ECode CGLImpl::GlFinish()
{
    glFinish();
    return NOERROR;
}

ECode CGLImpl::GlFlush()
{
    glFlush();
    return NOERROR;
}

ECode CGLImpl::GlFogf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glFogf(
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_FOG_MODE)
        case GL_FOG_MODE:
#endif // defined(GL_FOG_MODE)
#if defined(GL_FOG_DENSITY)
        case GL_FOG_DENSITY:
#endif // defined(GL_FOG_DENSITY)
#if defined(GL_FOG_START)
        case GL_FOG_START:
#endif // defined(GL_FOG_START)
#if defined(GL_FOG_END)
        case GL_FOG_END:
#endif // defined(GL_FOG_END)
            _needed = 1;
            break;
#if defined(GL_FOG_COLOR)
        case GL_FOG_COLOR:
#endif // defined(GL_FOG_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *) params_ref->GetPayload();
    params = params_base + offset;

    glFogfv(
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_FOG_MODE)
        case GL_FOG_MODE:
#endif // defined(GL_FOG_MODE)
#if defined(GL_FOG_DENSITY)
        case GL_FOG_DENSITY:
#endif // defined(GL_FOG_DENSITY)
#if defined(GL_FOG_START)
        case GL_FOG_START:
#endif // defined(GL_FOG_START)
#if defined(GL_FOG_END)
        case GL_FOG_END:
#endif // defined(GL_FOG_END)
            _needed = 1;
            break;
#if defined(GL_FOG_COLOR)
        case GL_FOG_COLOR:
#endif // defined(GL_FOG_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glFogfv(
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlFogx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glFogx(
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLImpl::GlFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_FOG_MODE)
        case GL_FOG_MODE:
#endif // defined(GL_FOG_MODE)
#if defined(GL_FOG_DENSITY)
        case GL_FOG_DENSITY:
#endif // defined(GL_FOG_DENSITY)
#if defined(GL_FOG_START)
        case GL_FOG_START:
#endif // defined(GL_FOG_START)
#if defined(GL_FOG_END)
        case GL_FOG_END:
#endif // defined(GL_FOG_END)
            _needed = 1;
            break;
#if defined(GL_FOG_COLOR)
        case GL_FOG_COLOR:
#endif // defined(GL_FOG_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *) params_ref->GetPayload();
    params = params_base + offset;

    glFogxv(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_FOG_MODE)
        case GL_FOG_MODE:
#endif // defined(GL_FOG_MODE)
#if defined(GL_FOG_DENSITY)
        case GL_FOG_DENSITY:
#endif // defined(GL_FOG_DENSITY)
#if defined(GL_FOG_START)
        case GL_FOG_START:
#endif // defined(GL_FOG_START)
#if defined(GL_FOG_END)
        case GL_FOG_END:
#endif // defined(GL_FOG_END)
            _needed = 1;
            break;
#if defined(GL_FOG_COLOR)
        case GL_FOG_COLOR:
#endif // defined(GL_FOG_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glFogxv(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlFrontFace(
    /* [in] */ Int32 mode)
{
    glFrontFace(
        (GLenum)mode
    );
    return NOERROR;
}

ECode CGLImpl::GlFrustumf(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    glFrustumf(
        (GLfloat)left,
        (GLfloat)right,
        (GLfloat)bottom,
        (GLfloat)top,
        (GLfloat)zNear,
        (GLfloat)zFar
    );
    return NOERROR;
}

ECode CGLImpl::GlFrustumx(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    glFrustumx(
        (GLfixed)left,
        (GLfixed)right,
        (GLfixed)bottom,
        (GLfixed)top,
        (GLfixed)zNear,
        (GLfixed)zFar
    );
    return NOERROR;
}

ECode CGLImpl::GlGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures_ref,
    /* [in] */ Int32 offset)
{
    GLuint *textures_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *textures = (GLuint *) 0;

    if (!textures_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = textures_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    textures_base = (GLuint *)textures_ref->GetPayload();
    textures = textures_base + offset;

    glGenTextures(
        (GLsizei)n,
        (GLuint *)textures
    );
    return NOERROR;
}

ECode CGLImpl::GlGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *textures = (GLuint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(textures_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    textures = (GLuint *)tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (textures == NULL) {
        char * _texturesBase = reinterpret_cast<char *>(_array);
        textures = (GLuint *) (_texturesBase + _bufferOffset);
    }
    glGenTextures(
        (GLsizei)n,
        (GLuint *)textures
    );
    return NOERROR;
}

ECode CGLImpl::GlGetError(
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error)

    GLenum _returnValue;
    _returnValue = glGetError();
    *error = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlGetString(
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    const char* chars = (const char*) glGetString((GLenum) name);
    *str = String(chars);
    return NOERROR;
}

ECode CGLImpl::GlHint(
    /* [in] */ Int32 target,
    /* [in] */ Int32 mode)
{
    glHint(
        (GLenum)target,
        (GLenum)mode
    );
    return NOERROR;
}

ECode CGLImpl::GlLightModelf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glLightModelf(
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_LIGHT_MODEL_TWO_SIDE)
        case GL_LIGHT_MODEL_TWO_SIDE:
#endif // defined(GL_LIGHT_MODEL_TWO_SIDE)
            _needed = 1;
            break;
#if defined(GL_LIGHT_MODEL_AMBIENT)
        case GL_LIGHT_MODEL_AMBIENT:
#endif // defined(GL_LIGHT_MODEL_AMBIENT)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glLightModelfv(
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_LIGHT_MODEL_TWO_SIDE)
        case GL_LIGHT_MODEL_TWO_SIDE:
#endif // defined(GL_LIGHT_MODEL_TWO_SIDE)
            _needed = 1;
            break;
#if defined(GL_LIGHT_MODEL_AMBIENT)
        case GL_LIGHT_MODEL_AMBIENT:
#endif // defined(GL_LIGHT_MODEL_AMBIENT)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glLightModelfv(
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlLightModelx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glLightModelx(
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLImpl::GlLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_LIGHT_MODEL_TWO_SIDE)
        case GL_LIGHT_MODEL_TWO_SIDE:
#endif // defined(GL_LIGHT_MODEL_TWO_SIDE)
            _needed = 1;
            break;
#if defined(GL_LIGHT_MODEL_AMBIENT)
        case GL_LIGHT_MODEL_AMBIENT:
#endif // defined(GL_LIGHT_MODEL_AMBIENT)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glLightModelxv(
        (GLenum)pname,
        (GLfixed *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_LIGHT_MODEL_TWO_SIDE)
        case GL_LIGHT_MODEL_TWO_SIDE:
#endif // defined(GL_LIGHT_MODEL_TWO_SIDE)
            _needed = 1;
            break;
#if defined(GL_LIGHT_MODEL_AMBIENT)
        case GL_LIGHT_MODEL_AMBIENT:
#endif // defined(GL_LIGHT_MODEL_AMBIENT)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glLightModelxv(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlLightf(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glLightf(
        (GLenum)light,
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glLightfv(
        (GLenum)light,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glLightfv(
        (GLenum)light,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlLightx(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glLightf(
        (GLenum)light,
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glLightxv(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glLightxv(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlLineWidth(
    /* [in] */ Float width)
{
    glLineWidth(
        (GLfloat)width
    );
    return NOERROR;
}

ECode CGLImpl::GlLineWidthx(
    /* [in] */ Int32 width)
{
    glLineWidthx(
        (GLfixed)width
    );
    return NOERROR;
}

ECode CGLImpl::GlLoadIdentity()
{
    glLoadIdentity();
    return NOERROR;
}

ECode CGLImpl::GlLoadMatrixf(
    /* [in] */ ArrayOf<Float>* m_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *m_base = (GLfloat *) 0;
    GLfloat *m = (GLfloat *) 0;

    if (!m_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    m_base = (GLfloat *)m_ref->GetPayload();
    m = m_base + offset;

    glLoadMatrixf(
        (GLfloat *)m
    );
    return NOERROR;
}

ECode CGLImpl::GlLoadMatrixf(
    /* [in] */ IFloatBuffer* m_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *m = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(m_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    m = (GLfloat *)tmp;
    if (m == NULL) {
        char * _mBase = reinterpret_cast<char *>(_array);
        m = (GLfloat *) (_mBase + _bufferOffset);
    }
    glLoadMatrixf(
        (GLfloat *)m
    );
    return NOERROR;
}

ECode CGLImpl::GlLoadMatrixx(
    /* [in] */ ArrayOf<Int32>* m_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *m_base = (GLfixed *) 0;
    GLfixed *m = (GLfixed *) 0;

    if (!m_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    m_base = (GLfixed *)m_ref->GetPayload();
    m = m_base + offset;

    glLoadMatrixx(
        (GLfixed *)m
    );
    return NOERROR;
}

ECode CGLImpl::GlLoadMatrixx(
    /* [in] */ IInt32Buffer* m_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *m = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(m_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    m = (GLfixed *)tmp;
    if (m == NULL) {
        char * _mBase = reinterpret_cast<char *>(_array);
        m = (GLfixed *) (_mBase + _bufferOffset);
    }
    glLoadMatrixx(
        (GLfixed *)m
    );
    return NOERROR;
}

ECode CGLImpl::GlLogicOp(
    /* [in] */ Int32 opcode)
{
    glLogicOp(
        (GLenum)opcode
    );
    return NOERROR;
}

ECode CGLImpl::GlMaterialf(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glMaterialf(
        (GLenum)face,
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glMaterialfv(
        (GLenum)face,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glMaterialfv(
        (GLenum)face,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlMaterialx(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glMaterialx(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLImpl::GlMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glMaterialxv(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glMaterialxv(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlMatrixMode(
    /* [in] */ Int32 mode)
{
    glMatrixMode(
        (GLenum)mode
    );
    return NOERROR;
}

ECode CGLImpl::GlMultMatrixf(
    /* [in] */ ArrayOf<Float>* m_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *m_base = (GLfloat *) 0;
    GLfloat *m = (GLfloat *) 0;

    if (!m_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    m_base = (GLfloat *)m_ref->GetPayload();
    m = m_base + offset;

    glMultMatrixf(
        (GLfloat *)m
    );
    return NOERROR;
}

ECode CGLImpl::GlMultMatrixf(
    /* [in] */ IFloatBuffer* m_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *m = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(m_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    m = (GLfloat *)tmp;
    if (m == NULL) {
        char * _mBase = reinterpret_cast<char *>(_array);
        m = (GLfloat *) (_mBase + _bufferOffset);
    }
    glMultMatrixf(
        (GLfloat *)m
    );
    return NOERROR;
}

ECode CGLImpl::GlMultMatrixx(
    /* [in] */ ArrayOf<Int32>* m_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *m_base = (GLfixed *) 0;
    GLfixed *m = (GLfixed *) 0;

    if (!m_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    m_base = (GLfixed *)m_ref->GetPayload();
    m = m_base + offset;

    glMultMatrixx(
        (GLfixed *)m
    );

    return NOERROR;
}

ECode CGLImpl::GlMultMatrixx(
    /* [in] */ IInt32Buffer* m_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *m = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(m_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    m = (GLfixed *)tmp;
    if (m == NULL) {
        char * _mBase = reinterpret_cast<char *>(_array);
        m = (GLfixed *) (_mBase + _bufferOffset);
    }
    glMultMatrixx(
        (GLfixed *)m
    );
    return NOERROR;
}

ECode CGLImpl::GlMultiTexCoord4f(
    /* [in] */ Int32 target,
    /* [in] */ Float s,
    /* [in] */ Float t,
    /* [in] */ Float r,
    /* [in] */ Float q)
{
    glMultiTexCoord4f(
        (GLenum)target,
        (GLfloat)s,
        (GLfloat)t,
        (GLfloat)r,
        (GLfloat)q
    );
    return NOERROR;
}

ECode CGLImpl::GlMultiTexCoord4x(
    /* [in] */ Int32 target,
    /* [in] */ Int32 s,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 q)
{
    glMultiTexCoord4x(
        (GLenum)target,
        (GLfixed)s,
        (GLfixed)t,
        (GLfixed)r,
        (GLfixed)q
    );
    return NOERROR;
}

ECode CGLImpl::GlNormal3f(
    /* [in] */ Float nx,
    /* [in] */ Float ny,
    /* [in] */ Float nz)
{
    glNormal3f(
        (GLfloat)nx,
        (GLfloat)ny,
        (GLfloat)nz
    );
    return NOERROR;
}

ECode CGLImpl::GlNormal3x(
    /* [in] */ Int32 nx,
    /* [in] */ Int32 ny,
    /* [in] */ Int32 nz)
{
    glNormal3x(
        (GLfixed)nx,
        (GLfixed)ny,
        (GLfixed)nz
    );
    return NOERROR;
}

ECode CGLImpl::GlNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Int32 remaining;
    pointer->GetRemaining(&remaining);
    GlNormalPointerBounds(
        type,
        stride,
        pointer,
        remaining
    );
    if (((type == GL_FLOAT) ||
         (type == GL_BYTE) ||
         (type == GL_SHORT) ||
         (type == GL_FIXED)) &&
        (stride >= 0)) {
        m_NormalPointer = pointer;
    }
    return NOERROR;
}

ECode CGLImpl::GlNormalPointerBounds(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        Handle64 tmp;
        GetDirectBufferPointer(pointer_buf, &tmp);
        pointer = (GLvoid *)tmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glNormalPointerBounds(
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

ECode CGLImpl::GlOrthof(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    glOrthof(
        (GLfloat)left,
        (GLfloat)right,
        (GLfloat)bottom,
        (GLfloat)top,
        (GLfloat)zNear,
        (GLfloat)zFar
    );
    return NOERROR;
}

ECode CGLImpl::GlOrthox(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    glOrthox(
        (GLfixed)left,
        (GLfixed)right,
        (GLfixed)bottom,
        (GLfixed)top,
        (GLfixed)zNear,
        (GLfixed)zFar
    );
    return NOERROR;
}

ECode CGLImpl::GlPixelStorei(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glPixelStorei(
        (GLenum)pname,
        (GLint)param
    );
    return NOERROR;
}

ECode CGLImpl::GlPointSize(
    /* [in] */ Float size)
{
    glPointSize(
        (GLfloat)size
    );
    return NOERROR;
}

ECode CGLImpl::GlPointSizex(
    /* [in] */ Int32 size)
{
    glPointSizex(
        (GLfixed)size
    );
    return NOERROR;
}

ECode CGLImpl::GlPolygonOffset(
    /* [in] */ Float factor,
    /* [in] */ Float units)
{
    glPolygonOffset(
        (GLfloat)factor,
        (GLfloat)units
    );
    return NOERROR;
}

ECode CGLImpl::GlPolygonOffsetx(
    /* [in] */ Int32 factor,
    /* [in] */ Int32 units)
{
    glPolygonOffsetx(
        (GLfixed)factor,
        (GLfixed)units
    );
    return NOERROR;
}

ECode CGLImpl::GlPopMatrix()
{
    glPopMatrix();
    return NOERROR;
}

ECode CGLImpl::GlPushMatrix()
{
    glPushMatrix();
    return NOERROR;
}

ECode CGLImpl::GlReadPixels(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *pixels = (GLvoid *) 0;

    Handle64 tmp;
    GetPointer(pixels_buf, &_array, &_remaining, &_bufferOffset, &tmp);
    pixels = (GLvoid *)tmp;
    if (pixels == NULL) {
        char * _pixelsBase = reinterpret_cast<char *>(_array);
        pixels = (GLvoid *) (_pixelsBase + _bufferOffset);
    }
    glReadPixels(
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height,
        (GLenum)format,
        (GLenum)type,
        (GLvoid *)pixels
    );
    return NOERROR;
}

ECode CGLImpl::GlRotatef(
    /* [in] */ Float angle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    glRotatef(
        (GLfloat)angle,
        (GLfloat)x,
        (GLfloat)y,
        (GLfloat)z
    );
    return NOERROR;
}

ECode CGLImpl::GlRotatex(
    /* [in] */ Int32 angle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    glRotatex(
        (GLfixed)angle,
        (GLfixed)x,
        (GLfixed)y,
        (GLfixed)z
    );
    return NOERROR;
}

ECode CGLImpl::GlSampleCoverage(
    /* [in] */ Float value,
    /* [in] */ Boolean invert)
{
    glSampleCoverage(
        (GLclampf)value,
        (GLboolean)invert
    );
    return NOERROR;
}

ECode CGLImpl::GlSampleCoveragex(
    /* [in] */ Int32 value,
    /* [in] */ Boolean invert)
{
    glSampleCoveragex(
        (GLclampx)value,
        (GLboolean)invert
    );
    return NOERROR;
}

ECode CGLImpl::GlScalef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    glScalef(
        (GLfloat)x,
        (GLfloat)y,
        (GLfloat)z
    );
    return NOERROR;
}

ECode CGLImpl::GlScalex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    glScalex(
        (GLfixed)x,
        (GLfixed)y,
        (GLfixed)z
    );
    return NOERROR;
}

ECode CGLImpl::GlScissor(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glScissor(
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

ECode CGLImpl::GlShadeModel(
    /* [in] */ Int32 mode)
{
    glShadeModel(
        (GLenum)mode
    );
    return NOERROR;
}

ECode CGLImpl::GlStencilFunc(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref,
    /* [in] */ Int32 mask)
{
    glStencilFunc(
        (GLenum)func,
        (GLint)ref,
        (GLuint)mask
    );
    return NOERROR;
}

ECode CGLImpl::GlStencilMask(
    /* [in] */ Int32 mask)
{
    glStencilMask(
        (GLuint)mask
    );
    return NOERROR;
}

ECode CGLImpl::GlTexCoordPointerBounds(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        Handle64 tmp;
        GetDirectBufferPointer(pointer_buf, &tmp);
        pointer = (GLvoid *)tmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glTexCoordPointerBounds(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

ECode CGLImpl::GlTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Int32 remaining;
    pointer->GetRemaining(&remaining);
    glTexCoordPointerBounds(
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
        m_TexCoordPointer = pointer;
    }
    return NOERROR;
}

ECode CGLImpl::GlTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *pixels = (GLvoid *) 0;

    if (pixels_buf) {
        Handle64 tmp;
        GetPointer(pixels_buf, &_array, &_remaining, &_bufferOffset, &tmp);
        pixels = (GLvoid *)tmp;
    }
    if (pixels_buf && pixels == NULL) {
        char * _pixelsBase = reinterpret_cast<char *>(_array);
        pixels = (GLvoid *) (_pixelsBase + _bufferOffset);
    }
    glTexImage2D(
        (GLenum)target,
        (GLint)level,
        (GLint)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLint)border,
        (GLenum)format,
        (GLenum)type,
        (GLvoid *)pixels
    );
    return NOERROR;
}

ECode CGLImpl::GlTexParameterf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glTexParameterf(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexParameterx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexParameterx(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *pixels = (GLvoid *) 0;

    if (pixels_buf) {
        Handle64 tmp;
        GetPointer(pixels_buf, &_array, &_remaining, &_bufferOffset, &tmp);
        pixels = (GLvoid *)tmp;
    }
    if (pixels_buf && pixels == NULL) {
        char * _pixelsBase = reinterpret_cast<char *>(_array);
        pixels = (GLvoid *) (_pixelsBase + _bufferOffset);
    }
    glTexSubImage2D(
        (GLenum)target,
        (GLint)level,
        (GLint)xoffset,
        (GLint)yoffset,
        (GLsizei)width,
        (GLsizei)height,
        (GLenum)format,
        (GLenum)type,
        (GLvoid *)pixels
    );
    return NOERROR;
}

ECode CGLImpl::GlTranslatef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    glTranslatef(
        (GLfloat)x,
        (GLfloat)y,
        (GLfloat)z
    );
    return NOERROR;
}

ECode CGLImpl::GlTranslatex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    glTranslatex(
        (GLfixed)x,
        (GLfixed)y,
        (GLfixed)z
    );
    return NOERROR;
}

ECode CGLImpl::GlVertexPointerBounds(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        Handle64 tmp;
        FAIL_RETURN(GetDirectBufferPointer(pointer_buf, &tmp));
        pointer = (GLvoid *)tmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glVertexPointerBounds(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

ECode CGLImpl::GlVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Int32 remaining;
    pointer->GetRemaining(&remaining);
    ECode ec = GlVertexPointerBounds(
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
        m_VertexPointer = pointer;
    }
    return ec;
}

ECode CGLImpl::GlViewport(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    glViewport(
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height
    );
    return NOERROR;
}

ECode CGLImpl::GlQueryMatrixxOES(
    /* [in] */ ArrayOf<Int32>* mantissa_ref,
    /* [in] */ Int32 mantissaOffset,
    /* [in] */ ArrayOf<Int32>* exponent_ref,
    /* [in] */ Int32 exponentOffset,
    /* [out] */ Int32* matrixxOES)
{
    VALIDATE_NOT_NULL(matrixxOES)

    GLbitfield _returnValue = -1;
    GLfixed *mantissa_base = (GLfixed *) 0;
    Int32 _mantissaRemaining;
    GLfixed *mantissa = (GLfixed *) 0;
    GLint *exponent_base = (GLint *) 0;
    Int32 _exponentRemaining;
    GLint *exponent = (GLint *) 0;

    if (!mantissa_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mantissaOffset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _mantissaRemaining = mantissa_ref->GetLength() - mantissaOffset;
    if (_mantissaRemaining < 16) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mantissa_base = (GLfixed *)mantissa_ref->GetPayload();
    mantissa = mantissa_base + mantissaOffset;

    if (!exponent_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (exponentOffset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _exponentRemaining = exponent_ref->GetLength() - exponentOffset;
    if (_exponentRemaining < 16) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    exponent_base = (GLint *)exponent_ref->GetPayload();
    exponent = exponent_base + exponentOffset;

    _returnValue = glQueryMatrixxOES(
        (GLfixed *)mantissa,
        (GLint *)exponent
    );

    *matrixxOES = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlQueryMatrixxOES(
    /* [in] */ IInt32Buffer* mantissa_buf,
    /* [in] */ IInt32Buffer* exponent_buf,
    /* [out] */ Int32* matrixxOES)
{
    VALIDATE_NOT_NULL(matrixxOES)

    Handle64 _mantissaArray = (Handle64) 0;
    Int32 _mantissaBufferOffset = (Int32) 0;
    Handle64 _exponentArray = (Handle64) 0;
    Int32 _exponentBufferOffset = (Int32) 0;
    GLbitfield _returnValue = -1;
    Int32 _mantissaRemaining;
    GLfixed *mantissa = (GLfixed *) 0;
    Int32 _exponentRemaining;
    GLint *exponent = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(mantissa_buf), &_mantissaArray, &_mantissaRemaining, &_mantissaBufferOffset, &tmp);
    mantissa = (GLfixed *)tmp;
    if (_mantissaRemaining < 16) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    GetPointer(IBuffer::Probe(exponent_buf), &_exponentArray, &_exponentRemaining, &_exponentBufferOffset, &tmp);
    exponent = (GLint *)tmp;
    if (_exponentRemaining < 16) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mantissa == NULL) {
        char * _mantissaBase = reinterpret_cast<char *>(_mantissaArray);
        mantissa = (GLfixed *) (_mantissaBase + _mantissaBufferOffset);
    }
    if (exponent == NULL) {
        char * _exponentBase = reinterpret_cast<char *>(_exponentArray);
        exponent = (GLint *) (_exponentBase + _exponentBufferOffset);
    }
    _returnValue = glQueryMatrixxOES(
        (GLfixed *)mantissa,
        (GLint *)exponent
    );

    *matrixxOES = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glTexParameterfv(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glTexParameterfv(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlCurrentPaletteMatrixOES(
    /* [in] */ Int32 matrixpaletteindex)
{
    glCurrentPaletteMatrixOES(
        (GLuint)matrixpaletteindex
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawTexfOES(
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

ECode CGLImpl::GlDrawTexfvOES(
    /* [in] */ ArrayOf<Float>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *coords_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *coords = (GLfloat *) 0;

    if (!coords_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLfloat *)coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexfvOES(
        (GLfloat *)coords
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawTexfvOES(
    /* [in] */ IFloatBuffer* coords_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *coords = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    coords = (GLfloat *)tmp;
    if (_remaining < 5) {
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

ECode CGLImpl::GlDrawTexiOES(
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

ECode CGLImpl::GlDrawTexivOES(
    /* [in] */ ArrayOf<Int32>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLint *coords_base = (GLint *) 0;
    Int32 _remaining;
    GLint *coords = (GLint *) 0;

    if (!coords_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLint *)coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexivOES(
        (GLint *)coords
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawTexivOES(
    /* [in] */ IInt32Buffer* coords_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *coords = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    coords = (GLint *)tmp;
    if (_remaining < 5) {
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

ECode CGLImpl::GlDrawTexsOES(
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

ECode CGLImpl::GlDrawTexsvOES(
    /* [in] */ ArrayOf<Int16>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLshort *coords_base = (GLshort *) 0;
    Int32 _remaining;
    GLshort *coords = (GLshort *) 0;

    if (!coords_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLshort *)coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexsvOES(
        (GLshort *)coords
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawTexsvOES(
    /* [in] */ IInt16Buffer* coords_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLshort *coords = (GLshort *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    coords = (GLshort *)tmp;
    if (_remaining < 5) {
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

ECode CGLImpl::GlDrawTexxOES(
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

ECode CGLImpl::GlDrawTexxvOES(
    /* [in] */ ArrayOf<Int32>* coords_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *coords_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *coords = (GLfixed *) 0;

    if (!coords_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = coords_ref->GetLength() - offset;
    if (_remaining < 5) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    coords_base = (GLfixed *)coords_ref->GetPayload();
    coords = coords_base + offset;

    glDrawTexxvOES(
        (GLfixed *)coords
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawTexxvOES(
    /* [in] */ IInt32Buffer* coords_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *coords = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(coords_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    coords = (GLfixed *)tmp;
    if (_remaining < 5) {
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

ECode CGLImpl::GlEnable(
    /* [in] */ Int32 cap)
{
    glEnable(
        (GLenum)cap
    );
    return NOERROR;
}

ECode CGLImpl::GlEnableClientState(
    /* [in] */ Int32 array)
{
    glEnableClientState(
        (GLenum)array
    );
    return NOERROR;
}

ECode CGLImpl::GlLoadPaletteFromModelViewMatrixOES()
{
    glLoadPaletteFromModelViewMatrixOES();
    return NOERROR;
}

ECode CGLImpl::GlMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
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
        m_MatrixIndexPointerOES = pointer;
    }
    return ec;
}

ECode CGLImpl::GlMatrixIndexPointerOESBounds(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        Handle64 tmp;
        FAIL_RETURN(GetDirectBufferPointer(pointer_buf, &tmp));
        pointer = (GLvoid *) tmp;
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

ECode CGLImpl::GlMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    glMatrixIndexPointerOES(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

ECode CGLImpl::GlWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
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

ECode CGLImpl::GlWeightPointerOESBounds(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        Handle64 tmp;
        FAIL_RETURN(GetDirectBufferPointer(pointer_buf, &tmp));
        pointer = (GLvoid *) tmp;
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

ECode CGLImpl::GlWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    glWeightPointerOES(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

ECode CGLImpl::GlBindFramebufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 framebuffer)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glBindFramebufferOES(
        (GLint)target,
        (GLint)framebuffer
    );
    return NOERROR;
}

ECode CGLImpl::GlBindRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 renderbuffer)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glBindRenderbufferOES(
        (GLint)target,
        (GLint)renderbuffer
    );
    return NOERROR;
}

ECode CGLImpl::GlBlendEquation(
    /* [in] */ Int32 mode)
{
    if (! SupportsExtension(this, HAVE_OES_BLEND_SUBTRACTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glBlendEquation(
        (GLint)mode
    );
    return NOERROR;
}

ECode CGLImpl::GlBlendEquationSeparate(
    /* [in] */ Int32 modeRGB,
    /* [in] */ Int32 modeAlpha)
{
    if (! SupportsExtension(this, HAVE_OES_BLEND_EQUATION_SEPARATEID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glBlendEquationSeparate(
        (GLint)modeRGB,
        (GLint)modeAlpha
    );
    return NOERROR;
}

ECode CGLImpl::GlBlendFuncSeparate(
    /* [in] */ Int32 srcRGB,
    /* [in] */ Int32 dstRGB,
    /* [in] */ Int32 srcAlpha,
    /* [in] */ Int32 dstAlpha)
{
    if (! SupportsExtension(this, HAVE_OES_BLEND_EQUATION_SEPARATEID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glBlendFuncSeparate(
        (GLint)srcRGB,
        (GLint)dstRGB,
        (GLint)srcAlpha,
        (GLint)dstAlpha
    );
    return NOERROR;
}

ECode CGLImpl::GlCheckFramebufferStatusOES(
    /* [in] */ Int32 target,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)

    if (! SupportsExtension(this, HAVE_OES_BLEND_EQUATION_SEPARATEID)) {
        *status = 0;
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLint _returnValue = 0;
    _returnValue = glCheckFramebufferStatusOES(
        (GLint)target
    );
    *status = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlCompressedTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *data = (GLvoid *) 0;
    Handle64 tmp;
    GetPointer(data_buf, &_array, &_remaining, &_bufferOffset, &tmp);
    data = (GLvoid *) tmp;
    if (data == NULL) {
        char * _dataBase = reinterpret_cast<char *>(_array);
        data = (GLvoid *) (_dataBase + _bufferOffset);
    }
    glCompressedTexImage2D(
        (GLenum)target,
        (GLint)level,
        (GLenum)internalformat,
        (GLsizei)width,
        (GLsizei)height,
        (GLint)border,
        (GLsizei)imageSize,
        (GLvoid *)data
    );

    return NOERROR;
}

ECode CGLImpl::GlCopyTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border)
{
    glCopyTexImage2D(
        (GLenum)target,
        (GLint)level,
        (GLenum)internalformat,
        (GLint)x,
        (GLint)y,
        (GLsizei)width,
        (GLsizei)height,
        (GLint)border
    );
    return NOERROR;
}

ECode CGLImpl::GlDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLuint *framebuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;

    if (!framebuffers_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = framebuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    framebuffers_base = (GLuint *)framebuffers_ref->GetPayload();
    framebuffers = framebuffers_base + offset;

    glDeleteFramebuffersOES(
        (GLint)n,
        (GLuint *)framebuffers
    );
    return NOERROR;
}

ECode CGLImpl::GlDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers_buf)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(framebuffers_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    framebuffers = (GLuint *)tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (framebuffers == NULL) {
        char * _framebuffersBase = reinterpret_cast<char *>(_array);
        framebuffers = (GLuint *) (_framebuffersBase + _bufferOffset);
    }
    glDeleteFramebuffersOES(
        (GLint)n,
        (GLuint *)framebuffers
    );

    return NOERROR;
}

ECode CGLImpl::GlDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    GLuint *renderbuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;

    if (!renderbuffers_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = renderbuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    renderbuffers_base = (GLuint *)renderbuffers_ref->GetPayload();
    renderbuffers = renderbuffers_base + offset;

    glDeleteRenderbuffersOES(
        (GLint)n,
        (GLuint *)renderbuffers
    );
    return NOERROR;
}

ECode CGLImpl::GlDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers_buf)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(renderbuffers_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    renderbuffers = (GLuint *)tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (renderbuffers == NULL) {
        char * _renderbuffersBase = reinterpret_cast<char *>(_array);
        renderbuffers = (GLuint *) (_renderbuffersBase + _bufferOffset);
    }
    glDeleteRenderbuffersOES(
        (GLint)n,
        (GLuint *)renderbuffers
    );
    return NOERROR;
}

ECode CGLImpl::GlFramebufferRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 renderbuffertarget,
    /* [in] */ Int32 renderbuffer)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glFramebufferRenderbufferOES(
        (GLint)target,
        (GLint)attachment,
        (GLint)renderbuffertarget,
        (GLint)renderbuffer
    );
    return NOERROR;
}

ECode CGLImpl::GlFramebufferTexture2DOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 textarget,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glFramebufferTexture2DOES(
        (GLint)target,
        (GLint)attachment,
        (GLint)textarget,
        (GLint)texture,
        (GLint)level
    );
    return NOERROR;
}

ECode CGLImpl::GlGenerateMipmapOES(
    /* [in] */ Int32 target)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glGenerateMipmapOES(
        (GLint)target
    );
    return NOERROR;
}

ECode CGLImpl::GlGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    GLuint *framebuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;

    if (!framebuffers_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = framebuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    framebuffers_base = (GLuint *)framebuffers_ref->GetPayload();
    framebuffers = framebuffers_base + offset;

    glGenFramebuffersOES(
        (GLint)n,
        (GLuint *)framebuffers
    );
    return NOERROR;
}

ECode CGLImpl::GlGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers_buf)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *framebuffers = (GLuint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(framebuffers_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    framebuffers = (GLuint *) tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (framebuffers == NULL) {
        char * _framebuffersBase = reinterpret_cast<char *>(_array);
        framebuffers = (GLuint *) (_framebuffersBase + _bufferOffset);
    }
    glGenFramebuffersOES(
        (GLint)n,
        (GLuint *)framebuffers
    );
    return NOERROR;
}

ECode CGLImpl::GlGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    GLuint *renderbuffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;

    if (!renderbuffers_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = renderbuffers_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    renderbuffers_base = (GLuint *)renderbuffers_ref->GetPayload();
    renderbuffers = renderbuffers_base + offset;

    glGenRenderbuffersOES(
        (GLint)n,
        (GLuint *)renderbuffers
    );

    return NOERROR;
}

ECode CGLImpl::GlGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers_buf)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *renderbuffers = (GLuint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(renderbuffers_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    renderbuffers = (GLuint *)tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (renderbuffers == NULL) {
        char * _renderbuffersBase = reinterpret_cast<char *>(_array);
        renderbuffers = (GLuint *) (_renderbuffersBase + _bufferOffset);
    }
    glGenRenderbuffersOES(
        (GLint)n,
        (GLuint *)renderbuffers
    );

    return NOERROR;
}

ECode CGLImpl::GlGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glGetFramebufferAttachmentParameterivOES(
        (GLint)target,
        (GLint)attachment,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetFramebufferAttachmentParameterivOES(
        (GLint)target,
        (GLint)attachment,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_ALPHA_BITS)
        case GL_ALPHA_BITS:
#endif // defined(GL_ALPHA_BITS)
#if defined(GL_ALPHA_TEST_FUNC)
        case GL_ALPHA_TEST_FUNC:
#endif // defined(GL_ALPHA_TEST_FUNC)
#if defined(GL_ALPHA_TEST_REF)
        case GL_ALPHA_TEST_REF:
#endif // defined(GL_ALPHA_TEST_REF)
#if defined(GL_BLEND_DST)
        case GL_BLEND_DST:
#endif // defined(GL_BLEND_DST)
#if defined(GL_BLUE_BITS)
        case GL_BLUE_BITS:
#endif // defined(GL_BLUE_BITS)
#if defined(GL_COLOR_ARRAY_BUFFER_BINDING)
        case GL_COLOR_ARRAY_BUFFER_BINDING:
#endif // defined(GL_COLOR_ARRAY_BUFFER_BINDING)
#if defined(GL_COLOR_ARRAY_SIZE)
        case GL_COLOR_ARRAY_SIZE:
#endif // defined(GL_COLOR_ARRAY_SIZE)
#if defined(GL_COLOR_ARRAY_STRIDE)
        case GL_COLOR_ARRAY_STRIDE:
#endif // defined(GL_COLOR_ARRAY_STRIDE)
#if defined(GL_COLOR_ARRAY_TYPE)
        case GL_COLOR_ARRAY_TYPE:
#endif // defined(GL_COLOR_ARRAY_TYPE)
#if defined(GL_CULL_FACE)
        case GL_CULL_FACE:
#endif // defined(GL_CULL_FACE)
#if defined(GL_DEPTH_BITS)
        case GL_DEPTH_BITS:
#endif // defined(GL_DEPTH_BITS)
#if defined(GL_DEPTH_CLEAR_VALUE)
        case GL_DEPTH_CLEAR_VALUE:
#endif // defined(GL_DEPTH_CLEAR_VALUE)
#if defined(GL_DEPTH_FUNC)
        case GL_DEPTH_FUNC:
#endif // defined(GL_DEPTH_FUNC)
#if defined(GL_DEPTH_WRITEMASK)
        case GL_DEPTH_WRITEMASK:
#endif // defined(GL_DEPTH_WRITEMASK)
#if defined(GL_FOG_DENSITY)
        case GL_FOG_DENSITY:
#endif // defined(GL_FOG_DENSITY)
#if defined(GL_FOG_END)
        case GL_FOG_END:
#endif // defined(GL_FOG_END)
#if defined(GL_FOG_MODE)
        case GL_FOG_MODE:
#endif // defined(GL_FOG_MODE)
#if defined(GL_FOG_START)
        case GL_FOG_START:
#endif // defined(GL_FOG_START)
#if defined(GL_FRONT_FACE)
        case GL_FRONT_FACE:
#endif // defined(GL_FRONT_FACE)
#if defined(GL_GREEN_BITS)
        case GL_GREEN_BITS:
#endif // defined(GL_GREEN_BITS)
#if defined(GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES)
        case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
#endif // defined(GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES)
#if defined(GL_IMPLEMENTATION_COLOR_READ_TYPE_OES)
        case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
#endif // defined(GL_IMPLEMENTATION_COLOR_READ_TYPE_OES)
#if defined(GL_LIGHT_MODEL_COLOR_CONTROL)
        case GL_LIGHT_MODEL_COLOR_CONTROL:
#endif // defined(GL_LIGHT_MODEL_COLOR_CONTROL)
#if defined(GL_LIGHT_MODEL_LOCAL_VIEWER)
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
#endif // defined(GL_LIGHT_MODEL_LOCAL_VIEWER)
#if defined(GL_LIGHT_MODEL_TWO_SIDE)
        case GL_LIGHT_MODEL_TWO_SIDE:
#endif // defined(GL_LIGHT_MODEL_TWO_SIDE)
#if defined(GL_LINE_SMOOTH_HINT)
        case GL_LINE_SMOOTH_HINT:
#endif // defined(GL_LINE_SMOOTH_HINT)
#if defined(GL_LINE_WIDTH)
        case GL_LINE_WIDTH:
#endif // defined(GL_LINE_WIDTH)
#if defined(GL_LOGIC_OP_MODE)
        case GL_LOGIC_OP_MODE:
#endif // defined(GL_LOGIC_OP_MODE)
#if defined(GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES)
        case GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES)
#if defined(GL_MATRIX_INDEX_ARRAY_SIZE_OES)
        case GL_MATRIX_INDEX_ARRAY_SIZE_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_SIZE_OES)
#if defined(GL_MATRIX_INDEX_ARRAY_STRIDE_OES)
        case GL_MATRIX_INDEX_ARRAY_STRIDE_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_STRIDE_OES)
#if defined(GL_MATRIX_INDEX_ARRAY_TYPE_OES)
        case GL_MATRIX_INDEX_ARRAY_TYPE_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_TYPE_OES)
#if defined(GL_MATRIX_MODE)
        case GL_MATRIX_MODE:
#endif // defined(GL_MATRIX_MODE)
#if defined(GL_MAX_CLIP_PLANES)
        case GL_MAX_CLIP_PLANES:
#endif // defined(GL_MAX_CLIP_PLANES)
#if defined(GL_MAX_ELEMENTS_INDICES)
        case GL_MAX_ELEMENTS_INDICES:
#endif // defined(GL_MAX_ELEMENTS_INDICES)
#if defined(GL_MAX_ELEMENTS_VERTICES)
        case GL_MAX_ELEMENTS_VERTICES:
#endif // defined(GL_MAX_ELEMENTS_VERTICES)
#if defined(GL_MAX_LIGHTS)
        case GL_MAX_LIGHTS:
#endif // defined(GL_MAX_LIGHTS)
#if defined(GL_MAX_MODELVIEW_STACK_DEPTH)
        case GL_MAX_MODELVIEW_STACK_DEPTH:
#endif // defined(GL_MAX_MODELVIEW_STACK_DEPTH)
#if defined(GL_MAX_PALETTE_MATRICES_OES)
        case GL_MAX_PALETTE_MATRICES_OES:
#endif // defined(GL_MAX_PALETTE_MATRICES_OES)
#if defined(GL_MAX_PROJECTION_STACK_DEPTH)
        case GL_MAX_PROJECTION_STACK_DEPTH:
#endif // defined(GL_MAX_PROJECTION_STACK_DEPTH)
#if defined(GL_MAX_TEXTURE_SIZE)
        case GL_MAX_TEXTURE_SIZE:
#endif // defined(GL_MAX_TEXTURE_SIZE)
#if defined(GL_MAX_TEXTURE_STACK_DEPTH)
        case GL_MAX_TEXTURE_STACK_DEPTH:
#endif // defined(GL_MAX_TEXTURE_STACK_DEPTH)
#if defined(GL_MAX_TEXTURE_UNITS)
        case GL_MAX_TEXTURE_UNITS:
#endif // defined(GL_MAX_TEXTURE_UNITS)
#if defined(GL_MAX_VERTEX_UNITS_OES)
        case GL_MAX_VERTEX_UNITS_OES:
#endif // defined(GL_MAX_VERTEX_UNITS_OES)
#if defined(GL_MODELVIEW_STACK_DEPTH)
        case GL_MODELVIEW_STACK_DEPTH:
#endif // defined(GL_MODELVIEW_STACK_DEPTH)
#if defined(GL_NORMAL_ARRAY_BUFFER_BINDING)
        case GL_NORMAL_ARRAY_BUFFER_BINDING:
#endif // defined(GL_NORMAL_ARRAY_BUFFER_BINDING)
#if defined(GL_NORMAL_ARRAY_STRIDE)
        case GL_NORMAL_ARRAY_STRIDE:
#endif // defined(GL_NORMAL_ARRAY_STRIDE)
#if defined(GL_NORMAL_ARRAY_TYPE)
        case GL_NORMAL_ARRAY_TYPE:
#endif // defined(GL_NORMAL_ARRAY_TYPE)
#if defined(GL_NUM_COMPRESSED_TEXTURE_FORMATS)
        case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
#endif // defined(GL_NUM_COMPRESSED_TEXTURE_FORMATS)
#if defined(GL_PACK_ALIGNMENT)
        case GL_PACK_ALIGNMENT:
#endif // defined(GL_PACK_ALIGNMENT)
#if defined(GL_PERSPECTIVE_CORRECTION_HINT)
        case GL_PERSPECTIVE_CORRECTION_HINT:
#endif // defined(GL_PERSPECTIVE_CORRECTION_HINT)
#if defined(GL_POINT_SIZE)
        case GL_POINT_SIZE:
#endif // defined(GL_POINT_SIZE)
#if defined(GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES)
        case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
#endif // defined(GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES)
#if defined(GL_POINT_SIZE_ARRAY_STRIDE_OES)
        case GL_POINT_SIZE_ARRAY_STRIDE_OES:
#endif // defined(GL_POINT_SIZE_ARRAY_STRIDE_OES)
#if defined(GL_POINT_SIZE_ARRAY_TYPE_OES)
        case GL_POINT_SIZE_ARRAY_TYPE_OES:
#endif // defined(GL_POINT_SIZE_ARRAY_TYPE_OES)
#if defined(GL_POINT_SMOOTH_HINT)
        case GL_POINT_SMOOTH_HINT:
#endif // defined(GL_POINT_SMOOTH_HINT)
#if defined(GL_POLYGON_OFFSET_FACTOR)
        case GL_POLYGON_OFFSET_FACTOR:
#endif // defined(GL_POLYGON_OFFSET_FACTOR)
#if defined(GL_POLYGON_OFFSET_UNITS)
        case GL_POLYGON_OFFSET_UNITS:
#endif // defined(GL_POLYGON_OFFSET_UNITS)
#if defined(GL_PROJECTION_STACK_DEPTH)
        case GL_PROJECTION_STACK_DEPTH:
#endif // defined(GL_PROJECTION_STACK_DEPTH)
#if defined(GL_RED_BITS)
        case GL_RED_BITS:
#endif // defined(GL_RED_BITS)
#if defined(GL_SHADE_MODEL)
        case GL_SHADE_MODEL:
#endif // defined(GL_SHADE_MODEL)
#if defined(GL_STENCIL_BITS)
        case GL_STENCIL_BITS:
#endif // defined(GL_STENCIL_BITS)
#if defined(GL_STENCIL_CLEAR_VALUE)
        case GL_STENCIL_CLEAR_VALUE:
#endif // defined(GL_STENCIL_CLEAR_VALUE)
#if defined(GL_STENCIL_FAIL)
        case GL_STENCIL_FAIL:
#endif // defined(GL_STENCIL_FAIL)
#if defined(GL_STENCIL_FUNC)
        case GL_STENCIL_FUNC:
#endif // defined(GL_STENCIL_FUNC)
#if defined(GL_STENCIL_PASS_DEPTH_FAIL)
        case GL_STENCIL_PASS_DEPTH_FAIL:
#endif // defined(GL_STENCIL_PASS_DEPTH_FAIL)
#if defined(GL_STENCIL_PASS_DEPTH_PASS)
        case GL_STENCIL_PASS_DEPTH_PASS:
#endif // defined(GL_STENCIL_PASS_DEPTH_PASS)
#if defined(GL_STENCIL_REF)
        case GL_STENCIL_REF:
#endif // defined(GL_STENCIL_REF)
#if defined(GL_STENCIL_VALUE_MASK)
        case GL_STENCIL_VALUE_MASK:
#endif // defined(GL_STENCIL_VALUE_MASK)
#if defined(GL_STENCIL_WRITEMASK)
        case GL_STENCIL_WRITEMASK:
#endif // defined(GL_STENCIL_WRITEMASK)
#if defined(GL_SUBPIXEL_BITS)
        case GL_SUBPIXEL_BITS:
#endif // defined(GL_SUBPIXEL_BITS)
#if defined(GL_TEXTURE_BINDING_2D)
        case GL_TEXTURE_BINDING_2D:
#endif // defined(GL_TEXTURE_BINDING_2D)
#if defined(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING)
        case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
#endif // defined(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING)
#if defined(GL_TEXTURE_COORD_ARRAY_SIZE)
        case GL_TEXTURE_COORD_ARRAY_SIZE:
#endif // defined(GL_TEXTURE_COORD_ARRAY_SIZE)
#if defined(GL_TEXTURE_COORD_ARRAY_STRIDE)
        case GL_TEXTURE_COORD_ARRAY_STRIDE:
#endif // defined(GL_TEXTURE_COORD_ARRAY_STRIDE)
#if defined(GL_TEXTURE_COORD_ARRAY_TYPE)
        case GL_TEXTURE_COORD_ARRAY_TYPE:
#endif // defined(GL_TEXTURE_COORD_ARRAY_TYPE)
#if defined(GL_TEXTURE_STACK_DEPTH)
        case GL_TEXTURE_STACK_DEPTH:
#endif // defined(GL_TEXTURE_STACK_DEPTH)
#if defined(GL_UNPACK_ALIGNMENT)
        case GL_UNPACK_ALIGNMENT:
#endif // defined(GL_UNPACK_ALIGNMENT)
#if defined(GL_VERTEX_ARRAY_BUFFER_BINDING)
        case GL_VERTEX_ARRAY_BUFFER_BINDING:
#endif // defined(GL_VERTEX_ARRAY_BUFFER_BINDING)
#if defined(GL_VERTEX_ARRAY_SIZE)
        case GL_VERTEX_ARRAY_SIZE:
#endif // defined(GL_VERTEX_ARRAY_SIZE)
#if defined(GL_VERTEX_ARRAY_STRIDE)
        case GL_VERTEX_ARRAY_STRIDE:
#endif // defined(GL_VERTEX_ARRAY_STRIDE)
#if defined(GL_VERTEX_ARRAY_TYPE)
        case GL_VERTEX_ARRAY_TYPE:
#endif // defined(GL_VERTEX_ARRAY_TYPE)
#if defined(GL_WEIGHT_ARRAY_BUFFER_BINDING_OES)
        case GL_WEIGHT_ARRAY_BUFFER_BINDING_OES:
#endif // defined(GL_WEIGHT_ARRAY_BUFFER_BINDING_OES)
#if defined(GL_WEIGHT_ARRAY_SIZE_OES)
        case GL_WEIGHT_ARRAY_SIZE_OES:
#endif // defined(GL_WEIGHT_ARRAY_SIZE_OES)
#if defined(GL_WEIGHT_ARRAY_STRIDE_OES)
        case GL_WEIGHT_ARRAY_STRIDE_OES:
#endif // defined(GL_WEIGHT_ARRAY_STRIDE_OES)
#if defined(GL_WEIGHT_ARRAY_TYPE_OES)
        case GL_WEIGHT_ARRAY_TYPE_OES:
#endif // defined(GL_WEIGHT_ARRAY_TYPE_OES)
            _needed = 1;
            break;
#if defined(GL_ALIASED_POINT_SIZE_RANGE)
        case GL_ALIASED_POINT_SIZE_RANGE:
#endif // defined(GL_ALIASED_POINT_SIZE_RANGE)
#if defined(GL_ALIASED_LINE_WIDTH_RANGE)
        case GL_ALIASED_LINE_WIDTH_RANGE:
#endif // defined(GL_ALIASED_LINE_WIDTH_RANGE)
#if defined(GL_DEPTH_RANGE)
        case GL_DEPTH_RANGE:
#endif // defined(GL_DEPTH_RANGE)
#if defined(GL_MAX_VIEWPORT_DIMS)
        case GL_MAX_VIEWPORT_DIMS:
#endif // defined(GL_MAX_VIEWPORT_DIMS)
#if defined(GL_SMOOTH_LINE_WIDTH_RANGE)
        case GL_SMOOTH_LINE_WIDTH_RANGE:
#endif // defined(GL_SMOOTH_LINE_WIDTH_RANGE)
#if defined(GL_SMOOTH_POINT_SIZE_RANGE)
        case GL_SMOOTH_POINT_SIZE_RANGE:
#endif // defined(GL_SMOOTH_POINT_SIZE_RANGE)
            _needed = 2;
            break;
#if defined(GL_COLOR_CLEAR_VALUE)
        case GL_COLOR_CLEAR_VALUE:
#endif // defined(GL_COLOR_CLEAR_VALUE)
#if defined(GL_COLOR_WRITEMASK)
        case GL_COLOR_WRITEMASK:
#endif // defined(GL_COLOR_WRITEMASK)
#if defined(GL_FOG_COLOR)
        case GL_FOG_COLOR:
#endif // defined(GL_FOG_COLOR)
#if defined(GL_LIGHT_MODEL_AMBIENT)
        case GL_LIGHT_MODEL_AMBIENT:
#endif // defined(GL_LIGHT_MODEL_AMBIENT)
#if defined(GL_SCISSOR_BOX)
        case GL_SCISSOR_BOX:
#endif // defined(GL_SCISSOR_BOX)
#if defined(GL_VIEWPORT)
        case GL_VIEWPORT:
#endif // defined(GL_VIEWPORT)
            _needed = 4;
            break;
#if defined(GL_MODELVIEW_MATRIX)
        case GL_MODELVIEW_MATRIX:
#endif // defined(GL_MODELVIEW_MATRIX)
#if defined(GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES)
        case GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES:
#endif // defined(GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES)
#if defined(GL_PROJECTION_MATRIX)
        case GL_PROJECTION_MATRIX:
#endif // defined(GL_PROJECTION_MATRIX)
#if defined(GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES)
        case GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES:
#endif // defined(GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES)
#if defined(GL_TEXTURE_MATRIX)
        case GL_TEXTURE_MATRIX:
#endif // defined(GL_TEXTURE_MATRIX)
#if defined(GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES)
        case GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES:
#endif // defined(GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES)
            _needed = 16;
            break;
#if defined(GL_COMPRESSED_TEXTURE_FORMATS)
        case GL_COMPRESSED_TEXTURE_FORMATS:
#endif // defined(GL_COMPRESSED_TEXTURE_FORMATS)
            _needed = getNumCompressedTextureFormats();
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glGetIntegerv(
        (GLenum)pname,
        (GLint *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_ALPHA_BITS)
        case GL_ALPHA_BITS:
#endif // defined(GL_ALPHA_BITS)
#if defined(GL_ALPHA_TEST_FUNC)
        case GL_ALPHA_TEST_FUNC:
#endif // defined(GL_ALPHA_TEST_FUNC)
#if defined(GL_ALPHA_TEST_REF)
        case GL_ALPHA_TEST_REF:
#endif // defined(GL_ALPHA_TEST_REF)
#if defined(GL_BLEND_DST)
        case GL_BLEND_DST:
#endif // defined(GL_BLEND_DST)
#if defined(GL_BLUE_BITS)
        case GL_BLUE_BITS:
#endif // defined(GL_BLUE_BITS)
#if defined(GL_COLOR_ARRAY_BUFFER_BINDING)
        case GL_COLOR_ARRAY_BUFFER_BINDING:
#endif // defined(GL_COLOR_ARRAY_BUFFER_BINDING)
#if defined(GL_COLOR_ARRAY_SIZE)
        case GL_COLOR_ARRAY_SIZE:
#endif // defined(GL_COLOR_ARRAY_SIZE)
#if defined(GL_COLOR_ARRAY_STRIDE)
        case GL_COLOR_ARRAY_STRIDE:
#endif // defined(GL_COLOR_ARRAY_STRIDE)
#if defined(GL_COLOR_ARRAY_TYPE)
        case GL_COLOR_ARRAY_TYPE:
#endif // defined(GL_COLOR_ARRAY_TYPE)
#if defined(GL_CULL_FACE)
        case GL_CULL_FACE:
#endif // defined(GL_CULL_FACE)
#if defined(GL_DEPTH_BITS)
        case GL_DEPTH_BITS:
#endif // defined(GL_DEPTH_BITS)
#if defined(GL_DEPTH_CLEAR_VALUE)
        case GL_DEPTH_CLEAR_VALUE:
#endif // defined(GL_DEPTH_CLEAR_VALUE)
#if defined(GL_DEPTH_FUNC)
        case GL_DEPTH_FUNC:
#endif // defined(GL_DEPTH_FUNC)
#if defined(GL_DEPTH_WRITEMASK)
        case GL_DEPTH_WRITEMASK:
#endif // defined(GL_DEPTH_WRITEMASK)
#if defined(GL_FOG_DENSITY)
        case GL_FOG_DENSITY:
#endif // defined(GL_FOG_DENSITY)
#if defined(GL_FOG_END)
        case GL_FOG_END:
#endif // defined(GL_FOG_END)
#if defined(GL_FOG_MODE)
        case GL_FOG_MODE:
#endif // defined(GL_FOG_MODE)
#if defined(GL_FOG_START)
        case GL_FOG_START:
#endif // defined(GL_FOG_START)
#if defined(GL_FRONT_FACE)
        case GL_FRONT_FACE:
#endif // defined(GL_FRONT_FACE)
#if defined(GL_GREEN_BITS)
        case GL_GREEN_BITS:
#endif // defined(GL_GREEN_BITS)
#if defined(GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES)
        case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
#endif // defined(GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES)
#if defined(GL_IMPLEMENTATION_COLOR_READ_TYPE_OES)
        case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
#endif // defined(GL_IMPLEMENTATION_COLOR_READ_TYPE_OES)
#if defined(GL_LIGHT_MODEL_COLOR_CONTROL)
        case GL_LIGHT_MODEL_COLOR_CONTROL:
#endif // defined(GL_LIGHT_MODEL_COLOR_CONTROL)
#if defined(GL_LIGHT_MODEL_LOCAL_VIEWER)
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
#endif // defined(GL_LIGHT_MODEL_LOCAL_VIEWER)
#if defined(GL_LIGHT_MODEL_TWO_SIDE)
        case GL_LIGHT_MODEL_TWO_SIDE:
#endif // defined(GL_LIGHT_MODEL_TWO_SIDE)
#if defined(GL_LINE_SMOOTH_HINT)
        case GL_LINE_SMOOTH_HINT:
#endif // defined(GL_LINE_SMOOTH_HINT)
#if defined(GL_LINE_WIDTH)
        case GL_LINE_WIDTH:
#endif // defined(GL_LINE_WIDTH)
#if defined(GL_LOGIC_OP_MODE)
        case GL_LOGIC_OP_MODE:
#endif // defined(GL_LOGIC_OP_MODE)
#if defined(GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES)
        case GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES)
#if defined(GL_MATRIX_INDEX_ARRAY_SIZE_OES)
        case GL_MATRIX_INDEX_ARRAY_SIZE_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_SIZE_OES)
#if defined(GL_MATRIX_INDEX_ARRAY_STRIDE_OES)
        case GL_MATRIX_INDEX_ARRAY_STRIDE_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_STRIDE_OES)
#if defined(GL_MATRIX_INDEX_ARRAY_TYPE_OES)
        case GL_MATRIX_INDEX_ARRAY_TYPE_OES:
#endif // defined(GL_MATRIX_INDEX_ARRAY_TYPE_OES)
#if defined(GL_MATRIX_MODE)
        case GL_MATRIX_MODE:
#endif // defined(GL_MATRIX_MODE)
#if defined(GL_MAX_CLIP_PLANES)
        case GL_MAX_CLIP_PLANES:
#endif // defined(GL_MAX_CLIP_PLANES)
#if defined(GL_MAX_ELEMENTS_INDICES)
        case GL_MAX_ELEMENTS_INDICES:
#endif // defined(GL_MAX_ELEMENTS_INDICES)
#if defined(GL_MAX_ELEMENTS_VERTICES)
        case GL_MAX_ELEMENTS_VERTICES:
#endif // defined(GL_MAX_ELEMENTS_VERTICES)
#if defined(GL_MAX_LIGHTS)
        case GL_MAX_LIGHTS:
#endif // defined(GL_MAX_LIGHTS)
#if defined(GL_MAX_MODELVIEW_STACK_DEPTH)
        case GL_MAX_MODELVIEW_STACK_DEPTH:
#endif // defined(GL_MAX_MODELVIEW_STACK_DEPTH)
#if defined(GL_MAX_PALETTE_MATRICES_OES)
        case GL_MAX_PALETTE_MATRICES_OES:
#endif // defined(GL_MAX_PALETTE_MATRICES_OES)
#if defined(GL_MAX_PROJECTION_STACK_DEPTH)
        case GL_MAX_PROJECTION_STACK_DEPTH:
#endif // defined(GL_MAX_PROJECTION_STACK_DEPTH)
#if defined(GL_MAX_TEXTURE_SIZE)
        case GL_MAX_TEXTURE_SIZE:
#endif // defined(GL_MAX_TEXTURE_SIZE)
#if defined(GL_MAX_TEXTURE_STACK_DEPTH)
        case GL_MAX_TEXTURE_STACK_DEPTH:
#endif // defined(GL_MAX_TEXTURE_STACK_DEPTH)
#if defined(GL_MAX_TEXTURE_UNITS)
        case GL_MAX_TEXTURE_UNITS:
#endif // defined(GL_MAX_TEXTURE_UNITS)
#if defined(GL_MAX_VERTEX_UNITS_OES)
        case GL_MAX_VERTEX_UNITS_OES:
#endif // defined(GL_MAX_VERTEX_UNITS_OES)
#if defined(GL_MODELVIEW_STACK_DEPTH)
        case GL_MODELVIEW_STACK_DEPTH:
#endif // defined(GL_MODELVIEW_STACK_DEPTH)
#if defined(GL_NORMAL_ARRAY_BUFFER_BINDING)
        case GL_NORMAL_ARRAY_BUFFER_BINDING:
#endif // defined(GL_NORMAL_ARRAY_BUFFER_BINDING)
#if defined(GL_NORMAL_ARRAY_STRIDE)
        case GL_NORMAL_ARRAY_STRIDE:
#endif // defined(GL_NORMAL_ARRAY_STRIDE)
#if defined(GL_NORMAL_ARRAY_TYPE)
        case GL_NORMAL_ARRAY_TYPE:
#endif // defined(GL_NORMAL_ARRAY_TYPE)
#if defined(GL_NUM_COMPRESSED_TEXTURE_FORMATS)
        case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
#endif // defined(GL_NUM_COMPRESSED_TEXTURE_FORMATS)
#if defined(GL_PACK_ALIGNMENT)
        case GL_PACK_ALIGNMENT:
#endif // defined(GL_PACK_ALIGNMENT)
#if defined(GL_PERSPECTIVE_CORRECTION_HINT)
        case GL_PERSPECTIVE_CORRECTION_HINT:
#endif // defined(GL_PERSPECTIVE_CORRECTION_HINT)
#if defined(GL_POINT_SIZE)
        case GL_POINT_SIZE:
#endif // defined(GL_POINT_SIZE)
#if defined(GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES)
        case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
#endif // defined(GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES)
#if defined(GL_POINT_SIZE_ARRAY_STRIDE_OES)
        case GL_POINT_SIZE_ARRAY_STRIDE_OES:
#endif // defined(GL_POINT_SIZE_ARRAY_STRIDE_OES)
#if defined(GL_POINT_SIZE_ARRAY_TYPE_OES)
        case GL_POINT_SIZE_ARRAY_TYPE_OES:
#endif // defined(GL_POINT_SIZE_ARRAY_TYPE_OES)
#if defined(GL_POINT_SMOOTH_HINT)
        case GL_POINT_SMOOTH_HINT:
#endif // defined(GL_POINT_SMOOTH_HINT)
#if defined(GL_POLYGON_OFFSET_FACTOR)
        case GL_POLYGON_OFFSET_FACTOR:
#endif // defined(GL_POLYGON_OFFSET_FACTOR)
#if defined(GL_POLYGON_OFFSET_UNITS)
        case GL_POLYGON_OFFSET_UNITS:
#endif // defined(GL_POLYGON_OFFSET_UNITS)
#if defined(GL_PROJECTION_STACK_DEPTH)
        case GL_PROJECTION_STACK_DEPTH:
#endif // defined(GL_PROJECTION_STACK_DEPTH)
#if defined(GL_RED_BITS)
        case GL_RED_BITS:
#endif // defined(GL_RED_BITS)
#if defined(GL_SHADE_MODEL)
        case GL_SHADE_MODEL:
#endif // defined(GL_SHADE_MODEL)
#if defined(GL_STENCIL_BITS)
        case GL_STENCIL_BITS:
#endif // defined(GL_STENCIL_BITS)
#if defined(GL_STENCIL_CLEAR_VALUE)
        case GL_STENCIL_CLEAR_VALUE:
#endif // defined(GL_STENCIL_CLEAR_VALUE)
#if defined(GL_STENCIL_FAIL)
        case GL_STENCIL_FAIL:
#endif // defined(GL_STENCIL_FAIL)
#if defined(GL_STENCIL_FUNC)
        case GL_STENCIL_FUNC:
#endif // defined(GL_STENCIL_FUNC)
#if defined(GL_STENCIL_PASS_DEPTH_FAIL)
        case GL_STENCIL_PASS_DEPTH_FAIL:
#endif // defined(GL_STENCIL_PASS_DEPTH_FAIL)
#if defined(GL_STENCIL_PASS_DEPTH_PASS)
        case GL_STENCIL_PASS_DEPTH_PASS:
#endif // defined(GL_STENCIL_PASS_DEPTH_PASS)
#if defined(GL_STENCIL_REF)
        case GL_STENCIL_REF:
#endif // defined(GL_STENCIL_REF)
#if defined(GL_STENCIL_VALUE_MASK)
        case GL_STENCIL_VALUE_MASK:
#endif // defined(GL_STENCIL_VALUE_MASK)
#if defined(GL_STENCIL_WRITEMASK)
        case GL_STENCIL_WRITEMASK:
#endif // defined(GL_STENCIL_WRITEMASK)
#if defined(GL_SUBPIXEL_BITS)
        case GL_SUBPIXEL_BITS:
#endif // defined(GL_SUBPIXEL_BITS)
#if defined(GL_TEXTURE_BINDING_2D)
        case GL_TEXTURE_BINDING_2D:
#endif // defined(GL_TEXTURE_BINDING_2D)
#if defined(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING)
        case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
#endif // defined(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING)
#if defined(GL_TEXTURE_COORD_ARRAY_SIZE)
        case GL_TEXTURE_COORD_ARRAY_SIZE:
#endif // defined(GL_TEXTURE_COORD_ARRAY_SIZE)
#if defined(GL_TEXTURE_COORD_ARRAY_STRIDE)
        case GL_TEXTURE_COORD_ARRAY_STRIDE:
#endif // defined(GL_TEXTURE_COORD_ARRAY_STRIDE)
#if defined(GL_TEXTURE_COORD_ARRAY_TYPE)
        case GL_TEXTURE_COORD_ARRAY_TYPE:
#endif // defined(GL_TEXTURE_COORD_ARRAY_TYPE)
#if defined(GL_TEXTURE_STACK_DEPTH)
        case GL_TEXTURE_STACK_DEPTH:
#endif // defined(GL_TEXTURE_STACK_DEPTH)
#if defined(GL_UNPACK_ALIGNMENT)
        case GL_UNPACK_ALIGNMENT:
#endif // defined(GL_UNPACK_ALIGNMENT)
#if defined(GL_VERTEX_ARRAY_BUFFER_BINDING)
        case GL_VERTEX_ARRAY_BUFFER_BINDING:
#endif // defined(GL_VERTEX_ARRAY_BUFFER_BINDING)
#if defined(GL_VERTEX_ARRAY_SIZE)
        case GL_VERTEX_ARRAY_SIZE:
#endif // defined(GL_VERTEX_ARRAY_SIZE)
#if defined(GL_VERTEX_ARRAY_STRIDE)
        case GL_VERTEX_ARRAY_STRIDE:
#endif // defined(GL_VERTEX_ARRAY_STRIDE)
#if defined(GL_VERTEX_ARRAY_TYPE)
        case GL_VERTEX_ARRAY_TYPE:
#endif // defined(GL_VERTEX_ARRAY_TYPE)
#if defined(GL_WEIGHT_ARRAY_BUFFER_BINDING_OES)
        case GL_WEIGHT_ARRAY_BUFFER_BINDING_OES:
#endif // defined(GL_WEIGHT_ARRAY_BUFFER_BINDING_OES)
#if defined(GL_WEIGHT_ARRAY_SIZE_OES)
        case GL_WEIGHT_ARRAY_SIZE_OES:
#endif // defined(GL_WEIGHT_ARRAY_SIZE_OES)
#if defined(GL_WEIGHT_ARRAY_STRIDE_OES)
        case GL_WEIGHT_ARRAY_STRIDE_OES:
#endif // defined(GL_WEIGHT_ARRAY_STRIDE_OES)
#if defined(GL_WEIGHT_ARRAY_TYPE_OES)
        case GL_WEIGHT_ARRAY_TYPE_OES:
#endif // defined(GL_WEIGHT_ARRAY_TYPE_OES)
            _needed = 1;
            break;
#if defined(GL_ALIASED_POINT_SIZE_RANGE)
        case GL_ALIASED_POINT_SIZE_RANGE:
#endif // defined(GL_ALIASED_POINT_SIZE_RANGE)
#if defined(GL_ALIASED_LINE_WIDTH_RANGE)
        case GL_ALIASED_LINE_WIDTH_RANGE:
#endif // defined(GL_ALIASED_LINE_WIDTH_RANGE)
#if defined(GL_DEPTH_RANGE)
        case GL_DEPTH_RANGE:
#endif // defined(GL_DEPTH_RANGE)
#if defined(GL_MAX_VIEWPORT_DIMS)
        case GL_MAX_VIEWPORT_DIMS:
#endif // defined(GL_MAX_VIEWPORT_DIMS)
#if defined(GL_SMOOTH_LINE_WIDTH_RANGE)
        case GL_SMOOTH_LINE_WIDTH_RANGE:
#endif // defined(GL_SMOOTH_LINE_WIDTH_RANGE)
#if defined(GL_SMOOTH_POINT_SIZE_RANGE)
        case GL_SMOOTH_POINT_SIZE_RANGE:
#endif // defined(GL_SMOOTH_POINT_SIZE_RANGE)
            _needed = 2;
            break;
#if defined(GL_COLOR_CLEAR_VALUE)
        case GL_COLOR_CLEAR_VALUE:
#endif // defined(GL_COLOR_CLEAR_VALUE)
#if defined(GL_COLOR_WRITEMASK)
        case GL_COLOR_WRITEMASK:
#endif // defined(GL_COLOR_WRITEMASK)
#if defined(GL_FOG_COLOR)
        case GL_FOG_COLOR:
#endif // defined(GL_FOG_COLOR)
#if defined(GL_LIGHT_MODEL_AMBIENT)
        case GL_LIGHT_MODEL_AMBIENT:
#endif // defined(GL_LIGHT_MODEL_AMBIENT)
#if defined(GL_SCISSOR_BOX)
        case GL_SCISSOR_BOX:
#endif // defined(GL_SCISSOR_BOX)
#if defined(GL_VIEWPORT)
        case GL_VIEWPORT:
#endif // defined(GL_VIEWPORT)
            _needed = 4;
            break;
#if defined(GL_MODELVIEW_MATRIX)
        case GL_MODELVIEW_MATRIX:
#endif // defined(GL_MODELVIEW_MATRIX)
#if defined(GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES)
        case GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES:
#endif // defined(GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES)
#if defined(GL_PROJECTION_MATRIX)
        case GL_PROJECTION_MATRIX:
#endif // defined(GL_PROJECTION_MATRIX)
#if defined(GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES)
        case GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES:
#endif // defined(GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES)
#if defined(GL_TEXTURE_MATRIX)
        case GL_TEXTURE_MATRIX:
#endif // defined(GL_TEXTURE_MATRIX)
#if defined(GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES)
        case GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES:
#endif // defined(GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES)
            _needed = 16;
            break;
#if defined(GL_COMPRESSED_TEXTURE_FORMATS)
        case GL_COMPRESSED_TEXTURE_FORMATS:
#endif // defined(GL_COMPRESSED_TEXTURE_FORMATS)
            _needed = getNumCompressedTextureFormats();
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetIntegerv(
        (GLenum)pname,
        (GLint *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glGetRenderbufferParameterivOES(
        (GLint)target,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetRenderbufferParameterivOES(
        (GLint)target,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLfloat *params_base = (GLfloat *) 0;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexGenfv(
        (GLint)coord,
        (GLint)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glGetTexGenfv(
        (GLint)coord,
        (GLint)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexGeniv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetTexGeniv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexGenxv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetTexGenxv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlIsFramebufferOES(
    /* [in] */ Int32 framebuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        *result = FALSE;
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLboolean _returnValue = FALSE;
    _returnValue = glIsFramebufferOES(
        (GLint)framebuffer
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlIsRenderbufferOES(
    /* [in] */ Int32 renderbuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        *result = FALSE;
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLboolean _returnValue = FALSE;
    _returnValue = glIsRenderbufferOES(
        (GLint)renderbuffer
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlRenderbufferStorageOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (! SupportsExtension(this, HAVE_OES_FRAMEBUFFER_OBJECTID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glRenderbufferStorageOES(
        (GLint)target,
        (GLint)internalformat,
        (GLint)width,
        (GLint)height
    );
    return NOERROR;
}

ECode CGLImpl::GlStencilOp(
    /* [in] */ Int32 fail,
    /* [in] */ Int32 zfail,
    /* [in] */ Int32 zpass)
{
    glStencilOp(
        (GLenum)fail,
        (GLenum)zfail,
        (GLenum)zpass
    );
    return NOERROR;
}

ECode CGLImpl::GlTexEnvf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glTexEnvf(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glTexEnvfv(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glTexEnvfv(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlTexEnvx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexEnvx(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glTexEnvxv(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glTexEnvxv(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlTexGenf(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glTexGenf(
        (GLint)coord,
        (GLint)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLfloat *params_base = (GLfloat *) 0;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glTexGenfv(
        (GLint)coord,
        (GLint)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glTexGenfv(
        (GLint)coord,
        (GLint)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGeni(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glTexGeni(
        (GLint)coord,
        (GLint)pname,
        (GLint)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glTexGeniv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glTexGeniv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGenx(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    glTexGenx(
        (GLint)coord,
        (GLint)pname,
        (GLint)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    GLint *params_base = (GLint *) 0;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glTexGenxv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    if (! SupportsExtension(this, HAVE_OES_TEXTURE_CUBE_MAPID)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glTexGenxv(
        (GLint)coord,
        (GLint)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetPointerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<IBuffer*>* params)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CGLImpl::GlBindBuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 buffer)
{
    glBindBuffer(
        (GLenum)target,
        (GLuint)buffer
    );
    return NOERROR;
}

ECode CGLImpl::GlBufferData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data_buf,
    /* [in] */ Int32 usage)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *data = (GLvoid *) 0;

    if (data_buf) {
        Handle64 tmp;
        GetPointer(data_buf, &_array, &_remaining, &_bufferOffset, &tmp);
        data = (GLvoid *)tmp;
        if (_remaining < size) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    if (data_buf && data == NULL) {
        char * _dataBase = reinterpret_cast<char *>(_array);
        data = (GLvoid *) (_dataBase + _bufferOffset);
    }
    glBufferData(
        (GLenum)target,
        (GLsizeiptr)size,
        (GLvoid *)data,
        (GLenum)usage
    );

    return NOERROR;
}

ECode CGLImpl::GlBufferSubData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLvoid *data = (GLvoid *) 0;

    Handle64 tmp;
    GetPointer(data_buf, &_array, &_remaining, &_bufferOffset, &tmp);
    data = (GLvoid *)tmp;
    if (_remaining < size) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (data == NULL) {
        char * _dataBase = reinterpret_cast<char *>(_array);
        data = (GLvoid *) (_dataBase + _bufferOffset);
    }
    glBufferSubData(
        (GLenum)target,
        (GLintptr)offset,
        (GLsizeiptr)size,
        (GLvoid *)data
    );
    return NOERROR;
}

ECode CGLImpl::GlClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Float>* equation_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *equation_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *equation = (GLfloat *) 0;

    if (!equation_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = equation_ref->GetLength() - offset;
    if (_remaining < 4) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    equation_base = (GLfloat *)equation_ref->GetPayload();
    equation = equation_base + offset;

    glClipPlanef(
        (GLenum)plane,
        (GLfloat *)equation
    );
    return NOERROR;
}

ECode CGLImpl::GlClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ IFloatBuffer* equation_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *equation = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(equation_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    equation = (GLfloat *)tmp;
    if (_remaining < 4) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (equation == NULL) {
        char * _equationBase = reinterpret_cast<char *>(_array);
        equation = (GLfloat *) (_equationBase + _bufferOffset);
    }
    glClipPlanef(
        (GLenum)plane,
        (GLfloat *)equation
    );
    return NOERROR;
}

ECode CGLImpl::GlClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Int32>* equation_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *equation_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *equation = (GLfixed *) 0;

    if (!equation_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = equation_ref->GetLength() - offset;
    if (_remaining < 4) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    equation_base = (GLfixed *)equation_ref->GetPayload();
    equation = equation_base + offset;

    glClipPlanex(
        (GLenum)plane,
        (GLfixed *)equation
    );
    return NOERROR;
}

ECode CGLImpl::GlClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ IInt32Buffer* equation_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *equation = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(equation_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    equation = (GLfixed *)tmp;
    if (_remaining < 4) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (equation == NULL) {
        char * _equationBase = reinterpret_cast<char *>(_array);
        equation = (GLfixed *) (_equationBase + _bufferOffset);
    }
    glClipPlanex(
        (GLenum)plane,
        (GLfixed *)equation
    );
    return NOERROR;
}

ECode CGLImpl::GlColor4ub(
    /* [in] */ Byte red,
    /* [in] */ Byte green,
    /* [in] */ Byte blue,
    /* [in] */ Byte alpha)
{
    glColor4ub(
        (GLubyte)red,
        (GLubyte)green,
        (GLubyte)blue,
        (GLubyte)alpha
    );
    return NOERROR;
}

ECode CGLImpl::GlColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    glColorPointer(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

ECode CGLImpl::GlDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers_ref,
    /* [in] */ Int32 offset)
{
    GLuint *buffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *buffers = (GLuint *) 0;

    if (!buffers_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = buffers_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    buffers_base = (GLuint *)buffers_ref->GetPayload();
    buffers = buffers_base + offset;

    glDeleteBuffers(
        (GLsizei)n,
        (GLuint *)buffers
    );
    return NOERROR;
}

ECode CGLImpl::GlDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *buffers = (GLuint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(buffers_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    buffers = (GLuint *) tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (buffers == NULL) {
        char * _buffersBase = reinterpret_cast<char *>(_array);
        buffers = (GLuint *) (_buffersBase + _bufferOffset);
    }
    glDeleteBuffers(
        (GLsizei)n,
        (GLuint *)buffers
    );
    return NOERROR;
}

ECode CGLImpl::GlDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset)
{
    glDrawElements(
        (GLenum)mode,
        (GLsizei)count,
        (GLenum)type,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

ECode CGLImpl::GlGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers_ref,
    /* [in] */ Int32 offset)
{
    GLuint *buffers_base = (GLuint *) 0;
    Int32 _remaining;
    GLuint *buffers = (GLuint *) 0;

    if (!buffers_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = buffers_ref->GetLength() - offset;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    buffers_base = (GLuint *) buffers_ref->GetPayload();
    buffers = buffers_base + offset;

    glGenBuffers(
        (GLsizei)n,
        (GLuint *)buffers
    );
    return NOERROR;
}

ECode CGLImpl::GlGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLuint *buffers = (GLuint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(buffers_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    buffers = (GLuint *)tmp;
    if (_remaining < n) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (buffers == NULL) {
        char * _buffersBase = reinterpret_cast<char *>(_array);
        buffers = (GLuint *) (_buffersBase + _bufferOffset);
    }
    glGenBuffers(
        (GLsizei)n,
        (GLuint *)buffers
    );
    return NOERROR;
}

ECode CGLImpl::GlGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Boolean>* params_ref,
    /* [in] */ Int32 offset)
{
    GLboolean *params_base = (GLboolean *) 0;
    GLboolean *params = (GLboolean *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLboolean *)params_ref->GetPayload();
    params = params_base + offset;

    glGetBooleanv(
        (GLenum)pname,
        (GLboolean *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLboolean *params = (GLboolean *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLboolean *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLboolean *) (_paramsBase + _bufferOffset);
    }
    glGetBooleanv(
        (GLenum)pname,
        (GLboolean *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CGLImpl::GlGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CGLImpl::GlGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* eqn_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *eqn_base = (GLfloat *) 0;
    GLfloat *eqn = (GLfloat *) 0;

    if (!eqn_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    eqn_base = (GLfloat *)eqn_ref->GetPayload();
    eqn = eqn_base + offset;

    glGetClipPlanef(
        (GLenum)pname,
        (GLfloat *)eqn
    );
    return NOERROR;
}

ECode CGLImpl::GlGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* eqn_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *eqn = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(eqn_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    eqn = (GLfloat *)tmp;
    if (eqn == NULL) {
        char * _eqnBase = reinterpret_cast<char *>(_array);
        eqn = (GLfloat *) (_eqnBase + _bufferOffset);
    }
    glGetClipPlanef(
        (GLenum)pname,
        (GLfloat *)eqn
    );
    return NOERROR;
}

ECode CGLImpl::GlGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* eqn_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *eqn_base = (GLfixed *) 0;
    GLfixed *eqn = (GLfixed *) 0;

    if (!eqn_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    eqn_base = (GLfixed *)eqn_ref->GetPayload();
    eqn = eqn_base + offset;

    glGetClipPlanex(
        (GLenum)pname,
        (GLfixed *)eqn
    );
    return NOERROR;
}

ECode CGLImpl::GlGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* eqn_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *eqn = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(eqn_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    eqn = (GLfixed *)tmp;
    if (eqn == NULL) {
        char * _eqnBase = reinterpret_cast<char *>(_array);;
        eqn = (GLfixed *) (_eqnBase + _bufferOffset);
    }
    glGetClipPlanex(
        (GLenum)pname,
        (GLfixed *)eqn
    );
    return NOERROR;
}

ECode CGLImpl::GlGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glGetFixedv(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetFixedv(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glGetFloatv(
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glGetFloatv(
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glGetLightfv(
        (GLenum)light,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glGetLightfv(
        (GLenum)light,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glGetLightxv(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SPOT_EXPONENT)
        case GL_SPOT_EXPONENT:
#endif // defined(GL_SPOT_EXPONENT)
#if defined(GL_SPOT_CUTOFF)
        case GL_SPOT_CUTOFF:
#endif // defined(GL_SPOT_CUTOFF)
#if defined(GL_CONSTANT_ATTENUATION)
        case GL_CONSTANT_ATTENUATION:
#endif // defined(GL_CONSTANT_ATTENUATION)
#if defined(GL_LINEAR_ATTENUATION)
        case GL_LINEAR_ATTENUATION:
#endif // defined(GL_LINEAR_ATTENUATION)
#if defined(GL_QUADRATIC_ATTENUATION)
        case GL_QUADRATIC_ATTENUATION:
#endif // defined(GL_QUADRATIC_ATTENUATION)
            _needed = 1;
            break;
#if defined(GL_SPOT_DIRECTION)
        case GL_SPOT_DIRECTION:
#endif // defined(GL_SPOT_DIRECTION)
            _needed = 3;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetLightxv(
        (GLenum)light,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glGetMaterialfv(
        (GLenum)face,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glGetMaterialfv(
        (GLenum)face,
        (GLenum)pname,
        (GLfloat *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glGetMaterialxv(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_SHININESS)
        case GL_SHININESS:
#endif // defined(GL_SHININESS)
            _needed = 1;
            break;
#if defined(GL_AMBIENT)
        case GL_AMBIENT:
#endif // defined(GL_AMBIENT)
#if defined(GL_DIFFUSE)
        case GL_DIFFUSE:
#endif // defined(GL_DIFFUSE)
#if defined(GL_SPECULAR)
        case GL_SPECULAR:
#endif // defined(GL_SPECULAR)
#if defined(GL_EMISSION)
        case GL_EMISSION:
#endif // defined(GL_EMISSION)
#if defined(GL_AMBIENT_AND_DIFFUSE)
        case GL_AMBIENT_AND_DIFFUSE:
#endif // defined(GL_AMBIENT_AND_DIFFUSE)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetMaterialxv(
        (GLenum)face,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexEnviv(
        (GLenum)env,
        (GLenum)pname,
        (GLint *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetTexEnviv(
        (GLenum)env,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexEnvxv(
        (GLenum)env,
        (GLenum)pname,
        (GLfixed *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetTexEnvxv(
        (GLenum)env,
        (GLenum)pname,
        (GLfixed *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexParameterfv(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glGetTexParameterfv(
        (GLenum)target,
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexParameteriv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glGetTexParameteriv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glGetTexParameterxv(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glGetTexParameterxv(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlIsBuffer(
    /* [in] */ Int32 buffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsBuffer(
        (GLuint)buffer
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlIsEnabled(
    /* [in] */ Int32 cap,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsEnabled(
        (GLenum)cap
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlIsTexture(
    /* [in] */ Int32 texture,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLboolean _returnValue;
    _returnValue = glIsTexture(
        (GLuint)texture
    );
    *result = _returnValue;
    return NOERROR;
}

ECode CGLImpl::GlNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    glNormalPointer(
        (GLenum)type,
        (GLsizei)stride,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

ECode CGLImpl::GlPointParameterf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    glPointParameterf(
        (GLenum)pname,
        (GLfloat)param
    );
    return NOERROR;
}

ECode CGLImpl::GlPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfloat *params_base = (GLfloat *) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfloat *)params_ref->GetPayload();
    params = params_base + offset;

    glPointParameterfv(
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfloat *params = (GLfloat *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfloat *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfloat *) (_paramsBase + _bufferOffset);
    }
    glPointParameterfv(
        (GLenum)pname,
        (GLfloat *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlPointParameterx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glPointParameterx(
        (GLenum)pname,
        (GLfixed)param
    );
    return NOERROR;
}

ECode CGLImpl::GlPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glPointParameterxv(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glPointParameterxv(
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlPointSizePointerOES(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Int32 remaining;
    pointer->GetRemaining(&remaining);
    ECode ec = GlPointSizePointerOESBounds(
        type,
        stride,
        pointer,
        remaining
    );
    if (((type == GL_FLOAT) ||
         (type == GL_FIXED)) &&
        (stride >= 0)) {
        m_PointSizePointerOES = pointer;
    }
    return ec;
}

ECode CGLImpl::GlPointSizePointerOESBounds(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer_buf,
    /* [in] */ Int32 remaining)
{
    GLvoid *pointer = (GLvoid *) 0;

    if (pointer_buf) {
        Handle64 tmp;
        FAIL_RETURN(GetDirectBufferPointer(pointer_buf, &tmp));
        pointer = (GLvoid *)tmp;
        if ( ! pointer ) {
            return NOERROR;
        }
    }
    glPointSizePointerOESBounds(
        (GLenum)type,
        (GLsizei)stride,
        (GLvoid *)pointer,
        (GLsizei)remaining
    );
    return NOERROR;
}

ECode CGLImpl::GlTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    glTexCoordPointer(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

ECode CGLImpl::GlTexEnvi(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexEnvi(
        (GLenum)target,
        (GLenum)pname,
        (GLint)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glTexEnviv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    Int32 _needed;
    switch (pname) {
#if defined(GL_TEXTURE_ENV_MODE)
        case GL_TEXTURE_ENV_MODE:
#endif // defined(GL_TEXTURE_ENV_MODE)
#if defined(GL_COMBINE_RGB)
        case GL_COMBINE_RGB:
#endif // defined(GL_COMBINE_RGB)
#if defined(GL_COMBINE_ALPHA)
        case GL_COMBINE_ALPHA:
#endif // defined(GL_COMBINE_ALPHA)
            _needed = 1;
            break;
#if defined(GL_TEXTURE_ENV_COLOR)
        case GL_TEXTURE_ENV_COLOR:
#endif // defined(GL_TEXTURE_ENV_COLOR)
            _needed = 4;
            break;
        default:
            _needed = 1;
            break;
    }
    if (_remaining < _needed) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glTexEnviv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );

    return NOERROR;
}

ECode CGLImpl::GlTexParameteri(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    glTexParameteri(
        (GLenum)target,
        (GLenum)pname,
        (GLint)param
    );
    return NOERROR;
}

ECode CGLImpl::GlTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLint *params_base = (GLint *) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLint *)params_ref->GetPayload();
    params = params_base + offset;

    glTexParameteriv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLint *params = (GLint *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLint *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLint *) (_paramsBase + _bufferOffset);
    }
    glTexParameteriv(
        (GLenum)target,
        (GLenum)pname,
        (GLint *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params_ref,
    /* [in] */ Int32 offset)
{
    GLfixed *params_base = (GLfixed *) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    if (!params_ref) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _remaining = params_ref->GetLength() - offset;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    params_base = (GLfixed *)params_ref->GetPayload();
    params = params_base + offset;

    glTexParameterxv(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params_buf)
{
    Handle64 _array = (Handle64) 0;
    Int32 _bufferOffset = (Int32) 0;
    Int32 _remaining;
    GLfixed *params = (GLfixed *) 0;

    Handle64 tmp;
    GetPointer(IBuffer::Probe(params_buf), &_array, &_remaining, &_bufferOffset, &tmp);
    params = (GLfixed *)tmp;
    if (_remaining < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        char * _paramsBase = reinterpret_cast<char *>(_array);
        params = (GLfixed *) (_paramsBase + _bufferOffset);
    }
    glTexParameterxv(
        (GLenum)target,
        (GLenum)pname,
        (GLfixed *)params
    );
    return NOERROR;
}

ECode CGLImpl::GlVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    glVertexPointer(
        (GLint)size,
        (GLenum)type,
        (GLsizei)stride,
        reinterpret_cast<GLvoid *>(offset)
    );
    return NOERROR;
}

Boolean CGLImpl::AllowIndirectBuffers(
    /* [in] */ const String& appName)
{
    Boolean result = false;
    Int32 version = 0;
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    // try {
        AutoPtr<IApplicationInfo> applicationInfo;
        pm->GetApplicationInfo(appName, 0, UserHandle::GetMyUserId(), (IApplicationInfo**)&applicationInfo);
        if (applicationInfo != NULL) {
            applicationInfo->GetTargetSdkVersion(&version);
        }
    // } catch (android.os.RemoteException e) {
    //     // ignore
    // }
    SLOGGERE("OpenGLES",
        "Application %s (SDK target %d) called a GL11 Pointer method with an indirect Buffer.",
        appName.string(), version)
    if (version <= Build::VERSION_CODES::CUPCAKE) {
        result = TRUE;
    }
    return result;
}

Boolean CGLImpl::AllowIndirectBuffers()
{
    static Int32 sIndirectBufferCompatability;
    if (sIndirectBufferCompatability == 0) {
        String appName(::__progname);
        sIndirectBufferCompatability = AllowIndirectBuffers(appName) ? 2 : 1;
    }
    return sIndirectBufferCompatability == 2;
}

ECode CGLImpl::GetPointer(
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
        *rst = (Handle64)pointer;
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

ECode CGLImpl::GetDirectBufferPointer(
    /* [in] */ IBuffer* buffer,
    /* [out] */ Handle64* result)
{
    VALIDATE_NOT_NULL(result)

    Handle64 buf;
    buffer->GetEffectiveDirectAddress(&buf);
    if (buf != 0) {
        Int32 position, elementSizeShift;
        buffer->GetPosition(&position);
        buffer->GetElementSizeShift(&elementSizeShift);
        buf += position << elementSizeShift;
    } else {
        if(AllowIndirectBuffers()) {
            Handle64 array = 0;
            Int32 remaining;
            Int32 offset;
            GetPointer(buffer, &array, &remaining, &offset, &buf);
            buf += offset;
        } else {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    *result = buf;
    return NOERROR;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos
