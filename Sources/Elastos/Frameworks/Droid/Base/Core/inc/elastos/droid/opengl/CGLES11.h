
#ifndef __ELASTOS_DROID_OPENGL_CGLES11_H__
#define __ELASTOS_DROID_OPENGL_CGLES11_H__

#include "_Elastos_Droid_Opengl_CGLES11.h"

using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES11)
{
public:
    /*................Function defined in GLES10..................*/
    CARAPI glActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI glAlphaFunc(
        /* [in] */ Int32 func,
        /* [in] */ Float ref);

    CARAPI glAlphaFuncx(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref);

    CARAPI glBindTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 texture);

    CARAPI glBlendFunc(
        /* [in] */ Int32 sfactor,
        /* [in] */ Int32 dfactor);

    CARAPI glClear(
        /* [in] */ Int32 mask);

    CARAPI glClearColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI glClearColorx(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI glClearDepthf(
        /* [in] */ Float depth);

    CARAPI glClearDepthx(
        /* [in] */ Int32 depth);

    CARAPI glClearStencil(
        /* [in] */ Int32 s);

    CARAPI glClientActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI glColor4f(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI glColor4x(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI glColorMask(
        /* [in] */ Boolean red,
        /* [in] */ Boolean green,
        /* [in] */ Boolean blue,
        /* [in] */ Boolean alpha);

    CARAPI glColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI glCompressedTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ Elastos::IO::IBuffer* data_buf);

    CARAPI glCompressedTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 imageSize,
        /* [in] */ Elastos::IO::IBuffer* data_buf);

    CARAPI glCopyTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border);

    CARAPI glCopyTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glCullFace(
        /* [in] */ Int32 mode);

    CARAPI glDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures_buf,
        /* [in] */ Int32 offset);

    CARAPI glDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* textures_buf);

    CARAPI glDepthFunc(
        /* [in] */ Int32 func);

    CARAPI glDepthMask(
        /* [in] */ Boolean flag);

    CARAPI glDepthRangef(
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glDepthRangex(
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glDisable(
        /* [in] */ Int32 cap);

    CARAPI glDisableClientState(
        /* [in] */ Int32 array);

    CARAPI glDrawArrays(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 first,
        /* [in] */ Int32 count);

    CARAPI glDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* indices_buf);

    CARAPI glEnable(
        /* [in] */ Int32 cap);

    CARAPI glEnableClientState(
        /* [in] */ Int32 array);

    CARAPI glFinish();

    CARAPI glFlush();

    CARAPI glFogf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI glFogx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI glFrontFace(
        /* [in] */ Int32 mode);

    CARAPI glFrustumf(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glFrustumx(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures_ref,
        /* [in] */ Int32 offset);

    CARAPI glGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* textures_buf);

    CARAPI glGetError(
        /* [out] */ Int32* error);

    CARAPI glGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI glGetString(
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI glHint(
        /* [in] */ Int32 target,
        /* [in] */ Int32 mode);

    CARAPI glLightModelf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI glLightModelx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI glLightf(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI glLightx(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI glLineWidth(
        /* [in] */ Float width);

    CARAPI glLineWidthx(
        /* [in] */ Int32 width);

    CARAPI glLoadIdentity();

    CARAPI glLoadMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI glLoadMatrixf(
        /* [in] */ Elastos::IO::IFloatBuffer* m);

    CARAPI glLoadMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI glLoadMatrixx(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

    CARAPI glLogicOp(
        /* [in] */ Int32 opcode);

    CARAPI glMaterialf(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI glMaterialx(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI glMatrixMode(
        /* [in] */ Int32 mode);

    CARAPI glMultMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI glMultMatrixf(
        /* [in] */ Elastos::IO::IFloatBuffer* m);

    CARAPI glMultMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI glMultMatrixx(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

    CARAPI glMultiTexCoord4f(
        /* [in] */ Int32 target,
        /* [in] */ Float s,
        /* [in] */ Float t,
        /* [in] */ Float r,
        /* [in] */ Float q);

    CARAPI glMultiTexCoord4x(
        /* [in] */ Int32 target,
        /* [in] */ Int32 s,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 q);

    CARAPI glNormal3f(
        /* [in] */ Float nx,
        /* [in] */ Float ny,
        /* [in] */ Float nz);

    CARAPI glNormal3x(
        /* [in] */ Int32 nx,
        /* [in] */ Int32 ny,
        /* [in] */ Int32 nz);

    CARAPI glNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI glOrthof(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glOrthox(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glPixelStorei(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glPointSize(
        /* [in] */ Float size);

    CARAPI glPointSizex(
        /* [in] */ Int32 size);

    CARAPI glPolygonOffset(
        /* [in] */ Float factor,
        /* [in] */ Float units);

    CARAPI glPolygonOffsetx(
        /* [in] */ Int32 factor,
        /* [in] */ Int32 units);

    CARAPI glPopMatrix();

    CARAPI glPushMatrix();

    CARAPI glReadPixels(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* pixels);

    CARAPI glRotatef(
        /* [in] */ Float angle,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI glRotatex(
        /* [in] */ Int32 angle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glSampleCoverage(
        /* [in] */ Float value,
        /* [in] */ Boolean invert);

    CARAPI glSampleCoveragex(
        /* [in] */ Int32 value,
        /* [in] */ Boolean invert);

    CARAPI glScalef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI glScalex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glScissor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glShadeModel(
        /* [in] */ Int32 mode);

    CARAPI glStencilFunc(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    CARAPI glStencilMask(
        /* [in] */ Int32 mask);

    CARAPI glStencilOp(
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    CARAPI glTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI glTexEnvf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI glTexEnvx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI glTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI glTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* pixels);

    CARAPI glTexParameterf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glTexParameterx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* pixels);

    CARAPI glTranslatef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI glTranslatex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI glViewport(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

public:
    CARAPI glBindBuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 buffer);

    CARAPI glBufferData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 size,
        /* [in] */ Elastos::IO::IBuffer* data,
        /* [in] */ Int32 usage);

    CARAPI glBufferSubData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Elastos::IO::IBuffer* data);

    CARAPI glClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Float>* equation,
        /* [in] */ Int32 offset);

    CARAPI glClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ Elastos::IO::IFloatBuffer* equation);

    CARAPI glClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Int32>* equation,
        /* [in] */ Int32 offset);

    CARAPI glClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ Elastos::IO::IInt32Buffer* equation);

    CARAPI glColor4ub(
        /* [in] */ Byte red,
        /* [in] */ Byte green,
        /* [in] */ Byte blue,
        /* [in] */ Byte alpha);

    CARAPI glColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI glDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* buffers);

    CARAPI glDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset);

    CARAPI glGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI glGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* buffers);

    CARAPI glGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Boolean>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* eqn,
        /* [in] */ Int32 offset);

    CARAPI glGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* eqn);

    CARAPI glGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* eqn,
        /* [in] */ Int32 offset);

    CARAPI glGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* eqn);

    CARAPI glGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexEnvfv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexEnvfv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glIsBuffer(
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* isBuffer);

    CARAPI glIsEnabled(
        /* [in] */ Int32 cap,
        /* [out] */ Boolean* isEnabled);

    CARAPI glIsTexture(
        /* [in] */ Int32 texture,
        /* [out] */ Boolean* isTexture);

    CARAPI glNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glPointParameterf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glPointParameterx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glPointSizePointerOES(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI glTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glTexEnvi(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glTexParameteri(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

private:
    CARAPI GetDirectBufferPointer(
        /* [in] */ IBuffer* buffer,
        /* [out] */ Handle32* result);

    CARAPI GetPointer(
        /* [in] */ IBuffer* buffer,
        /* [in, out] */ Handle32* array,
        /* [in, out] */ Int32* remaining,
        /* [in, out] */ Int32* offset,
        /* [out] */ Handle32* rst);

    CARAPI glPointSizePointerOESBounds(
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
