#include "elastos/droid/opengl/GLErrorWrapper.h"
#include "elastos/droid/opengl/CGLU.h"
#include <elastos/core/Thread.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

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

ECode GLErrorWrapper::GlActiveTexture(
    /* [in] */ Int32 texture)
{
    GLCHECK(mgl, GlActiveTexture, texture)
    return NOERROR;
}

ECode GLErrorWrapper::GlAlphaFunc(
    /* [in] */ Int32 func,
    /* [in] */ Float ref)
{
    GLCHECK(mgl, GlAlphaFunc, func, ref)
    return NOERROR;
}

ECode GLErrorWrapper::GlAlphaFuncx(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref)
{
    GLCHECK(mgl, GlAlphaFuncx, func, ref)
    return NOERROR;
}

ECode GLErrorWrapper::GlBlendFunc(
    /* [in] */ Int32 sfactor,
    /* [in] */ Int32 dfactor)
{
    GLCHECK(mgl, GlBlendFunc, sfactor, dfactor)
    return NOERROR;
}

ECode GLErrorWrapper::GlClear(
    /* [in] */ Int32 mask)
{
    GLCHECK(mgl, GlClear, mask)
    return NOERROR;
}

ECode GLErrorWrapper::GlClearColor(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    GLCHECK(mgl, GlClearColor, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::GlClearColorx(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    GLCHECK(mgl, GlClearColorx, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::GlClearDepthf(
    /* [in] */ Float depth)
{
    GLCHECK(mgl, GlClearDepthf, depth)
    return NOERROR;
}

ECode GLErrorWrapper::GlClearDepthx(
    /* [in] */ Int32 depth)
{
    GLCHECK(mgl, GlClearDepthx, depth)
    return NOERROR;
}

ECode GLErrorWrapper::GlClearStencil(
    /* [in] */ Int32 s)
{
    GLCHECK(mgl, GlClearStencil, s)
    return NOERROR;
}

ECode GLErrorWrapper::GlClientActiveTexture(
    /* [in] */ Int32 texture)
{
    GLCHECK(mgl, GlClientActiveTexture, texture)
    return NOERROR;
}

ECode GLErrorWrapper::GlColor4f(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    GLCHECK(mgl, GlColor4f, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::GlColor4x(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    GLCHECK(mgl, GlColor4x, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::GlColorMask(
    /* [in] */ Boolean red,
    /* [in] */ Boolean green,
    /* [in] */ Boolean blue,
    /* [in] */ Boolean alpha)
{
    GLCHECK(mgl, GlColorMask, red, green, blue, alpha)
    return NOERROR;
}

ECode GLErrorWrapper::GlColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlColorPointer, size, type, stride, pointer)
    return NOERROR;
}

ECode GLErrorWrapper::GlCompressedTexSubImage2D(
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

ECode GLErrorWrapper::GlCopyTexSubImage2D(
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

ECode GLErrorWrapper::GlCullFace(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlCullFace, mode)
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlDeleteTextures, n, textures, offset)
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures)
{
    GLCHECK(mgl, GlDeleteTextures, n, textures)
    return NOERROR;
}

ECode GLErrorWrapper::GlDepthFunc(
    /* [in] */ Int32 func)
{
    GLCHECK(mgl, GlDepthFunc, func)
    return NOERROR;
}

ECode GLErrorWrapper::GlDepthMask(
    /* [in] */ Boolean flag)
{
    GLCHECK(mgl, GlDepthMask, flag)
    return NOERROR;
}

ECode GLErrorWrapper::GlDepthRangef(
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    GLCHECK(mgl, GlDepthRangef, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::GlDepthRangex(
    /* [in] */ Int32 zNear,
    /* [in] */ Int32 zFar)
{
    GLCHECK(mgl, GlDepthRangex, zNear, zFar);
    return NOERROR;
}

ECode GLErrorWrapper::GlDisable(
    /* [in] */ Int32 cap)
{
    GLCHECK(mgl, GlDisable, cap);
    return NOERROR;
}

ECode GLErrorWrapper::GlDisableClientState(
    /* [in] */ Int32 array)
{
    GLCHECK(mgl, GlDisableClientState, array);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawArrays(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 first,
    /* [in] */ Int32 count)
{
    GLCHECK(mgl, GlDrawArrays, mode, first, count);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* indices)
{
    GLCHECK(mgl, GlDrawElements, mode, count, type, indices);
    return NOERROR;
}

ECode GLErrorWrapper::GlFinish()
{
    GLCHECK(mgl, GlFinish);
    return NOERROR;
}

ECode GLErrorWrapper::GlFlush()
{
    GLCHECK(mgl, GlFlush);
    return NOERROR;
}

ECode GLErrorWrapper::GlFogf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlFogf, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlFogfv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlFogfv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlFogx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlFogx, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlFogxv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlFogxv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlFrontFace(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlFrontFace, mode);
    return NOERROR;
}

ECode GLErrorWrapper::GlFrustumf(
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

ECode GLErrorWrapper::GlFrustumx(
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

ECode GLErrorWrapper::GlGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlGenTextures, n, textures, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures)
{
    GLCHECK(mgl, GlGenTextures, n, textures);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetError(
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error)

    GLCHECK(mgl, GlGetError, error);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetString(
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    GLCHECK(mgl, GlGetString, name, str);
    return NOERROR;
}

ECode GLErrorWrapper::GlHint(
    /* [in] */ Int32 target,
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlHint, target, mode);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightModelf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlLightModelf, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightModelfv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlLightModelfv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightModelx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlLightModelx, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightModelxv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlLightModelxv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightf(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlLightf, light, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightfv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlLightfv, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightx(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlLightx, light, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLightxv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlLightxv, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlLineWidth(
    /* [in] */ Float width)
{
    GLCHECK(mgl, GlLineWidth, width);
    return NOERROR;
}

ECode GLErrorWrapper::GlLineWidthx(
    /* [in] */ Int32 width)
{
    GLCHECK(mgl, GlLineWidthx, width);
    return NOERROR;
}

ECode GLErrorWrapper::GlLoadIdentity()
{
    GLCHECK(mgl, GlLoadIdentity);
    return NOERROR;
}

ECode GLErrorWrapper::GlLoadMatrixf(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLoadMatrixf, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlLoadMatrixf(
    /* [in] */ IFloatBuffer* m)
{
    GLCHECK(mgl, GlLoadMatrixf, m);
    return NOERROR;
}

ECode GLErrorWrapper::GlLoadMatrixx(
    /* [in] */ ArrayOf<Int32>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlLoadMatrixx, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlLoadMatrixx(
    /* [in] */ IInt32Buffer* m)
{
    GLCHECK(mgl, GlLoadMatrixx, m);
    return NOERROR;
}

ECode GLErrorWrapper::GlLogicOp(
    /* [in] */ Int32 opcode)
{
    GLCHECK(mgl, GlLogicOp, opcode);
    return NOERROR;
}

ECode GLErrorWrapper::GlMaterialf(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl, GlMaterialf, face, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMaterialfv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl, GlMaterialfv, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlMaterialx(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlMaterialx, face, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMaterialxv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl, GlMaterialxv, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlMatrixMode(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlMatrixMode, mode);
    return NOERROR;
}

ECode GLErrorWrapper::GlMultMatrixf(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMultMatrixf, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlMultMatrixf(
    /* [in] */ IFloatBuffer* m)
{
    GLCHECK(mgl, GlMultMatrixf, m);
    return NOERROR;
}

ECode GLErrorWrapper::GlMultMatrixx(
    /* [in] */ ArrayOf<Int32>* m,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl, GlMultMatrixx, m, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlMultMatrixx(
    /* [in] */ IInt32Buffer* m)
{
    GLCHECK(mgl, GlMultMatrixx, m);
    return NOERROR;
}

ECode GLErrorWrapper::GlMultiTexCoord4f(
    /* [in] */ Int32 target,
    /* [in] */ Float s,
    /* [in] */ Float t,
    /* [in] */ Float r,
    /* [in] */ Float q)
{
    GLCHECK(mgl, GlMultiTexCoord4f, target, s, t, r, q);
    return NOERROR;
}

ECode GLErrorWrapper::GlMultiTexCoord4x(
    /* [in] */ Int32 target,
    /* [in] */ Int32 s,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 q)
{
    GLCHECK(mgl, GlMultiTexCoord4x, target, s, t, r, q);
    return NOERROR;
}

ECode GLErrorWrapper::GlNormal3f(
    /* [in] */ Float nx,
    /* [in] */ Float ny,
    /* [in] */ Float nz)
{
    GLCHECK(mgl, GlNormal3f, nx, ny, nz);
    return NOERROR;
}

ECode GLErrorWrapper::GlNormal3x(
    /* [in] */ Int32 nx,
    /* [in] */ Int32 ny,
    /* [in] */ Int32 nz)
{
    GLCHECK(mgl, GlNormal3x, nx, ny, nz);
    return NOERROR;
}

ECode GLErrorWrapper::GlNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlNormalPointer, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::GlOrthof(
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

ECode GLErrorWrapper::GlOrthox(
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

ECode GLErrorWrapper::GlPixelStorei(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlPixelStorei, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointSize(
    /* [in] */ Float size)
{
    GLCHECK(mgl, GlPointSize, size);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointSizex(
    /* [in] */ Int32 size)
{
    GLCHECK(mgl, GlPointSizex, size);
    return NOERROR;
}

ECode GLErrorWrapper::GlPolygonOffset(
    /* [in] */ Float factor,
    /* [in] */ Float units)
{
    GLCHECK(mgl, GlPolygonOffset, factor, units);
    return NOERROR;
}

ECode GLErrorWrapper::GlPolygonOffsetx(
    /* [in] */ Int32 factor,
    /* [in] */ Int32 units)
{
    GLCHECK(mgl, GlPolygonOffsetx, factor, units);
    return NOERROR;
}

ECode GLErrorWrapper::GlPopMatrix()
{
    GLCHECK(mgl, GlPopMatrix);
    return NOERROR;
}

ECode GLErrorWrapper::GlPushMatrix()
{
    GLCHECK(mgl, GlPushMatrix);
    return NOERROR;
}

ECode GLErrorWrapper::GlReadPixels(
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

ECode GLErrorWrapper::GlRotatef(
    /* [in] */ Float angle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    GLCHECK(mgl, GlRotatef, angle, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::GlRotatex(
    /* [in] */ Int32 angle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    GLCHECK(mgl, GlRotatex, angle, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::GlSampleCoverage(
    /* [in] */ Float value,
    /* [in] */ Boolean invert)
{
    GLCHECK(mgl, GlSampleCoverage, value, invert);
    return NOERROR;
}

ECode GLErrorWrapper::GlSampleCoveragex(
    /* [in] */ Int32 value,
    /* [in] */ Boolean invert)
{
    GLCHECK(mgl, GlSampleCoveragex, value, invert);
    return NOERROR;
}

ECode GLErrorWrapper::GlScalef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    GLCHECK(mgl, GlScalef, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::GlScalex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    GLCHECK(mgl, GlScalex, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::GlScissor(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl, GlScissor, x, y, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::GlShadeModel(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl, GlShadeModel, mode);
    return NOERROR;
}

ECode GLErrorWrapper::GlStencilFunc(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref,
    /* [in] */ Int32 mask)
{
    GLCHECK(mgl, GlStencilFunc, func, ref, mask);
    return NOERROR;
}

ECode GLErrorWrapper::GlStencilMask(
    /* [in] */ Int32 mask)
{
    GLCHECK(mgl, GlStencilMask, mask);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlTexCoordPointer, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexImage2D(
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

ECode GLErrorWrapper::GlTexParameterx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl, GlTexParameterx, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexSubImage2D(
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

ECode GLErrorWrapper::GlTranslatef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    GLCHECK(mgl, GlTranslatef, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::GlTranslatex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    GLCHECK(mgl, GlTranslatex, x, y, z);
    return NOERROR;
}

ECode GLErrorWrapper::GlVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl, GlVertexPointer, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::GlViewport(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl, GlViewport, x, y, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::GlEnable(
    /* [in] */ Int32 cap)
{
    GLCHECK(IGL::Probe(mgl), GlEnable, cap);
    return NOERROR;
}

ECode GLErrorWrapper::GlEnableClientState(
    /* [in] */ Int32 array)
{
    GLCHECK(IGL::Probe(mgl), GlEnableClientState, array);
    return NOERROR;
}


ECode GLErrorWrapper::GlTexParameterf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(IGL::Probe(mgl), GlTexParameterf, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(IGL::Probe(mgl11), GlTexParameterfv, target,  pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(IGL::Probe(mgl11), GlTexParameterfv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlBindTexture(
    /* [in] */ Int32 target,
    /* [in] */ Int32 texture)
{
    GLCHECK(IGL::Probe(mgl), GlBindTexture, target, texture);
    return NOERROR;
}

ECode GLErrorWrapper::GlCompressedTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data)
{
    GLCHECK(IGL::Probe(mgl), GlCompressedTexImage2D, target, level, internalformat, width, height, border, imageSize, data);
    return NOERROR;
}

ECode GLErrorWrapper::GlCopyTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border)
{
    GLCHECK(IGL::Probe(mgl), GlCopyTexImage2D, target, level, internalformat, x, y, width, height, border);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(IGL::Probe(mgl), GlGetIntegerv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(IGL::Probe(mgl), GlGetIntegerv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlStencilOp(
    /* [in] */ Int32 fail,
    /* [in] */ Int32 zfail,
    /* [in] */ Int32 zpass)
{
    GLCHECK(IGL::Probe(mgl), GlStencilOp, fail, zfail, zpass);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnvf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(IGL::Probe(mgl), GlTexEnvf, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(IGL::Probe(mgl), GlTexEnvfv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(IGL::Probe(mgl), GlTexEnvfv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnvx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(IGL::Probe(mgl), GlTexEnvx, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(IGL::Probe(mgl), GlTexEnvxv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(IGL::Probe(mgl), GlTexEnvxv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlQueryMatrixxOES(
    /* [in] */ ArrayOf<Int32>* mantissa,
    /* [in] */ Int32 mantissaOffset,
    /* [in] */ ArrayOf<Int32>* exponent,
    /* [in] */ Int32 exponentOffset,
    /* [out] */ Int32* matrixxOES)
{
    VALIDATE_NOT_NULL(matrixxOES)

    GLCHECK(mgl10Ext, GlQueryMatrixxOES, mantissa, mantissaOffset, exponent, exponentOffset, matrixxOES);
    return NOERROR;
}

ECode GLErrorWrapper::GlQueryMatrixxOES(
    /* [in] */ IInt32Buffer* mantissa,
    /* [in] */ IInt32Buffer* exponent,
    /* [out] */ Int32* matrixxOES)
{
    VALIDATE_NOT_NULL(matrixxOES)

    GLCHECK(mgl10Ext, GlQueryMatrixxOES, mantissa, exponent, matrixxOES);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetPointerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<IBuffer*>* params)
{
    GLCHECK(mgl11, GlGetPointerv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlBindBuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 buffer)
{
    GLCHECK(mgl11, GlBindBuffer, target, buffer);
    return NOERROR;
}

ECode GLErrorWrapper::GlBufferData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data,
    /* [in] */ Int32 usage)
{
    GLCHECK(mgl11, GlBufferData, target, size, data, usage);
    return NOERROR;
}

ECode GLErrorWrapper::GlBufferSubData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data)
{
    GLCHECK(mgl11, GlBufferSubData, target, offset, size, data);
    return NOERROR;
}

ECode GLErrorWrapper::GlClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Float>* equation,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlClipPlanef, plane, equation, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ IFloatBuffer* equation)
{
    GLCHECK(mgl11, GlClipPlanef, plane, equation);
    return NOERROR;
}

ECode GLErrorWrapper::GlClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Int32>* equation,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlClipPlanex, plane, equation, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ IInt32Buffer* equation)
{
    GLCHECK(mgl11, GlClipPlanex, plane, equation);
    return NOERROR;
}

ECode GLErrorWrapper::GlColor4ub(
    /* [in] */ Byte red,
    /* [in] */ Byte green,
    /* [in] */ Byte blue,
    /* [in] */ Byte alpha)
{
    GLCHECK(mgl11, GlColor4ub, red, green, blue, alpha);
    return NOERROR;
}

ECode GLErrorWrapper::GlColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlColorPointer, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlDeleteBuffers, n, buffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers)
{
    GLCHECK(mgl11, GlDeleteBuffers, n, buffers);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlDrawElements, mode, count, type, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGenBuffers, n, buffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers)
{
    GLCHECK(mgl11, GlGenBuffers, n, buffers);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Boolean>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetBooleanv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetBooleanv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetBufferParameteriv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetBufferParameteriv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* eqn,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetClipPlanef, pname, eqn, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* eqn)
{
    GLCHECK(mgl11, GlGetClipPlanef, pname, eqn);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* eqn,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetClipPlanex, pname, eqn, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* eqn)
{
    GLCHECK(mgl11, GlGetClipPlanex, pname, eqn);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetFixedv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetFixedv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetFloatv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetFloatv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetLightfv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetLightfv, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetLightxv, light, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetLightxv, light, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetMaterialfv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetMaterialfv, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetMaterialxv, face, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetMaterialxv, face, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexEnviv, env, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexEnviv, env, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexEnvxv, env, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexEnvxv, env, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexParameterfv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlGetTexParameterfv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexParameteriv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexParameteriv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlGetTexParameterxv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlGetTexParameterxv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlIsBuffer(
    /* [in] */ Int32 buffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLCHECK(mgl11, GlIsBuffer, buffer, result);
    return NOERROR;
}

ECode GLErrorWrapper::GlIsEnabled(
    /* [in] */ Int32 cap,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLCHECK(mgl11, GlIsEnabled, cap, result);
    return NOERROR;
}

ECode GLErrorWrapper::GlIsTexture(
    /* [in] */ Int32 texture,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLCHECK(mgl11, GlIsTexture, texture, result);
    return NOERROR;
}

ECode GLErrorWrapper::GlNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlNormalPointer, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointParameterf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl11, GlPointParameterf, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlPointParameterfv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11, GlPointParameterfv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointParameterx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11, GlPointParameterx, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlPointParameterxv, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlPointParameterxv, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlPointSizePointerOES(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl11, GlPointSizePointerOES, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexCoordPointer, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnvi(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11, GlTexEnvi, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexEnviv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlTexEnviv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexParameteri(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11, GlTexParameteri, target, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexParameteriv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlTexParameteriv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlTexParameterxv, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11, GlTexParameterxv, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11, GlVertexPointer, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlCurrentPaletteMatrixOES(
    /* [in] */ Int32 matrixpaletteindex)
{
    GLCHECK(mgl11Ext, GlCurrentPaletteMatrixOES, matrixpaletteindex);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexfOES(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    GLCHECK(mgl11Ext, GlDrawTexfOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexfvOES(
    /* [in] */ ArrayOf<Float>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexfvOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexfvOES(
    /* [in] */ IFloatBuffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexfvOES, coords);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexiOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl11Ext, GlDrawTexiOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexivOES(
    /* [in] */ ArrayOf<Int32>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexivOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexivOES(
    /* [in] */ IInt32Buffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexivOES, coords);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexsOES(
    /* [in] */ Int16 x,
    /* [in] */ Int16 y,
    /* [in] */ Int16 z,
    /* [in] */ Int16 width,
    /* [in] */ Int16 height)
{
    GLCHECK(mgl11Ext, GlDrawTexsOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexsvOES(
    /* [in] */ ArrayOf<Int16>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexsvOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexsvOES(
    /* [in] */ IInt16Buffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexsvOES, coords);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexxOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl11Ext, GlDrawTexxOES, x, y, z, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexxvOES(
    /* [in] */ ArrayOf<Int32>* coords,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlDrawTexxvOES, coords, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDrawTexxvOES(
    /* [in] */ IInt32Buffer* coords)
{
    GLCHECK(mgl11Ext, GlDrawTexxvOES, coords);
    return NOERROR;
}

ECode GLErrorWrapper::GlLoadPaletteFromModelViewMatrixOES()
{
    GLCHECK(mgl11Ext, GlLoadPaletteFromModelViewMatrixOES);
    return NOERROR;
}

ECode GLErrorWrapper::GlMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl11Ext, GlMatrixIndexPointerOES, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::GlMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlMatrixIndexPointerOES, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    GLCHECK(mgl11Ext, GlWeightPointerOES, size, type, stride, pointer);
    return NOERROR;
}

ECode GLErrorWrapper::GlWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11Ext, GlWeightPointerOES, size, type, stride, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlBindFramebufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 framebuffer)
{
    GLCHECK(mgl11ExtensionPack, GlBindFramebufferOES, target, framebuffer);
    return NOERROR;
}

ECode GLErrorWrapper::GlBindRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 renderbuffer)
{
    GLCHECK(mgl11ExtensionPack, GlBindRenderbufferOES, target, renderbuffer);
    return NOERROR;
}

ECode GLErrorWrapper::GlBlendEquation(
    /* [in] */ Int32 mode)
{
    GLCHECK(mgl11ExtensionPack, GlBlendEquation, mode);
    return NOERROR;
}

ECode GLErrorWrapper::GlBlendEquationSeparate(
    /* [in] */ Int32 modeRGB,
    /* [in] */ Int32 modeAlpha)
{
    GLCHECK(mgl11ExtensionPack, GlBlendEquationSeparate, modeRGB, modeAlpha);
    return NOERROR;
}

ECode GLErrorWrapper::GlBlendFuncSeparate(
    /* [in] */ Int32 srcRGB,
    /* [in] */ Int32 dstRGB,
    /* [in] */ Int32 srcAlpha,
    /* [in] */ Int32 dstAlpha)
{
    GLCHECK(mgl11ExtensionPack, GlBlendFuncSeparate, srcRGB, dstRGB, srcAlpha, dstAlpha);
    return NOERROR;
}

ECode GLErrorWrapper::GlCheckFramebufferStatusOES(
    /* [in] */ Int32 target,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)

    GLCHECK(mgl11ExtensionPack, GlCheckFramebufferStatusOES, target, status);
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteFramebuffersOES, n, framebuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteFramebuffersOES, n, framebuffers);
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteRenderbuffersOES, n, renderbuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers)
{
    GLCHECK(mgl11ExtensionPack, GlDeleteRenderbuffersOES, n, renderbuffers);
    return NOERROR;
}

ECode GLErrorWrapper::GlFramebufferRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 renderbuffertarget,
    /* [in] */ Int32 renderbuffer)
{
    GLCHECK(mgl11ExtensionPack, GlFramebufferRenderbufferOES, target, attachment, renderbuffertarget, renderbuffer);
    return NOERROR;
}

ECode GLErrorWrapper::GlFramebufferTexture2DOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 textarget,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level)
{
    GLCHECK(mgl11ExtensionPack, GlFramebufferTexture2DOES, target, attachment, textarget, texture, level);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenerateMipmapOES(
    /* [in] */ Int32 target)
{
    GLCHECK(mgl11ExtensionPack, GlGenerateMipmapOES, target);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGenFramebuffersOES, n, framebuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers)
{
    GLCHECK(mgl11ExtensionPack, GlGenFramebuffersOES, n, framebuffers);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGenRenderbuffersOES, n, renderbuffers, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers)
{
    GLCHECK(mgl11ExtensionPack, GlGenRenderbuffersOES, n, renderbuffers);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetFramebufferAttachmentParameterivOES, target, attachment, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetFramebufferAttachmentParameterivOES, target, attachment, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetRenderbufferParameterivOES, target, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetRenderbufferParameterivOES, target, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenfv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenfv, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGeniv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGeniv, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenxv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlGetTexGenxv, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlIsFramebufferOES(
    /* [in] */ Int32 framebuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLCHECK(mgl11ExtensionPack, GlIsFramebufferOES, framebuffer, result);
    return NOERROR;
}

ECode GLErrorWrapper::GlIsRenderbufferOES(
    /* [in] */ Int32 renderbuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    GLCHECK(mgl11ExtensionPack, GlIsRenderbufferOES, renderbuffer, result);
    return NOERROR;
}

ECode GLErrorWrapper::GlRenderbufferStorageOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    GLCHECK(mgl11ExtensionPack, GlRenderbufferStorageOES, target, internalformat, width, height);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGenf(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenf, coord, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenfv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenfv, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGeni(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11ExtensionPack, GlTexGeni, coord, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlTexGeniv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlTexGeniv, coord, pname, params);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGenx(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenx, coord, pname, param);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenxv, coord, pname, params, offset);
    return NOERROR;
}

ECode GLErrorWrapper::GlTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    GLCHECK(mgl11ExtensionPack, GlTexGenxv, coord, pname, params);
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
                errStr = String("Unknown error 0x") + StringUtils::ToString(glError, 16);
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