
#ifndef __ELASTOS_DROID_OPENGL_CGLES11_H__
#define __ELASTOS_DROID_OPENGL_CGLES11_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CGLES11.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IBuffer;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES11)
     , public Singleton
     , public IGLES11
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /*................Function defined in GLES10..................*/
    CARAPI GlActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI GlAlphaFunc(
        /* [in] */ Int32 func,
        /* [in] */ Float ref);

    CARAPI GlAlphaFuncx(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref);

    CARAPI GlBindTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 texture);

    CARAPI GlBlendFunc(
        /* [in] */ Int32 sfactor,
        /* [in] */ Int32 dfactor);

    CARAPI GlClear(
        /* [in] */ Int32 mask);

    CARAPI GlClearColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI GlClearColorx(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI GlClearDepthf(
        /* [in] */ Float depth);

    CARAPI GlClearDepthx(
        /* [in] */ Int32 depth);

    CARAPI GlClearStencil(
        /* [in] */ Int32 s);

    CARAPI GlClientActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI GlColor4f(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI GlColor4x(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI GlColorMask(
        /* [in] */ Boolean red,
        /* [in] */ Boolean green,
        /* [in] */ Boolean blue,
        /* [in] */ Boolean alpha);

    CARAPI GlColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlCompressedTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ Elastos::IO::IBuffer* data_buf);

    CARAPI GlCompressedTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 imageSize,
        /* [in] */ Elastos::IO::IBuffer* data_buf);

    CARAPI GlCopyTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border);

    CARAPI GlCopyTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlCullFace(
        /* [in] */ Int32 mode);

    CARAPI GlDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures_buf,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* textures_buf);

    CARAPI GlDepthFunc(
        /* [in] */ Int32 func);

    CARAPI GlDepthMask(
        /* [in] */ Boolean flag);

    CARAPI GlDepthRangef(
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI GlDepthRangex(
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI GlDisable(
        /* [in] */ Int32 cap);

    CARAPI GlDisableClientState(
        /* [in] */ Int32 array);

    CARAPI GlDrawArrays(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 first,
        /* [in] */ Int32 count);

    CARAPI GlDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* indices_buf);

    CARAPI GlEnable(
        /* [in] */ Int32 cap);

    CARAPI GlEnableClientState(
        /* [in] */ Int32 array);

    CARAPI GlFinish();

    CARAPI GlFlush();

    CARAPI GlFogf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlFogx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlFrontFace(
        /* [in] */ Int32 mode);

    CARAPI GlFrustumf(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI GlFrustumx(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI GlGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures_ref,
        /* [in] */ Int32 offset);

    CARAPI GlGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* textures_buf);

    CARAPI GlGetError(
        /* [out] */ Int32* error);

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlGetString(
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI GlHint(
        /* [in] */ Int32 target,
        /* [in] */ Int32 mode);

    CARAPI GlLightModelf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlLightModelx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlLightf(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlLightx(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlLineWidth(
        /* [in] */ Float width);

    CARAPI GlLineWidthx(
        /* [in] */ Int32 width);

    CARAPI GlLoadIdentity();

    CARAPI GlLoadMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI GlLoadMatrixf(
        /* [in] */ Elastos::IO::IFloatBuffer* m);

    CARAPI GlLoadMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI GlLoadMatrixx(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

    CARAPI GlLogicOp(
        /* [in] */ Int32 opcode);

    CARAPI GlMaterialf(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlMaterialx(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlMatrixMode(
        /* [in] */ Int32 mode);

    CARAPI GlMultMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI GlMultMatrixf(
        /* [in] */ Elastos::IO::IFloatBuffer* m);

    CARAPI GlMultMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI GlMultMatrixx(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

    CARAPI GlMultiTexCoord4f(
        /* [in] */ Int32 target,
        /* [in] */ Float s,
        /* [in] */ Float t,
        /* [in] */ Float r,
        /* [in] */ Float q);

    CARAPI GlMultiTexCoord4x(
        /* [in] */ Int32 target,
        /* [in] */ Int32 s,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 q);

    CARAPI GlNormal3f(
        /* [in] */ Float nx,
        /* [in] */ Float ny,
        /* [in] */ Float nz);

    CARAPI GlNormal3x(
        /* [in] */ Int32 nx,
        /* [in] */ Int32 ny,
        /* [in] */ Int32 nz);

    CARAPI GlNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlOrthof(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI GlOrthox(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI GlPixelStorei(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlPointSize(
        /* [in] */ Float size);

    CARAPI GlPointSizex(
        /* [in] */ Int32 size);

    CARAPI GlPolygonOffset(
        /* [in] */ Float factor,
        /* [in] */ Float units);

    CARAPI GlPolygonOffsetx(
        /* [in] */ Int32 factor,
        /* [in] */ Int32 units);

    CARAPI GlPopMatrix();

    CARAPI GlPushMatrix();

    CARAPI GlReadPixels(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* pixels);

    CARAPI GlRotatef(
        /* [in] */ Float angle,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GlRotatex(
        /* [in] */ Int32 angle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI GlSampleCoverage(
        /* [in] */ Float value,
        /* [in] */ Boolean invert);

    CARAPI GlSampleCoveragex(
        /* [in] */ Int32 value,
        /* [in] */ Boolean invert);

    CARAPI GlScalef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GlScalex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI GlScissor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlShadeModel(
        /* [in] */ Int32 mode);

    CARAPI GlStencilFunc(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    CARAPI GlStencilMask(
        /* [in] */ Int32 mask);

    CARAPI GlStencilOp(
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    CARAPI GlTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlTexEnvf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlTexEnvx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* pixels);

    CARAPI GlTexParameterf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlTexParameterx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* pixels);

    CARAPI GlTranslatef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GlTranslatex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI GlVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlViewport(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

public:
    CARAPI GlBindBuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 buffer);

    CARAPI GlBufferData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 size,
        /* [in] */ Elastos::IO::IBuffer* data,
        /* [in] */ Int32 usage);

    CARAPI GlBufferSubData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Elastos::IO::IBuffer* data);

    CARAPI GlClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Float>* equation,
        /* [in] */ Int32 offset);

    CARAPI GlClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ Elastos::IO::IFloatBuffer* equation);

    CARAPI GlClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Int32>* equation,
        /* [in] */ Int32 offset);

    CARAPI GlClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ Elastos::IO::IInt32Buffer* equation);

    CARAPI GlColor4ub(
        /* [in] */ Byte red,
        /* [in] */ Byte green,
        /* [in] */ Byte blue,
        /* [in] */ Byte alpha);

    CARAPI GlColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* buffers);

    CARAPI GlDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset);

    CARAPI GlGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI GlGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* buffers);

    CARAPI GlGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Boolean>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* eqn,
        /* [in] */ Int32 offset);

    CARAPI GlGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* eqn);

    CARAPI GlGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* eqn,
        /* [in] */ Int32 offset);

    CARAPI GlGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* eqn);

    CARAPI GlGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI GlGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI GlGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI GlGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetTexEnvfv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexEnvfv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI GlGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI GlGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlIsBuffer(
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* isBuffer);

    CARAPI GlIsEnabled(
        /* [in] */ Int32 cap,
        /* [out] */ Boolean* isEnabled);

    CARAPI GlIsTexture(
        /* [in] */ Int32 texture,
        /* [out] */ Boolean* isTexture);

    CARAPI GlNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlPointParameterf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI GlPointParameterx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlPointSizePointerOES(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvi(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI GlTexParameteri(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI GlVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

private:
    CARAPI GetDirectBufferPointer(
        /* [in] */ IBuffer* buffer,
        /* [out] */ Handle64* result);

    CARAPI GlPointSizePointerOESBounds(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer_buf,
        /* [in] */ Int32 remaining);

private:
    static AutoPtr<IGLES10> sGl10;
    static AutoPtr<IBuffer> sPointSizePointerOES;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES11_H__
