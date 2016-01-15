#include "GLErrorWrapper.h"
#include <elastos/utility/logging/Slogger.h>
#include "CGLU.h"
#include <elastos/core/Thread.h>
#include <elastos/core/StringUtils.h>

#define GLCHECK(gl, func, ...)                                       \
FAIL_RETURN(CheckThread());                                          \
FAIL_RETURN(gl->func(__VA_ARGS__));                                  \
FAIL_RETURN(CheckError());

using Elastos::Core::Thread;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Opengl {

GLErrorWrapper::GLErrorWrapper(
    /* [in] */ IGL* gl,
    /* [in] */ Int32 configFlags)
    : GLWrapperBase(gl)
{
    mCheckError = (configFlags & IGLDebugHelper::_CONFIG_CHECK_GL_ERROR) != 0;
    mCheckThread = (configFlags & IGLDebugHelper::_CONFIG_CHECK_THREAD) != 0;
}

ECode GLErrorWrapper::glActiveTexture(
    /* [in] */ Int32 texture)
{
    GLCHECK(mgl, GlActiveTexture, texture)
    return NOERROR;
}

ECode GLErrorWrapper::glAlphaFunc(
    /* [in] */ Int32 func,
    /* [in] */ Float ref)
{
    GLCHECK(mgl, GlAlphaFunc, func, ref)
    return NOERROR;
}

ECode GLErrorWrapper::glAlphaFuncx(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref)
{
    GLCHECK(mgl, GlAlphaFuncx, func, ref)
    return NOERROR;
}

ECode GLErrorWrapper::glBlendFunc(
    /* [in] */ Int32 sfactor,
    /* [in] */ Int32 dfactor)
{
    GLCHECK(mgl, GlBlendFunc, sfactor, dfactor)
    return NOERROR;
}

ECode GLErrorWrapper::glClear(
    /* [in] */ Int32 mask)
{
    GLCHECK(mgl, GlClear, mask)
    return NOERROR;
}

ECode GLErrorWrapper::glClearColor(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    GLCHECK(mgl, GlClearColor, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::glClearColorx(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    GLCHECK(mgl, GlClearColorx, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::glClearDepthf(
    /* [in] */ Float depth)
{
    GLCHECK(mgl, GlClearDepthf, depth)
    return NOERROR;
}

ECode GLErrorWrapper::glClearDepthx(
    /* [in] */ Int32 depth)
{
    GLCHECK(mgl, GlClearDepthx, depth)
    return NOERROR;
}

ECode GLErrorWrapper::glClearStencil(
    /* [in] */ Int32 s)
{
    GLCHECK(mgl, GlClearStencil, s)
    return NOERROR;
}

ECode GLErrorWrapper::glClientActiveTexture(
    /* [in] */ Int32 texture)
{
    GLCHECK(mgl, GlClientActiveTexture, texture)
    return NOERROR;
}

ECode GLErrorWrapper::glColor4f(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    GLCHECK(mgl, GlColor4f, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::glColor4x(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    GLCHECK(mgl, GlColor4x, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::glColorMask(
    /* [in] */ Boolean red,
    /* [in] */ Boolean green,
    /* [in] */ Boolean blue,
    /* [in] */ Boolean alpha)
{
    GLCHECK(mgl, GlColorMask, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::glColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlColorPointer, size, type, stride, pointer)
    return NOERROR;
}

ECode GLErrorWrapper::glCompressedTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data)
{
    GLCHECK(mgl, GlCompressedTexSubImage2D, target, level, xoffset, yoffset, width, height, format, imageSize, data)
    return NOERROR;
}

ECode GLErrorWrapper::glCopyTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl, GlCopyTexSubImage2D, target, level, xoffset, yoffset, x, y, width, height)
    return NOERROR;
}

ECode GLErrorWrapper::glCullFace(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlCullFace, mode)
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlDeleteTextures, n, textures, offset)
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures)
{
    GLCHECK(mgl, GlDeleteTexturesEx, n, textures)
    return NOERROR;
}

ECode GLErrorWrapper::glDepthFunc(
    /* [in] */ Int32 func)
{
    GLCHECK(mgl, GlDepthFunc, func)
    return NOERROR;
}

ECode GLErrorWrapper::glDepthMask(
    /* [in] */ Boolean flag)
{
    GLCHECK(mgl, GlDepthMask, flag)
    return NOERROR;
}

ECode GLErrorWrapper::glDepthRangef(
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    GLCHECK(mgl, GlDepthRangef, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::glDepthRangex(
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    GLCHECK(mgl, GlDepthRangex, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::glDisable(
    /* [in] */ Int32 cap)
{
    GLCHECK(mgl, GlDisable, cap);
    return NOERROR;
}

ECode GLErrorWrapper::glDisableClientState(
    /* [in] */ Int32 array)
{
    GLCHECK(mgl, GlDisableClientState, array);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawArrays(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 first,
    /* [in] */ Int32 count)
{
    GLCHECK(mgl, GlDrawArrays, mode, first, count);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* indices)
{
    GLCHECK(mgl, GlDrawElements, mode, count, type, indices);
    return NOERROR;
}

ECode GLErrorWrapper::glFinish()
{
    GLCHECK(mgl, GlFinish);
    return NOERROR;
}

ECode GLErrorWrapper::glFlush()
{
    GLCHECK(mgl, GlFlush);
    return NOERROR;
}

ECode GLErrorWrapper::glFogf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlFogf, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlFogfv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlFogfvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glFogx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlFogx, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlFogxv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlFogxvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glFrontFace(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlFrontFace, mode);
    return NOERROR;
}

ECode GLErrorWrapper::glFrustumf(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    GLCHECK(mgl, GlFrustumf, left, right, bottom, top, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::glFrustumx(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    GLCHECK(mgl, GlFrustumx, left, right, bottom, top, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::glGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlGenTextures, n, textures, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures)
{
    GLCHECK(mgl, GlGenTexturesEx, n, textures);
    return NOERROR;
}

ECode GLErrorWrapper::glGetError(
    /* [out] */ Int32* error)
{
    GLCHECK(mgl, GlGetError, error);
    return NOERROR;
}

ECode GLErrorWrapper::glGetString(
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    GLCHECK(mgl, GlGetString, name, str);
    return NOERROR;
}

ECode GLErrorWrapper::glHint(
    /* [in] */ Int32 target,
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlHint, target, mode);
    return NOERROR;
}

ECode GLErrorWrapper::glLightModelf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlLightModelf, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightModelfv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlLightModelfvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glLightModelx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlLightModelx, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightModelxv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlLightModelxvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glLightf(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlLightf, light, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightfv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlLightfvEx, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glLightx(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlLightx, light, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightxv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlLightxvEx, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glLineWidth(
    /* [in] */ Float width)
{
    GLCHECK(mgl, GlLineWidth, width);
    return NOERROR;
}

ECode GLErrorWrapper::glLineWidthx(
    /* [in] */ Int32 width)
{
    GLCHECK(mgl, GlLineWidthx, width);
    return NOERROR;
}

ECode GLErrorWrapper::glLoadIdentity()
{
    GLCHECK(mgl, GlLoadIdentity);
    return NOERROR;
}

ECode GLErrorWrapper::glLoadMatrixf(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLoadMatrixf, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glLoadMatrixf(
    /* [in] */ IFloatBuffer* m)
{
    GLCHECK(mgl, GlLoadMatrixfEx, m);
    return NOERROR;
}

ECode GLErrorWrapper::glLoadMatrixx(
    /* [in] */ ArrayOf<Int32>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLoadMatrixx, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glLoadMatrixx(
    /* [in] */ IInt32Buffer* m)
{
    GLCHECK(mgl, GlLoadMatrixxEx, m);
    return NOERROR;
}

ECode GLErrorWrapper::glLogicOp(
    /* [in] */ Int32 opcode)
{
    GLCHECK(mgl, GlLogicOp, opcode);
    return NOERROR;
}

ECode GLErrorWrapper::glMaterialf(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlMaterialf, face, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMaterialfv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlMaterialfvEx, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glMaterialx(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlMaterialx, face, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMaterialxv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlMaterialxvEx, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glMatrixMode(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlMatrixMode, mode);
    return NOERROR;
}

ECode GLErrorWrapper::glMultMatrixf(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMultMatrixf, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glMultMatrixf(
    /* [in] */ IFloatBuffer* m)
{
    GLCHECK(mgl, GlMultMatrixfEx, m);
    return NOERROR;
}

ECode GLErrorWrapper::glMultMatrixx(
    /* [in] */ ArrayOf<Int32>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMultMatrixx, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glMultMatrixx(
    /* [in] */ IInt32Buffer* m)
{
    GLCHECK(mgl, GlMultMatrixxEx, m);
    return NOERROR;
}

ECode GLErrorWrapper::glMultiTexCoord4f(
    /* [in] */ Int32 target,
    /* [in] */ Float s,
    /* [in] */ Float t,
    /* [in] */ Float r,
    /* [in] */ Float q)
{
    GLCHECK(mgl, GlMultiTexCoord4f, target, s, t, r, q);
    return NOERROR;
}

ECode GLErrorWrapper::glMultiTexCoord4x(
    /* [in] */ Int32 target,
    /* [in] */ Int32 s,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 q)
{
    GLCHECK(mgl, GlMultiTexCoord4x, target, s, t, r, q);
    return NOERROR;
}

ECode GLErrorWrapper::glNormal3f(
    /* [in] */ Float nx,
    /* [in] */ Float ny,
    /* [in] */ Float nz)
{
    GLCHECK(mgl, GlNormal3f, nx, ny, nz);
    return NOERROR;
}

ECode GLErrorWrapper::glNormal3x(
    /* [in] */ Int32 nx,
    /* [in] */ Int32 ny,
    /* [in] */ Int32 nz)
{
    GLCHECK(mgl, GlNormal3x, nx, ny, nz);
    return NOERROR;
}

ECode GLErrorWrapper::glNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlNormalPointer, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::glOrthof(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    GLCHECK(mgl, GlOrthof, left, right, bottom, top, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::glOrthox(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    GLCHECK(mgl, GlOrthox, left, right, bottom, top, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::glPixelStorei(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlPixelStorei, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glPointSize(
    /* [in] */ Float size)
{
    GLCHECK(mgl, GlPointSize, size);
    return NOERROR;
}

ECode GLErrorWrapper::glPointSizex(
    /* [in] */ Int32 size)
{
    GLCHECK(mgl, GlPointSizex, size);
    return NOERROR;
}

ECode GLErrorWrapper::glPolygonOffset(
    /* [in] */ Float factor,
    /* [in] */ Float units)
{
    GLCHECK(mgl, GlPolygonOffset, factor, units);
    return NOERROR;
}

ECode GLErrorWrapper::glPolygonOffsetx(
    /* [in] */ Int32 factor,
    /* [in] */ Int32 units)
{
    GLCHECK(mgl, GlPolygonOffsetx, factor, units);
    return NOERROR;
}

ECode GLErrorWrapper::glPopMatrix()
{
    GLCHECK(mgl, GlPopMatrix);
    return NOERROR;
}

ECode GLErrorWrapper::glPushMatrix()
{
    GLCHECK(mgl, GlPushMatrix);
    return NOERROR;
}

ECode GLErrorWrapper::glReadPixels(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels)
{
    GLCHECK(mgl, GlReadPixels, x, y, width, height, format, type, pixels);
    return NOERROR;
}

ECode GLErrorWrapper::glRotatef(
    /* [in] */ Float angle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    GLCHECK(mgl, GlRotatef, angle, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::glRotatex(
    /* [in] */ Int32 angle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    GLCHECK(mgl, GlRotatex, angle, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::glSampleCoverage(
    /* [in] */ Float value,
    /* [in] */ Boolean invert)
{
    GLCHECK(mgl, GlSampleCoverage, value, invert);
    return NOERROR;
}

ECode GLErrorWrapper::glSampleCoveragex(
    /* [in] */ Int32 value,
    /* [in] */ Boolean invert)
{
    GLCHECK(mgl, GlSampleCoveragex, value, invert);
    return NOERROR;
}

ECode GLErrorWrapper::glScalef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    GLCHECK(mgl, GlScalef, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::glScalex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    GLCHECK(mgl, GlScalex, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::glScissor(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl, GlScissor, x, y, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::glShadeModel(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlShadeModel, mode);
    return NOERROR;
}

ECode GLErrorWrapper::glStencilFunc(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref,
    /* [in] */ Int32 mask)
{
    GLCHECK(mgl, GlStencilFunc, func, ref, mask);
    return NOERROR;
}

ECode GLErrorWrapper::glStencilMask(
    /* [in] */ Int32 mask)
{
    GLCHECK(mgl, GlStencilMask, mask);
    return NOERROR;
}

ECode GLErrorWrapper::glTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlTexCoordPointer, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::glTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels)
{
    GLCHECK(mgl, GlTexImage2D, target, level, internalformat, width, height, border, format, type, pixels);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameterx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlTexParameterx, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels)
{
    GLCHECK(mgl, GlTexSubImage2D, target, level, xoffset, yoffset, width, height, format, type, pixels);
    return NOERROR;
}

ECode GLErrorWrapper::glTranslatef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    GLCHECK(mgl, GlTranslatef, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::glTranslatex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    GLCHECK(mgl, GlTranslatex, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::glVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlVertexPointer, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::glViewport(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl, GlViewport, x, y, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::glEnable(
    /* [in] */ Int32 cap)
{
    GLCHECK(mgl, GlEnable, cap);
    return NOERROR;
}

ECode GLErrorWrapper::glEnableClientState(
    /* [in] */ Int32 array)
{
    GLCHECK(mgl, GlEnableClientState, array);
    return NOERROR;
}


ECode GLErrorWrapper::glTexParameterf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlTexParameterf, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexParameterfv, target,  pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlTexParameterfvEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glBindTexture(
    /* [in] */ Int32 target,
    /* [in] */ Int32 texture)
{
    GLCHECK(mgl, GlBindTexture, target, texture);
    return NOERROR;
}

ECode GLErrorWrapper::glCompressedTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data)
{
    GLCHECK(mgl, GlCompressedTexImage2D, target, level, internalformat, width, height, border, imageSize, data);
    return NOERROR;
}

ECode GLErrorWrapper::glCopyTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border)
{
    GLCHECK(mgl, GlCopyTexImage2D, target, level, internalformat, x, y, width, height, border);
    return NOERROR;
}

ECode GLErrorWrapper::glGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlGetIntegerv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlGetIntegervEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glStencilOp(
    /* [in] */ Int32 fail,
    /* [in] */ Int32 zfail,
    /* [in] */ Int32 zpass)
{
    GLCHECK(mgl, GlStencilOp, fail, zfail, zpass);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnvf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlTexEnvf, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlTexEnvfv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlTexEnvfvEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnvx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlTexEnvx, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlTexEnvxv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlTexEnvxvEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glQueryMatrixxOES(
    /* [in] */ ArrayOf<Int32>* mantissa,
    /* [in] */ Int32 mantissaOffset,
    /* [in] */ ArrayOf<Int32>* exponent,
    /* [in] */ Int32 exponentOffset,
    /* [out] */ Int32* matrixxOES)
{
    GLCHECK(mgl10Ext, GlQueryMatrixxOES, mantissa, mantissaOffset, exponent, exponentOffset, matrixxOES);
    return NOERROR;
}

ECode GLErrorWrapper::glQueryMatrixxOES(
    /* [in] */ IInt32Buffer* mantissa,
    /* [in] */ IInt32Buffer* exponent,
    /* [out] */ Int32* matrixxOES)
{
    GLCHECK(mgl10Ext, GlQueryMatrixxOESEx, mantissa, exponent, matrixxOES);
    return NOERROR;
}

ECode GLErrorWrapper::glGetPointerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<IBuffer*>* params)
{
    GLCHECK(mgl11, GlGetPointerv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glBindBuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 buffer)
{
    GLCHECK(mgl11, GlBindBuffer, target, buffer);
    return NOERROR;
}

ECode GLErrorWrapper::glBufferData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data,
    /* [in] */ Int32 usage)
{
    GLCHECK(mgl11, GlBufferData, target, size, data, usage);
    return NOERROR;
}

ECode GLErrorWrapper::glBufferSubData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data)
{
    GLCHECK(mgl11, GlBufferSubData, target, offset, size, data);
    return NOERROR;
}

ECode GLErrorWrapper::glClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Float>* equation,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlClipPlanef, plane, equation, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ IFloatBuffer* equation)
{
    GLCHECK(mgl11, GlClipPlanefEx, plane, equation);
    return NOERROR;
}

ECode GLErrorWrapper::glClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Int32>* equation,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlClipPlanex, plane, equation, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ IInt32Buffer* equation)
{
    GLCHECK(mgl11, GlClipPlanexEx, plane, equation);
    return NOERROR;
}

ECode GLErrorWrapper::glColor4ub(
    /* [in] */ Byte red,
    /* [in] */ Byte green,
    /* [in] */ Byte blue,
    /* [in] */ Byte alpha)
{
    GLCHECK(mgl11, GlColor4ub, red, green, blue, alpha);
    return NOERROR;
}

ECode GLErrorWrapper::glColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlColorPointerEx, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlDeleteBuffers, n, buffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers)
{
    GLCHECK(mgl11, GlDeleteBuffersEx, n, buffers);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlDrawElementsEx, mode, count, type, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGenBuffers, n, buffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers)
{
    GLCHECK(mgl11, GlGenBuffersEx, n, buffers);
    return NOERROR;
}

ECode GLErrorWrapper::glGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Boolean>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetBooleanv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetBooleanvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetBufferParameteriv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetBufferParameterivEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* eqn,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetClipPlanef, pname, eqn, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* eqn)
{
    GLCHECK(mgl11, GlGetClipPlanefEx, pname, eqn);
    return NOERROR;
}

ECode GLErrorWrapper::glGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* eqn,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetClipPlanex, pname, eqn, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* eqn)
{
    GLCHECK(mgl11, GlGetClipPlanexEx, pname, eqn);
    return NOERROR;
}

ECode GLErrorWrapper::glGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetFixedv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetFixedvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetFloatv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetFloatvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetLightfv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetLightfvEx, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetLightxv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetLightxvEx, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetMaterialfv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetMaterialfvEx, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetMaterialxv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetMaterialxvEx, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexEnviv, env, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexEnvivEx, env, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexEnvxv, env, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexEnvxvEx, env, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexParameterfv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetTexParameterfvEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexParameteriv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexParameterivEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexParameterxv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexParameterxvEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glIsBuffer(
    /* [in] */ Int32 buffer,
    /* [out] */ Boolean* result)
{
    GLCHECK(mgl11, GlIsBuffer, buffer, result);
    return NOERROR;
}

ECode GLErrorWrapper::glIsEnabled(
    /* [in] */ Int32 cap,
    /* [out] */ Boolean* result)
{
    GLCHECK(mgl11, GlIsEnabled, cap, result);
    return NOERROR;
}

ECode GLErrorWrapper::glIsTexture(
    /* [in] */ Int32 texture,
    /* [out] */ Boolean* result)
{
    GLCHECK(mgl11, GlIsTexture, texture, result);
    return NOERROR;
}

ECode GLErrorWrapper::glNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlNormalPointerEx, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glPointParameterf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl11, GlPointParameterf, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlPointParameterfv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlPointParameterfvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glPointParameterx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11, GlPointParameterx, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlPointParameterxv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlPointParameterxvEx, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glPointSizePointerOES(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl11, GlPointSizePointerOES, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::glTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexCoordPointerEx, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnvi(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11, GlTexEnvi, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexEnviv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlTexEnvivEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameteri(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11, GlTexParameteri, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexParameteriv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlTexParameterivEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexParameterxv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlTexParameterxvEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlVertexPointerEx, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glCurrentPaletteMatrixOES(
    /* [in] */ Int32 matrixpaletteindex)
{
    GLCHECK(mgl11Ext, GlCurrentPaletteMatrixOES, matrixpaletteindex);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexfOES(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    GLCHECK(mgl11Ext, GlDrawTexfOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexfvOES(
    /* [in] */ ArrayOf<Float>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexfvOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexfvOES(
    /* [in] */ IFloatBuffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexfvOESEx, coords);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexiOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl11Ext, GlDrawTexiOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexivOES(
    /* [in] */ ArrayOf<Int32>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexivOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexivOES(
    /* [in] */ IInt32Buffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexivOESEx, coords);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexsOES(
    /* [in] */ Int16 x,
    /* [in] */ Int16 y,
    /* [in] */ Int16 z,
    /* [in] */ Int16 width,
    /* [in] */ Int16 height)
{
    GLCHECK(mgl11Ext, GlDrawTexsOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexsvOES(
    /* [in] */ ArrayOf<Int16>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexsvOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexsvOES(
    /* [in] */ IInt16Buffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexsvOESEx, coords);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexxOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl11Ext, GlDrawTexxOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexxvOES(
    /* [in] */ ArrayOf<Int32>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexxvOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDrawTexxvOES(
    /* [in] */ IInt32Buffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexxvOESEx, coords);
    return NOERROR;
}

ECode GLErrorWrapper::glLoadPaletteFromModelViewMatrixOES()
{
    GLCHECK(mgl11Ext, GlLoadPaletteFromModelViewMatrixOES);
    return NOERROR;
}

ECode GLErrorWrapper::glMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl11Ext, GlMatrixIndexPointerOES, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::glMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlMatrixIndexPointerOESEx, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl11Ext, GlWeightPointerOES, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::glWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlWeightPointerOESEx, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glBindFramebufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 framebuffer)
{
    GLCHECK(mgl11ExtensionPack, GlBindFramebufferOES, target, framebuffer);
    return NOERROR;
}

ECode GLErrorWrapper::glBindRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 renderbuffer)
{
    GLCHECK(mgl11ExtensionPack, GlBindRenderbufferOES, target, renderbuffer);
    return NOERROR;
}

ECode GLErrorWrapper::glBlendEquation(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl11ExtensionPack, GlBlendEquation, mode);
    return NOERROR;
}

ECode GLErrorWrapper::glBlendEquationSeparate(
    /* [in] */ Int32 modeRGB,
    /* [in] */ Int32 modeAlpha)
{
    GLCHECK(mgl11ExtensionPack, GlBlendEquationSeparate, modeRGB, modeAlpha);
    return NOERROR;
}

ECode GLErrorWrapper::glBlendFuncSeparate(
    /* [in] */ Int32 srcRGB,
    /* [in] */ Int32 dstRGB,
    /* [in] */ Int32 srcAlpha,
    /* [in] */ Int32 dstAlpha)
{
    GLCHECK(mgl11ExtensionPack, GlBlendFuncSeparate, srcRGB, dstRGB, srcAlpha, dstAlpha);
    return NOERROR;
}

ECode GLErrorWrapper::glCheckFramebufferStatusOES(
    /* [in] */ Int32 target,
    /* [out] */ Int32* status)
{
    GLCHECK(mgl11ExtensionPack, GlCheckFramebufferStatusOES, target, status);
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteFramebuffersOES, n, framebuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteFramebuffersOESEx, n, framebuffers);
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteRenderbuffersOES, n, renderbuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteRenderbuffersOESEx, n, renderbuffers);
    return NOERROR;
}

ECode GLErrorWrapper::glFramebufferRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 renderbuffertarget,
    /* [in] */ Int32 renderbuffer)
{
    GLCHECK(mgl11ExtensionPack, GlFramebufferRenderbufferOES, target, attachment, renderbuffertarget, renderbuffer);
    return NOERROR;
}

ECode GLErrorWrapper::glFramebufferTexture2DOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 textarget,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level)
{
    GLCHECK(mgl11ExtensionPack, GlFramebufferTexture2DOES, target, attachment, textarget, texture, level);
    return NOERROR;
}

ECode GLErrorWrapper::glGenerateMipmapOES(
    /* [in] */ Int32 target)
{
    GLCHECK(mgl11ExtensionPack, GlGenerateMipmapOES, target);
    return NOERROR;
}

ECode GLErrorWrapper::glGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGenFramebuffersOES, n, framebuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers)
{
    GLCHECK(mgl11ExtensionPack, GlGenFramebuffersOESEx, n, framebuffers);
    return NOERROR;
}

ECode GLErrorWrapper::glGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGenRenderbuffersOES, n, renderbuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers)
{
    GLCHECK(mgl11ExtensionPack, GlGenRenderbuffersOESEx, n, renderbuffers);
    return NOERROR;
}

ECode GLErrorWrapper::glGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetFramebufferAttachmentParameterivOES, target, attachment, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetFramebufferAttachmentParameterivOESEx, target, attachment, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetRenderbufferParameterivOES, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetRenderbufferParameterivOESEx, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenfv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenfvEx, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGeniv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenivEx, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenxv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenxvEx, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glIsFramebufferOES(
    /* [in] */ Int32 framebuffer,
    /* [out] */ Boolean* result)
{
    GLCHECK(mgl11ExtensionPack, GlIsFramebufferOES, framebuffer, result);
    return NOERROR;
}

ECode GLErrorWrapper::glIsRenderbufferOES(
    /* [in] */ Int32 renderbuffer,
    /* [out] */ Boolean* result)
{
    GLCHECK(mgl11ExtensionPack, GlIsRenderbufferOES, renderbuffer, result);
    return NOERROR;
}

ECode GLErrorWrapper::glRenderbufferStorageOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl11ExtensionPack, GlRenderbufferStorageOES, target, internalformat, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGenf(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenf, coord, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenfv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenfvEx, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGeni(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11ExtensionPack, GlTexGeni, coord, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlTexGeniv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenivEx, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGenx(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenx, coord, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenxv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::glTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenxvEx, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::CheckThread()
{
    if (mCheckThread) {
        AutoPtr<IThread> currentThread = Thread::GetCurrentThread();
        if (mOurThread == NULL) {
            mOurThread = currentThread;
        } else {
            AutoPtr<IObject> objTrd = IObject::Probe(mOurThread);
            Boolean eql;
            if (objTrd->Equals(currentThread->Probe(EIID_IInterface), &eql), !eql) {
                SLOGGERE("GLErrorWrapper", "OpenGL method called from wrong thread.")
                return E_GL_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode GLErrorWrapper::CheckError()
{
    if (mCheckError) {
        Int32 glError;
        mgl->GlGetError(&glError);
        if (glError != 0) {
            AutoPtr<IGLU> glu;
            CGLU::AcquireSingleton((IGLU**)&glu);
            String errStr;
            glu->GluErrorString(glError, &errStr);
            if (errStr == NULL) {
                errStr = String("Unknown error 0x") + StringUtils::Int32ToString(glError, 16);
            }
            SLOGGERE("GLErrorWrapper", errStr)
            return E_GL_EXCEPTION;
        }
    }
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos